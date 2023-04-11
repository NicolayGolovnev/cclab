//
// Created by kolya on 12/1/2021.
//

#include <iostream>
#include "Semantic.h"
#include "../magazine/Magazine.h"


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    Magazine *analyze;
    char* fileName = const_cast<char*>("semantic/test-debug.txt");

    analyze = new Magazine(fileName);
    analyze->run();
    // print tree
//    analyze->printTree();

    return 0;
}
