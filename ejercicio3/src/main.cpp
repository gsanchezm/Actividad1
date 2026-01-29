#include <bits/stdc++.h>
#include "TennisTournament.h"
#include "XochimilcoPlanner.h"

using namespace std;

static void runExercise1() {
    int n;
    cout << "Numero de jugadores n: ";
    cin >> n;

    auto R = TennisTournament::generate(n);

    cout << "\n=== Tabla Jugador x Dia ===\n";
    cout << left << setw(5) << "Jug";
    for (int d = 1; d <= R.days; d++) cout << setw(5) << ("d" + to_string(d));
    cout << "\n";

    for (int i = 1; i <= n; i++) {
        cout << left << setw(5) << i;
        for (int d = 0; d < R.days; d++) {
            cout << setw(5) << (R.table[i][d] == 0 ? "-" : to_string(R.table[i][d]));
        }
        cout << "\n";
    }

    cout << "\n=== Partidos por dia ===\n";
    for (int d = 0; d < R.days; d++) {
        cout << "Dia " << (d + 1) << ": ";
        for (int k = 0; k < (int)R.matchesPerDay[d].size(); k++) {
            auto [a,b] = R.matchesPerDay[d][k];
            if (k) cout << " | ";
            cout << a << " vs " << b;
        }
        cout << "\n";
    }

    cout << "\nTecnica usada: Voraz (greedy) / constructiva. Complejidad: O(n^2).\n";
    if (n % 2 == 1) cout << "n impar -> se usa BYE y cada jugador descansa exactamente 1 dia.\n";
}

static void runExercise2() {
    int n;
    cout << "Numero de embarcaderos n: ";
    cin >> n;

    cout << "Ingresa matriz n x n (enteros). Usa -1 donde no aplica (i>=j).\n";
    vector<vector<long long>> cost(n + 1, vector<long long>(n + 1, XochimilcoPlanner::INF));

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            long long x; cin >> x;
            if (i < j && x >= 0) cost[i][j] = x;
        }
    }

    auto R = XochimilcoPlanner::computeAllPairsMinCost(cost);

    cout << "\n=== dist[i][j] (solo i<j) ===\n    ";
    for (int j = 1; j <= n; j++) cout << setw(8) << j;
    cout << "\n";

    for (int i = 1; i <= n; i++) {
        cout << setw(4) << i;
        for (int j = 1; j <= n; j++) {
            if (j <= i) cout << setw(8) << "-";
            else if (R.dist[i][j] >= XochimilcoPlanner::INF/2) cout << setw(8) << "INF";
            else cout << setw(8) << R.dist[i][j];
        }
        cout << "\n";
    }

    cout << "\nReconstruir ruta? (1=si, 0=no): ";
    int ok; cin >> ok;
    if (ok == 1) {
        int i, j;
        cout << "Dame i y j (i<j): ";
        cin >> i >> j;
        if (!(1 <= i && i < j && j <= n)) {
            cout << "Par invalido.\n";
        } else if (R.dist[i][j] >= XochimilcoPlanner::INF/2) {
            cout << "No hay ruta.\n";
        } else {
            auto path = XochimilcoPlanner::reconstructPath(i, j, R);
            cout << "Coste minimo: " << R.dist[i][j] << "\nRuta: ";
            for (int t = 0; t < (int)path.size(); t++) {
                if (t) cout << " -> ";
                cout << path[t];
            }
            cout << "\n";
        }
    }

    cout << "\nTecnica usada: Programacion Dinamica. Complejidad: O(n^3), memoria O(n^2).\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Selecciona ejercicio:\n";
    cout << "1) Torneo de tenis (Greedy)\n";
    cout << "2) Viajes en Xochimilco (DP)\n";
    cout << "Opcion: ";

    int op;
    cin >> op;

    if (op == 1) runExercise1();
    else if (op == 2) runExercise2();
    else cout << "Opcion invalida.\n";

    return 0;
}
