//go:build !darwin

package platform

import "github.com/ujaandas/yumcha/internal/core"

type stubPlatform struct{}

func defaultPlatform() Platform {
	return stubPlatform{}
}

func (stubPlatform) Screens() ([]core.Screen, error)        { panic("Unimplemented!") }
func (stubPlatform) FocusedWindow() (core.Window, error)    { panic("Unimplemented!") }
func (stubPlatform) Windows() ([]core.Window, error)        { panic("Unimplemented!") }
func (stubPlatform) VisibleWindows() ([]core.Window, error) { panic("Unimplemented!") }

func (stubPlatform) TransformWindow(core.Window) error { panic("Unimplemented!") }
func (stubPlatform) ShowWindow(core.Window) error      { panic("Unimplemented!") }
func (stubPlatform) HideWindow(core.Window) error      { panic("Unimplemented!") }
