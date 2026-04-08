#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>

int focused_window_rect_for_pid(pid_t pid, int *x, int *y, int *w, int *h);