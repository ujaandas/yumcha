#include <CoreGraphics/CoreGraphics.h>
#include <cstdio>
#include <unistd.h>
#include <unordered_set>

extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

int main() {
  std::unordered_set<pid_t> seen;

  CFArrayRef windowList =
      CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);

  int numWindows = CFArrayGetCount(windowList);

  for (int i = 0; i < numWindows; i++) {
    const CFDictionaryRef windowInfo =
        (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);

    const CFNumberRef windowPid =
        (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerPID);
    int pid = 0;
    CFNumberGetValue(windowPid, kCFNumberIntType, &pid);

    if (seen.count(pid) || pid == 33410) {
      continue;
    }

    seen.insert(pid);

    const CFStringRef windowName =
        (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
    char name[256];
    CFStringGetCString(windowName, name, sizeof(name), kCFStringEncodingUTF8);

    printf("%s-> PID %d\n", name, pid);
    hideAppByPID(pid);
  }

  CFRelease(windowList);
  return 0;
}