#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <cstdio>
#include <unistd.h>
#include <unordered_set>

extern "C" CGSize getScreenSize();
extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

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

int main() {
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
    printf("%s-> PID %d\n", name, pid);
    hideAppByPID(pid);

    // Add to list of seen
    seen.insert(pid);
  }

  // Play with VSCodium window
  CGSize size = getScreenSize();
  printf("Screen is %dx%d\n", (int)size.width, (int)size.height);
  setApplicationPos(33410, 0, 0);
  setApplicationSize(33410, size.width, size.height);

  CFRelease(windowList);
  return 0;
}
