#include "RobotPathFinder.h"
#include <queue>
#include <iostream>
#include <iomanip>

using namespace std;

RobotPathFinder::RobotPathFinder(int n, int m) : n_(n), m_(m) {}

bool RobotPathFinder::inBounds(int r, int c) const {
    return (r >= 0 && r < n_ && c >= 0 && c < m_);
}

// ---------- a) BFS ----------
bool RobotPathFinder::existsPathBFS() {
    if (n_ <= 0 || m_ <= 0) return false;

    vector<vector<bool>> visited(n_, vector<bool>(m_, false));
    queue<Cell> q;

    q.push({0,0});
    visited[0][0] = true;

    while (!q.empty()) {
        auto [r,c] = q.front(); q.pop();
        if (r == n_-1 && c == m_-1) return true;

        // derecha +3
        int nr = r, nc = c + RIGHT_STEP;
        if (inBounds(nr, nc) && !visited[nr][nc]) {
            visited[nr][nc] = true;
            q.push({nr,nc});
        }

        // abajo +2
        nr = r + DOWN_STEP; nc = c;
        if (inBounds(nr, nc) && !visited[nr][nc]) {
            visited[nr][nc] = true;
            q.push({nr,nc});
        }
    }
    return false;
}

// ---------- b) Backtracking (enumerar) ----------
vector<RobotPathFinder::Path> RobotPathFinder::allPathsBacktracking() {
    vector<Path> result;
    if (n_ <= 0 || m_ <= 0) return result;

    Path current;
    current.push_back({0,0});
    dfsAll(0, 0, current, result);
    return result;
}

void RobotPathFinder::dfsAll(int r, int c, Path& current, vector<Path>& result) {
    if (r == n_-1 && c == m_-1) {
        result.push_back(current);
        return;
    }

    // derecha
    int nr = r, nc = c + RIGHT_STEP;
    if (inBounds(nr, nc)) {
        current.push_back({nr,nc});
        dfsAll(nr, nc, current, result);
        current.pop_back();
    }

    // abajo
    nr = r + DOWN_STEP; nc = c;
    if (inBounds(nr, nc)) {
        current.push_back({nr,nc});
        dfsAll(nr, nc, current, result);
        current.pop_back();
    }
}

// ---------- MAPA (DP en DAG) ----------
vector<vector<RobotPathFinder::Big>> RobotPathFinder::pathThroughCountMap() {
    vector<vector<Big>> through;
    if (n_ <= 0 || m_ <= 0) return through;

    vector<vector<Big>> waysTo(n_, vector<Big>(m_, 0));
    vector<vector<Big>> waysFrom(n_, vector<Big>(m_, 0));

    // waysTo: del origen hacia adelante
    waysTo[0][0] = 1;
    for (int r = 0; r < n_; r++) {
        for (int c = 0; c < m_; c++) {
            if (waysTo[r][c] == 0) continue;

            int r1 = r, c1 = c + RIGHT_STEP;
            if (inBounds(r1, c1)) waysTo[r1][c1] += waysTo[r][c];

            int r2 = r + DOWN_STEP, c2 = c;
            if (inBounds(r2, c2)) waysTo[r2][c2] += waysTo[r][c];
        }
    }

    // waysFrom: del destino hacia atrás (scan reverse)
    waysFrom[n_-1][m_-1] = 1;
    for (int r = n_-1; r >= 0; r--) {
        for (int c = m_-1; c >= 0; c--) {
            if (r == n_-1 && c == m_-1) continue;

            Big sum = 0;
            // desde (r,c) puedes ir a (r, c+3) y (r+2, c)
            if (inBounds(r, c + RIGHT_STEP)) sum += waysFrom[r][c + RIGHT_STEP];
            if (inBounds(r + DOWN_STEP, c))  sum += waysFrom[r + DOWN_STEP][c];
            waysFrom[r][c] = sum;
        }
    }

    // through = waysTo * waysFrom (solo celdas en caminos completos)
    through.assign(n_, vector<Big>(m_, 0));
    for (int r = 0; r < n_; r++) {
        for (int c = 0; c < m_; c++) {
            if (waysTo[r][c] != 0 && waysFrom[r][c] != 0) {
                through[r][c] = waysTo[r][c] * waysFrom[r][c];
            }
        }
    }
    return through;
}

string RobotPathFinder::formatBig(const Big& x, int width) {
    std::string s = x.convert_to<std::string>();
    if ((int)s.size() <= width) return s;

    // si es muy grande, lo recortamos: abc..xyz
    if (width < 6) return s.substr(0, width);
    return s.substr(0, 3) + ".." + s.substr(s.size() - 3);
}

void RobotPathFinder::printPathMap() {
    auto through = pathThroughCountMap();
    if (through.empty()) {
        cout << "Tablero invalido.\n";
        return;
    }

    cout << "\n=== MAPA DE CAMINOS (DP): #caminos que pasan por cada celda ===\n";
    cout << "S = start (0,0), G = goal (" << (n_-1) << "," << (m_-1) << ")\n";
    cout << "'.' = no participa en ningun camino completo\n\n";

    // Encabezado columnas
    cout << "     ";
    for (int c = 0; c < m_; c++) cout << setw(8) << c;
    cout << "\n";

    for (int r = 0; r < n_; r++) {
        cout << setw(4) << r << " ";
        for (int c = 0; c < m_; c++) {
            if (r == 0 && c == 0) {
                cout << setw(8) << "S";
            } else if (r == n_-1 && c == m_-1) {
                cout << setw(8) << "G";
            } else if (through[r][c] == 0) {
                cout << setw(8) << ".";
            } else {
                cout << setw(8) << formatBig(through[r][c], 8);
            }
        }
        cout << "\n";
    }
    cout << "\n";
}

void RobotPathFinder::printSinglePathOverlay(const Path& p) {
    vector<vector<char>> grid(n_, vector<char>(m_, '.'));

    for (auto [r,c] : p) {
        if (inBounds(r,c)) grid[r][c] = 'o';
    }
    grid[0][0] = 'S';
    grid[n_-1][m_-1] = 'G';

    cout << "\n=== OVERLAY DE UN CAMINO ('o') ===\n";
    for (int r = 0; r < n_; r++) {
        for (int c = 0; c < m_; c++) {
            cout << grid[r][c] << ' ';
        }
        cout << "\n";
    }
    cout << "\n";
}
