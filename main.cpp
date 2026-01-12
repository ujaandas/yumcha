#include <unistd.h>

extern "C" void hideAppByPID(pid_t pid);
extern "C" void unhideAppByPID(pid_t pid);

int main() {
  hideAppByPID(30142);
  return 0;
}