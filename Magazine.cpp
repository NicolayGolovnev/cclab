//
// Created by kolya on 09.03.2023.
//

#include "Magazine.h"
#include <iostream>

#define isConst(type) ((type == TypeConstInt) || (type == TypeConstHex) || (type == TypeConstExp))
#define isType(type) ((type == TypeInt) || (type == TypeShort) || (type == TypeLong) || (type == TypeDouble) || (type == TypeIdent))

Magazine::Magazine(char *filename) {
    this->scanner = new Scaner(filename);
    this->curMagPtr = 0;
}

void Magazine::ptrUp() {
    this->curMagPtr++;
}

void Magazine::ptrDown() {
    this->curMagPtr--;
}

void Magazine::setLexFromScanner(TypeLex iLex) {
    memcpy(this->magazine[this->curMagPtr].lex, iLex, sizeof(iLex));
//    memcpy(this->lex, iLex, sizeof(iLex));
}

int Magazine::lookForward(int toForward) {
    TypeLex localLex;
    int position = this->scanner->getPos();
    int line = this->scanner->getLine();
    int newLinePos = this->scanner->getNewLinePos();

    int nextType = TypeErr;
    for (int i = 0; i < toForward; i++)
        nextType = this->scanner->scan(localLex);
    this->scanner->setPos(position);
    this->scanner->setLine(line);
    this->scanner->setNewLinePos(newLinePos);

    return nextType;
}

// Чистим тип символа из магазина
void Magazine::epsilon() {
    this->curMagPtr--;
}

void Magazine::run() {
    int flag = 1;

}

void Magazine::analyzeNonTerm(int lexType) {
    switch (this->magazine[this->curMagPtr].typeSymb) {
        case TypeProgNonTerm:
            if (lexType == TypeStruct || lexType == TypeConst || isType(lexType))
                progRule();
            else if (lexType == TypeEnd) epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected description or end of program"),
                    this->magazine[this->curMagPtr].lex
                    );
            break;
        case TypeDescriptionNonTerm:
            break;
        case TypeStructNonTerm:
            break;
        case TypeTypeNonTerm:
            break;
        case TypeMainOrVarsNonTerm:
            break;
        case TypeMainNonTerm:
            break;
        case TypeEndofVarList:
            break;
        case TypeMayEqualNonTerm:
            break;
        case TypeCompoundOperatorNonTerm:
            break;
        case TypeCompoundBodyNonTerm:
            break;
        case TypeOperatorNonTerm:
            break;
        case TypeSimpleOperatorNonTerm:
            break;
        case TypeForNonTerm:
            break;
        case TypeEqualNonTerm:
            break;
        case TypeExprNonTerm:
            break;
        case TypeEndofExprNonTerm:
            break;
        case TypeXorBitNonTerm:
            break;
        case TypeEndofXorNonTerm:
            break;
        case TypeAndBitNonTerm:
            break;
        case TypeEndofAndNonTerm:
            break;
        case TypeComparisonNonTerm:
            break;
        case TypeEndofComparisonNonTerm:
            break;
        case TypeCompSignNonTerm:
            break;
        case TypeAdditierNonTerm:
            break;
        case TypeEndofAdditierNonTerm:
            break;
        case TypeMultiplierNonTerm:
            break;
        case TypeEndofMultiplierNonTerm:
            break;
        case TypeBasicExprNonTerm:
            break;
        case TypePreIdentNonTerm:
            break;
        case TypeEndofIdentNonTerm:
            break;
        default:
            this->scanner->printError(
                    const_cast<char*>("Unexpected type of non-terminal from MP-automaton"),
                    this->magazine[this->curMagPtr].lex
                    );
    }
}

void Magazine::progRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeProgNonTerm;
    this->magazine[this->curMagPtr].term = false;

    this->magazine[this->curMagPtr].typeSymb = TypeDescriptionNonTerm;
    this->magazine[this->curMagPtr].term = false;
}

void Magazine::descriptionRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeStructNonTerm;
    this->magazine[this->curMagPtr].term = false;
}

void Magazine::descriptionRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeMainOrVarsNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeTypeNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::descriptionRule3() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofVarList;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeTypeNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeConst;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::structRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeRightFB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMainOrVarsNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeLeftFB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeStruct;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::typeRuleInt() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeInt;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::typeRuleShort() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeShort;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::typeRuleLong() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeLong;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::typeRuleDouble() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeDouble;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::typeRuleIdent() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::mainOrVarsRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeMainNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::mainOrVarsRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofVarList;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMayEqualNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::mainRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeCompoundOperatorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeRightRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeLeftRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMain;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::endofVarListRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofVarList;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMayEqualNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::mayEqualRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAssign;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compoundOperatorRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeRightFB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeCompoundBodyNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeLeftFB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compoundBodyRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeCompoundBodyNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMainOrVarsNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::compoundBodyRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeCompoundBodyNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeOperatorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::operatorRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeSimpleOperatorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::operatorRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeCompoundOperatorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::operatorRule3() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::simpleOperatorRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeForNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::simpleOperatorRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEqualNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::forRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeOperatorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeRightRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEqualNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeEqualNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeTypeNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeRightRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeFor;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::equalRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndComma;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAssign;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypePreIdentNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::exprRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeXorBitNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofExprRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeXorBitNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeBitXor;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::xorBitRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofXorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAndBitNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofXorRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofXorNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAndBitNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeBitXor;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::andBitRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofAndNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeComparisonNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofAndRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofAndNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAndBitNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeBitAnd;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::comparisonRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofComparisonNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeComparisonNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofComparisonRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofComparisonNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeComparisonNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeCompSignNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::compSignE() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEqual;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compSignNE() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeNotEqual;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compSignG() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeMore;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compSignGOE() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeMoreOrEqual;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compSignL() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeLess;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::compSignLOE() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeLessOrEqual;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::additierRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofAdditierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofAdditierRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofAdditierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAdditierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAdd;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::endofAdditierRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofAdditierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeAdditierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeSub;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::multiplierRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeBasicExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::endofMultiplierRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMul;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::endofMultiplierRule2() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeDiv;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::endofMultiplierRule3() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeMultiplierNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeDivPart;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::basicExprRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypePreIdentNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::basicExprRule2Exp() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeConstExp;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::basicExprRule2Int() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeConstInt;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::basicExprRule2Hex() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeConstHex;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::basicExprRule3() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeRightRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeExprNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeLeftRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::preIdentRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofIdentNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::endofIdentRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeEndofIdentNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeIdent;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeDot;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}