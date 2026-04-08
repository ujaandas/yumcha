package app

import (
	"context"
	"fmt"
	"time"

	"github.com/ujaandas/yumcha/internal/platform"
	"github.com/ujaandas/yumcha/internal/tree"
)

type Config struct {
	TargetPID int
	Debug     bool
}

func Run(ctx context.Context, cfg Config) error {
	if cfg.TargetPID == 0 {
		return fmt.Errorf("target pid is required")
	}

	p := platform.Default()

	screen, err := p.ScreenSize()
	if err != nil {
		return err
	}

	t := tree.New(tree.VSplitNode{})
	t.AddNode(tree.Node{Window: tree.WindowNode{PID: cfg.TargetPID}})

	if err := p.SetApplicationPos(cfg.TargetPID, 0, 0); err != nil {
		return err
	}
	if err := p.SetApplicationSize(cfg.TargetPID, screen.Width, screen.Height); err != nil {
		return err
	}

	ticker := time.NewTicker(16 * time.Millisecond)
	defer ticker.Stop()

	lastVisibleCount := 0

	for {
		select {
		case <-ctx.Done():
			return nil

		case <-ticker.C:
			pids, err := p.VisiblePIDs()
			if err != nil {
				return err
			}

			visible := map[int]struct{}{}
			for _, pid := range pids {
				if pid == cfg.TargetPID {
					continue
				}
				hidden, err := p.HideAppByPID(pid)
				if err != nil {
					return err
				}
				if hidden {
					visible[pid] = struct{}{}
				}
			}

			if len(visible) != lastVisibleCount {
				if cfg.Debug {
					fmt.Printf("window count changed! now=%d old=%d\n", len(visible), lastVisibleCount)
				}
				lastVisibleCount = len(visible)
			}
		}
	}
}
