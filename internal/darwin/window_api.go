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
	Rect     foundation.Rect
	PID      int
	WindowID uint32
	Title    string
	Layer    int
	Visible  bool
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
		dictRef := C.CFArrayGetValueAtIndex(windows, C.CFIndex(i))
		if dictRef == nil {
			continue
		}
		// TODO: Implement smth like these
		// bounds := foundation.RectFromCFDictionary(dictRef, "Bounds")
		// pid := foundation.IntFromCFDictionary(dictRef, "OwnerPID")
		// windowID := foundation.UInt32FromCFDictionary(dictRef, "WindowNumber")
		// title := foundation.StringFromCFDictionary(dictRef, "Name")
		// layer := foundation.IntFromCFDictionary(dictRef, "Layer")
		// alpha := foundation.FloatFromCFDictionary(dictRef, "Alpha")
		// visible := alpha > 0.0

		// result = append(result, Window{
		// 	Rect:     bounds,
		// 	PID:      pid,
		// 	WindowID: windowID,
		// 	Title:    title,
		// 	Layer:    layer,
		// 	Visible:  visible,
		// })
	}

	return result, nil
}
func (WindowAPI) VisibleWindows() ([]foundation.Rect, error) { panic("Unimplemented!") }

func (WindowAPI) TransformWindow(foundation.Rect) error { panic("Unimplemented!") }
func (WindowAPI) ShowWindow(foundation.Rect) error      { panic("Unimplemented!") }
func (WindowAPI) HideWindow(foundation.Rect) error      { panic("Unimplemented!") }
