package core

type WindowID int

type Rect struct {
	X, Y          int
	Width, Height int
}

type Window struct {
	ID     WindowID
	Rect   Rect
	Hidden bool
}

type Screen struct {
	ID   int
	Rect Rect
}
