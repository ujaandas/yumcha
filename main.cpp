#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <cstdio>
#include <unistd.h>
#include <unordered_set>

extern "C" void hideAllApps();
extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

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

  // Get accessibility object
  AXUIElementRef app = AXUIElementCreateApplication(33410);

  // Get windows
  CFArrayRef vscWindows;
  if (AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                    (CFTypeRef *)&vscWindows) !=
      kAXErrorSuccess) {
    CFRelease(app);
    return 1;
  }

  // Iterate over windows
  numWindows = CFArrayGetCount(vscWindows);
  for (CFIndex i = 0; i < numWindows; i++) {
    // Get window object
    AXUIElementRef window =
        (AXUIElementRef)CFArrayGetValueAtIndex(vscWindows, i);

    // Set position
    const CGPoint pos = CGPointMake(300, 300);
    AXValueRef posValue = AXValueCreate(kAXValueTypeCGPoint, &pos);
    AXUIElementSetAttributeValue(window, kAXPositionAttribute, posValue);
    CFRelease(posValue);

    // Set size
    CGSize size = CGSizeMake(300, 300);
    AXValueRef sizeValue = AXValueCreate(kAXValueTypeCGSize, &size);
    AXUIElementSetAttributeValue(window, kAXSizeAttribute, sizeValue);
    CFRelease(sizeValue);
  }

  CFRelease(windowList);
  CFRelease(vscWindows);
  CFRelease(app);
  return 0;
}