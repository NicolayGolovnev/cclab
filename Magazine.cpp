//
// Created by kolya on 09.03.2023.
//

#include "Magazine.h"
#include <iostream>

#define isConst(type) ((type == TypeConstInt) || (type == TypeConstHex) || (type == TypeConstExp))
#define isType(type) ((type == TypeInt) || (type == TypeShort) || (type == TypeLong) || (type == TypeDouble) || (type == TypeIdent))
#define isComparisonSign(type) ((type == TypeEqual) || (type == TypeNotEqual) || (type == TypeLess) || (type == TypeLessOrEqual) || (type == TypeMore) || (type == TypeMoreOrEqual))

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
    this->magazine[this->curMagPtr].typeSymb = TypeEnd;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeProgNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    TypeLex lex;
    int type = this->scanner->scan(lex);
    while (flag) {
        if (this->magazine[this->curMagPtr - 1].term) {
            if (this->magazine[this->curMagPtr - 1].typeSymb == type) {
                // Тип в магазине совпадает с отсканированным типом
                if (type == TypeEnd) flag = 0;
                else {
                    type = this->scanner->scan(lex);
                    this->ptrDown();
                }
            } else {
                std::string errorMsg = "Wrong symbol - expected another lexeme with type " + std::to_string(type);
                this->scanner->printError(
                        const_cast<char *>(errorMsg.c_str()), lex
                );
            }


        } else
            this->analyzeNonTerm(type, lex);
    }
}

void Magazine::analyzeNonTerm(int lexType, char *lex) {
    switch (this->magazine[this->curMagPtr - 1].typeSymb) {
        case TypeProgNonTerm:
            if (lexType == TypeStruct || lexType == TypeConst || isType(lexType))
                progRule();
            else if (lexType == TypeEnd) epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected description or end of program"),
                    lex
                    );
            break;
        case TypeDescriptionNonTerm:
            if (lexType == TypeStruct)
                descriptionRule1();
            else if (isType(lexType))
                descriptionRule2();
            else if (lexType == TypeConst)
                descriptionRule3();
            else this->scanner->printError(
                        const_cast<char *>("Expected description of structure, const-vars, vars or main-function"),
                        lex
                        );
            break;
        case TypeStructNonTerm:
            structRule();
            break;
        case TypeStructVarsNonTerm:
            if (isType(lexType) || lexType == TypeConst)
                structVarsRule();
            else if (lexType == TypeRightFB)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected vars description of structure"),
                    lex
                    );
            break;
        case TypeTypeNonTerm:
            if (isType(lexType)) {
                if (lexType == TypeInt)
                    typeRuleInt();
                else if (lexType == TypeShort)
                    typeRuleShort();
                else if (lexType == TypeLong)
                    typeRuleLong();
                else if (lexType == TypeDouble)
                    typeRuleDouble();
                else if (lexType == TypeIdent)
                    typeRuleIdent();
                else this->scanner->printError(
                        const_cast<char *>("Expected correct type of var or structure"),
                        lex
                        );
            } else this->scanner->printError(
                    const_cast<char *>("Expected type \'int\', \'short\', \'long\', \'double\' or identifier of a structure"),
                    lex
                    );
            break;
        case TypeMainOrVarsNonTerm:
            if (lexType == TypeMain)
                mainOrVarsRule1();
            else if (lexType == TypeIdent)
                mainOrVarsRule2();
            else this->scanner->printError(
                    const_cast<char *>("Expected description of main-function or var-list"),
                    lex
                    );
            break;
        case TypeMainNonTerm:
            mainRule();
            break;
        case TypeEndofVarList:
            if (lexType == TypeComma)
                endofVarListRule();
            else if (lexType == TypeEndComma)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected symbol \';\' or continue of var-list"),
                    lex
                    );
            break;
        case TypeMayEqualNonTerm:
            if (lexType == TypeAssign)
                mayEqualRule();
            else if (lexType == TypeComma || lexType == TypeEndComma)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected operation of assign"),
                    lex
                    );
            break;
        case TypeVarsNonTerm:
            if (isType(lexType))
                varsRule1();
            else if (lexType == TypeConst)
                varsRule2();
            break;
        case TypeCompoundOperatorNonTerm:
            compoundOperatorRule();
            break;
        case TypeCompoundBodyNonTerm:
            // TODO поставить lookForward(1) - так как оператор присваивания и описание переменных начинаются с идентификатора (может быть типом)
            if ((isType(lexType) || lexType == TypeConst) &&
                (this->lookForward(1) != TypeAssign && this->lookForward(1) != TypeDot))
                compoundBodyRule1();
            else if (lexType == TypeEndComma || lexType == TypeFor || lexType == TypeIdent || lexType == TypeLeftFB)
                compoundBodyRule2();
            else if (lexType == TypeRightFB)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected vars or operators in compound body"),
                    lex
                    );
            break;
        case TypeOperatorNonTerm:
            if (lexType == TypeFor || lexType == TypeIdent)
                operatorRule1();
            else if (lexType == TypeLeftFB)
                operatorRule2();
            else if (lexType == TypeEndComma)
                operatorRule3();
            else this->scanner->printError(
                    const_cast<char *>("Expected simple or compound operators"),
                    lex
                    );
            break;
        case TypeSimpleOperatorNonTerm:
            if (lexType == TypeFor)
                simpleOperatorRule1();
            else if (lexType == TypeIdent)
                simpleOperatorRule2();
            else this->scanner->printError(
                    const_cast<char *>("Expected for or equal operators"),
                    lex
                    );
            break;
        case TypeForNonTerm:
            forRule();
            break;
        case TypeEqualNonTerm:
            equalRule();
            break;
        case TypeExprNonTerm:
            exprRule();
            break;
        case TypeEndofExprNonTerm:
            if (lexType == TypeBitOr)
                endofExprRule();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected or-bit operator"),
                    lex
                    );
            break;
        case TypeXorBitNonTerm:
            xorBitRule();
            break;
        case TypeEndofXorNonTerm:
            if (lexType == TypeBitXor)
                endofXorRule();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB || lexType == TypeBitOr)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected xor-bit operator"),
                    lex
                    );
            break;
        case TypeAndBitNonTerm:
            andBitRule();
            break;
        case TypeEndofAndNonTerm:
            if (lexType == TypeBitAnd)
                endofAndRule();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB
                || lexType == TypeBitOr || lexType == TypeBitXor)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected and-bit operator"),
                    lex
                    );
            break;
        case TypeComparisonNonTerm:
            comparisonRule();
            break;
        case TypeEndofComparisonNonTerm:
            if (isComparisonSign(lexType))
                endofComparisonRule();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB
                || lexType == TypeBitAnd || lexType == TypeBitXor || lexType == TypeBitOr)
                epsilon();
            else this->scanner->printError(
                        const_cast<char *>("Expected comparison operator"),
                        lex
                );
            break;
        case TypeCompSignNonTerm:
            if (lexType == TypeEqual)
                compSignE();
            else if (lexType == TypeNotEqual)
                compSignNE();
            else if (lexType == TypeLess)
                compSignL();
            else if (lexType == TypeLessOrEqual)
                compSignLOE();
            else if (lexType == TypeMore)
                compSignG();
            else if (lexType == TypeMoreOrEqual)
                compSignGOE();
            else this->scanner->printError(
                        const_cast<char *>("Expected comparison operator (==, !=, <, <=, >, >=)"),
                        lex
                );
            break;
        case TypeAdditierNonTerm:
            additierRule();
            break;
        case TypeEndofAdditierNonTerm:
            if (lexType == TypeAdd)
                endofAdditierRule1();
            else if (lexType == TypeSub)
                endofAdditierRule2();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB
                || lexType == TypeBitAnd || lexType == TypeBitXor || lexType == TypeBitOr || isComparisonSign(lexType))
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected add or sub operators"),
                    lex
                    );
            break;
        case TypeMultiplierNonTerm:
            multiplierRule();
            break;
        case TypeEndofMultiplierNonTerm:
            if (lexType == TypeMul)
                endofMultiplierRule1();
            else if (lexType == TypeDiv)
                endofMultiplierRule2();
            else if (lexType == TypeDivPart)
                endofMultiplierRule3();
            else if (lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB
                || lexType == TypeBitAnd || lexType == TypeBitXor || lexType == TypeBitOr
                || isComparisonSign(lexType) || lexType == TypeAdd || lexType == TypeSub)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected mul, div or div-part operators"),
                    lex
                    );
            break;
        case TypeBasicExprNonTerm:
            if (lexType == TypeIdent)
                basicExprRule1();
            else if (isConst(lexType)) {
                if (lexType == TypeConstExp)
                    basicExprRule2Exp();
                else if (lexType == TypeConstHex)
                    basicExprRule2Hex();
                else if (lexType == TypeConstInt)
                    basicExprRule2Int();
            } else if (lexType == TypeLeftRB)
                basicExprRule3();
            else this->scanner->printError(
                    const_cast<char *>("Expected identifier, constant or high-privilege expression"),
                    lex
                    );
            break;
        case TypePreIdentNonTerm:
            preIdentRule();
            break;
        case TypeEndofIdentNonTerm:
            if (lexType == TypeDot)
                endofIdentRule();
            else if (lexType == TypeAssign || lexType == TypeEndComma || lexType == TypeComma || lexType == TypeRightRB
                || lexType == TypeBitAnd || lexType == TypeBitXor || lexType == TypeBitOr || isComparisonSign(lexType)
                || lexType == TypeAdd || lexType == TypeSub || lexType == TypeMul || lexType == TypeDiv || lexType == TypeDivPart)
                epsilon();
            else this->scanner->printError(
                    const_cast<char *>("Expected identifier of structure operator"),
                    lex
                    );
            break;
        default:
            this->scanner->printError(
                    const_cast<char*>("Unexpected type of non-terminal from MP-automaton"),
                    lex
                    );
    }
}

void Magazine::progRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeProgNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeDescriptionNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::descriptionRule1() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeStructNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
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

    this->magazine[this->curMagPtr].typeSymb = TypeStructVarsNonTerm;
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

void Magazine::structVarsRule() {
    this->ptrDown();

    this->magazine[this->curMagPtr].typeSymb = TypeStructVarsNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeVarsNonTerm;
    this->magazine[this->curMagPtr].term = false;
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

void Magazine::varsRule1() {
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

    this->magazine[this->curMagPtr].typeSymb = TypeTypeNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();
}

void Magazine::varsRule2() {
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

    this->magazine[this->curMagPtr].typeSymb = TypeTypeNonTerm;
    this->magazine[this->curMagPtr].term = false;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeConst;
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

    this->magazine[this->curMagPtr].typeSymb = TypeVarsNonTerm;
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

    this->magazine[this->curMagPtr].typeSymb = TypeLeftRB;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();

    this->magazine[this->curMagPtr].typeSymb = TypeFor;
    this->magazine[this->curMagPtr].term = true;
    this->ptrUp();
}

void Magazine::equalRule() {
    this->ptrDown();

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

    this->magazine[this->curMagPtr].typeSymb = TypeBitOr;
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

    this->magazine[this->curMagPtr].typeSymb = TypeAdditierNonTerm;
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