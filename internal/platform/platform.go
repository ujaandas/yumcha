//go:build darwin

package platform

import (
	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/progrium/darwinkit/macos/foundation"
)

type Platform interface {
	Screens() ([]appkit.Screen, error)
	FocusedWindow() (foundation.Rect, error)
	Windows() ([]foundation.Rect, error)
	VisibleWindows() ([]foundation.Rect, error)

	TransformWindow(foundation.Rect) error
	ShowWindow(foundation.Rect) error
	HideWindow(foundation.Rect) error
}

func Default() Platform {
	return defaultPlatform()
}
