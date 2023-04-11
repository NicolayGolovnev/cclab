//
// Created by kolya on 09.03.2023.
//

#ifndef MAG_ANALYZATOR_MAGAZINE_H
#define MAG_ANALYZATOR_MAGAZINE_H

#include <iostream>
#include "../defs.h"
#include "../scaner/Scaner.h"
#include "../translator/Translate.h"

// Размер магазина
#define MAX_LEN_MAG 1000

enum TypeSymb {
    Prog, Description,
};

struct MagOneSymb {
    bool term = false;
    bool operation = false;
    int typeSymb; // Терминалы, нетерминалы, операции в defs.h
    TypeLex lex;
};

class Magazine {
private:
    Scaner *scanner;
    Translate* translate;

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
    void structVarsRule1();
    void structVarsRule2();
    void typeRuleInt();
    void typeRuleShort();
    void typeRuleLong();
    void typeRuleDouble();
    void typeRuleIdent();
    void mainOrVarsRule1();
    void mainOrVarsRule2();
    void mainRule();
    void varsRule();
    void endofVarListRule();
    void mayEqualRule();
    void constVarsRule();
    void endofConstVarListRule();

    void compoundOperatorRule();
    void compoundBodyRule1();
    void compoundBodyRule2();
    void compoundBodyRule3();

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

    std::string parseType(int lexType);
    int lookForward(int toForward);

public:
    Magazine(char *filename);

    void run();
    void analyzeNonTerm(int lexType, char *lex);
    void analyzeOperation(int lexType, char *lex);
    // TODO print end tree func
};


#endif //MAG_ANALYZATOR_MAGAZINE_H
