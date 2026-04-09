#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <unistd.h>

int focused_window_rect_for_pid(pid_t pid, int *x, int *y, int *w, int *h);

int get_window_list(CFArrayRef *windows, int *windowLen);

int get_window_dict_vals(CFArrayRef *windows, int i, int *pid, char *name,
                         int *windowNumber, int *windowLayer,
                         CGRect *windowBounds, int *windowSharingState,
                         float *windowAlpha);