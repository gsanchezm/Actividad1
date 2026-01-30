#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <fstream>

#include "../avl/AVLTree.h"
#include "../btree/BTree.h"

using clock_t_ = std::chrono::steady_clock;

static std::vector<int> make_unique_data(int n, int seed=123) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(1, n*20);

    std::unordered_set<int> used;
    used.reserve(n*2);

    std::vector<int> v;
    v.reserve(n);

    while ((int)v.size() < n) {
        int x = dist(rng);
        if (used.insert(x).second) v.push_back(x);
    }
    return v;
}

template <class Func>
static double time_ms(Func&& f) {
    auto t0 = clock_t_::now();
    f();
    auto t1 = clock_t_::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(&std::cout);

    // tamaños de muestra
    std::vector<int> sizes = {1000, 5000, 10000, 20000, 50000};

    // CSV
    std::ofstream out("bench.csv");
    out << "n,tree,insert_ms,search_ms\n";

    for (int n : sizes) {
        auto data = make_unique_data(n);
        int target = data[n/2]; // mismo elemento para ambas estructuras

        // AVL en RAM
        AVLTree avl;
        double ins_avl = time_ms([&]{
            for (int x : data) avl.insert(x);
        });

        double search_avl = time_ms([&]{
            volatile bool found = false;
            for (int rep=0; rep<2000; rep++) found ^= avl.contains(target);
            (void)found;
        });

        out << n << ",AVL," << ins_avl << "," << search_avl << "\n";

        // B-Tree en disco (usa archivo distinto por tamaño para evitar “residuo”)
        std::string file = "btree_" + std::to_string(n) + ".bin";
        BTree bt(file);

        double ins_bt = time_ms([&]{
            for (int x : data) bt.insert(x);
        });

        double search_bt = time_ms([&]{
            volatile bool found = false;
            for (int rep=0; rep<2000; rep++) found ^= bt.contains(target);
            (void)found;
        });

        out << n << ",BTreeDisk," << ins_bt << "," << search_bt << "\n";

        std::cout << "n=" << n << " listo\n";
    }

    std::cout << "✅ Generado bench.csv\n";
}