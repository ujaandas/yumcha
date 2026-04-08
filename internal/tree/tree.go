package tree

type VSplitNode struct{}

type WindowNode struct {
	PID int
}

type Node struct {
	Window WindowNode
	Split  VSplitNode
}

type Tree struct {
	root any
}

func New(root any) *Tree {
	return &Tree{root: root}
}

func (t *Tree) AddNode(n Node) {
	_ = n
}
