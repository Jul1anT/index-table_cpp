#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>

// Employee record structure
struct Employee {
    int id_edad;
    std::string nombre;
    double salario;
    std::string cargo;
};

// B+ Tree order (maximum number of children per node)
constexpr int ORDER = 4;
// Maximum keys in a node = ORDER - 1 = 3
constexpr int MAX_KEYS = ORDER - 1;
// Minimum keys in a non-root node = ceil(ORDER/2) - 1 = 1
constexpr int MIN_KEYS = (ORDER + 1) / 2 - 1;

// Forward declaration
template <typename K, typename V>
class BPlusTree;

// Base node class for B+ Tree
template <typename K, typename V>
class Node {
public:
    std::vector<K> keys;
    bool isLeaf;
    Node* parent;

    Node(bool leaf = false) : isLeaf(leaf), parent(nullptr) {}
    virtual ~Node() = default;
};

// Internal node class - stores keys and pointers to children
template <typename K, typename V>
class InternalNode : public Node<K, V> {
public:
    std::vector<Node<K, V>*> children;

    InternalNode() : Node<K, V>(false) {}

    ~InternalNode() override {
        // Clean up children
        for (auto* child : children) {
            delete child;
        }
    }
};

// Leaf node class - stores keys and values, linked to other leaves
template <typename K, typename V>
class LeafNode : public Node<K, V> {
public:
    std::vector<V> values;
    LeafNode* next;  // Pointer to next leaf for range queries

    LeafNode() : Node<K, V>(true), next(nullptr) {}
};

// B+ Tree implementation
template <typename K, typename V>
class BPlusTree {
private:
    Node<K, V>* root;

    // Find the appropriate leaf node for a key
    LeafNode<K, V>* findLeaf(const K& key) const {
        if (!root) return nullptr;

        Node<K, V>* current = root;
        while (!current->isLeaf) {
            auto* internal = static_cast<InternalNode<K, V>*>(current);
            
            // Find the child to follow
            size_t i = 0;
            while (i < internal->keys.size() && key >= internal->keys[i]) {
                i++;
            }
            current = internal->children[i];
        }
        return static_cast<LeafNode<K, V>*>(current);
    }

    // Insert a key-value pair into a leaf node, may cause split
    void insertIntoLeaf(LeafNode<K, V>* leaf, const K& key, const V& value) {
        // Find position to insert
        auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        size_t pos = it - leaf->keys.begin();

        // Check if key already exists (update value)
        if (it != leaf->keys.end() && *it == key) {
            leaf->values[pos] = value;
            return;
        }

        // Insert key and value
        leaf->keys.insert(it, key);
        leaf->values.insert(leaf->values.begin() + pos, value);

        // Check if split is needed
        if (leaf->keys.size() > MAX_KEYS) {
            splitLeaf(leaf);
        }
    }

    // Split a leaf node
    void splitLeaf(LeafNode<K, V>* leaf) {
        auto* newLeaf = new LeafNode<K, V>();
        
        // Split point
        size_t mid = (leaf->keys.size() + 1) / 2;

        // Move second half to new leaf
        newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        newLeaf->values.assign(leaf->values.begin() + mid, leaf->values.end());

        // Truncate original leaf
        leaf->keys.resize(mid);
        leaf->values.resize(mid);

        // Update linked list pointers
        newLeaf->next = leaf->next;
        leaf->next = newLeaf;

        // Insert into parent
        K newKey = newLeaf->keys[0];  // Smallest key in new leaf
        insertIntoParent(leaf, newKey, newLeaf);
    }

    // Split an internal node
    void splitInternal(InternalNode<K, V>* node) {
        auto* newNode = new InternalNode<K, V>();
        
        // Split point
        size_t mid = node->keys.size() / 2;
        K midKey = node->keys[mid];

        // Move second half of keys to new node (excluding mid key which goes up)
        newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        
        // Move corresponding children
        newNode->children.assign(node->children.begin() + mid + 1, node->children.end());
        
        // Update parent pointers for moved children
        for (auto* child : newNode->children) {
            child->parent = newNode;
        }

        // Truncate original node
        node->keys.resize(mid);
        node->children.resize(mid + 1);

        // Insert mid key into parent
        insertIntoParent(node, midKey, newNode);
    }

    // Insert a new key into the parent after a split
    void insertIntoParent(Node<K, V>* leftChild, const K& key, Node<K, V>* rightChild) {
        if (leftChild == root) {
            // Create new root
            auto* newRoot = new InternalNode<K, V>();
            newRoot->keys.push_back(key);
            newRoot->children.push_back(leftChild);
            newRoot->children.push_back(rightChild);
            leftChild->parent = newRoot;
            rightChild->parent = newRoot;
            root = newRoot;
            return;
        }

        auto* parent = static_cast<InternalNode<K, V>*>(leftChild->parent);
        
        // Find position to insert
        auto it = std::lower_bound(parent->keys.begin(), parent->keys.end(), key);
        size_t pos = it - parent->keys.begin();

        // Insert key
        parent->keys.insert(it, key);
        
        // Insert child pointer after the left child
        parent->children.insert(parent->children.begin() + pos + 1, rightChild);
        rightChild->parent = parent;

        // Check if split is needed
        if (parent->keys.size() > MAX_KEYS) {
            splitInternal(parent);
        }
    }

public:
    BPlusTree() : root(nullptr) {}

    ~BPlusTree() {
        delete root;  // Recursive deletion through node destructors
    }

    // Insert a key-value pair
    void insert(const K& key, const V& value) {
        if (!root) {
            // Create first leaf
            auto* leaf = new LeafNode<K, V>();
            leaf->keys.push_back(key);
            leaf->values.push_back(value);
            root = leaf;
            return;
        }

        LeafNode<K, V>* leaf = findLeaf(key);
        insertIntoLeaf(leaf, key, value);
    }

    // Search for a key and return the associated value
    std::optional<V> search(const K& key) const {
        LeafNode<K, V>* leaf = findLeaf(key);
        if (!leaf) return std::nullopt;

        // Binary search in the leaf
        auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        if (it != leaf->keys.end() && *it == key) {
            size_t pos = it - leaf->keys.begin();
            return leaf->values[pos];
        }
        return std::nullopt;
    }

    // Check if tree is empty
    bool empty() const {
        return root == nullptr;
    }

    // Print tree structure (for debugging)
    void printTree() const {
        if (!root) {
            std::cout << "Empty tree\n";
            return;
        }
        printNode(root, 0);
    }

private:
    // Helper to print a node and its children
    void printNode(Node<K, V>* node, int level) const {
        std::string indent(level * 2, ' ');
        std::cout << indent << "[";
        for (size_t i = 0; i < node->keys.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << node->keys[i];
        }
        std::cout << "]";
        
        if (node->isLeaf) {
            std::cout << " (leaf)\n";
        } else {
            std::cout << " (internal)\n";
            auto* internal = static_cast<InternalNode<K, V>*>(node);
            for (auto* child : internal->children) {
                printNode(child, level + 1);
            }
        }
    }
};

#endif // BPLUSTREE_H
