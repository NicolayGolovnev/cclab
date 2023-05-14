//
// Created by kolya on 08.05.2023.
//

#ifndef MAG_ANALYZATOR_GENERTRIAD_H
#define MAG_ANALYZATOR_GENERTRIAD_H


#include "../semantic/Semantic.h"
#include "TriadHelper.h"

class GenerTriad {
private:
    Tree* tree;
    GlobalData* global;
    int generationIndex = 0; // Индекс первой генерируемой триады
    std::stack<Triad*> triads; // Триады

    void deltaMatch();

    void generateTriad(TypeLex operationType);

    int getResultOfOperation(TypeLex operation, int firstValue, int secondValue);

public:
    GenerTriad();
    void setTree(Tree* tree);
    void setGlobal(GlobalData* global);

    void deltaRememberComparisonSign();

    void deltaPushOperand(bool isConst);

    void deltaMatchLeft(); // assign operation
    void deltaMatchBitOr();
    void deltaMatchBitXor();
    void deltaMatchBitAnd();
    void deltaMatchComparison();
    void deltaMatchAdd();
    void deltaMatchSub();
    void deltaMatchMul();
    void deltaMatchDiv();
    void deltaMatchDivPart();

    void deltaGenerateIf();
    void deltaGenerateGoTo();
    void deltaGenerateNOP();
    void deltaSetAddrForMarkers();
    void deltaFormForCycle();

    void deltaOptimizeExpressionTriads();

    void printTriads();
};


#endif //MAG_ANALYZATOR_GENERTRIAD_H
