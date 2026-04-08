package platform

import (
	"github.com/progrium/darwinkit/macos/foundation"
	"github.com/ujaandas/yumcha/internal/core"
)

type frameLike interface {
	Frame() foundation.Rect
}

func convertScreens(screens []frameLike) []core.Screen {
	out := make([]core.Screen, 0, len(screens))

	for i, v := range screens {
		frame := v.Frame()
		out = append(out, core.Screen{
			// TODO: query DeviceDescription() with String_StringWithUTF8String("NSScreenNumber")
			ID: i,
			Rect: core.Rect{
				X:      int(frame.Origin.X),
				Y:      int(frame.Origin.Y),
				Width:  int(frame.Size.Width),
				Height: int(frame.Size.Height),
			},
		})
	}

	return out
}
