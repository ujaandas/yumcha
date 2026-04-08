// platform_darwin_test.go
//go:build darwin
// +build darwin

package platform

import (
	"testing"
)

func TestDarwinScreens(t *testing.T) {
	p := darwinPlatform{}

	screens, err := p.Screens()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if len(screens) == 0 {
		t.Fatal("expected at least one screen from the OS")
	}

	for i, s := range screens {
		frame := s.Frame()
		if frame.Size.Width <= 0 || frame.Size.Height <= 0 {
			t.Errorf("screen %d has invalid size: %+v", i, frame)
		}
	}
}

func TestDarwinFocusedWindow(t *testing.T) {
	p := darwinPlatform{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got window at: %.0fx%.0f, at pos: %.0fx%.0f", window.Size.Width, window.Size.Height, window.Origin.X, window.Origin.Y)

	if window.Size.Width <= 0 || window.Size.Height <= 0 {
		t.Errorf("window dimensions broken: %.0fx%.0f", window.Size.Width, window.Size.Height)
	}

	if window.Origin.X < 0 || window.Origin.Y < 0 {
		t.Errorf("window position broken: %.0fx%.0f", window.Origin.X, window.Origin.Y)
	}

}
