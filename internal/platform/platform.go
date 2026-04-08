package platform

import "github.com/ujaandas/yumcha/internal/core"

type Platform interface {
	Screens() ([]core.Screen, error)
	FocusedWindow() (core.Window, error)
	Windows() ([]core.Window, error)
	VisibleWindows() ([]core.Window, error)

	TransformWindow(core.Window) error
	ShowWindow(core.Window) error
	HideWindow(core.Window) error
}

func Default() Platform {
	return defaultPlatform()
}
