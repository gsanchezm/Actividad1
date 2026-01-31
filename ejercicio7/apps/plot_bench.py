import csv
from collections import defaultdict
import matplotlib.pyplot as plt

def load_csv(path):
    data = defaultdict(lambda: {"insert": [], "search": []})
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        for row in r:
            n = int(row["n"])
            tree = row["tree"]
            ins = float(row["insert_ms"])
            sea = float(row["search_ms"])
            data[tree]["insert"].append((n, ins))
            data[tree]["search"].append((n, sea))

    for tree in data:
        data[tree]["insert"].sort(key=lambda p: p[0])
        data[tree]["search"].sort(key=lambda p: p[0])
    return data

def plot_compare(metric, out_file, linux, windows):
    plt.figure()

    # Plot Linux
    for tree, series in linux.items():
        xs = [n for n, ms in series[metric]]
        ys = [ms for n, ms in series[metric]]
        plt.plot(xs, ys, marker="o", label=f"{tree} (Linux)")

    # Plot Windows
    for tree, series in windows.items():
        xs = [n for n, ms in series[metric]]
        ys = [ms for n, ms in series[metric]]
        plt.plot(xs, ys, marker="o", linestyle="--", label=f"{tree} (Windows)")

    plt.xlabel("Tamaño de población (n)")
    plt.ylabel("Tiempo total (ms)")
    plt.title(f"Benchmark {metric}: AVL vs B-Tree (Linux vs Windows)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_file, dpi=200)
    print(f"✅ Guardado {out_file}")

def main():
    linux = load_csv("bench_linux.csv")
    windows = load_csv("bench_windows.csv")

    plot_compare("insert", "bench_compare_insert.png", linux, windows)
    plot_compare("search", "bench_compare_search.png", linux, windows)

if __name__ == "__main__":
    main()