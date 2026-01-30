#include <iostream>
#include "../btree/BTree.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(&std::cout);

    // archivo “disco”
    BTree bt("btree_data.bin");

    while (true) {
        std::cout << "\nB-Tree Demo (disk-backed):\n"
                  << "1) Insertar\n2) Borrar\n3) Buscar\n4) Imprimir ASC\n5) Imprimir DESC\n"
                  << "6) Verificar B-Tree\n0) Salir\nOpcion: " << std::flush;

        int op; if (!(std::cin >> op)) break;
        if (op == 0) break;

        int key;
        switch (op) {
            case 1: std::cout << "key: "; std::cin >> key; bt.insert(key); break;
            case 2: std::cout << "key: "; std::cin >> key; bt.erase(key); break;
            case 3: std::cout << "key: "; std::cin >> key;
                    std::cout << (bt.contains(key) ? "SI\n" : "NO\n"); break;
            case 4: bt.printAscending(std::cout); break;
            case 5: bt.printDescending(std::cout); break;
            case 6: std::cout << (bt.isValidBTree() ? "B-Tree valido\n" : "B-Tree NO valido\n"); break;
            default: std::cout << "Opcion invalida\n"; break;
        }
    }
}
