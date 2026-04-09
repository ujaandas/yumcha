//go:build darwin

package darwin

// #cgo LDFLAGS: -framework Cocoa -framework ApplicationServices -framework CoreFoundation
// #include "ax.h"
import "C"

import (
	"fmt"

	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/progrium/darwinkit/macos/foundation"
)

type Window struct {
	Rect         foundation.Rect
	PID          int
	WindowID     int
	Title        string
	Layer        int
	Visible      bool
	SharingState int
	Alpha        float32
}

type WindowAPI struct{}

func (WindowAPI) Screens() ([]appkit.Screen, error) {
	akScreens := appkit.Screen_Screens()

	if len(akScreens) < 1 {
		return nil, fmt.Errorf("no screens found")
	}

	return akScreens, nil
}

func (WindowAPI) FocusedWindow() (foundation.Rect, error) {
	app := appkit.Workspace_SharedWorkspace().FrontmostApplication()
	if app.Ptr() == nil {
		return foundation.Rect{}, fmt.Errorf("no frontmost application")
	}

	pid := int(app.ProcessIdentifier())

	var x, y, w, h C.int
	rc := C.focused_window_rect_for_pid(C.pid_t(pid), &x, &y, &w, &h)
	if rc != 0 {
		return foundation.Rect{}, fmt.Errorf("accessiblity cooked, rc=%d", int(rc))
	}
	return foundation.Rect{
		Origin: foundation.Point{X: float64(x), Y: float64(y)},
		Size:   foundation.Size{Width: float64(w), Height: float64(h)},
	}, nil
}

func (WindowAPI) Windows() ([]Window, error) {
	var windows C.CFArrayRef
	var numWindows C.int
	rc := C.get_window_list(&windows, &numWindows)
	defer C.CFRelease(C.CFTypeRef(windows))
	if rc != 0 {
		return []Window{}, fmt.Errorf("could not fetch window list, rc=%d", int(rc))
	}

	count := int(numWindows)
	result := make([]Window, 0, count)

	for i := 0; i < count; i++ {
		var pid C.int
		var name [256]C.char
		var windowNumber C.int
		var windowLayer C.int
		var windowBounds C.CGRect
		var windowSharingState C.int
		var windowAlpha C.float

		rc := C.get_window_dict_vals(&windows, C.int(i), &pid, &name[0], &windowNumber, &windowLayer, &windowBounds, &windowSharingState, &windowAlpha)
		if rc != 0 {
			return nil, fmt.Errorf("failed to get window data, rc=%d", int(rc))
		}

		title := C.GoString(&name[0])

		result = append(result, Window{
			Rect: foundation.Rect{
				Origin: foundation.Point{
					X: float64(windowBounds.origin.x),
					Y: float64(windowBounds.origin.y)},
				Size: foundation.Size{
					Width:  float64(windowBounds.size.width),
					Height: float64(windowBounds.size.height),
				}},
			PID:          int(pid),
			WindowID:     int(windowNumber),
			Title:        title,
			Layer:        int(windowLayer),
			SharingState: int(windowSharingState),
			Alpha:        float32(windowAlpha),
		})
	}

	return result, nil
}

func (WindowAPI) VisibleWindows() ([]foundation.Rect, error) { panic("Unimplemented!") }

func (WindowAPI) TransformWindow(foundation.Rect) error { panic("Unimplemented!") }
func (WindowAPI) ShowWindow(foundation.Rect) error      { panic("Unimplemented!") }
func (WindowAPI) HideWindow(foundation.Rect) error      { panic("Unimplemented!") }
