#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <atomic>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include <unordered_set>

extern "C" CGSize getScreenSize();
extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

std::atomic<bool> running{true};

CGEventRef kpCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event,
                      void *refcon) {
  if (type == kCGEventKeyDown) {
    CGKeyCode key = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    printf("Keycode: %d\n", key);
  }
  return event;
}

void eventTapThread() {
  // Create event mask for what we want
  CGEventMask mask = CGEventMaskBit(kCGEventKeyDown);

  // Make tap
  CFMachPortRef tap =
      CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap,
                       kCGEventTapOptionDefault, mask, kpCallback, nullptr);

  // Get source and loop
  CFRunLoopSourceRef source =
      CFMachPortCreateRunLoopSource(kCFAllocatorDefault, tap, 0);
  CFRunLoopRef loop = CFRunLoopGetCurrent();

  // Activate event loop
  CGEventTapEnable(tap, true);
  CFRunLoopAddSource(loop, source, kCFRunLoopDefaultMode);

  // Run event loop
  while (running) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, true);
  }

  // Cleanup
  CFRunLoopRemoveSource(loop, source, kCFRunLoopDefaultMode);
  CFRelease(source);
  CFRelease(tap);
}

void setApplicationSize(pid_t pid, int x, int y) {
  // Get accessibility object
  AXUIElementRef app = AXUIElementCreateApplication(pid);

  // Get windows
  CFArrayRef windows;
  if (AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                    (CFTypeRef *)&windows) != kAXErrorSuccess) {
    CFRelease(app);
    return;
  }

  // Iterate over windows
  const int numWindows = CFArrayGetCount(windows);
  for (CFIndex i = 0; i < numWindows; i++) {
    // Get window object
    AXUIElementRef window = (AXUIElementRef)CFArrayGetValueAtIndex(windows, i);

    // Set size
    CGSize size = CGSizeMake(x, y);
    AXValueRef sizeValue = AXValueCreate(kAXValueTypeCGSize, &size);
    AXUIElementSetAttributeValue(window, kAXSizeAttribute, sizeValue);
    CFRelease(sizeValue);
  }

  CFRelease(windows);
  CFRelease(app);
}

void setApplicationPos(pid_t pid, int x, int y) {
  // Get accessibility object
  AXUIElementRef app = AXUIElementCreateApplication(pid);

  // Get windows
  CFArrayRef windows;
  if (AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                    (CFTypeRef *)&windows) != kAXErrorSuccess) {
    CFRelease(app);
    return;
  }

  // Iterate over windows
  const int numWindows = CFArrayGetCount(windows);
  for (CFIndex i = 0; i < numWindows; i++) {
    // Get window object
    AXUIElementRef window = (AXUIElementRef)CFArrayGetValueAtIndex(windows, i);

    // Set position
    const CGPoint pos = CGPointMake(x, y);
    AXValueRef posValue = AXValueCreate(kAXValueTypeCGPoint, &pos);
    AXUIElementSetAttributeValue(window, kAXPositionAttribute, posValue);
    CFRelease(posValue);
  }

  CFRelease(windows);
  CFRelease(app);
}

void handleSigint(int) { running.store(false); }

int main() {
  // Handle sigint
  signal(SIGINT, handleSigint);

  // Start listener thread
  std::thread kp(eventTapThread);

  while (running.load()) {
    // Track seen PIDs
    std::unordered_set<pid_t> seen;

    // Get list of visible windows
    const CFArrayRef windowList =
        CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);

    // Get number of visible windows
    int numWindows = CFArrayGetCount(windowList);

    // Iterate over windows
    for (int i = 0; i < numWindows; i++) {

      // Get window dict
      const CFDictionaryRef windowInfo =
          (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);

      // Get window PID
      int pid = 0;
      const CFNumberRef windowPid =
          (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerPID);
      CFNumberGetValue(windowPid, kCFNumberIntType, &pid);

      // Skip VSCodium and seen PIDs
      if (seen.count(pid) || pid == 33410) {
        continue;
      }

      // Get window name
      char name[256];
      const CFStringRef windowName =
          (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
      CFStringGetCString(windowName, name, sizeof(name), kCFStringEncodingUTF8);

      // Hide window
      // printf("%s-> PID %d\n", name, pid);
      hideAppByPID(pid);

      // Add to list of seen
      seen.insert(pid);
    }

    // Play with VSCodium window
    CGSize size = getScreenSize();
    // printf("Screen is %dx%d\n", (int)size.width, (int)size.height);
    setApplicationPos(33410, 0, 0);
    setApplicationSize(33410, size.width, size.height);

    // Cleanup
    CFRelease(windowList);

    // Avoid busy-wait
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  kp.join();

  return 0;
}
