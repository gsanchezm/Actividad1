#include <iostream>
#include "../avl/AVLTree.h"

int main()
{
        std::ios::sync_with_stdio(false);
        std::cin.tie(&std::cout);

        AVLTree avl;

        while (true)
        {
                std::cout << "\nAVL Demo:\n"
                          << "1) Insertar\n2) Borrar\n3) Buscar\n4) Imprimir ASC\n5) Imprimir DESC\n"
                          << "6) Altura(key)\n7) Profundidad(key)\n8) Nivel(key)\n9) Verificar AVL\n0) Salir\n"
                          << "Opcion: " << std::flush;

                int op;
                if (!(std::cin >> op))
                        break;
                if (op == 0)
                        break;

                int key;
                switch (op)
                {
                case 1:
                        std::cout << "key: ";
                        std::cin >> key;
                        avl.insert(key);
                        break;
                case 2:
                        std::cout << "key: ";
                        std::cin >> key;
                        avl.erase(key);
                        break;
                case 3:
                        std::cout << "key: ";
                        std::cin >> key;
                        std::cout << (avl.contains(key) ? "SI\n" : "NO\n");
                        break;
                case 4:
                        avl.printAscending(std::cout);
                        break;
                case 5:
                        avl.printDescending(std::cout);
                        break;
                case 6:
                        std::cout << "key: ";
                        std::cin >> key;
                        if (auto v = avl.heightOf(key))
                                std::cout << "Altura = " << *v << "\n";
                        else
                                std::cout << "No existe\n";
                        break;

                case 7:
                        std::cout << "key: ";
                        std::cin >> key;
                        if (auto v = avl.depthOf(key))
                                std::cout << "Profundidad = " << *v << "\n";
                        else
                                std::cout << "No existe\n";
                        break;

                case 8:
                        std::cout << "key: ";
                        std::cin >> key;
                        if (auto v = avl.levelOf(key))
                                std::cout << "Nivel = " << *v << "\n";
                        else
                                std::cout << "No existe\n";
                        break;
                case 9:
                        std::cout << (avl.isValidAVL() ? "AVL valido\n" : "AVL NO valido\n");
                        break;
                default:
                        std::cout << "Opcion invalida\n";
                        break;
                }
        }
}
