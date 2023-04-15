//
// Created by kolya on 09.04.2023.
//

#include <iostream>
#include "Translate.h"

void Translate::setTree(Tree *tree) {
    this->tree = tree;
}

OBJECT_TYPE Translate::getObjectTypeFromDataType() {
    if (this->global->dataType == DATA_TYPE::TYPE_DATASTRUCT)
        return OBJECT_TYPE::TYPE_STRUCT;
    return TYPE_UNDEFINED;
}

void Translate::copyLex(char *lex) {
    int localDataType = this->tree->getType(lex);
    if (localDataType == DATA_TYPE::TYPE_DATASTRUCT)
        memcpy(this->global->structIdent, lex, sizeof(lex));
    else
        memcpy(this->global->prevLex, lex, sizeof(lex));
}

void Translate::deltaStartDeclaration() {
    this->global->flagDeclaration = true;
    // get type from prevLex ('identifier', short, long, int, double)
    this->global->dataType = this->tree->getType(this->global->prevLex);

    // check dataType == DATASTRUCT is a rly struct
    if (this->global->dataType == DATA_TYPE::TYPE_DATASTRUCT)
        if (!this->tree->isStruct(Tree::cur, this->global->structIdent))
            this->tree->printError("Identifier is not a structure", this->global->structIdent);
}

void Translate::deltaSetConstDeclaration() {
    this->global->isConstDeclaration = true;
}

void Translate::deltaEndDeclaration() {
    this->global->flagDeclaration = false;
    this->global->isConstDeclaration = false;
}

void Translate::deltaSetIdentifier() {
    if (!this->tree->findDuplicate(Tree::cur)) {
        this->global->identPtr = this->tree->semanticInclude(
                this->global->prevLex,
                OBJECT_TYPE::TYPE_VAR,
                this->global->dataType
                );
    } else ;
        // TODO print error - FIND DUPLICATES
}

void Translate::deltaSetPropertiesForIdent() {
    if (this->global->isConstDeclaration) {
        this->tree->semanticSetConst(this->global->identPtr, true);
        this->tree->semanticSetData(
                this->global->identPtr,
                this->global->dataType,
                this->global->prevLex
        );
        this->tree->semanticTypeCastCheck(this->global->identPtr, this->global->prevLex);
    }
    else
        this->tree->semanticSetConst(this->global->identPtr, false);

    if (this->global->dataType == DATA_TYPE::TYPE_DATASTRUCT) {
        this->tree->semanticSetInit(this->global->identPtr, true);
        this->tree->semanticSetStruct(
                this->global->identPtr,
                this->tree->semanticGetStructData(this->global->structIdent)
                );
    }
}

void Translate::deltaSetFunc() {
    Tree* func = this->tree->semanticInclude(
            this->global->prevLex,
            OBJECT_TYPE::TYPE_FUNC,
            DATA_TYPE::TYPE_INTEGER
            );
    this->tree->semanticSetInit(func, false);

    this->global->levels.push(func);
}

void Translate::deltaSetStruct() {
    Tree* structPtr = this->tree->semanticInclude(
            this->global->prevLex,
            OBJECT_TYPE::TYPE_UNDEFINED,
            DATA_TYPE::TYPE_DATASTRUCT
    );

    this->global->levels.push(structPtr);
}

void Translate::deltaSetNewLevel() {
    Tree* compoundOper = this->tree->compoundOperator();
    this->global->levels.push(compoundOper);
}

void Translate::deltaReturnLevel() {
    Tree* returned = this->global->levels.top();
    this->global->levels.pop();

    this->tree->setCur(returned);
}

void Translate::deltaFindIdentifier() {
    this->global->identPtr = this->tree->findUp(Tree::cur, this->global->prevLex);
    // TODO проверка, что это правильный тип - отдельным правилом delta,
    //  после лексемы a.a.a будет стоять
}

void Translate::deltaCheckIdentType() {
    DATA_TYPE type = this->tree->getType(this->global->identPtr);
    if (type == TYPE_DATASTRUCT)
        this->tree->printError("Cannot use a object of structure in expressions", this->global->identPtr);
}

void Translate::deltaDeleteCompound() {
    Tree* returned = this->global->levels.top();
    this->global->levels.pop();

    //печать синтаксического дерева до его удаления
    std::cout << "\nPrint a compound operator before deleting:" << std::endl;
    this->tree->print();
    //удаляем сложный оператор
    this->tree->deleteTreeFrom(returned);
    std::cout << "\nPrint a tree after deleting compound operator:" << std::endl;
    this->tree->print();
}
