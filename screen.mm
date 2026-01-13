#include <AppKit/AppKit.h>

extern "C" CGSize getScreenSize() { return NSScreen.mainScreen.frame.size; }