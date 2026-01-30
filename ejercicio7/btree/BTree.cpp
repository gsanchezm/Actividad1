#include "BTree.h"
#include <algorithm>
#include <limits>

BTree::BTree(const std::string& filePath) : storage_(filePath) {
    rootId_ = storage_.readRootId();
    if (rootId_ == -1) {
        rootId_ = storage_.allocateNode(true);
        storage_.writeRootId(rootId_);
    }
}

int BTree::findKeyIndex(const DiskStorage::Node& n, int key) {
    int idx = 0;
    while (idx < n.keyCount && n.keys[idx] < key) idx++;
    return idx;
}

bool BTree::contains(int key) {
    int64_t curId = rootId_;
    while (true) {
        auto node = storage_.readNode(curId);
        int i = findKeyIndex(node, key);
        if (i < node.keyCount && node.keys[i] == key) return true;
        if (node.leaf) return false;
        curId = node.child[i];
    }
}

void BTree::splitChild(DiskStorage::Node& parent, int childIndex, DiskStorage::Node& child) {
    // child está lleno (2t-1)
    // crear nuevo nodo z
    int64_t zId = storage_.allocateNode(child.leaf);
    auto z = storage_.readNode(zId);
    z.leaf = child.leaf;

    // z toma t-1 llaves del child (las mayores)
    z.keyCount = DiskStorage::t - 1;
    for (int j = 0; j < DiskStorage::t - 1; j++) {
        z.keys[j] = child.keys[j + DiskStorage::t];
    }

    // si no es hoja, también children
    if (!child.leaf) {
        for (int j = 0; j < DiskStorage::t; j++) {
            z.child[j] = child.child[j + DiskStorage::t];
        }
        for (int j = DiskStorage::t; j < DiskStorage::MaxChildren; j++) z.child[j] = -1;
    }

    // reducir child
    int midKey = child.keys[DiskStorage::t - 1];
    child.keyCount = DiskStorage::t - 1;

    // mover hijos del parent para insertar nuevo child
    for (int j = parent.keyCount; j >= childIndex + 1; j--) {
        parent.child[j + 1] = parent.child[j];
    }
    parent.child[childIndex + 1] = zId;

    // mover llaves del parent para insertar midKey
    for (int j = parent.keyCount - 1; j >= childIndex; j--) {
        parent.keys[j + 1] = parent.keys[j];
    }
    parent.keys[childIndex] = midKey;
    parent.keyCount++;

    // escribir cambios
    storage_.writeNode(child);
    storage_.writeNode(z);
    storage_.writeNode(parent);
}

void BTree::insert(int key) {
    if (contains(key)) return; // KISS: sin duplicados

    auto root = storage_.readNode(rootId_);
    if (root.keyCount == DiskStorage::MaxKeys) {
        // nuevo root
        int64_t sId = storage_.allocateNode(false);
        auto s = storage_.readNode(sId);
        s.leaf = false;
        s.keyCount = 0;
        for (int i=0;i<DiskStorage::MaxChildren;i++) s.child[i] = -1;
        s.child[0] = rootId_;
        storage_.writeNode(s);

        // split
        splitChild(s, 0, root);

        rootId_ = sId;
        storage_.writeRootId(rootId_);
        insertNonFull(rootId_, key);
    } else {
        insertNonFull(rootId_, key);
    }
}

void BTree::insertNonFull(int64_t nodeId, int key) {
    auto node = storage_.readNode(nodeId);

    int i = node.keyCount - 1;
    if (node.leaf) {
        while (i >= 0 && node.keys[i] > key) {
            node.keys[i + 1] = node.keys[i];
            i--;
        }
        node.keys[i + 1] = key;
        node.keyCount++;
        storage_.writeNode(node);
        return;
    }

    // bajar al hijo correcto
    while (i >= 0 && node.keys[i] > key) i--;
    i++;

    auto child = storage_.readNode(node.child[i]);
    if (child.keyCount == DiskStorage::MaxKeys) {
        splitChild(node, i, child);
        node = storage_.readNode(nodeId); // recargar parent tras split
        if (node.keys[i] < key) i++;
    }
    insertNonFull(node.child[i], key);
}

// ---------- TRAVERSAL ----------
void BTree::printAscending(std::ostream& os) { traverseAsc(rootId_, os); os << "\n"; }
void BTree::printDescending(std::ostream& os) { traverseDesc(rootId_, os); os << "\n"; }

void BTree::traverseAsc(int64_t nodeId, std::ostream& os) {
    auto node = storage_.readNode(nodeId);
    for (int i = 0; i < node.keyCount; i++) {
        if (!node.leaf) traverseAsc(node.child[i], os);
        os << node.keys[i] << " ";
    }
    if (!node.leaf) traverseAsc(node.child[node.keyCount], os);
}

void BTree::traverseDesc(int64_t nodeId, std::ostream& os) {
    auto node = storage_.readNode(nodeId);
    if (!node.leaf) traverseDesc(node.child[node.keyCount], os);
    for (int i = node.keyCount - 1; i >= 0; i--) {
        os << node.keys[i] << " ";
        if (!node.leaf) traverseDesc(node.child[i], os);
    }
}

// ---------- DELETE (CLRS) ----------
void BTree::erase(int key) {
    removeFromNode(rootId_, key);

    // si root se queda vacío y no es hoja, bajar nivel
    auto root = storage_.readNode(rootId_);
    if (root.keyCount == 0 && !root.leaf) {
        rootId_ = root.child[0];
        storage_.writeRootId(rootId_);
    }
}

void BTree::removeFromNode(int64_t nodeId, int key) {
    auto node = storage_.readNode(nodeId);
    int idx = findKeyIndex(node, key);

    if (idx < node.keyCount && node.keys[idx] == key) {
        if (node.leaf) {
            removeFromLeaf(node, idx);
            storage_.writeNode(node);
        } else {
            removeFromNonLeaf(nodeId, node, idx);
        }
        return;
    }

    if (node.leaf) return; // no encontrado

    // clave no está en este nodo => bajar al hijo
    bool lastChild = (idx == node.keyCount);
    auto child = storage_.readNode(node.child[idx]);

    // asegurar que child tenga al menos t llaves
    if (child.keyCount < DiskStorage::t) {
        fillChild(nodeId, node, idx);
        node = storage_.readNode(nodeId); // parent pudo cambiar
    }

    // tras fill, el hijo puede haber cambiado (merge)
    int64_t nextChildId = node.child[idx];
    if (lastChild && idx > node.keyCount) nextChildId = node.child[idx - 1];

    removeFromNode(nextChildId, key);
}

void BTree::removeFromLeaf(DiskStorage::Node& node, int idx) {
    for (int i = idx + 1; i < node.keyCount; i++) node.keys[i - 1] = node.keys[i];
    node.keyCount--;
}

int BTree::getPredecessor(int64_t nodeId) {
    auto cur = storage_.readNode(nodeId);
    while (!cur.leaf) cur = storage_.readNode(cur.child[cur.keyCount]);
    return cur.keys[cur.keyCount - 1];
}

int BTree::getSuccessor(int64_t nodeId) {
    auto cur = storage_.readNode(nodeId);
    while (!cur.leaf) cur = storage_.readNode(cur.child[0]);
    return cur.keys[0];
}

void BTree::removeFromNonLeaf(int64_t nodeId, DiskStorage::Node& node, int idx) {
    int k = node.keys[idx];

    auto leftChild = storage_.readNode(node.child[idx]);
    auto rightChild = storage_.readNode(node.child[idx + 1]);

    if (leftChild.keyCount >= DiskStorage::t) {
        int pred = getPredecessor(leftChild.id);
        node.keys[idx] = pred;
        storage_.writeNode(node);
        removeFromNode(leftChild.id, pred);
    } else if (rightChild.keyCount >= DiskStorage::t) {
        int succ = getSuccessor(rightChild.id);
        node.keys[idx] = succ;
        storage_.writeNode(node);
        removeFromNode(rightChild.id, succ);
    } else {
        // merge left + k + right
        mergeChildren(nodeId, node, idx);
        // ahora k está en el hijo fusionado
        auto merged = storage_.readNode(node.child[idx]);
        removeFromNode(merged.id, k);
    }
}

void BTree::fillChild(int64_t parentId, DiskStorage::Node& parent, int idx) {
    // si hay hermano izq con >= t => borrowFromPrev
    if (idx != 0) {
        auto prev = storage_.readNode(parent.child[idx - 1]);
        if (prev.keyCount >= DiskStorage::t) {
            borrowFromPrev(parent, idx);
            return;
        }
    }
    // si hay hermano der con >= t => borrowFromNext
    if (idx != parent.keyCount) {
        auto next = storage_.readNode(parent.child[idx + 1]);
        if (next.keyCount >= DiskStorage::t) {
            borrowFromNext(parent, idx);
            return;
        }
    }
    // si no, merge
    if (idx != parent.keyCount) mergeChildren(parentId, parent, idx);
    else mergeChildren(parentId, parent, idx - 1);
}

void BTree::borrowFromPrev(DiskStorage::Node& parent, int idx) {
    auto child = storage_.readNode(parent.child[idx]);
    auto sibling = storage_.readNode(parent.child[idx - 1]);

    // mover child una posición a la derecha
    for (int i = child.keyCount - 1; i >= 0; i--) child.keys[i + 1] = child.keys[i];

    if (!child.leaf) {
        for (int i = child.keyCount; i >= 0; i--) child.child[i + 1] = child.child[i];
    }

    // traer llave del parent al child
    child.keys[0] = parent.keys[idx - 1];

    // subir llave del sibling al parent
    parent.keys[idx - 1] = sibling.keys[sibling.keyCount - 1];

    if (!child.leaf) {
        child.child[0] = sibling.child[sibling.keyCount];
    }

    child.keyCount++;
    sibling.keyCount--;

    storage_.writeNode(sibling);
    storage_.writeNode(child);
    storage_.writeNode(parent);
}

void BTree::borrowFromNext(DiskStorage::Node& parent, int idx) {
    auto child = storage_.readNode(parent.child[idx]);
    auto sibling = storage_.readNode(parent.child[idx + 1]);

    // traer llave del parent al final de child
    child.keys[child.keyCount] = parent.keys[idx];

    // subir primera llave del sibling al parent
    parent.keys[idx] = sibling.keys[0];

    if (!child.leaf) {
        child.child[child.keyCount + 1] = sibling.child[0];
    }

    // correr sibling a la izquierda
    for (int i = 1; i < sibling.keyCount; i++) sibling.keys[i - 1] = sibling.keys[i];
    if (!sibling.leaf) {
        for (int i = 1; i <= sibling.keyCount; i++) sibling.child[i - 1] = sibling.child[i];
    }

    child.keyCount++;
    sibling.keyCount--;

    storage_.writeNode(sibling);
    storage_.writeNode(child);
    storage_.writeNode(parent);
}

void BTree::mergeChildren(int64_t parentId, DiskStorage::Node& parent, int idx) {
    auto child = storage_.readNode(parent.child[idx]);
    auto sibling = storage_.readNode(parent.child[idx + 1]);

    // bajar llave del parent al medio
    child.keys[DiskStorage::t - 1] = parent.keys[idx];

    // copiar llaves del sibling
    for (int i = 0; i < sibling.keyCount; i++) {
        child.keys[i + DiskStorage::t] = sibling.keys[i];
    }

    // copiar hijos si aplica
    if (!child.leaf) {
        for (int i = 0; i <= sibling.keyCount; i++) {
            child.child[i + DiskStorage::t] = sibling.child[i];
        }
    }

    child.keyCount += sibling.keyCount + 1;

    // quitar llave del parent y child pointer
    for (int i = idx + 1; i < parent.keyCount; i++) parent.keys[i - 1] = parent.keys[i];
    for (int i = idx + 2; i <= parent.keyCount; i++) parent.child[i - 1] = parent.child[i];

    parent.keyCount--;

    // write back
    storage_.writeNode(child);
    storage_.writeNode(parent);

    // sibling queda “huérfano” (YAGNI: no reciclamos IDs)
    (void)parentId;
}

bool BTree::isValidBTree() {
    int leafDepth = -1;
    return verifyNode(rootId_, 0, leafDepth,
                      std::numeric_limits<long long>::min(),
                      std::numeric_limits<long long>::max(), true);
}

bool BTree::verifyNode(int64_t nodeId, int depth, int& leafDepth,
                       long long lo, long long hi, bool isRoot) {
    auto n = storage_.readNode(nodeId);

    // keyCount bounds
    if (isRoot) {
        if (n.keyCount < 1 || n.keyCount > DiskStorage::MaxKeys) return false;
    } else {
        if (n.keyCount < DiskStorage::t - 1 || n.keyCount > DiskStorage::MaxKeys) return false;
    }

    // keys strictly increasing & within bounds
    for (int i=0;i<n.keyCount;i++) {
        if (!(lo < n.keys[i] && n.keys[i] < hi)) return false;
        if (i>0 && n.keys[i-1] >= n.keys[i]) return false;
    }

    if (n.leaf) {
        if (leafDepth == -1) leafDepth = depth;
        return leafDepth == depth;
    }

    // internal: children count must be keyCount+1 and valid
    for (int i=0;i<=n.keyCount;i++) {
        if (n.child[i] < 0) return false;
    }

    // ranges by separator keys
    // child0 in (lo, key0), child1 in (key0, key1), ..., last in (keyLast, hi)
    // verify recursively
    // child 0
    if (!verifyNode(n.child[0], depth+1, leafDepth, lo, n.keys[0], false)) return false;
    for (int i=1;i<n.keyCount;i++) {
        if (!verifyNode(n.child[i], depth+1, leafDepth, n.keys[i-1], n.keys[i], false)) return false;
    }
    if (!verifyNode(n.child[n.keyCount], depth+1, leafDepth, n.keys[n.keyCount-1], hi, false)) return false;

    return true;
}