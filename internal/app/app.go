//go:build darwin

package app

import (
	"context"
	"fmt"
)

type Config struct {
	TargetPID int
	Debug     bool
}

func Run(ctx context.Context, cfg Config) error {
	if cfg.TargetPID == 0 {
		return fmt.Errorf("target pid is required")
	}

	manager := Manager{}
	manager.Init()

	window := manager.focusedWindow
	fmt.Printf("\nfocused window: %v\n", window)

	windows := manager.windows
	for i, window := range windows {
		fmt.Printf("\nwindow %d: %v\n", i, window)
	}

	return nil
}
