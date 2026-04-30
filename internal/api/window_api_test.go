//go:build darwin
// +build darwin

package api

import (
	"testing"

	"github.com/progrium/darwinkit/kernel"
	"github.com/progrium/darwinkit/macos/appkit"
)

func TestScreens(t *testing.T) {
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

func TestFocusedWindow(t *testing.T) {
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

func TestMovesFocusedWindow(t *testing.T) {
	p := WindowAPI{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got %s window at: %.0fx%.0f, at pos: %.0fx%.0f", window.Title, window.Rect.Size.Width, window.Rect.Size.Height, window.Rect.Origin.X, window.Rect.Origin.Y)

	initialPos := window.Rect.Origin

	if err := p.MoveWindow(window.PID, 50, 50); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if err := p.MoveWindow(window.PID, int(initialPos.X), int(initialPos.Y)); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
}

func TestResizesFocusedWindow(t *testing.T) {
	p := WindowAPI{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got %s window at: %.0fx%.0f, at pos: %.0fx%.0f", window.Title, window.Rect.Size.Width, window.Rect.Size.Height, window.Rect.Origin.X, window.Rect.Origin.Y)

	initialSize := window.Rect.Size

	if err := p.ResizeWindow(window.PID, 50, 50); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if err := p.ResizeWindow(window.PID, int(initialSize.Width), int(initialSize.Height)); err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
}

func TestHidesUnhidesWindow(t *testing.T) {
	p := WindowAPI{}

	window, err := p.FocusedWindow()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	t.Logf("got %s window at: %.0fx%.0f, at pos: %.0fx%.0f", window.Title, window.Rect.Size.Width, window.Rect.Size.Height, window.Rect.Origin.X, window.Rect.Origin.Y)

	if err := p.Hide(window.PID); err != nil {
		t.Fatalf("failed to hide app, %v", err)
	}

	if err := p.Unhide(window.PID); err != nil {
		t.Fatalf("failed to unhide app, %v", err)
	}

	app := appkit.RunningApplication_RunningApplicationWithProcessIdentifier(kernel.Pid(window.PID))
	if app.IsHidden() {
		t.Fatal("expected app to be visible, but it isn't")
	}
}
