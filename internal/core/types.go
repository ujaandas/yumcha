package core

type Rect struct {
	X, Y          int
	Width, Height int
}

type Window struct {
	ID     int
	Rect   Rect
	Hidden bool
}

type Screen struct {
	ID   int
	Rect Rect
}
