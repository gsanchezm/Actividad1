#include <iostream>
#include <string>
#include "../btree/BTree.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(&std::cout);

    // KISS: archivo fijo. 
    const std::string dataFile = "btree_data.bin";
    std::cout << "B-Tree Demo (disk-backed)\n"
              << "Archivo (disco): " << dataFile << "\n\n";

    BTree bt(dataFile);

    while (true) {
        std::cout << "\nB-Tree Demo:\n"
                  << "1) Insertar\n"
                  << "2) Borrar\n"
                  << "3) Buscar\n"
                  << "4) Imprimir ASC\n"
                  << "5) Imprimir DESC\n"
                  << "6) Verificar B-Tree\n"
                  << "0) Salir\n"
                  << "Opcion: " << std::flush;

        int op;
        if (!(std::cin >> op)) break;
        if (op == 0) break;

        int key;
        switch (op) {
            case 1:
                std::cout << "key: " << std::flush;
                std::cin >> key;
                bt.insert(key);
                std::cout << "OK\n";
                break;

            case 2:
                std::cout << "key: " << std::flush;
                std::cin >> key;
                bt.erase(key);
                std::cout << "OK\n";
                break;

            case 3:
                std::cout << "key: " << std::flush;
                std::cin >> key;
                std::cout << (bt.contains(key) ? "SI\n" : "NO\n");
                break;

            case 4:
                bt.printAscending(std::cout);
                break;

            case 5:
                bt.printDescending(std::cout);
                break;

            case 6:
                std::cout << (bt.isValidBTree() ? "B-Tree valido\n" : "B-Tree NO valido\n");
                break;

            default:
                std::cout << "Opcion invalida\n";
                break;
        }
    }

    return 0;
}