#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

// extern "C" void hideAllApps() {
//   for (NSRunningApplication *app in
//        [[NSWorkspace sharedWorkspace] runningApplications]) {
//     [app hide];
//   }
// }

extern "C" bool isPidVisible(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  return app.hidden;
}

// Return true if operation was successful
extern "C" bool hideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  // Flip this for better ergonomics
  return ![app hide];
}

// Return true if operation was successful
extern "C" bool unhideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  return ![app unhide];
}