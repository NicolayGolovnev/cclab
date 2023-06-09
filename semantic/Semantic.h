//
// Created by kolya on 12/1/2021.
//

#ifndef ANALYZATOR_SEMANTIC_H
#define ANALYZATOR_SEMANTIC_H

#include <string>
#include <vector>
#include "../defs.h"
#include "../scaner/Scaner.h"
#include "../translator/TranslatorHelper.h"

#pragma once

union DataValue {
    int vInt;
    double vDouble;
};

enum TypeDecl {
    DQ = 8,
    DD = 4,
    DW = 2,
    DB = 1
};

class Tree;
// Структура для хранения в таблице идентификаторов
struct Node {
    OBJECT_TYPE objectType;     // тип объекта - переменная, структура или функция
    TypeLex id;                 // идентификатор
    DATA_TYPE dataType;         // тип значения - short, int, long, double, объект структуры

    DataValue dataValue;

    TypeLex asmId;              // Уникальный идентификатор ассемблерный
    TypeDecl declarationType;   // DQ, DD, DW, DB
    int len;                    // Размер переменной
    int level;                  // Уровень вложенности идентификатора
    int stackAddr;              // Смещение адреса в стеке

    //для переменной
    bool isConst;               // Флаг константы
    bool init;                  // Инициализация переменной
    //для объекта структуры
    Tree* dataStruct = nullptr;           // Ссылка на объекты структуры
    TypeLex structLex;
};

struct ExpresData {
    DATA_TYPE dataType;
    DataValue dataValue;

    double getResult() {
        if (dataType == DATA_TYPE::TYPE_INTEGER)
            return (double) dataValue.vInt;
        else
            return dataValue.vDouble;
    };
};

class Tree {
private:
    Node* node;
    Tree *up, *left, *right;
    Scaner* sc;

    int computeStackAddr(Tree* from, int stackAddr);
public:
    static Tree* cur;
    Tree();
    Tree(Tree* l, Tree* r, Tree* up, Node* data);
    ~Tree();
    void setLeft(Node* data);
    void setRight(Node* data);

    Tree* findUp(Tree* from, TypeLex id);
    Tree* findUpOnLevel(Tree* from, TypeLex id);
    std::string generateFullLexFromStruct(Tree* structLevel, TypeLex lex);

    void print();
    void printError(std::string error, TypeLex a);
    int findDuplicate(Tree* addr, TypeLex a);

    void setCur(Tree* a);
    void setScaner(Scaner* scaner);

    Tree* semanticInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType);
    void semanticSetInit(Tree* addr, bool flag);
    void semanticSetConst(Tree *addr, bool flag);
    void semanticSetStruct(Tree* &addr, Tree* data);

    int isStruct(Tree* addr, TypeLex a);
    void checkInit(TypeLex a);
    void checkInit(Tree* a);
    void checkConst(TypeLex a);
    void checkConst(Tree* a);
    void semanticTypeCastCheck(TypeLex a, TypeLex b);
    void semanticTypeCastCheck(DATA_TYPE a, DATA_TYPE b);

    Tree* semanticGetVar(TypeLex a);
    Tree* semanticGetStructData(TypeLex a);
    Tree* semanticGetStructData(Tree* a);
    Tree* semanticGetIdentifierInStruct(Tree* from, TypeLex a);
    DATA_TYPE getType(TypeLex a);
    DATA_TYPE getType(Tree* a);
    char* getIdentifier(Tree* from);

    Tree* compoundOperator();

    //#2
    void semanticSetData(Tree* a, DATA_TYPE dt, char* data);
    Tree* copyTree(Tree* from, Tree* up);
    Tree* deleteTreeFrom(Tree* a);

    //#3
    void semanticSetValue(Tree* a, ExpresData* data);
    void semanticGetData(Tree* a, ExpresData* data);
    void semanticGetStringValue(TypeLex value, ExpresData* data, int type);

    //#4
    void semanticMakeBiOperation(ExpresData* data1, ExpresData* data2, int type);
    void printInfo(Tree* a, std::string beforeText);

    //#5-6
    static bool flagInterpret;

    std::string generateAsmName(Node node);
    char* getAsmIdentifier(Tree* from);
    Tree* goNext(Tree* from);
    Node* getNode(Tree* from);

    int computeLengthOfStruct(Tree* structAddr);
    int computeMemorySizeOfFunc(Tree* funcAddr);

    void insertLevel(Tree* from, int level);
    void computeStackAddrInMain(Tree* root);
};

#endif //ANALYZATOR_SEMANTIC_H
