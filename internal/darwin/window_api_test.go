//go:build darwin
// +build darwin

package darwin

import (
	"testing"
)

func TestDarwinScreens(t *testing.T) {
	p := WindowAPI{}

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
	p := WindowAPI{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got %s window at: %.0fx%.0f, at pos: %.0fx%.0f", window.Title, window.Rect.Size.Width, window.Rect.Size.Height, window.Rect.Origin.X, window.Rect.Origin.Y)

	if window.Rect.Size.Width <= 0 || window.Rect.Size.Height <= 0 {
		t.Errorf("window dimensions broken: %.0fx%.0f", window.Rect.Size.Width, window.Rect.Size.Height)
	}

	if window.Rect.Origin.X < 0 || window.Rect.Origin.Y < 0 {
		t.Errorf("window position broken: %.0fx%.0f", window.Rect.Origin.X, window.Rect.Origin.Y)
	}

}
