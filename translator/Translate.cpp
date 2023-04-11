//
// Created by kolya on 09.04.2023.
//

#include "Translate.h"

void Translate::setTree(Tree *tree) {
    this->tree = tree;
}

void Translate::copyLex(char *lex) {
    if (this->global->dataType == DATA_TYPE::TYPE_DATASTRUCT)
        memcpy(this->global->structIdent, lex, sizeof(lex));
    else
        memcpy(this->global->prevLex, lex, sizeof(lex));
}


void Translate::deltaStartDeclaration() {
    this->global->flagDeclaration = true;
//    this->global->dataType = this->tree->getType(this->global->prevLex);
    // TODO get type from prevLex ('identifier', short, long, int, double)
}

void Translate::deltaEndDeclaration() {
    this->global->flagDeclaration = false;
}

// Описание новой переменной
void Translate::deltaSetIdentifier(OBJECT_TYPE objectType) {
    if (this->global->flagDeclaration) {
        this->tree->semanticInclude(this->global->prevLex, objectType, this->global->dataType);
        // isConst
        // isInit
        // TypeCastCheck
        // SetData
    }
}





