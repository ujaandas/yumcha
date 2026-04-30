package app

import (
	"testing"

	"github.com/ujaandas/yumcha/internal/api"
)

func TestFocusedWindowIsFocusedWindow(t *testing.T) {
	api := api.WindowAPI{}

	focused, err := api.FocusedWindow()
	if err != nil {
		t.Fatalf("could not get focused window: %v", err)
	}

	m := Manager{}
	m.Init()

	if m.focusedWindow != focused {
		t.Fatal("manager's focused window neq api focused window")
	}
}
