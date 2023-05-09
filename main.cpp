#include <iostream>
#include "magazine/Magazine.h"

int main(int argc, char *argv[]) {
    Magazine *analyze;
    // test3 - Process finished with exit code -1073741819 (0xC0000005)
    char* fileName = const_cast<char*>("triad/test-debug.txt");

    analyze = new Magazine(fileName);
    analyze->run();
    printf("LLk(1) complete without errors!\n");

    printf("Semantic tree:\n");
    analyze->printTree();
    analyze->printTriads();
    return 0;
}