//
// Created by kolya on 09.04.2023.
//

#ifndef MAG_ANALYZATOR_TRANSLATE_H
#define MAG_ANALYZATOR_TRANSLATE_H


#include "../semantic/Semantic.h"

class Translate {
    GlobalData* global;
    Tree* tree;

    OBJECT_TYPE getObjectTypeFromDataType();

public:
    void setTree(Tree* tree);
    void copyLex(TypeLex lex);

    // declarations
    void deltaStartDeclaration();
    void deltaEndDeclaration();
    void deltaSetIdentifier(OBJECT_TYPE objectType);
    void deltaIdentifier();
    void deltaSetNewLevel();
    void deltaReturnLevel();
    // matchLeft
};


#endif //MAG_ANALYZATOR_TRANSLATE_H
