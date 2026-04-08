package platform

import (
	"testing"

	"github.com/progrium/darwinkit/macos/foundation"
)

type mockScreen struct {
	frame foundation.Rect
}

func (m mockScreen) Frame() foundation.Rect {
	return m.frame
}

func TestConvertScreens(t *testing.T) {
	input := []frameLike{
		mockScreen{
			frame: foundation.Rect{
				Origin: foundation.Point{X: 10, Y: 20},
				Size:   foundation.Size{Width: 800, Height: 600},
			},
		},
	}

	got := convertScreens(input)

	if len(got) != 1 {
		t.Fatalf("expected 1 screen, got %d", len(got))
	}

	s := got[0]

	if s.ID != 0 {
		t.Errorf("expected ID 0, got %d", s.ID)
	}

	if s.Rect.X != 10 || s.Rect.Y != 20 {
		t.Errorf("wrong origin: %+v", s.Rect)
	}

	if s.Rect.Width != 800 || s.Rect.Height != 600 {
		t.Errorf("wrong size: %+v", s.Rect)
	}
}
