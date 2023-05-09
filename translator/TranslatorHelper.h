//
// Created by kolya on 09.04.2023.
//

#ifndef MAG_ANALYZATOR_TRANSLATORHELPER_H
#define MAG_ANALYZATOR_TRANSLATORHELPER_H

#include <stack>
#include "../defs.h"
#include "../semantic/Semantic.h"
#include "../triad/TriadHelper.h"

enum DATA_TYPE {
    TYPE_NONE = 0, TYPE_SHORT, TYPE_INTEGER, TYPE_LONG, TYPE_DOUBLE, TYPE_DATASTRUCT
};

enum OBJECT_TYPE {
    TYPE_UNDEFINED = 0, TYPE_VAR, TYPE_STRUCT, TYPE_FUNC
};

class Tree;

struct GlobalData {
    // Для описания переменных
    TypeLex prevLex;
    // Признак обработки описания
    bool flagDeclaration;
    bool isConstDeclaration;
    // Признак необходимости инициализации простых переменных
    bool isMayEqualInitialization;
    // Семантичееский тип данных
    DATA_TYPE dataType;

    // Идентификатор структуры
    TypeLex structIdent;
    // Указатель на идентификатор в дереве
    Tree* identPtr = nullptr;
    // Указатель на структуру в дереве
    Tree* structPtr = nullptr;

    // Стек указателей в дереве (для заполнения структуры, сложных операторов (анонимные, for), main)
    std::stack<Tree*> levels;

    // Стек операндов, его типов и переходов для генерации триад
    std::stack<Operand> operands;
    std::stack<DATA_TYPE> operandTypes;
    std::deque<Triad> resultTriads;

    TypeLex comparisonSign; // Нужен для определения операции сравнения

    // Для формирования for-циклов
    std::stack<int> addrMarkers;
    std::stack<int> ifMarkers;
    std::stack<int> gotoMarkers;
};


#endif //MAG_ANALYZATOR_TRANSLATORHELPER_H
