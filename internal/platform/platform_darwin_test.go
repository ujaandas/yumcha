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
