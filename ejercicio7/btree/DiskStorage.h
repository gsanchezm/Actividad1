#pragma once
#include <cstdint>
#include <string>

class DiskStorage {
public:
    // KISS: B-Tree de enteros con grado mínimo fijo
    static constexpr int t = 3;
    static constexpr int MaxKeys = 2*t - 1;       // 5
    static constexpr int MaxChildren = 2*t;       // 6

    struct Node {
        bool leaf = true;
        int32_t keyCount = 0;
        int32_t keys[MaxKeys]{};
        int64_t child[MaxChildren]{}; // IDs de hijos; -1 si no aplica
        int64_t id = -1;
    };

    explicit DiskStorage(const std::string& path);
    ~DiskStorage();

    DiskStorage(const DiskStorage&) = delete;
    DiskStorage& operator=(const DiskStorage&) = delete;

    // root
    int64_t readRootId();
    void writeRootId(int64_t id);

    // nodo
    Node readNode(int64_t id);
    void writeNode(const Node& n);
    int64_t allocateNode(bool leaf);

private:
    std::string path_;
    std::fstream* file_ = nullptr;

    static constexpr int64_t HEADER_SIZE = 16; // rootId + nextId
    int64_t readNextId();
    void writeNextId(int64_t v);

    int64_t nodeOffset(int64_t id) const;
};