#include <iostream>
#include "Magazine.h"

int main(int argc, char *argv[]) {
    Magazine *analyze;
    char* fileName = const_cast<char*>("test-debug.txt");

    analyze = new Magazine(fileName);
    analyze->run();
    printf("LLk(1) complete without errors!\n");
    return 0;
}