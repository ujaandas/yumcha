
#include <unistd.h>

void hideAppByPID(pid_t pid);
void unhideAppByPID(pid_t pid);

int main() {
  hideAppByPID(31513);
  return 0;
}