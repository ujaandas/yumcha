#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <unistd.h>

typedef struct {
  pid_t pid;
  size_t id;
  CGRect rect;
  int layer;
  bool visible;
  int sharingState;
  float alpha;
  char title[256];
} Window;

int all_visible_windows(Window *windows, size_t capacity, size_t *count);

int window_info_for_pid(pid_t pid, Window *window);

int set_window_pid_pos(pid_t pid, int x, int y);

int set_window_pid_size(pid_t pid, int x, int y);
