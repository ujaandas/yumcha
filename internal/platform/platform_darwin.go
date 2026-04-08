//go:build darwin

package platform

// #cgo LDFLAGS: -framework Cocoa -framework ApplicationServices -framework CoreFoundation
// #include <ApplicationServices/ApplicationServices.h>
// #include <CoreFoundation/CoreFoundation.h>
//
// static int focused_window_rect_for_pid(pid_t pid, int *x, int *y, int *w, int *h) {
//     AXUIElementRef app = AXUIElementCreateApplication(pid);
//     if (!app) return 1;
//
//     CFTypeRef winValue = NULL;
//     AXError err = AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute, &winValue);
//     CFRelease(app);
//     if (err != kAXErrorSuccess || !winValue) return 2;
//
//     AXUIElementRef win = (AXUIElementRef)winValue;
//
//     CFTypeRef posValue = NULL;
//     CFTypeRef sizeValue = NULL;
//
//     CGPoint pos;
//     CGSize size;
//
//     err = AXUIElementCopyAttributeValue(win, kAXPositionAttribute, &posValue);
//     if (err != kAXErrorSuccess || !posValue) { CFRelease(win); return 3; }
//
//     err = AXUIElementCopyAttributeValue(win, kAXSizeAttribute, &sizeValue);
//     if (err != kAXErrorSuccess || !sizeValue) { CFRelease(posValue); CFRelease(win); return 4; }
//
//     if (!AXValueGetValue((AXValueRef)posValue, kAXValueCGPointType, &pos)) {
//         CFRelease(sizeValue); CFRelease(posValue); CFRelease(win); return 5;
//     }
//     if (!AXValueGetValue((AXValueRef)sizeValue, kAXValueCGSizeType, &size)) {
//         CFRelease(sizeValue); CFRelease(posValue); CFRelease(win); return 6;
//     }
//
//     CFRelease(sizeValue);
//     CFRelease(posValue);
//     CFRelease(win);
//
//     *x = (int)pos.x;
//     *y = (int)pos.y;
//     *w = (int)size.width;
//     *h = (int)size.height;
//     return 0;
// }
import "C"

import (
	"fmt"

	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/ujaandas/yumcha/internal/core"
)

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

func (darwinPlatform) Screens() ([]core.Screen, error) {
	akScreens := appkit.Screen_Screens()

	if len(akScreens) < 1 {
		return nil, fmt.Errorf("no screens found")
	}

	screens := make([]frameLike, len(akScreens))

	for i := range akScreens {
		screens[i] = akScreens[i]
	}

	return convertScreens(screens), nil
}

func (darwinPlatform) FocusedWindow() (core.Window, error) {
	app := appkit.Workspace_SharedWorkspace().FrontmostApplication()
	if app.Ptr() == nil {
		return core.Window{}, fmt.Errorf("no frontmost application")
	}

	pid := int(app.ProcessIdentifier())

	var x, y, w, h C.int
	rc := C.focused_window_rect_for_pid(C.pid_t(pid), &x, &y, &w, &h)
	if rc != 0 {
		return core.Window{}, fmt.Errorf("accessiblity cooked, rc=%d", int(rc))
	}
	return core.Window{ID: 0, Hidden: false, Rect: core.Rect{X: int(x), Y: int(y), Width: int(w), Height: int(h)}}, nil
}

func (darwinPlatform) Windows() ([]core.Window, error)        { panic("Unimplemented!") }
func (darwinPlatform) VisibleWindows() ([]core.Window, error) { panic("Unimplemented!") }

func (darwinPlatform) TransformWindow(core.Window) error { panic("Unimplemented!") }
func (darwinPlatform) ShowWindow(core.Window) error      { panic("Unimplemented!") }
func (darwinPlatform) HideWindow(core.Window) error      { panic("Unimplemented!") }
