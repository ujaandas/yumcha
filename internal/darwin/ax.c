#include "ax.h"

// TODO: add better error handling?
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