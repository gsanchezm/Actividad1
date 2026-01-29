#include <bits/stdc++.h>
#include "TennisTournament.h"
#include "XochimilcoPlanner.h"

using namespace std;

static int readInt(const string& prompt) {
    while (true) {
        cout << prompt << flush;
        int x;
        if (cin >> x) return x;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Intenta de nuevo.\n";
    }
}

static long long readLL(const string& prompt) {
    while (true) {
        cout << prompt << flush;
        long long x;
        if (cin >> x) return x;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Intenta de nuevo.\n";
    }
}

// -------------------- EJERCICIO 1 --------------------
static void runExercise1() {
    int n = readInt("Numero de jugadores n: ");

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

// -------------------- EJERCICIO 2 --------------------
static vector<vector<long long>> readCostMatrixGuided(int n) {
    vector<vector<long long>> cost(n + 1, vector<long long>(n + 1, XochimilcoPlanner::INF));

    cout << "\nModo guiado: ingresa costo de i -> j (i < j). Usa -1 si no hay viaje directo.\n";
    for (int i = 1; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            long long x = readLL("Costo " + to_string(i) + " -> " + to_string(j) + " (-1 si no hay): ");
            if (x >= 0) cost[i][j] = x;
        }
    }
    return cost;
}

static vector<vector<long long>> readCostMatrixPasted(int n) {
    vector<vector<long long>> cost(n + 1, vector<long long>(n + 1, XochimilcoPlanner::INF));

    cout << "\nModo pegado: pega una matriz n x n.\n"
         << "Usa -1 donde no aplica (i>=j) o donde no exista ruta.\n\n";

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            long long x; cin >> x;
            if (i < j && x >= 0) cost[i][j] = x;
        }
    }
    return cost;
}

static void runExercise2() {
    int n = readInt("Numero de embarcaderos n: ");

    cout << "\nComo quieres ingresar los datos?\n"
         << "1) Guiado (pregunta costo por cada par i<j)\n"
         << "2) Pegado (matriz completa n x n)\n";
    int mode = readInt("Opcion: ");

    vector<vector<long long>> cost;
    if (mode == 1) cost = readCostMatrixGuided(n);
    else cost = readCostMatrixPasted(n);

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

    int ok = readInt("\nReconstruir ruta? (1=si, 0=no): ");
    if (ok == 1) {
        int i = readInt("Dame i: ");
        int j = readInt("Dame j: ");
        if (!(1 <= i && i < j && j <= n)) {
            cout << "Par invalido (debe ser i<j).\n";
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
    cin.tie(&cout);

    cout << "Selecciona ejercicio:\n";
    cout << "1) Torneo de tenis (Greedy)\n";
    cout << "2) Viajes en Xochimilco (DP)\n";
    int op = readInt("Opcion: ");

    if (op == 1) runExercise1();
    else if (op == 2) runExercise2();
    else cout << "Opcion invalida.\n";

    return 0;
}
