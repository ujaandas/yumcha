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

	return nil
}
