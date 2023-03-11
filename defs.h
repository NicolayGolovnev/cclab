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
#define TypeTypeNonTerm 113
#define TypeMainOrVarsNonTerm 114
#define TypeMainNonTerm 115
#define TypeEndofVarList 116
#define TypeMayEqualNonTerm 117
#define TypeCompoundOperatorNonTerm 118
#define TypeCompoundBodyNonTerm 119
#define TypeOperatorNonTerm 120
#define TypeSimpleOperatorNonTerm 121
#define TypeForNonTerm 122
#define TypeEqualNonTerm 123
#define TypeExprNonTerm 124
#define TypeEndofExprNonTerm 125
#define TypeXorBitNonTerm 126
#define TypeEndofXorNonTerm 127
#define TypeAndBitNonTerm 128
#define TypeEndofAndNonTerm 129
#define TypeComparisonNonTerm 130
#define TypeEndofComparisonNonTerm 131
#define TypeCompSignNonTerm 132
#define TypeAdditierNonTerm 133
#define TypeEndofAdditierNonTerm 134
#define TypeMultiplierNonTerm 135
#define TypeEndofMultiplierNonTerm 136
#define TypeBasicExprNonTerm 137
#define TypePreIdentNonTerm 138
#define TypeEndofIdentNonTerm 139

#define TypeErr 100
#define TypeEnd 200

#endif //ANALYZATOR_DEFS_H
