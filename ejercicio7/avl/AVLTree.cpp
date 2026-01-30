#include "AVLTree.h"
#include <algorithm>
#include <limits>

AVLTree::~AVLTree() { destroy(root_); }

int AVLTree::h(Node* n) { return n ? n->height : 0; }

int AVLTree::balance(Node* n) { return n ? h(n->left) - h(n->right) : 0; }

void AVLTree::updateHeight(Node* n) {
    if (!n) return;
    n->height = 1 + std::max(h(n->left), h(n->right));
}

AVLTree::Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLTree::Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVLTree::Node* AVLTree::insertRec(Node* node, int key) {
    if (!node) return new Node(key);

    if (key < node->key) node->left = insertRec(node->left, key);
    else if (key > node->key) node->right = insertRec(node->right, key);
    else return node; // no duplicados (KISS)

    updateHeight(node);

    int b = balance(node);

    // 4 casos
    if (b > 1 && key < node->left->key) return rotateRight(node);               // LL
    if (b < -1 && key > node->right->key) return rotateLeft(node);              // RR
    if (b > 1 && key > node->left->key) {                                       // LR
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (b < -1 && key < node->right->key) {                                     // RL
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

AVLTree::Node* AVLTree::minNode(Node* node) {
    Node* cur = node;
    while (cur && cur->left) cur = cur->left;
    return cur;
}

AVLTree::Node* AVLTree::eraseRec(Node* node, int key) {
    if (!node) return nullptr;

    if (key < node->key) node->left = eraseRec(node->left, key);
    else if (key > node->key) node->right = eraseRec(node->right, key);
    else {
        // 0 o 1 hijo
        if (!node->left || !node->right) {
            Node* temp = node->left ? node->left : node->right;
            if (!temp) { // 0 hijos
                delete node;
                return nullptr;
            } else {     // 1 hijo
                Node* old = node;
                node = temp;
                delete old;
            }
        } else {
            // 2 hijos: reemplazar por sucesor
            Node* temp = minNode(node->right);
            node->key = temp->key;
            node->right = eraseRec(node->right, temp->key);
        }
    }

    updateHeight(node);
    int b = balance(node);

    // Rebalance
    if (b > 1 && balance(node->left) >= 0) return rotateRight(node);             // LL
    if (b > 1 && balance(node->left) < 0) {                                      // LR
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (b < -1 && balance(node->right) <= 0) return rotateLeft(node);            // RR
    if (b < -1 && balance(node->right) > 0) {                                    // RL
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

bool AVLTree::containsRec(Node* node, int key) {
    while (node) {
        if (key < node->key) node = node->left;
        else if (key > node->key) node = node->right;
        else return true;
    }
    return false;
}

void AVLTree::inorder(Node* node, std::ostream& os) {
    if (!node) return;
    inorder(node->left, os);
    os << node->key << " ";
    inorder(node->right, os);
}

void AVLTree::reverseInorder(Node* node, std::ostream& os) {
    if (!node) return;
    reverseInorder(node->right, os);
    os << node->key << " ";
    reverseInorder(node->left, os);
}

void AVLTree::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

bool AVLTree::validate(Node* node, long long lo, long long hi, int& outHeight) {
    if (!node) { outHeight = 0; return true; }

    if (!(lo < node->key && node->key < hi)) return false;

    int hl=0, hr=0;
    if (!validate(node->left, lo, node->key, hl)) return false;
    if (!validate(node->right, node->key, hi, hr)) return false;

    int expected = 1 + std::max(hl, hr);
    if (node->height != expected) return false;

    int b = hl - hr;
    if (b < -1 || b > 1) return false;

    outHeight = expected;
    return true;
}

// API pública
void AVLTree::insert(int key) { root_ = insertRec(root_, key); }
void AVLTree::erase(int key)  { root_ = eraseRec(root_, key);  }
bool AVLTree::contains(int key) const { return containsRec(root_, key); }

void AVLTree::printAscending(std::ostream& os) const { inorder(root_, os); os << "\n"; }
void AVLTree::printDescending(std::ostream& os) const { reverseInorder(root_, os); os << "\n"; }

std::optional<int> AVLTree::heightOf(int key) const {
    Node* cur = root_;
    while (cur) {
        if (key < cur->key) cur = cur->left;
        else if (key > cur->key) cur = cur->right;
        else return cur->height;
    }
    return std::nullopt;
}

std::optional<int> AVLTree::depthOf(int key) const {
    Node* cur = root_;
    int depth = 0;
    while (cur) {
        if (key < cur->key) { cur = cur->left; depth++; }
        else if (key > cur->key) { cur = cur->right; depth++; }
        else return depth;
    }
    return std::nullopt;
}

std::optional<int> AVLTree::levelOf(int key) const {
    auto d = depthOf(key);
    if (!d) return std::nullopt;
    return *d + 1;
}

bool AVLTree::isValidAVL() const {
    int h = 0;
    return validate(root_, std::numeric_limits<long long>::min(),
                    std::numeric_limits<long long>::max(), h);
}