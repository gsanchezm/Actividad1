#include <bits/stdc++.h>
using namespace std;

// Para evitar que el compilador elimine llamadas en benchmarks
static volatile unsigned long long SINK = 0;

// -------------------- 1) POTENCIA --------------------
unsigned long long power_iter(unsigned long long x, unsigned int n) {
    unsigned long long res = 1;
    for (unsigned int i = 0; i < n; i++) res *= x;
    return res;
}

unsigned long long power_rec(unsigned long long x, unsigned int n) {
    if (n == 0) return 1ULL;
    return x * power_rec(x, n - 1);
}

// -------------------- 2) FIBONACCI --------------------
unsigned long long fib_iter(unsigned int n) {
    if (n <= 1) return n;
    unsigned long long a = 0, b = 1;
    for (unsigned int i = 2; i <= n; i++) {
        unsigned long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}

// Fibonacci recursivo naive: O(2^n)
unsigned long long fib_rec(unsigned int n) {
    if (n <= 1) return n;
    return fib_rec(n - 1) + fib_rec(n - 2);
}

// -------------------- 3) MCD EUCLIDES --------------------
long long gcd_iter(long long a, long long b) {
    a = llabs(a); b = llabs(b);
    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
    }
    return a;
}

long long gcd_rec_nonneg(long long a, long long b) {
    if (b == 0) return a;
    return gcd_rec_nonneg(b, a % b);
}

long long gcd_rec(long long a, long long b) {
    a = llabs(a); b = llabs(b);
    return gcd_rec_nonneg(a, b);
}

// -------------------- BENCHMARK HELPERS --------------------
template <class Func>
double avg_time_ms(Func&& f) {
    using clock = chrono::steady_clock;

    long long reps = 1;
    while (true) {
        auto t0 = clock::now();
        for (long long i = 0; i < reps; i++) {
            auto val = f();
            SINK ^= (unsigned long long)val; // evita optimización
        }
        auto t1 = clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();

        if (ms >= 20.0 || reps >= 100000000) { // umbral para estabilidad
            return ms / reps;                  // promedio por llamada
        }
        reps *= 10;
    }
}

struct Row {
    string problem;
    string version;
    long long input;
    double ms;
};

void run_bench() {
    vector<Row> rows;

    // ----- Potencia -----
    unsigned long long x = 2;
    vector<int> powerN = {10, 20, 30, 40, 60};

    for (int n : powerN) {
        rows.push_back({"power", "iter", n, avg_time_ms([&]{ return power_iter(x, n); })});
        rows.push_back({"power", "rec",  n, avg_time_ms([&]{ return power_rec(x, n);  })});
    }

    // ----- Fibonacci -----
    vector<int> fibN = {10, 20, 30, 35, 40};
    for (int n : fibN) {
        rows.push_back({"fibonacci", "iter", n, avg_time_ms([&]{ return fib_iter(n); })});
        rows.push_back({"fibonacci", "rec",  n, avg_time_ms([&]{ return fib_rec(n);  })});
    }

    // ----- GCD: pares consecutivos de Fibonacci (peor caso típico) -----
    vector<int> kVals = {10, 20, 30, 40, 45};
    for (int k : kVals) {
        long long a = (long long)fib_iter(k + 1);
        long long b = (long long)fib_iter(k);

        // Usamos "input" = a (el mayor), para graficar de pequeño a grande
        rows.push_back({"gcd", "iter", a, avg_time_ms([&]{ return gcd_iter(a, b); })});
        rows.push_back({"gcd", "rec",  a, avg_time_ms([&]{ return gcd_rec(a, b);  })});
    }

    // Guardar CSV
    ofstream out("results.csv");
    out << "problem,version,input,time_ms\n";
    out << fixed << setprecision(8);
    for (auto& r : rows) {
        out << r.problem << "," << r.version << "," << r.input << "," << r.ms << "\n";
    }
    out.close();

    cout << "✅ Benchmark terminado.\n";
    cout << "Se genero results.csv\n";
    cout << "SINK (ignorar): " << SINK << "\n";
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    // Modo benchmark (sin interacción)
    if (argc >= 2 && string(argv[1]) == "--bench") {
        run_bench();
        return 0;
    }

    // Demo simple
    cout << "Demo rapido (usa --bench para generar CSV y graficas)\n\n";

    unsigned long long x;
    unsigned int n;
    cout << "[Potencia] x: " << flush; cin >> x;
    cout << "[Potencia] n: " << flush; cin >> n;
    cout << "Iter: " << power_iter(x, n) << "\n";
    cout << "Rec : " << power_rec(x, n) << "\n\n";

    unsigned int fn;
    cout << "[Fibonacci] n: " << flush; cin >> fn;
    cout << "Iter: " << fib_iter(fn) << "\n";
    cout << "Rec : " << fib_rec(fn) << " (ojo: naive, puede ser lento)\n\n";

    long long a,b;
    cout << "[MCD] a: " << flush; cin >> a;
    cout << "[MCD] b: " << flush; cin >> b;
    cout << "Iter: " << gcd_iter(a, b) << "\n";
    cout << "Rec : " << gcd_rec(a, b) << "\n";

    return 0;
}