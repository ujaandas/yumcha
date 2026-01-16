#include "tree.cpp"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include <unordered_set>

extern "C" CGSize getScreenSize();
extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

const pid_t vscPid = 6261;
std::atomic<bool> running{true};
CGSize size = getScreenSize();

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

    AXError err =
        AXUIElementSetAttributeValue(window, kAXSizeAttribute, sizeValue);
    printf("set size err: %d\n", err);

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

CGEventRef kpCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event,
                      void *refcon) {
  if (type == kCGEventKeyDown) {
    CGKeyCode keycode =
        CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    CGEventFlags flags = CGEventGetFlags(event);

    std::printf("keycode: %d\n", keycode);

    if (flags & kCGEventFlagMaskAlternate) {
      switch ((int)keycode) {
      // opt+q
      case 12:
        std::printf("quit\n");
        std::raise(SIGINT);
      // opt+h
      case 4:
        std::printf("left\n");
        setApplicationPos(vscPid, 0, 0);
        setApplicationSize(vscPid, size.width / 2, size.height);
        break;
      // opt+j
      case 38:
        std::printf("down\n");
        setApplicationPos(vscPid, 0, size.height / 2);
        setApplicationSize(vscPid, size.width, size.height / 2);
        break;
      // opt+k
      case 40:
        std::printf("up\n");
        setApplicationPos(vscPid, 0, 0);
        setApplicationSize(vscPid, size.width, size.height / 2);
        break;
      // opt+l
      case 37:
        std::printf("right\n");
        setApplicationPos(vscPid, size.width / 2, 0);
        setApplicationSize(vscPid, size.width / 2, size.height);
        break;
      // opt+space
      case 36:
        std::printf("full\n");
        setApplicationPos(vscPid, 0, 0);
        setApplicationSize(vscPid, size.width, size.height);
        break;
      };
    }
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
  while (running.load()) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, true);
  }

  // Cleanup
  CFRunLoopRemoveSource(loop, source, kCFRunLoopDefaultMode);
  CFRelease(source);
  CFRelease(tap);
}

void handleSigint(int) { running.store(false); }

int main() {
  // Handle sigint
  signal(SIGINT, handleSigint);

  // Initialize tree
  Tree tree = Tree(VSplitNode{});
  tree.addNode(Node{WindowNode{vscPid}});
  tree.traverse();

  // Start full-sized
  setApplicationPos(vscPid, 0, 0);
  setApplicationSize(vscPid, size.width, size.height);

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

      // Get window name
      char name[256];
      const CFStringRef windowName =
          (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
      CFStringGetCString(windowName, name, sizeof(name), kCFStringEncodingUTF8);

      // Skip VIPs
      if (seen.count(pid) || pid == vscPid) {
        continue;
      }

      // Hide window
      // printf("%s-> PID %d\n", name, pid);
      hideAppByPID(pid);

      // Add to list of seen
      seen.insert(pid);
    }

    // Cleanup
    CFRelease(windowList);

    // Avoid busy-wait
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  kp.join();

  return 0;
}
