#include "ax.h"

int window_info_for_pid(pid_t pid, int *id, int *layer, CGRect *windowBounds,
                        int *sharingState, float *alpha, char *name) {

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

    // Window ID
    CFNumberRef winNum =
        (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowNumber);
    CFNumberGetValue(winNum, kCFNumberIntType, id);

    // Layer
    CFNumberRef layerNum =
        (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowLayer);
    CFNumberGetValue(layerNum, kCFNumberIntType, layer);

    // Get the window bounds (CGRect)
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

          *windowBounds = CGRectMake(x, y, width, height);
        }
      }
    }

    // Sharing state
    CFNumberRef sharingNum =
        (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowSharingState);
    CFNumberGetValue(sharingNum, kCFNumberIntType, sharingState);

    // Alpha
    CFNumberRef alphaNum =
        (CFNumberRef)CFDictionaryGetValue(dict, kCGWindowAlpha);
    CFNumberGetValue(alphaNum, kCFNumberFloatType, alpha);

    // Title
    CFStringRef title =
        (CFStringRef)CFDictionaryGetValue(dict, kCGWindowOwnerName);
    if (title) {
      CFStringGetCString(title, name, 256, kCFStringEncodingUTF8);
    } else {
      snprintf(name, 256, "Unknown");
    }

    CFRelease(windowList);
    return 0;
  }

  CFRelease(windowList);
  return 2;
}

// int focused_window_rect_for_pid(pid_t pid, int *x, int *y, int *w, int *h)
// {
//   AXUIElementRef app = AXUIElementCreateApplication(pid);
//   if (!app)
//     return 1;

//   CFTypeRef winValue = NULL;
//   AXError err =
//       AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute,
//       &winValue);
//   CFRelease(app);
//   if (err != kAXErrorSuccess || !winValue)
//     return 2;

//   AXUIElementRef win = (AXUIElementRef)winValue;

//   CFTypeRef posValue = NULL;
//   CFTypeRef sizeValue = NULL;

//   CGPoint pos;
//   CGSize size;

//   err = AXUIElementCopyAttributeValue(win, kAXPositionAttribute,
//   &posValue); if (err != kAXErrorSuccess || !posValue) {
//     CFRelease(win);
//     return 3;
//   }

//   err = AXUIElementCopyAttributeValue(win, kAXSizeAttribute, &sizeValue);
//   if (err != kAXErrorSuccess || !sizeValue) {
//     CFRelease(posValue);
//     CFRelease(win);
//     return 4;
//   }

//   if (!AXValueGetValue((AXValueRef)posValue, kAXValueCGPointType, &pos)) {
//     CFRelease(sizeValue);
//     CFRelease(posValue);
//     CFRelease(win);
//     return 5;
//   }
//   if (!AXValueGetValue((AXValueRef)sizeValue, kAXValueCGSizeType, &size)) {
//     CFRelease(sizeValue);
//     CFRelease(posValue);
//     CFRelease(win);
//     return 6;
//   }

//   CFRelease(sizeValue);
//   CFRelease(posValue);
//   CFRelease(win);

//   *x = (int)pos.x;
//   *y = (int)pos.y;
//   *w = (int)size.width;
//   *h = (int)size.height;
//   return 0;
// }

// int get_window_list(CFArrayRef *windows, int *windowLen) {
//   const CFArrayRef windowList =
//       CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);

//   if (!windowList) {
//     return 1;
//   }

//   int numWindows = CFArrayGetCount(windowList);

//   *windows = windowList;
//   *windowLen = numWindows;

//   return 0;
// }

// int get_window_dict_vals(CFArrayRef *windows, int i, int *pid, char *name,
//                          int *windowNumber, int *windowLayer,
//                          CGRect *windowBounds, int *windowSharingState,
//                          float *windowAlpha) {
//   const CFDictionaryRef windowInfo =
//       (CFDictionaryRef)CFArrayGetValueAtIndex(*windows, i);

//   // Get the window PID
//   CFNumberRef windowPid =
//       (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerPID);
//   if (windowPid) {
//     if (!CFNumberGetValue(windowPid, kCFNumberIntType, pid)) {
//       return 1; // Failed to get PID value
//     }
//   } else {
//     return 2; // PID not found in dictionary
//   }

//   // Get the window name
//   const CFStringRef windowName =
//       (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
//   if (windowName) {
//     CFStringGetCString(windowName, name, 256, kCFStringEncodingUTF8);
//   } else {
//     snprintf(name, 256, "Unknown");
//   }

//   // Get the window number
//   CFNumberRef windowNumberRef =
//       (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowNumber);
//   if (windowNumberRef) {
//     if (!CFNumberGetValue(windowNumberRef, kCFNumberIntType, windowNumber))
//     {
//       return 3; // Failed to get window number
//     }
//   }

//   // Get the window layer
//   CFNumberRef windowLayerRef =
//       (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowLayer);
//   if (windowLayerRef) {
//     if (!CFNumberGetValue(windowLayerRef, kCFNumberIntType, windowLayer)) {
//       return 4; // Failed to get window layer
//     }
//   }

//   // Get the window bounds (CGRect)
//   CFDictionaryRef windowBoundsRef =
//       (CFDictionaryRef)CFDictionaryGetValue(windowInfo, kCGWindowBounds);
//   if (windowBoundsRef) {
//     CFNumberRef windowBoundsXRef =
//         (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("X"));
//     CFNumberRef windowBoundsYRef =
//         (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("Y"));
//     CFNumberRef windowBoundsWidthRef =
//         (CFNumberRef)CFDictionaryGetValue(windowBoundsRef, CFSTR("Width"));
//     CFNumberRef windowBoundsHeightRef =
//         (CFNumberRef)CFDictionaryGetValue(windowBoundsRef,
//         CFSTR("Height"));

//     if (windowBoundsXRef && windowBoundsYRef && windowBoundsWidthRef &&
//         windowBoundsHeightRef) {
//       float x, y, width, height;
//       if (CFNumberGetValue(windowBoundsXRef, kCFNumberFloat32Type, &x) &&
//           CFNumberGetValue(windowBoundsYRef, kCFNumberFloat32Type, &y) &&
//           CFNumberGetValue(windowBoundsWidthRef, kCFNumberFloat32Type,
//                            &width) &&
//           CFNumberGetValue(windowBoundsHeightRef, kCFNumberFloat32Type,
//                            &height)) {

//         *windowBounds = CGRectMake(x, y, width, height);
//       } else {
//         return 5; // Failed to extract window bounds
//       }
//     }
//   }

//   // Get the window sharing state
//   CFNumberRef windowSharingStateRef =
//       (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowSharingState);
//   if (windowSharingStateRef) {
//     if (!CFNumberGetValue(windowSharingStateRef, kCFNumberIntType,
//                           windowSharingState)) {
//       return 6; // Failed to get window sharing state
//     }
//   }

//   // Get the window alpha value
//   CFNumberRef windowAlphaRef =
//       (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowAlpha);
//   if (windowAlphaRef) {
//     if (!CFNumberGetValue(windowAlphaRef, kCFNumberFloat32Type,
//     windowAlpha))
//     {
//       return 7; // Failed to get window alpha
//     }
//   }

//   return 0; // Success
// }