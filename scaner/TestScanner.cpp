#include "../defs.h"
#include "Scaner.h"
#include <cstdio>

int testScanner() {
    Scaner* sc;
    TypeLex lex;
    int type;

    char* fileName = const_cast<char*>("/scaner/test1.txt");
    sc = new Scaner(fileName);

    do
    {
        type = sc->scan(lex);
        printf("%s - type %d \n", lex, type);
    } while (type != TypeEnd);
    return 0;
}
