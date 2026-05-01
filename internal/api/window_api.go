//go:build darwin

package api

// #cgo LDFLAGS: -framework Cocoa -framework ApplicationServices -framework CoreFoundation
// #include "window_driver.h"
import "C"

import (
	"fmt"
	"unsafe"

	"github.com/progrium/darwinkit/kernel"
	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/progrium/darwinkit/macos/foundation"
)

// Should we just use the C struct directly? DRY or WET?
type Window struct {
	Rect         foundation.Rect
	PID          kernel.Pid
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

func cWindowToWindow(cWindow C.Window) Window {
	return Window{
		PID:      kernel.Pid(cWindow.pid),
		WindowID: int(cWindow.id),
		Title:    C.GoString(&cWindow.title[0]),
		Layer:    int(cWindow.layer),
		Rect: foundation.Rect{
			Origin: foundation.Point{
				X: float64(cWindow.rect.origin.x),
				Y: float64(cWindow.rect.origin.y)},
			Size: foundation.Size{
				Width:  float64(cWindow.rect.size.width),
				Height: float64(cWindow.rect.size.height),
			}},
		Visible:      true,
		SharingState: int(cWindow.sharingState),
		Alpha:        float32(cWindow.alpha)}
}

func windowFromPid(pid int) (Window, error) {
	var window C.Window

	status := C.window_info_for_pid(C.pid_t(pid), &window)
	if status != 0 {
		return Window{}, fmt.Errorf("accessiblity cooked, status=%d", int(status))
	}

	return cWindowToWindow(window), nil
}

func windowFromApp(app appkit.RunningApplication) (Window, error) {
	pid := int(app.ProcessIdentifier())
	return windowFromPid(pid)
}

func (WindowAPI) FocusedWindow() (Window, error) {
	app := appkit.Workspace_SharedWorkspace().FrontmostApplication()
	if app.Ptr() == nil {
		return Window{}, fmt.Errorf("no frontmost application")
	}
	return windowFromApp(app)
}

func (WindowAPI) AllWindows() ([]Window, error) {
	cWindows := make([]C.Window, 1024)
	var count C.size_t

	status := C.all_visible_windows(
		(*C.Window)(unsafe.Pointer(&cWindows[0])),
		C.size_t(len(cWindows)),
		&count,
	)

	if status != 0 {
		return nil, fmt.Errorf("failed to get all windows")
	}

	cWindows = cWindows[:count]
	windows := make([]Window, count)

	for i, cWindow := range cWindows {
		windows[i] = cWindowToWindow(cWindow)
	}

	return windows, nil
}

func (WindowAPI) MoveWindow(pid kernel.Pid, x, y int) error {
	status := C.set_window_pid_pos(C.pid_t(pid), C.int(x), C.int(y))
	if status != 0 {
		return fmt.Errorf("failed to move window, status=%d", int(status))
	}

	return nil
}

func (WindowAPI) ResizeWindow(pid kernel.Pid, x, y int) error {
	status := C.set_window_pid_size(C.pid_t(pid), C.int(x), C.int(y))
	if status != 0 {
		return fmt.Errorf("failed to move window, status=%d", int(status))
	}

	return nil
}

func isAppRunning(pid kernel.Pid) (appkit.RunningApplication, bool) {
	app := appkit.RunningApplication_RunningApplicationWithProcessIdentifier(kernel.Pid(pid))
	if app.Ptr() == nil {
		return app, false
	}
	return app, true
}

func (WindowAPI) Hide(pid kernel.Pid) error {
	if app, ok := isAppRunning(pid); ok {
		if ok2 := app.Hide(); !ok2 {
			return nil
		}
		return fmt.Errorf("hide op failed")
	}
	return fmt.Errorf("app is not running")
}

func (WindowAPI) Unhide(pid kernel.Pid) error {
	if app, ok := isAppRunning(pid); ok {
		if ok2 := app.Unhide(); !ok2 {
			return nil
		}
		return fmt.Errorf("unhide op failed")
	}
	return fmt.Errorf("app is not running")
}
