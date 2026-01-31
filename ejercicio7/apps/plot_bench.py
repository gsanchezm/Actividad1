import csv
from collections import defaultdict
import matplotlib.pyplot as plt

def load_bench(path="bench.csv"):
    # data[tree]["insert"|"search"] = list of (n, ms)
    data = defaultdict(lambda: {"insert": [], "search": []})

    with open(path, newline="") as f:
        r = csv.DictReader(f)
        for row in r:
            n = int(row["n"])
            tree = row["tree"]
            insert_ms = float(row["insert_ms"])
            search_ms = float(row["search_ms"])

            data[tree]["insert"].append((n, insert_ms))
            data[tree]["search"].append((n, search_ms))

    # ordenar por n
    for tree in data:
        data[tree]["insert"].sort(key=lambda p: p[0])
        data[tree]["search"].sort(key=lambda p: p[0])

    return data

def plot_metric(data, metric, out_file):
    # metric: "insert" o "search"
    plt.figure()

    for tree, series in data.items():
        xs = [n for n, ms in series[metric]]
        ys = [ms for n, ms in series[metric]]
        plt.plot(xs, ys, marker="o", label=tree)

    plt.xlabel("Tamaño de población (n)")
    plt.ylabel("Tiempo total (ms)")
    plt.title(f"Benchmark {metric}: AVL vs B-Tree (disco)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_file, dpi=200)
    print(f"✅ Guardado {out_file}")

def main():
    data = load_bench("bench.csv")
    plot_metric(data, "insert", "bench_insert.png")
    plot_metric(data, "search", "bench_search.png")

if __name__ == "__main__":
    main()
