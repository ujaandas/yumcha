// go:build darwin

package app

import "github.com/ujaandas/yumcha/internal/api"

type Manager struct {
	api           api.WindowAPI
	windows       []api.Window
	focusedWindow api.Window
}
