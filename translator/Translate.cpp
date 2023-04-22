//
// Created by kolya on 09.04.2023.
//

#include <iostream>
#include "Translate.h"

Translate::Translate() {
    this->global = new GlobalData();
}

DATA_TYPE Translate::getType(char *lex) {
    if (strcmp(lex, "int") == 0)
        return DATA_TYPE::TYPE_INTEGER;
    else if (strcmp(lex, "short") == 0)
        return DATA_TYPE::TYPE_SHORT;
    else if (strcmp(lex, "long") == 0)
        return DATA_TYPE::TYPE_LONG;
    else if (strcmp(lex, "double") == 0)
        return DATA_TYPE::TYPE_DOUBLE;
    else return DATA_TYPE::TYPE_DATASTRUCT;
}

void Translate::setTree(Tree *tree) {
    this->tree = tree;
}

void Translate::copyLex(char *lex) {
    int localDataType = this->tree->getType(lex);
    memcpy(this->global->prevLex, lex, sizeof(lex));

    // Сохраняем лексему структуры для инициализации объектов структуры
    if (localDataType == DATA_TYPE::TYPE_DATASTRUCT)
        memcpy(this->global->structIdent, lex, sizeof(lex));
}

void Translate::deltaStartDeclaration() {
    this->global->flagDeclaration = true;
    // get type from prevLex ('identifier', short, long, int, double)
    this->global->dataType = this->getType(this->global->prevLex);

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
    if (!this->tree->findDuplicate(Tree::cur, this->global->prevLex)) {
        this->global->identPtr = this->tree->semanticInclude(
                this->global->prevLex,
                OBJECT_TYPE::TYPE_VAR,
                this->global->dataType
                );
    } else
        this->tree->printError(
                const_cast<char*>("Find duplicate of identifier in semantic tree"),
                this->global->prevLex
        );
}

void Translate::deltaSetPropertiesForIdent() {
    if (this->global->isConstDeclaration) {
        char* lex = this->tree->getIdentifier(this->global->identPtr);

        this->tree->semanticSetConst(this->global->identPtr, true);
        // TODO заменить this->global->prevLex на триаду, так как в присваивании может быть выражение
        this->tree->semanticSetData(
                this->global->identPtr,
                this->global->dataType,
                this->global->prevLex
        );
        this->tree->semanticTypeCastCheck(lex, this->global->prevLex);
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
    if (this->global->identPtr == nullptr)
        this->tree->printError("Description of identifier not found", this->global->prevLex);

    if (this->tree->getType(this->global->identPtr) == DATA_TYPE::TYPE_DATASTRUCT)
        this->global->structPtr = this->tree->semanticGetStructData(this->global->identPtr);

    // Проверка, что это правильный тип - отдельным правилом deltaCheckIdentType(),
    // так как это возможно будет идентификатор объекта структуры
}

void Translate::deltaFindIdentifierInStruct() {
    this->global->identPtr = this->tree->semanticGetIdentifierInStruct(this->global->structPtr, this->global->prevLex);
    if (this->global->identPtr == nullptr)
        this->tree->printError("Description of struct-identifier not found", this->global->prevLex);

    if (this->tree->getType(this->global->identPtr) == DATA_TYPE::TYPE_DATASTRUCT)
        this->global->structPtr = this->tree->semanticGetStructData(this->global->identPtr);
}

void Translate::deltaCheckIdentType() {
    char* lex = this->tree->getIdentifier(this->global->identPtr);
    DATA_TYPE type = this->tree->getType(this->global->identPtr);
    if (type == TYPE_DATASTRUCT)
        this->tree->printError("Cannot use a object of structure in expressions", lex);
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

void Translate::printTree() {
    this->tree->print();
}
