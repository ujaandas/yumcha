#include <cassert>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

enum class NodeType { HSplit, VSplit, Window };

struct Node {
  NodeType type;
  std::vector<std::unique_ptr<Node>> children;
};

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

  // For adding new windows
  void addNode(Node nn) {
    // Queue for BFS
    std::queue<Node *> q;
    q.push(root.get());

    while (!q.empty()) {
      Node *cur = q.front();
      q.pop();

      // If < 2 children, add node
      if (cur->children.size() < 2) {
        cur->children.push_back(std::make_unique<Node>(std::move(nn)));
        return;
      }

      // Add all children to queue
      for (auto &child : cur->children) {
        q.push(child.get());
      }
    }
  }
};