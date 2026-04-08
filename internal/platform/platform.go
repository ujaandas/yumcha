//go:build darwin

package platform

import (
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

type Platform interface {
	Screens() ([]appkit.Screen, error)
	FocusedWindow() (foundation.Rect, error)
	Windows() ([]Window, error)
	VisibleWindows() ([]foundation.Rect, error)

	TransformWindow(foundation.Rect) error
	ShowWindow(foundation.Rect) error
	HideWindow(foundation.Rect) error
}

func Default() Platform {
	return defaultPlatform()
}
