//go:build !darwin

package platform

type stubPlatform struct{}

func defaultPlatform() Platform {
	return stubPlatform{}
}

func (stubPlatform) ScreenSize() (Size, error) {
	return Size{Width: 1440, Height: 900}, nil
}

func (stubPlatform) SetApplicationPos(pid, x, y int) error  { return nil }
func (stubPlatform) SetApplicationSize(pid, w, h int) error { return nil }
func (stubPlatform) VisiblePIDs() ([]int, error)            { return []int{}, nil }
func (stubPlatform) HideAppByPID(pid int) (bool, error)     { return true, nil }
func (stubPlatform) UnhideAppByPID(pid int) (bool, error)   { return true, nil }
