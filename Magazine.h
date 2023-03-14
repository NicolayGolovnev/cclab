//
// Created by kolya on 09.03.2023.
//

#ifndef MAG_ANALYZATOR_MAGAZINE_H
#define MAG_ANALYZATOR_MAGAZINE_H

// Размер магазина
#include "defs.h"
#include "Scaner.h"

#define MAX_LEN_MAG 1000

enum TypeSymb {
    Prog, Description,
};

struct MagOneSymb {
    bool term;
    int typeSymb; // Терминалы и нетерминалы в defs.h
    TypeLex lex;
};

class Magazine {
private:
    Scaner *scanner;

    MagOneSymb magazine[MAX_LEN_MAG];
    int curMagPtr = 0; // Указатель на верхушку магазина

    void ptrUp();
    void ptrDown();
    void setLexFromScanner(TypeLex iLex);

    void epsilon();
    void progRule();
    void descriptionRule1();
    void descriptionRule2();
    void descriptionRule3();

    void structRule();
    void structVarsRule();
    void typeRuleInt();
    void typeRuleShort();
    void typeRuleLong();
    void typeRuleDouble();
    void typeRuleIdent();
    void mainOrVarsRule1();
    void mainOrVarsRule2();
    void mainRule();
    void endofVarListRule();
    void mayEqualRule();
    void varsRule();

    void compoundOperatorRule();
    void compoundBodyRule1();
    void compoundBodyRule2();

    void operatorRule1();
    void operatorRule2();
    void operatorRule3();
    void simpleOperatorRule1();
    void simpleOperatorRule2();
    void forRule();
    void equalRule();

    void exprRule();
    void endofExprRule();
    void xorBitRule();
    void endofXorRule();
    void andBitRule();
    void endofAndRule();
    void comparisonRule();
    void endofComparisonRule();
    void compSignE();
    void compSignNE();
    void compSignG();
    void compSignGOE();
    void compSignL();
    void compSignLOE();
    void additierRule();
    void endofAdditierRule1();
    void endofAdditierRule2();
    void multiplierRule();
    void endofMultiplierRule1();
    void endofMultiplierRule2();
    void endofMultiplierRule3();
    void basicExprRule1();
    void basicExprRule2Int();
    void basicExprRule2Hex();
    void basicExprRule2Exp();
    void basicExprRule3();

    void preIdentRule();
    void endofIdentRule();

    int lookForward(int toForward);

public:
    Magazine(char *filename);

    void run();
    void analyzeNonTerm(int lexType);
};


#endif //MAG_ANALYZATOR_MAGAZINE_H
