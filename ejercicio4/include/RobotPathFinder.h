#pragma once
#include <vector>
#include <utility>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>

class RobotPathFinder {
public:
    using Cell = std::pair<int,int>;
    using Path = std::vector<Cell>;
    using Big  = boost::multiprecision::cpp_int;

    RobotPathFinder(int n, int m);

    // a) Existe camino? (BFS)
    bool existsPathBFS();

    // b) Todos los caminos (Backtracking/DFS) - OJO: puede ser enorme
    std::vector<Path> allPathsBacktracking();

    // Mapa de caminos: cuántos caminos pasan por cada celda (DP en DAG)
    std::vector<std::vector<Big>> pathThroughCountMap();

    // Imprime el tablero como mapa (S=Start, G=Goal, .=no participa)
    void printPathMap();

    // (Opcional) Dibuja un camino específico en el tablero
    void printSinglePathOverlay(const Path& p);

private:
    int n_, m_;
    const int RIGHT_STEP = 3;
    const int DOWN_STEP  = 2;

    bool inBounds(int r, int c) const;

    void dfsAll(int r, int c, Path& current, std::vector<Path>& result);

    static std::string formatBig(const Big& x, int width = 8);
};
