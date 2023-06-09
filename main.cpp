#include <iostream>
#include "magazine/Magazine.h"

int main(int argc, char *argv[]) {
    Magazine *analyze;
    char* fileName = const_cast<char*>("triad/test-debug.txt");

    analyze = new Magazine(fileName);
    analyze->run();
    printf("LLk(1) complete without errors!\n");

    printf("Semantic tree:\n");
    analyze->printTree();
//    analyze->printTriads();

//    printf("After optimization:\n");
    analyze->optimizeTriads();
    analyze->printTriads();

    analyze->initAsm();
    analyze->generateAsm();
    analyze->printAsm();

    return 0;
}