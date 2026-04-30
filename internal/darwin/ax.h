#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <unistd.h>

int window_info_for_pid(pid_t pid, int *windowID, int *layer,
                        CGRect *windowBounds, int *sharingState, float *alpha,
                        char *name);