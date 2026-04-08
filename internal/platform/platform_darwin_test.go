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
		if s.ID != i {
			t.Errorf("screen ID mismatch: got %d, want %d", s.ID, i)
		}
		if s.Rect.Width <= 0 || s.Rect.Height <= 0 {
			t.Errorf("screen %d has invalid size: %+v", i, s.Rect)
		}
	}
}

func TestDarwinFocusedWindow(t *testing.T) {
	p := darwinPlatform{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got window at: %dx%d, at pos: %dx%d", window.Rect.Width, window.Rect.Height, window.Rect.X, window.Rect.Y)

	if window.Rect.Width <= 0 || window.Rect.Height <= 0 {
		t.Errorf("window dimensions broken: %dx%d", window.Rect.Width, window.Rect.Height)
	}

	if window.Rect.X < 0 || window.Rect.Y < 0 {
		t.Errorf("window position broken: %dx%d", window.Rect.X, window.Rect.Y)
	}

}
