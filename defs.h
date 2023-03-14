#ifndef ANALYZATOR_DEFS_H
#define ANALYZATOR_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Максимальный размер текста программы
#define MAX_TEXT 10000
// Максимальная длина лексемы
#define MAX_LEX	100
// Число ключевых слов
#define MAX_KEYWORDS 11
typedef char TypeLex[MAX_LEX];

#define TypeIdent 1

// Ключевые слова
#define TypeMain 10
#define TypeFor 11
#define TypeStruct 12
#define TypeConst 13
#define TypeInt 14
#define TypeShort 15
#define TypeLong 16
#define TypeDouble 17
#define TypeReturn 18

// Знаки операций
#define TypeBitOr 20
#define TypeBitXor 21
#define TypeBitAnd 22
#define TypeEqual 23
#define TypeNotEqual 24
#define TypeLess 25
#define TypeLessOrEqual 26
#define TypeMore 27
#define TypeMoreOrEqual 28
#define TypeAdd 29
#define TypeSub 30
#define TypeMul 31
#define TypeDiv 32
#define TypeDivPart 33
#define TypeAssign 34

// Константы
#define TypeConstInt 35
#define TypeConstHex 36
#define TypeConstExp 37

// Разделители
#define TypeLeftRB 40 // (
#define TypeRightRB 41 // )
#define TypeLeftFB 42 // {
#define TypeRightFB 43 // }
#define TypeComma 44 // ,
#define TypeEndComma 45 // ;
#define TypeDot 46 // .

// От 110 до 190 - правила нетерминалов
#define TypeProgNonTerm 110
#define TypeDescriptionNonTerm 111
#define TypeStructNonTerm 112
#define TypeStructVarsNonTerm 113
#define TypeTypeNonTerm 114
#define TypeMainOrVarsNonTerm 115
#define TypeMainNonTerm 116
#define TypeEndofVarList 117
#define TypeMayEqualNonTerm 118
#define TypeVarsNonTerm 119
#define TypeCompoundOperatorNonTerm 120
#define TypeCompoundBodyNonTerm 121
#define TypeOperatorNonTerm 122
#define TypeSimpleOperatorNonTerm 123
#define TypeForNonTerm 124
#define TypeEqualNonTerm 125
#define TypeExprNonTerm 126
#define TypeEndofExprNonTerm 127
#define TypeXorBitNonTerm 128
#define TypeEndofXorNonTerm 129
#define TypeAndBitNonTerm 130
#define TypeEndofAndNonTerm 131
#define TypeComparisonNonTerm 132
#define TypeEndofComparisonNonTerm 133
#define TypeCompSignNonTerm 134
#define TypeAdditierNonTerm 135
#define TypeEndofAdditierNonTerm 136
#define TypeMultiplierNonTerm 137
#define TypeEndofMultiplierNonTerm 138
#define TypeBasicExprNonTerm 139
#define TypePreIdentNonTerm 140
#define TypeEndofIdentNonTerm 141

#define TypeErr 100
#define TypeEnd 200

#endif //ANALYZATOR_DEFS_H
