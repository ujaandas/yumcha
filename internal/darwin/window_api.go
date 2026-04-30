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

func (WindowAPI) FocusedWindow() (Window, error) {
	app := appkit.Workspace_SharedWorkspace().FrontmostApplication()
	if app.Ptr() == nil {
		return Window{}, fmt.Errorf("no frontmost application")
	}

	pid := int(app.ProcessIdentifier())

	var name [256]C.char
	var id C.int
	var layer C.int
	var bounds C.CGRect
	var sharingState C.int
	var alpha C.float

	status := C.window_info_for_pid(C.pid_t(pid), &id, &layer, &bounds, &sharingState, &alpha, &name[0])
	if status != 0 {
		return Window{}, fmt.Errorf("accessiblity cooked, status=%d", int(status))
	}

	title := C.GoString(&name[0])
	return Window{
			PID:      int(pid),
			WindowID: int(id),
			Title:    string(title),
			Layer:    int(layer),
			Rect: foundation.Rect{
				Origin: foundation.Point{
					X: float64(bounds.origin.x),
					Y: float64(bounds.origin.y)},
				Size: foundation.Size{
					Width:  float64(bounds.size.width),
					Height: float64(bounds.size.height),
				}},
			Visible:      true,
			SharingState: int(sharingState),
			Alpha:        float32(alpha)},
		nil
}
