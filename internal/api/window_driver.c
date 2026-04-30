#include "window_driver.h"
#include <stdio.h>
#include <string.h>

// TODO: add better error handling?
// int visible_window_list() {
//   const CFArrayRef windowList = CGWindowListCopyWindowInfo(
//       kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

//   if (!windowList)
//     return 1;

//   int count = CFArrayGetCount(windowList);
// }

Window build_window_struct(CFDictionaryRef dict) {
  // PID
  int winPID = 0;
  CFNumberRef pidNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowOwnerPID);
  CFNumberGetValue(pidNum, kCFNumberIntType, &winPID);

  // Window ID
  pid_t id = 0;
  CFNumberRef winNum = (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowNumber);
  CFNumberGetValue(winNum, kCFNumberIntType, &id);

  // Layer
  int layer = 0;
  CFNumberRef layerNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowLayer);
  CFNumberGetValue(layerNum, kCFNumberIntType, &layer);

  // Get the window bounds (CGRect)
  CGRect rect;
  CFDictionaryRef windowBoundsRef =
      (CFDictionaryRef)CFDictionaryGetValue(dict, kCGWindowBounds);
  if (windowBoundsRef) {
    CFNumberRef windowBoundsXRef =
        (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("X"));
    CFNumberRef windowBoundsYRef =
        (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("Y"));
    CFNumberRef windowBoundsWidthRef =
        (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("Width"));
    CFNumberRef windowBoundsHeightRef =
        (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("Height"));

    if (windowBoundsXRef && windowBoundsYRef && windowBoundsWidthRef &&
        windowBoundsHeightRef) {
      float x, y, width, height;
      if (CFNumberGetValue(windowBoundsXRef, kCFNumberFloat32Type, &x) &&
          CFNumberGetValue(windowBoundsYRef, kCFNumberFloat32Type, &y) &&
          CFNumberGetValue(windowBoundsWidthRef, kCFNumberFloat32Type,
                           &width) &&
          CFNumberGetValue(windowBoundsHeightRef, kCFNumberFloat32Type,
                           &height)) {

        rect = CGRectMake(x, y, width, height);
      }
    }
  }

  // Sharing state
  int sharingState = 0;
  CFNumberRef sharingNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowSharingState);
  CFNumberGetValue(sharingNum, kCFNumberIntType, &sharingState);

  // Alpha
  float alpha = 0.0;
  CFNumberRef alphaNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowAlpha);
  CFNumberGetValue(alphaNum, kCFNumberFloatType, &alpha);

  // Title
  char name[256];
  CFStringRef title =
      (CFStringRef)CFDictionaryGetValue(dict, kCGWindowOwnerName);
  if (title) {
    CFStringGetCString(title, name, 256, kCFStringEncodingUTF8);
  }

  Window window = {rect, id, layer, true, sharingState, alpha};
  char *result = (char *)malloc(strlen(name) + 1);
  strcpy(result, name);
  window.title = result;
  return window;
}

int window_info_for_pid(pid_t pid, Window *window) {
  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

  if (!windowList)
    return 1;

  CFIndex count = CFArrayGetCount(windowList);

  for (CFIndex i = 0; i < count; i++) {
    CFDictionaryRef dict =
        (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);

    CFNumberRef pidNum =
        (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowOwnerPID);

    int winPID = 0;
    CFNumberGetValue(pidNum, kCFNumberIntType, &winPID);

    if (winPID != pid)
      continue;

    *window = build_window_struct(dict);

    CFRelease(windowList);
    return 0;
  }

  CFRelease(windowList);
  return 2;
}

int set_window_pid_pos(pid_t pid, int x, int y) {
  // Get accessibility object
  AXUIElementRef app = AXUIElementCreateApplication(pid);

  // Get windows
  CFArrayRef windows;
  if (AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                    (CFTypeRef *)&windows) != kAXErrorSuccess) {
    CFRelease(app);
    return 1;
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
  return 0;
}

int set_window_pid_size(pid_t pid, int x, int y) {
  // Get accessibility object
  AXUIElementRef app = AXUIElementCreateApplication(pid);

  // Get windows
  CFArrayRef windows;
  if (AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                    (CFTypeRef *)&windows) != kAXErrorSuccess) {
    CFRelease(app);
    return 1;
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

  return 0;
}