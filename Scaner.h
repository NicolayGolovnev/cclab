#ifndef ANALYZATOR_SCANER_H
#define ANALYZATOR_SCANER_H

#include "defs.h"
class Scaner{
private:
    char text[MAX_TEXT];
    int pos;
    int line;
    int newLinePos;
public:
    void setPos(int);
    int getPos();
    int getLine();
    void setLine(int);
    int getNewLinePos();
    void setNewLinePos(int);
    void printError(char*, char*, int, int);
    int scan(TypeLex lex);
    void getData(char*);
    Scaner(char*);
    ~Scaner() {}
};

#endif //ANALYZATOR_SCANER_H
