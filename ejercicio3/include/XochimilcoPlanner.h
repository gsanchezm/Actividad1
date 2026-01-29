#pragma once
#include <vector>

class XochimilcoPlanner {
public:
    struct Result {
        int n{};
        std::vector<std::vector<long long>> dist; // dist[i][j]
        std::vector<std::vector<int>> next;       // para reconstruir ruta
    };

    // cost[i][j] válido solo si i<j; si no aplica usa INF grande
    static Result computeAllPairsMinCost(const std::vector<std::vector<long long>>& cost);

    // Reconstruye camino i->j usando Result.next
    static std::vector<int> reconstructPath(int i, int j, const Result& R);

    static constexpr long long INF = (1LL << 60);
};
