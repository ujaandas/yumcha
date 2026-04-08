//go:build darwin

package main

import (
	"context"
	"os"
	"os/signal"
	"syscall"

	"github.com/ujaandas/yumcha/internal/app"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

func main() {
	rootCmd := &cobra.Command{
		Use:          "yumcha",
		Short:        "A tiling window manager inspired by yabai.",
		SilenceUsage: true,
		RunE: func(cmd *cobra.Command, args []string) error {
			ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
			defer stop()

			cfg := app.Config{
				TargetPID: viper.GetInt("pid"),
				Debug:     viper.GetBool("debug"),
			}

			return app.Run(ctx, cfg)
		},
	}

	rootCmd.Flags().Int("pid", 17362, "target PID")
	rootCmd.Flags().Bool("debug", false, "enable debug logging")

	viper.SetEnvPrefix("YUMCHA")
	viper.AutomaticEnv()
	_ = viper.BindPFlag("pid", rootCmd.Flags().Lookup("pid"))
	_ = viper.BindPFlag("debug", rootCmd.Flags().Lookup("debug"))

	if err := rootCmd.Execute(); err != nil {
		os.Exit(1)
	}
}
