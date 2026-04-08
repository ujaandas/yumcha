#include "ax.h"

int focused_window_rect_for_pid(pid_t pid, int *x, int *y, int *w, int *h) {
  AXUIElementRef app = AXUIElementCreateApplication(pid);
  if (!app)
    return 1;

  CFTypeRef winValue = NULL;
  AXError err =
      AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute, &winValue);
  CFRelease(app);
  if (err != kAXErrorSuccess || !winValue)
    return 2;

  AXUIElementRef win = (AXUIElementRef)winValue;

  CFTypeRef posValue = NULL;
  CFTypeRef sizeValue = NULL;

  CGPoint pos;
  CGSize size;

  err = AXUIElementCopyAttributeValue(win, kAXPositionAttribute, &posValue);
  if (err != kAXErrorSuccess || !posValue) {
    CFRelease(win);
    return 3;
  }

  err = AXUIElementCopyAttributeValue(win, kAXSizeAttribute, &sizeValue);
  if (err != kAXErrorSuccess || !sizeValue) {
    CFRelease(posValue);
    CFRelease(win);
    return 4;
  }

  if (!AXValueGetValue((AXValueRef)posValue, kAXValueCGPointType, &pos)) {
    CFRelease(sizeValue);
    CFRelease(posValue);
    CFRelease(win);
    return 5;
  }
  if (!AXValueGetValue((AXValueRef)sizeValue, kAXValueCGSizeType, &size)) {
    CFRelease(sizeValue);
    CFRelease(posValue);
    CFRelease(win);
    return 6;
  }

  CFRelease(sizeValue);
  CFRelease(posValue);
  CFRelease(win);

  *x = (int)pos.x;
  *y = (int)pos.y;
  *w = (int)size.width;
  *h = (int)size.height;
  return 0;
}

int get_window_list(CFArrayRef *windows, int *windowLen) {
  const CFArrayRef windowList =
      CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);

  if (!windowList) {
    return 1;
  }

  int numWindows = CFArrayGetCount(windowList);

  *windows = windowList;
  *windowLen = numWindows;

  return 0;
}