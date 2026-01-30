#pragma once
#include "DiskStorage.h"
#include <string>
#include <iostream>
#include <optional>

class BTree {
public:
    explicit BTree(const std::string& filePath);

    void insert(int key);
    void erase(int key);
    bool contains(int key);

    void printAscending(std::ostream& os);
    void printDescending(std::ostream& os);

    bool isValidBTree();

private:
    DiskStorage storage_;
    int64_t rootId_ = -1;

    // Helpers
    int findKeyIndex(const DiskStorage::Node& n, int key); // primer idx con keys[idx] >= key

    void splitChild(DiskStorage::Node& parent, int childIndex, DiskStorage::Node& child);
    void insertNonFull(int64_t nodeId, int key);

    void traverseAsc(int64_t nodeId, std::ostream& os);
    void traverseDesc(int64_t nodeId, std::ostream& os);

    // delete (CLRS-style)
    void removeFromNode(int64_t nodeId, int key);
    void removeFromLeaf(DiskStorage::Node& node, int idx);
    void removeFromNonLeaf(int64_t nodeId, DiskStorage::Node& node, int idx);

    int getPredecessor(int64_t nodeId);
    int getSuccessor(int64_t nodeId);

    void fillChild(int64_t parentId, DiskStorage::Node& parent, int idx);
    void borrowFromPrev(DiskStorage::Node& parent, int idx);
    void borrowFromNext(DiskStorage::Node& parent, int idx);
    void mergeChildren(int64_t parentId, DiskStorage::Node& parent, int idx);

    // verify
    bool verifyNode(int64_t nodeId, int depth, int& leafDepth,
                    long long lo, long long hi, bool isRoot);
};