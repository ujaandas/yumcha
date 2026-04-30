#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <unistd.h>

typedef struct {
  CGRect rect;
  pid_t id;
  int layer;
  bool visible;
  int sharingState;
  float alpha;
  char *title;
} Window;

int window_info_for_pid(pid_t pid, Window *window);

int set_window_pid_pos(pid_t pid, int x, int y);

int set_window_pid_size(pid_t pid, int x, int y);
