//go:build darwin

package app

import (
	"context"
	"fmt"
	"log"

	"github.com/ujaandas/yumcha/internal/darwin"
)

type Config struct {
	TargetPID int
	Debug     bool
}

func Run(ctx context.Context, cfg Config) error {
	if cfg.TargetPID == 0 {
		return fmt.Errorf("target pid is required")
	}

	api := darwin.WindowAPI{}

	if _, err := api.Screens(); err != nil {
		log.Fatalf("error fetching focused window: %v", err)
	}

	return nil
}
