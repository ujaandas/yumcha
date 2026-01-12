#include <Cocoa/Cocoa.h>

void hideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  [app hide];
}

void unhideAppByPID(pid_t pid) {
  NSRunningApplication *app =
      [NSRunningApplication runningApplicationWithProcessIdentifier:pid];

  [app unhide];
}