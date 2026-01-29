#include "TennisTournament.h"
#include <numeric>

TennisTournament::Result TennisTournament::generate(int n) {
    Result R{};
    R.originalN = n;

    if (n < 2) {
        R.effectiveN = n;
        R.days = 0;
        R.byePlayer = 0;
        R.table.assign(n + 1, {});
        return R;
    }

    // Usamos 0 como BYE (descanso)
    int N = n;
    bool hasBye = (N % 2 == 1);
    if (hasBye) N++;

    R.effectiveN = N;
    R.days = N - 1;
    R.byePlayer = hasBye ? 0 : 0;

    // participantes: 1..n y si falta uno, agregamos 0 (BYE)
    std::vector<int> p;
    p.reserve(N);
    for (int i = 1; i <= n; i++) p.push_back(i);
    if (hasBye) p.push_back(0);

    R.table.assign(n + 1, std::vector<int>(R.days, 0));
    R.matchesPerDay.assign(R.days, {});

    for (int d = 0; d < R.days; d++) {
        // Greedy day pairing: empareja extremos del arreglo actual
        for (int i = 0; i < N / 2; i++) {
            int a = p[i];
            int b = p[N - 1 - i];

            if (a == 0 || b == 0) {
                // descanso para el jugador real
                int real = (a == 0 ? b : a);
                if (real >= 1 && real <= n) R.table[real][d] = 0;
            } else {
                // partido real
                R.table[a][d] = b;
                R.table[b][d] = a;
                R.matchesPerDay[d].push_back({a, b});
            }
        }

        // Rotación (constructiva): fijo p[0], rota p[1..N-1]
        int last = p.back();
        for (int idx = N - 1; idx > 1; --idx) p[idx] = p[idx - 1];
        p[1] = last;
    }

    return R;
}
