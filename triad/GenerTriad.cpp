//
// Created by kolya on 08.05.2023.
//

#include "GenerTriad.h"

#define max(a, b) a > b ? a : b

GenerTriad::GenerTriad() = default;

void GenerTriad::setTree(Tree *tree) {
    this->tree = tree;
}

void GenerTriad::setGlobal(GlobalData *global) {
    this->global = global;
}

void GenerTriad::deltaRememberComparisonSign() {
    memcpy(this->global->comparisonSign, this->global->prevLex, sizeof(this->global->prevLex));
}

void GenerTriad::deltaMatch() {
    DATA_TYPE secondType = this->global->operandTypes.top();
    this->global->operandTypes.pop();
    DATA_TYPE firstType = this->global->operandTypes.top();
    this->global->operandTypes.pop();

    DATA_TYPE resultType;
    this->tree->semanticTypeCastCheck(firstType, secondType);

    // Генерация нового типа лексем (приведение типов)
    if (firstType == secondType) {
        resultType = firstType;
        this->global->operandTypes.push(resultType);
    }
    else {
        resultType = max(firstType, secondType);
        this->global->operandTypes.push(resultType);
    }
}

void GenerTriad::generateTriad(TypeLex operationType) {
    Operand secondOperand = this->global->operands.top();
    this->global->operands.pop();
    Operand firstOperand = this->global->operands.top();
    this->global->operands.pop();

    // Семантические проверки
    // Проверка на изменение констант
    if (!firstOperand.isLink && strcmp(operationType, (char*)"=") == 0) {
        // Попробуем найти в дереве, если это идентификатор
        Tree* pos = this->tree->findUp(this->tree->cur, firstOperand.lex);
        if (pos != nullptr)
            this->tree->checkConst(firstOperand.lex);
    }
    // Инициализация переменной
    if (!secondOperand.isLink) {
        // Попробуем найти в дереве, если это идентификатор
        Tree* pos = this->tree->findUp(this->tree->cur, secondOperand.lex);
        if (pos != nullptr)
            this->tree->checkInit(secondOperand.lex);
    }

    // Генерация триады по операции
    Triad newTriad;
    memcpy(newTriad.operation, operationType, sizeof(operationType));
    newTriad.firstOperand = firstOperand;
    newTriad.secondOperand = secondOperand;
    this->global->resultTriads.push_back(newTriad);

    // Генерация операнда в виде триады
    Operand triadOperand;
    triadOperand.isConst = false;
    triadOperand.isLink = true;
    triadOperand.number = this->generationIndex++;
    this->global->operands.push(triadOperand);
}

void GenerTriad::deltaPushOperand(bool isConst) {
    // Запоминаем тип лексемы (или константы)
    DATA_TYPE type = this->tree->getType(this->global->prevLex);
    this->global->operandTypes.push(type);

    // Запоминаем лексему как операнд
    Operand newOperand;
    newOperand.isLink = false;
    if (isConst) {
        newOperand.isConst = true;
    }
    else
        newOperand.isConst = false;
    memcpy(newOperand.lex, this->global->prevLex, sizeof(this->global->prevLex));
    // Запоминаем тип структуры, если лексема из структуры
    if (this->global->structPtr != nullptr) {
        newOperand.ptrTree = this->tree->semanticGetIdentifierInStruct(this->global->structPtr, this->global->prevLex);
        memcpy(newOperand.structLex, this->global->structIdent, sizeof(this->global->structIdent));
    }
    else
        newOperand.ptrTree = nullptr;
    this->global->operands.push(newOperand);
}

void GenerTriad::deltaMatchLeft() {
    this->deltaMatch();
    this->generateTriad((char*) "=");
}

void GenerTriad::deltaMatchBitOr() {
    this->deltaMatch();
    this->generateTriad((char*) "|");
}

void GenerTriad::deltaMatchBitXor() {
    this->deltaMatch();
    this->generateTriad((char*) "^");
}

void GenerTriad::deltaMatchBitAnd() {
    this->deltaMatch();
    this->generateTriad((char*) "&");
}

void GenerTriad::deltaMatchComparison() {
    this->deltaMatch();
    this->generateTriad(this->global->comparisonSign);
}

void GenerTriad::deltaMatchAdd() {
    this->deltaMatch();
    this->generateTriad((char*) "+");
}

void GenerTriad::deltaMatchSub() {
    this->deltaMatch();
    this->generateTriad((char*) "-");
}

void GenerTriad::deltaMatchMul() {
    this->deltaMatch();
    this->generateTriad((char*) "*");
}

void GenerTriad::deltaMatchDiv() {
    this->deltaMatch();
    this->generateTriad((char*) "/");
}

void GenerTriad::deltaMatchDivPart() {
    this->deltaMatch();
    this->generateTriad((char*) "%");
}

void GenerTriad::printTriads() {
    printf("Triads:\n");
    for (int i = 0; i < this->global->resultTriads.size(); i++) {
        Triad triad = this->global->resultTriads[i];
        printf("(%d): %s ", i, triad.operation);
        if (triad.firstOperand.number != -220) {
            if (triad.firstOperand.isLink)
                printf("(%d) ", triad.firstOperand.number);
            else {
                std::string outputLex = "";
                if (triad.firstOperand.ptrTree == nullptr)
                    outputLex = triad.firstOperand.lex;
                else {
                    // Значит лексема находится в структуре
                    outputLex = this->tree->generateFullLexFromStruct(
                            triad.firstOperand.ptrTree,
                            triad.firstOperand.lex
                    );
                    outputLex = triad.firstOperand.structLex + outputLex;
                }
                printf("%s ", outputLex.c_str());
            }
        }

        if (triad.secondOperand.number != -220) {
            if (triad.secondOperand.isLink)
                printf("(%d)", triad.secondOperand.number);
            else {
                std::string outputLex = "";
                if (triad.secondOperand.ptrTree == nullptr)
                    outputLex = triad.secondOperand.lex;
                else {
                    // Значит лексема находится в структуре
                    outputLex = this->tree->generateFullLexFromStruct(
                            triad.secondOperand.ptrTree,
                            triad.secondOperand.structLex
                    );
                }
                printf("%s", outputLex.c_str());
            }
        }
        printf("\n");
    }
    printf("\n");
}

void GenerTriad::deltaGenerateIf() {
    Triad ifTriad;
    memcpy(ifTriad.operation, "if", sizeof("if"));

    ifTriad.firstOperand.isLink = true;
    ifTriad.firstOperand.isConst = false;
    ifTriad.firstOperand.number = this->generationIndex + 1; // указывает на следующий после себя операнд

    ifTriad.secondOperand.isLink = true;
    ifTriad.secondOperand.isConst = false;
    ifTriad.secondOperand.number = -1; // укажем при deltaFormForCycle()

    this->global->ifMarkers.push(this->generationIndex++);
    this->global->resultTriads.push_back(ifTriad);
}

void GenerTriad::deltaGenerateGoTo() {
    Triad gotoTriad;
    memcpy(gotoTriad.operation, "goto", sizeof("goto"));

    gotoTriad.firstOperand.isLink = true;
    gotoTriad.firstOperand.isConst = false;
    gotoTriad.firstOperand.number = -1; // укажем при deltaFormForCycle()

    gotoTriad.secondOperand.isLink = true;
    gotoTriad.secondOperand.isConst = false;
    gotoTriad.secondOperand.number = -220; // неиспользуемая операнда

    this->global->gotoMarkers.push(this->generationIndex++);
    this->global->resultTriads.push_back(gotoTriad);
}

void GenerTriad::deltaGenerateNOP() {
    Triad nopTriad;
    this->generationIndex++;
    memcpy(nopTriad.operation, "nop", sizeof("nop"));

    nopTriad.firstOperand.isLink = true;
    nopTriad.firstOperand.isConst = false;
    nopTriad.firstOperand.number = -220; // неиспользуемая операнда

    nopTriad.secondOperand.isLink = true;
    nopTriad.secondOperand.isConst = false;
    nopTriad.secondOperand.number = -220; // неиспользуемая операнда

    this->global->resultTriads.push_back(nopTriad);
}

void GenerTriad::deltaSetAddrForMarkers() {
    // Запоминаем номер триады для последующего заполнения триад при deltaFormForCycle()
    this->global->addrMarkers.push(this->generationIndex);
}

void GenerTriad::deltaFormForCycle() {
    // Достаем адреса-номера триад
    // n + 3. Номер триады на тело цикла for
    // n + 2. Номер триады на 2 выражение цикла for (инкремент)
    // n + 1. Номер триады на 1 выражение цикла for (условие выполнения)
    int bodyCycleTriadNumber = this->global->addrMarkers.top();
    this->global->addrMarkers.pop();
    int secondExpressionTriadNumber = this->global->addrMarkers.top();
    this->global->addrMarkers.pop();
    int firstExpressionTriadNumber = this->global->addrMarkers.top();
    this->global->addrMarkers.pop();

    // Триада ветвления для 1 выражения цикла for
    int ifTriadIndex = this->global->ifMarkers.top();
    this->global->ifMarkers.pop();
    // Указываем точкой выхода на триаду NOP
    this->global->resultTriads[ifTriadIndex].secondOperand.number = this->generationIndex - 1;

    // Достаем триады GOTO
    // n + 3. Триада GOTO на 2 выражение цикла for
    // n + 2. Триада GOTO на 1 выражение цикла for
    // n + 1. Триада GOTO на тело выполнения цикла for
    int gotoSecondExpressionTriadNumber = this->global->gotoMarkers.top();
    this->global->gotoMarkers.pop();
    this->global->resultTriads[gotoSecondExpressionTriadNumber].firstOperand.number = secondExpressionTriadNumber;

    int gotoFirstExpressionTriadNumber = this->global->gotoMarkers.top();
    this->global->gotoMarkers.pop();
    this->global->resultTriads[gotoFirstExpressionTriadNumber].firstOperand.number = firstExpressionTriadNumber;

    int gotoCycleBodyTriadNumber = this->global->gotoMarkers.top();
    this->global->gotoMarkers.pop();
    this->global->resultTriads[gotoCycleBodyTriadNumber].firstOperand.number = bodyCycleTriadNumber;
}
