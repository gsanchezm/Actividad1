#include <bits/stdc++.h>
#include "MatrixSum.h"

using namespace std;

static vector<vector<long long>> readMatrixManual(int rows, int cols) {
    vector<vector<long long>> A(rows, vector<long long>(cols));
    cout << "Ingresa la matriz (" << rows << "x" << cols << ") fila por fila:\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << "A[" << i << "][" << j << "]: " << flush;
            cin >> A[i][j];
        }
    }
    return A;
}

static vector<vector<long long>> buildMatrixRandom(int rows, int cols, int seed=123, int minv=1, int maxv=9) {
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(minv, maxv);
    vector<vector<long long>> A(rows, vector<long long>(cols));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            A[i][j] = dist(rng);
    return A;
}

static void printMatrix(const vector<vector<long long>>& A) {
    cout << "\nMatriz:\n";
    for (auto& row : A) {
        for (auto v : row) cout << setw(5) << v;
        cout << "\n";
    }
    cout << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    int rows, cols;
    cout << "Filas (n): " << flush; cin >> rows;
    cout << "Columnas (m): " << flush; cin >> cols;

    cout << "\nModo de entrada:\n";
    cout << "1) Manual (teclear elementos)\n";
    cout << "2) Aleatoria (valores 1..9)\n";
    cout << "Opcion: " << flush;

    int op; cin >> op;

    vector<vector<long long>> A;
    if (op == 1) A = readMatrixManual(rows, cols);
    else A = buildMatrixRandom(rows, cols);

    printMatrix(A);

    long long sum = MatrixSum::sumRecursive(A);
    cout << "Suma recursiva (Divide y venceras) = " << sum << "\n";

    // Complejidad (para el reporte)
    cout << "\nComplejidad:\n";
    cout << "- Tiempo: Θ(n*m)\n";
    cout << "- Espacio: Θ(log(n*m)) por profundidad de recursion\n";

    return 0;
}
