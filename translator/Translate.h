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

    int getTypeOfOperation(TypeLex operation);
    ExpresData getValueFromOperand(Operand operand);

public:
    Translate();
    void setTree(Tree* tree);
    GlobalData* getGlobal();
    void copyLex(TypeLex lex);

    void printTree();

    // declarations
    void deltaStartDeclaration();
    void deltaEndDeclaration();
    void deltaSetMayEqualInitialization();
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
};


#endif //MAG_ANALYZATOR_TRANSLATE_H
