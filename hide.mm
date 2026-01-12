#include <Cocoa/Cocoa.h>

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