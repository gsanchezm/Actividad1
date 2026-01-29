#pragma once
#include <vector>
#include <utility>

class TennisTournament {
public:
    struct Result {
        int originalN{};
        int effectiveN{};  // n o n+1 si se agregó BYE
        int days{};        // effectiveN - 1
        int byePlayer{};   // 0 si no hay BYE, si hay BYE usamos 0 como descanso
        // table[player][day] = opponent, 0 = descanso
        std::vector<std::vector<int>> table;
        // matchesPerDay[day] = lista de partidos (a,b) (sin BYE)
        std::vector<std::vector<std::pair<int,int>>> matchesPerDay;
    };

    // Genera planificación Round-Robin (Greedy/constructivo)
    static Result generate(int n);
};
