#include <iostream>
#include "Magazine.h"

int testMagazine() {
    Magazine *analyze;
    char* fileName = const_cast<char*>("magazine/test-debug.txt");

    analyze = new Magazine(fileName);
    analyze->run();
    printf("LLk(1) complete without errors!\n");
    return 0;
}