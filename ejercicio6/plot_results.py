import csv
from collections import defaultdict
import matplotlib.pyplot as plt

def load_csv(path):
    data = defaultdict(lambda: defaultdict(list))
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            prob = row["problem"]
            ver  = row["version"]
            x    = int(row["input"])
            y    = float(row["time_ms"])
            data[prob][ver].append((x, y))

    # ordenar por input
    for prob in data:
        for ver in data[prob]:
            data[prob][ver].sort(key=lambda p: p[0])
    return data

def plot_problem(prob, series):
    plt.figure()
    for ver in ["iter", "rec"]:
        if ver in series:
            xs = [p[0] for p in series[ver]]
            ys = [p[1] for p in series[ver]]
            plt.plot(xs, ys, marker="o", label=ver)

    plt.xlabel("Entrada (n o tamaño)")
    plt.ylabel("Tiempo promedio por llamada (ms)")
    plt.title(f"{prob}: iter vs rec")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"{prob}.png", dpi=200)

def main():
    data = load_csv("results.csv")
    for prob, series in data.items():
        plot_problem(prob, series)
    print("✅ Graficas generadas: power.png, fibonacci.png, gcd.png")

if __name__ == "__main__":
    main()