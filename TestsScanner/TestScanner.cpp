#include "../defs.h"
#include "../Scaner.h"
#include <cstdio>

int testScanner(int argc, char *argv[])
{
    Scaner* sc;
    TypeLex lex;
    int type;

    char* fileName = const_cast<char*>("test3.txt");
    if (argc <= 1)
        sc = new Scaner(fileName);
    else
        sc = new Scaner(argv[1]);

    do
    {
        type = sc->scan(lex);
        printf("%s - type %d \n", lex, type);
    } while (type != TypeEnd);
    return 0;
}
