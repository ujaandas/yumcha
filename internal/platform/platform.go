package platform

type Size struct {
	Width  int
	Height int
}

type Platform interface {
	ScreenSize() (Size, error)
	SetApplicationPos(pid, x, y int) error
	SetApplicationSize(pid, width, height int) error
	VisiblePIDs() ([]int, error)
	HideAppByPID(pid int) (bool, error)
	UnhideAppByPID(pid int) (bool, error)
}

func Default() Platform {
	return defaultPlatform()
}
