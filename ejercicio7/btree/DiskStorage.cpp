#include "DiskStorage.h"
#include <fstream>
#include <stdexcept>
#include <cstring>

static void write_i64(std::fstream& f, int64_t v) { f.write(reinterpret_cast<const char*>(&v), sizeof(v)); }
static void read_i64(std::fstream& f, int64_t& v) { f.read(reinterpret_cast<char*>(&v), sizeof(v)); }

DiskStorage::DiskStorage(const std::string& path) : path_(path) {
    auto* fs = new std::fstream(path_, std::ios::in | std::ios::out | std::ios::binary);
    if (!fs->is_open()) {
        // crear archivo
        delete fs;
        fs = new std::fstream(path_, std::ios::out | std::ios::binary);
        if (!fs->is_open()) throw std::runtime_error("No se pudo crear archivo");
        // header: rootId=-1, nextId=0
        int64_t rootId = -1, nextId = 0;
        write_i64(*fs, rootId);
        write_i64(*fs, nextId);
        fs->close();
        delete fs;

        fs = new std::fstream(path_, std::ios::in | std::ios::out | std::ios::binary);
        if (!fs->is_open()) throw std::runtime_error("No se pudo abrir archivo");
    }
    file_ = fs;
}

DiskStorage::~DiskStorage() {
    if (file_) {
        file_->flush();
        file_->close();
        delete file_;
    }
}

int64_t DiskStorage::nodeOffset(int64_t id) const {
    // header + id * sizeof(NodeOnDisk)
    // NodeOnDisk: leaf(1 byte) + padding + keyCount(4) + keys + children + id(8)
    // Para KISS: serializamos manualmente en orden fijo => offset fijo.
    // Calculamos tamaño:
    const int64_t nodeSize =
        1 + 3 + // leaf + padding
        4 +     // keyCount
        (int64_t)MaxKeys * 4 +
        (int64_t)MaxChildren * 8 +
        8;      // id
    return HEADER_SIZE + id * nodeSize;
}

int64_t DiskStorage::readRootId() {
    file_->seekg(0);
    int64_t rootId;
    read_i64(*file_, rootId);
    return rootId;
}

void DiskStorage::writeRootId(int64_t id) {
    file_->seekp(0);
    write_i64(*file_, id);
    file_->flush();
}

int64_t DiskStorage::readNextId() {
    file_->seekg(8);
    int64_t v;
    read_i64(*file_, v);
    return v;
}

void DiskStorage::writeNextId(int64_t v) {
    file_->seekp(8);
    write_i64(*file_, v);
    file_->flush();
}

DiskStorage::Node DiskStorage::readNode(int64_t id) {
    Node n;
    n.id = id;

    file_->seekg(nodeOffset(id));
    char leaf;
    file_->read(&leaf, 1);
    n.leaf = (leaf != 0);

    // padding 3
    char pad[3];
    file_->read(pad, 3);

    file_->read(reinterpret_cast<char*>(&n.keyCount), 4);
    file_->read(reinterpret_cast<char*>(n.keys), MaxKeys * 4);
    file_->read(reinterpret_cast<char*>(n.child), MaxChildren * 8);
    file_->read(reinterpret_cast<char*>(&n.id), 8);
    return n;
}

void DiskStorage::writeNode(const Node& n) {
    file_->seekp(nodeOffset(n.id));
    char leaf = n.leaf ? 1 : 0;
    file_->write(&leaf, 1);

    char pad[3] = {0,0,0};
    file_->write(pad, 3);

    file_->write(reinterpret_cast<const char*>(&n.keyCount), 4);
    file_->write(reinterpret_cast<const char*>(n.keys), MaxKeys * 4);
    file_->write(reinterpret_cast<const char*>(n.child), MaxChildren * 8);
    file_->write(reinterpret_cast<const char*>(&n.id), 8);

    file_->flush();
}

int64_t DiskStorage::allocateNode(bool leaf) {
    int64_t id = readNextId();
    writeNextId(id + 1);

    Node n;
    n.leaf = leaf;
    n.keyCount = 0;
    n.id = id;
    for (int i=0;i<MaxChildren;i++) n.child[i] = -1;

    writeNode(n);
    return id;
}