//
// Created by kolya on 08.05.2023.
//

#ifndef MAG_ANALYZATOR_TRIADHELPER_H
#define MAG_ANALYZATOR_TRIADHELPER_H


#include "../defs.h"
#include "../semantic/Semantic.h"

enum RegisterType {
    eax,
    ebx,
    ecx,
    edx,
    esi,
    edi
};

class Tree;
struct Operand {
    bool isLink; // Признак операнда или ссылки не нее
    int number; // Номер триады-ссылки

    bool isConst; // Признак константы триады
    TypeLex lex; // Лексема

    Tree* ptrTree = nullptr; // Для определения лексемы в структуре
    TypeLex structLex; // Идентификатор структуры (если лексема в структуре)
};

struct Triad {
    TypeLex operation; // Операция триады в виде лексемы
    Operand firstOperand, secondOperand;

    RegisterType registerType; // Регистр, в котором будет хранится идентификатор (при использовании ссылки на операнд)
};

#endif //MAG_ANALYZATOR_TRIADHELPER_H
