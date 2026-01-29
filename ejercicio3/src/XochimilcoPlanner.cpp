#include "XochimilcoPlanner.h"

XochimilcoPlanner::Result
XochimilcoPlanner::computeAllPairsMinCost(const std::vector<std::vector<long long>>& cost) {
    int n = (int)cost.size() - 1; // 1-index
    Result R{};
    R.n = n;
    R.dist.assign(n + 1, std::vector<long long>(n + 1, INF));
    R.next.assign(n + 1, std::vector<int>(n + 1, -1));

    // init
    for (int i = 1; i <= n; i++) {
        R.dist[i][i] = 0;
        R.next[i][i] = i;
        for (int j = i + 1; j <= n; j++) {
            R.dist[i][j] = cost[i][j];
            if (cost[i][j] < INF / 2) R.next[i][j] = j;
        }
    }

    // DP: similar a Floyd pero restringido i<k<j (DAG hacia adelante)
    for (int k = 1; k <= n; k++) {
        for (int i = 1; i < k; i++) {
            if (R.dist[i][k] >= INF / 2) continue;
            for (int j = k + 1; j <= n; j++) {
                if (R.dist[k][j] >= INF / 2) continue;
                long long cand = R.dist[i][k] + R.dist[k][j];
                if (cand < R.dist[i][j]) {
                    R.dist[i][j] = cand;
                    R.next[i][j] = R.next[i][k];
                }
            }
        }
    }

    return R;
}

std::vector<int> XochimilcoPlanner::reconstructPath(int i, int j, const Result& R) {
    if (R.next[i][j] == -1) return {};
    std::vector<int> path;
    int cur = i;
    path.push_back(cur);
    while (cur != j) {
        cur = R.next[cur][j];
        if (cur == -1) return {};
        path.push_back(cur);
        if ((int)path.size() > 100000) return {};
    }
    return path;
}
