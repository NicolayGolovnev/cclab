//
// Created by kolya on 09.04.2023.
//

#ifndef MAG_ANALYZATOR_TRANSLATORHELPER_H
#define MAG_ANALYZATOR_TRANSLATORHELPER_H

#include "../defs.h"
#include "../semantic/Semantic.h"

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
    // Семантичееский тип данных
    DATA_TYPE dataType;

    // Идентификатор структуры
    TypeLex structIdent;
    // Буферное дерево (для заполнения структуры, сложных операторов, for, main)
    Tree* v = nullptr;
};

#endif //MAG_ANALYZATOR_TRANSLATORHELPER_H
