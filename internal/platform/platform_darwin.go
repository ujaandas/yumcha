//go:build darwin

package platform

/*
#cgo CFLAGS: -x objective-c
#cgo LDFLAGS: -framework ApplicationServices -framework CoreGraphics
*/
import "C"

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

// TODO: replace these with real AXUIElement / CoreGraphics calls.
func (darwinPlatform) ScreenSize() (Size, error) {
	return Size{Width: 1440, Height: 900}, nil
}

func (darwinPlatform) SetApplicationPos(pid, x, y int) error  { return nil }
func (darwinPlatform) SetApplicationSize(pid, w, h int) error { return nil }
func (darwinPlatform) VisiblePIDs() ([]int, error)            { return []int{}, nil }
func (darwinPlatform) HideAppByPID(pid int) (bool, error)     { return true, nil }
func (darwinPlatform) UnhideAppByPID(pid int) (bool, error)   { return true, nil }
