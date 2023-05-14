//
// Created by kolya on 09.04.2023.
//

#include <iostream>
#include "Translate.h"

Translate::Translate() {
    this->global = new GlobalData();
    this->global->isMayEqualInitialization = false;
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

GlobalData* Translate::getGlobal() {
    return this->global;
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
        this->tree->semanticSetInit(this->global->identPtr, true);
        // при инициализации констант могут быть выражения, поэтому достаем триаду
        // у которой 2 операнд будет иметь значение
        Triad expression = this->global->resultTriads.back();
        ExpresData dataValue = this->getValueFromOperand(expression.secondOperand);
        TypeLex value;
        sprintf(value, "%f", dataValue.getResult());

        this->tree->semanticTypeCastCheck(this->global->dataType, dataValue.dataType);
        this->tree->semanticSetData(
                this->global->identPtr,
                dataValue.dataType,
                value
        );
    }
    else {
        this->tree->semanticSetInit(this->global->identPtr, false);
        this->tree->semanticSetConst(this->global->identPtr, false);
    }

    // Если у переменной триада по его первому операнду - значит она инициализирована
    // Однако может быть такое, что переменная не инициализирована, но нашлась похожая - проверяем еще по флагу
    if (this->global->isMayEqualInitialization && !this->global->resultTriads.empty()) {
        Triad expression = this->global->resultTriads.back();
        char* identifier = this->tree->getIdentifier(this->global->identPtr);
        if (strcmp(expression.operation, (char*)"=") == 0 && strcmp(expression.firstOperand.lex, identifier) == 0) {
            ExpresData dataValue = this->getValueFromOperand(expression.secondOperand);
            TypeLex value;
            sprintf(value, "%f", dataValue.getResult());

            if (this->global->dataType != DATA_TYPE::TYPE_DATASTRUCT) {
                this->tree->semanticTypeCastCheck(this->global->dataType, dataValue.dataType);
                this->tree->semanticSetData(
                        this->global->identPtr,
                        dataValue.dataType,
                        value
                );
                this->tree->semanticSetInit(this->global->identPtr, true);
            } else
                this->tree->printError("Impossible initialize identifier-struct with some data!", this->global->prevLex);
        }
    }
    this->global->isMayEqualInitialization = false;

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
    else
        this->global->structPtr = nullptr;

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

void Translate::printTree() {
    this->tree->print();
    printf("\n");
}


ExpresData Translate::getValueFromOperand(Operand operand) {
    if (operand.isLink) {
        // Значит это триада - ищем операнды и вытаскиваем оттуда данные
        Operand firstOperand = this->global->resultTriads[operand.number].firstOperand;
        ExpresData firstDataValue = this->getValueFromOperand(firstOperand);

        Operand secondOperand = this->global->resultTriads[operand.number].secondOperand;
        ExpresData secondDataValue = this->getValueFromOperand(firstOperand);

        int typeOperation = this->getTypeOfOperation(this->global->resultTriads[operand.number].operation);
        this->tree->semanticMakeBiOperation(&firstDataValue, &secondDataValue, typeOperation);

        return firstDataValue;
    } else if (operand.isConst) {
        ExpresData dataValue;
        this->tree->semanticGetStringValue(operand.lex, &dataValue, 0);

        return dataValue;
    } else {
        // Это не константа и не триада - значит это идентификатор
        Tree* ptr = this->tree->semanticGetVar(operand.lex);
        ExpresData dataValue;
        this->tree->semanticGetData(ptr, &dataValue);

        return dataValue;
    }
}

int Translate::getTypeOfOperation(char *operation) {
    if (strcmp(operation, (char*)"|") == 0) {
        return TypeBitOr;
    } else if (strcmp(operation, (char*)"^") == 0) {
        return TypeBitXor;
    } else if (strcmp(operation, (char*)"&") == 0) {
        return TypeBitAnd;
    } else if (strcmp(operation, (char*)"==") == 0) {
        return TypeEqual;
    } else if (strcmp(operation, (char*)"!=") == 0) {
        return TypeNotEqual;
    } else if (strcmp(operation, (char*)">") == 0) {
        return TypeMore;
    } else if (strcmp(operation, (char*)">=") == 0) {
        return TypeMoreOrEqual;
    } else if (strcmp(operation, (char*)"<") == 0) {
        return TypeLess;
    } else if (strcmp(operation, (char*)"<=") == 0) {
        return TypeLessOrEqual;
    } else if (strcmp(operation, (char*)"+") == 0) {
        return TypeAdd;
    } else if (strcmp(operation, (char*)"-") == 0) {
        return TypeSub;
    } else if (strcmp(operation, (char*)"*") == 0) {
        return TypeMul;
    } else if (strcmp(operation, (char*)"/") == 0) {
        return TypeDiv;
    } else if (strcmp(operation, (char*)"%") == 0) {
        return TypeDivPart;
    } else
        this->tree->printError("Undefined operation for making it", operation);
}

void Translate::deltaSetMayEqualInitialization() {
    this->global->isMayEqualInitialization = true;
}
