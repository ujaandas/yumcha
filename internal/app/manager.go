//go:build darwin

package app

import (
	"fmt"

	"github.com/ujaandas/yumcha/internal/api"
)

type Manager struct {
	api           api.WindowAPI
	windows       []api.Window
	focusedWindow api.Window
}

func (m *Manager) Init() error {
	focusedWindow, err := m.api.FocusedWindow()
	if err != nil {
		return fmt.Errorf("failed to fetch focused window")
	}
	m.focusedWindow = focusedWindow

	return nil
}
