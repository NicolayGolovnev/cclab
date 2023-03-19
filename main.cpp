#include <iostream>
#include "Magazine.h"

int main() {
    Magazine *analyze;

    char* fileName = const_cast<char*>("test1.txt");
    analyze = new Magazine(fileName);
    analyze->run();
    return 0;
}