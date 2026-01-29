#include <bits/stdc++.h>
#include "RobotPathFinder.h"
using namespace std;

static void printPath(const RobotPathFinder::Path& path) {
    for (int i = 0; i < (int)path.size(); i++) {
        auto [r,c] = path[i];
        cout << "(" << r << "," << c << ")";
        if (i + 1 < (int)path.size()) cout << " -> ";
    }
    cout << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    int n, m;
    cout << "n (filas): " << flush;
    cin >> n;
    cout << "m (columnas): " << flush;
    cin >> m;

    RobotPathFinder solver(n, m);

    cout << "\n[a] Existe camino? (BFS): "
         << (solver.existsPathBFS() ? "SI" : "NO") << "\n";

    // Mapa de caminos (DP) — recomendado aunque no listes todos los caminos
    solver.printPathMap();

    // Si quieres, además listar todos (puede ser MUCHOS)
    cout << "Listar todos los caminos? (1=si, 0=no): " << flush;
    int opt; cin >> opt;

    if (opt == 1) {
        auto paths = solver.allPathsBacktracking();
        cout << "\nTotal caminos: " << paths.size() << "\n\n";
        for (int i = 0; i < (int)paths.size(); i++) {
            cout << "Camino " << (i+1) << ": ";
            printPath(paths[i]);
        }

        // Mostrar overlay del primer camino como ejemplo
        if (!paths.empty()) solver.printSinglePathOverlay(paths[0]);
    }

    return 0;
}
