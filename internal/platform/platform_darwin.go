//go:build darwin

package platform

import "github.com/ujaandas/yumcha/internal/core"

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

func (darwinPlatform) Screens() ([]core.Screen, error)        { panic("Unimplemented!") }
func (darwinPlatform) FocusedWindow() (core.Window, error)    { panic("Unimplemented!") }
func (darwinPlatform) Windows() ([]core.Window, error)        { panic("Unimplemented!") }
func (darwinPlatform) VisibleWindows() ([]core.Window, error) { panic("Unimplemented!") }

func (darwinPlatform) TransformWindow(core.Window) error { panic("Unimplemented!") }
func (darwinPlatform) ShowWindow(core.Window) error      { panic("Unimplemented!") }
func (darwinPlatform) HideWindow(core.Window) error      { panic("Unimplemented!") }
