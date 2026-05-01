#include "window_driver.h"
#include <stdio.h>
#include <string.h>

// TODO: add better error handling?

int build_window_struct(CFDictionaryRef dict, Window *out) {
  if (!dict || !out)
    return 1;

  Window window = {0};

  CFNumberRef pidNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowOwnerPID);
  if (!pidNum)
    return 2;
  CFNumberGetValue(pidNum, kCFNumberIntType, &window.pid);

  CFNumberRef winNum = (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowNumber);
  if (!winNum)
    return 3;
  CFNumberGetValue(winNum, kCFNumberIntType, &window.id);

  CFNumberRef layerNum =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowLayer);
  if (!layerNum)
    return 4;
  CFNumberGetValue(layerNum, kCFNumberIntType, &window.layer);

  CFDictionaryRef bounds =
      (CFDictionaryRef)CFDictionaryGetValue(dict, kCGWindowBounds);
  if (!bounds)
    return 5;

  CFNumberRef xRef = CFDictionaryGetValue(bounds, CFSTR("X"));
  CFNumberRef yRef = CFDictionaryGetValue(bounds, CFSTR("Y"));
  CFNumberRef wRef = CFDictionaryGetValue(bounds, CFSTR("Width"));
  CFNumberRef hRef = CFDictionaryGetValue(bounds, CFSTR("Height"));

  if (!xRef || !yRef || !wRef || !hRef)
    return 6;

  float x, y, w, h;
  if (!CFNumberGetValue(xRef, kCFNumberFloat32Type, &x) ||
      !CFNumberGetValue(yRef, kCFNumberFloat32Type, &y) ||
      !CFNumberGetValue(wRef, kCFNumberFloat32Type, &w) ||
      !CFNumberGetValue(hRef, kCFNumberFloat32Type, &h))
    return 7;

  window.rect = CGRectMake(x, y, w, h);

  CFNumberRef share =
      (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowSharingState);
  if (share)
    CFNumberGetValue(share, kCFNumberIntType, &window.sharingState);

  CFNumberRef alpha = (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowAlpha);
  if (alpha)
    CFNumberGetValue(alpha, kCFNumberFloatType, &window.alpha);

  CFStringRef title =
      (CFStringRef)CFDictionaryGetValue(dict, kCGWindowOwnerName);

  if (title) {
    CFStringGetCString(title, window.title, sizeof(window.title),
                       kCFStringEncodingUTF8);
  } else {
    window.title[0] = '\0';
  }

  window.visible = true;

  *out = window;
  return 0;
}

int all_visible_windows(Window *windows, size_t capacity, size_t *count) {
  CFArrayRef windowList = CGWindowListCopyWindowInfo(
      kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

  if (!windowList)
    return 1;

  CFIndex n = CFArrayGetCount(windowList);

  size_t outIndex = 0;

  for (CFIndex i = 0; i < n && outIndex < capacity; i++) {
    CFDictionaryRef dict =
        (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
    Window w;
    if (build_window_struct(dict, &w) != 0) {
      continue; // skip invalid window
    }
    windows[outIndex++] = w;
  }

  CFRelease(windowList);

  *count = outIndex;
  return 0;
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

    Window w;
    if (build_window_struct(dict, &w) != 0) {
      return 2; // skip invalid window
    }
    *window = w;

    CFRelease(windowList);
    return 0;
  }

  CFRelease(windowList);
  return 3;
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