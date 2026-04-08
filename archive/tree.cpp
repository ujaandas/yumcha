#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <variant>
#include <vector>

struct HSplitNode {};

struct VSplitNode {};

struct WindowNode {
  pid_t pid;
};

using NodeType = std::variant<HSplitNode, VSplitNode, WindowNode>;

struct Node {
  NodeType type;
  std::vector<std::unique_ptr<Node>> children;
};

// Use boolean return value if we need to stop the outer loop
typedef std::function<bool(Node *node)> tOpOnAllNodes;
typedef std::function<bool(Node *node)> tOpOnAllWindowNodes;

class Tree {
private:
  // Default node is a horizonatal split
  std::unique_ptr<Node> root;

  // Helper
  void insertNode(Node &root, Node nn) {
    root.children.push_back(std::make_unique<Node>(std::move(nn)));
  }

public:
  // Ctor
  Tree(NodeType initNodeType)
      : root{std::make_unique<Node>(Node{initNodeType, {}})} {}

  // Traverse with BFS
  void traverse(const tOpOnAllNodes allNodesFun = {},
                const tOpOnAllWindowNodes allWindowNodesFun = {}) {
    std::queue<Node *> q;
    q.push(root.get());

    while (!q.empty()) {
      // Get front node from queue
      Node *cur = q.front();
      q.pop();

      // Perform arbitrary node operation
      if (allNodesFun && allNodesFun(cur)) {
        return;
      };

      // Perform window-node-specific operation
      if (std::holds_alternative<WindowNode>(cur->type)) {
        printf("pid of %d is a window\n", std::get<WindowNode>(cur->type).pid);
        if (allWindowNodesFun && allWindowNodesFun(cur)) {
          return;
        }
      }

      // Add all children to queue
      for (auto &child : cur->children) {
        q.push(child.get());
      }
    }
  };

  // For adding new windows
  void addNode(Node &&nn) {
    traverse([&nn](Node *node) {
      if (node->children.size() < 2) {
        node->children.push_back(std::make_unique<Node>(std::move(nn)));
        // stop outer traverasl
        return true;
      }
      return false;
    });
  }
};