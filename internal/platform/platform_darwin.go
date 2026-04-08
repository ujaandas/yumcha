//go:build darwin

package platform

import (
	"log"

	"github.com/progrium/darwinkit/macos/appkit"
)

type darwinPlatform struct{}

func defaultPlatform() Platform {
	return darwinPlatform{}
}

// TODO: replace these with real AXUIElement / CoreGraphics calls.
func (darwinPlatform) ScreenSize() (Size, error) {
	size := appkit.Screen_MainScreen().Frame().Size
	log.Printf("got screen size: %dx%d", int(size.Width), int(size.Height))
	return Size{int(size.Width), int(size.Height)}, nil
}

func (darwinPlatform) SetApplicationPos(pid, x, y int) error  { return nil }
func (darwinPlatform) SetApplicationSize(pid, w, h int) error { return nil }
func (darwinPlatform) VisiblePIDs() ([]int, error)            { return []int{}, nil }
func (darwinPlatform) HideAppByPID(pid int) (bool, error)     { return true, nil }
func (darwinPlatform) UnhideAppByPID(pid int) (bool, error)   { return true, nil }
