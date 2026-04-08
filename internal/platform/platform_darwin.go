//go:build darwin

package platform

import (
	"github.com/progrium/darwinkit/macos/appkit"
	"github.com/ujaandas/yumcha/internal/core"
)

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

func (darwinPlatform) Screens() ([]core.Screen, error) {
	akScreens := appkit.Screen_Screens()

	screens := make([]core.Screen, 0, len(akScreens))

	for i, v := range akScreens {
		// TODO: query DeviceDescription() with String_StringWithUTF8String("NSScreenNumber")
		frame := v.Frame()
		screens = append(screens, core.Screen{
			ID: i,
			Rect: core.Rect{
				X:      int(frame.Origin.X),
				Y:      int(frame.Origin.Y),
				Width:  int(frame.Size.Width),
				Height: int(frame.Size.Height),
			},
		})
	}
	return screens, nil
}

func (darwinPlatform) FocusedWindow() (core.Window, error)    { panic("Unimplemented!") }
func (darwinPlatform) Windows() ([]core.Window, error)        { panic("Unimplemented!") }
func (darwinPlatform) VisibleWindows() ([]core.Window, error) { panic("Unimplemented!") }

func (darwinPlatform) TransformWindow(core.Window) error { panic("Unimplemented!") }
func (darwinPlatform) ShowWindow(core.Window) error      { panic("Unimplemented!") }
func (darwinPlatform) HideWindow(core.Window) error      { panic("Unimplemented!") }
