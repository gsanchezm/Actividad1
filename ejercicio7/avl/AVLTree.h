#pragma once
#include <functional>
#include <optional>
#include <iostream>

class AVLTree {
public:
    AVLTree() = default;
    ~AVLTree();

    // Operaciones requeridas
    void insert(int key);
    void erase(int key);
    bool contains(int key) const;

    void printAscending(std::ostream& os) const;
    void printDescending(std::ostream& os) const;

    // Altura/Profundidad/Nivel del nodo con clave "key"
    // Devuelven std::nullopt si no existe.
    std::optional<int> heightOf(int key) const;      // altura del nodo (altura de subárbol)
    std::optional<int> depthOf(int key) const;       // profundidad = #aristas desde raíz
    std::optional<int> levelOf(int key) const;       // nivel = profundidad + 1

    bool isValidAVL() const;

private:
    struct Node {
        int key;
        int height; // altura del nodo (subárbol)
        Node* left;
        Node* right;
        explicit Node(int k): key(k), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root_ = nullptr;

    // Helpers (KISS)
    static int h(Node* n);
    static int balance(Node* n);
    static void updateHeight(Node* n);

    static Node* rotateRight(Node* y);
    static Node* rotateLeft(Node* x);

    static Node* insertRec(Node* node, int key);
    static Node* eraseRec(Node* node, int key);
    static Node* minNode(Node* node);

    static bool containsRec(Node* node, int key);

    static void inorder(Node* node, std::ostream& os);
    static void reverseInorder(Node* node, std::ostream& os);

    static void destroy(Node* node);

    // validación AVL: BST + balance + alturas consistentes
    static bool validate(Node* node, long long lo, long long hi, int& outHeight);
};