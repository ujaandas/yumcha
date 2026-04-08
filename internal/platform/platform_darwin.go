//go:build darwin

package platform

// #cgo LDFLAGS: -framework Cocoa -framework ApplicationServices -framework CoreFoundation
// #include "ax.h"
import "C"

import (
	"fmt"

	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/progrium/darwinkit/macos/foundation"
)

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

func (darwinPlatform) Screens() ([]appkit.Screen, error) {
	akScreens := appkit.Screen_Screens()

	if len(akScreens) < 1 {
		return nil, fmt.Errorf("no screens found")
	}

	return akScreens, nil
}

func (darwinPlatform) FocusedWindow() (foundation.Rect, error) {
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

func (darwinPlatform) Windows() ([]foundation.Rect, error)        { panic("Unimplemented!") }
func (darwinPlatform) VisibleWindows() ([]foundation.Rect, error) { panic("Unimplemented!") }

func (darwinPlatform) TransformWindow(foundation.Rect) error { panic("Unimplemented!") }
func (darwinPlatform) ShowWindow(foundation.Rect) error      { panic("Unimplemented!") }
func (darwinPlatform) HideWindow(foundation.Rect) error      { panic("Unimplemented!") }
