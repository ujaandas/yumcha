#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

// extern "C" void hideAllApps() {
//   for (NSRunningApplication *app in
//        [[NSWorkspace sharedWorkspace] runningApplications]) {
//     [app hide];
//   }
// }

extern "C" void hideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  [app hide];
}

extern "C" void unhideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  [app unhide];
}