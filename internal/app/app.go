//go:build darwin

package app

import (
	"context"
	"fmt"
	"log"

	"github.com/ujaandas/yumcha/internal/driver"
)

type Config struct {
	TargetPID int
	Debug     bool
}

func Run(ctx context.Context, cfg Config) error {
	if cfg.TargetPID == 0 {
		return fmt.Errorf("target pid is required")
	}

	api := driver.WindowAPI{}
	v, err := api.FocusedWindow()
	if err != nil {
		log.Fatalf("error fetching focused window: %v", err)
	}
	log.Println(v)

	return nil
}
