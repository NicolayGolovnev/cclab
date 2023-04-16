//
// Created by kolya on 09.04.2023.
//

#ifndef MAG_ANALYZATOR_TRANSLATE_H
#define MAG_ANALYZATOR_TRANSLATE_H


#include "../semantic/Semantic.h"
#include "TranslatorHelper.h"

class Translate {
    GlobalData* global;
    Tree* tree;

    DATA_TYPE getType(TypeLex lex);

public:
    Translate();
    void setTree(Tree* tree);
    void copyLex(TypeLex lex);

    void printTree();

    // declarations
    void deltaStartDeclaration();
    void deltaEndDeclaration();
    void deltaSetConstDeclaration();
    void deltaSetIdentifier();
    void deltaSetPropertiesForIdent();

    void deltaSetFunc();
    void deltaSetStruct();
    void deltaSetNewLevel();
    void deltaReturnLevel();

    void deltaFindIdentifier();
    void deltaFindIdentifierInStruct();
    void deltaCheckIdentType();
    void deltaDeleteCompound();

    // matchLeft, match
};


#endif //MAG_ANALYZATOR_TRANSLATE_H
