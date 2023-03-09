//
// Created by kolya on 09.03.2023.
//

#ifndef MAG_ANALYZATOR_MAGAZINE_H
#define MAG_ANALYZATOR_MAGAZINE_H

// Размер магазина
#include "defs.h"

#define MAX_LEN_MAG 1000
struct MagOneSymb {
    bool term;
    TypeSymb typeSymb; // ???
    TypeLex lex;
};

class Magazine {
private:
    MagOneSymb Mag[MAX_LEN_MAG];
    // Указатель на верхушку магазина
    int curMag;
};


#endif //MAG_ANALYZATOR_MAGAZINE_H
