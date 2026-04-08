//go:build darwin

package platform

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

func (darwinPlatform) FocusedWindow() (core.Window, error)    { panic("Unimplemented!") }
func (darwinPlatform) Windows() ([]core.Window, error)        { panic("Unimplemented!") }
func (darwinPlatform) VisibleWindows() ([]core.Window, error) { panic("Unimplemented!") }

func (darwinPlatform) TransformWindow(core.Window) error { panic("Unimplemented!") }
func (darwinPlatform) ShowWindow(core.Window) error      { panic("Unimplemented!") }
func (darwinPlatform) HideWindow(core.Window) error      { panic("Unimplemented!") }
