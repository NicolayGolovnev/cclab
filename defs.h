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
#define TypeVarsNonTerm 117
#define TypeEndofVarList 118
#define TypeMayEqualNonTerm 119
#define TypeConstVarsNonTerm 120
#define TypeEndofConstVarList 121
#define TypeCompoundOperatorNonTerm 122
#define TypeCompoundBodyNonTerm 123
#define TypeOperatorNonTerm 124
#define TypeSimpleOperatorNonTerm 125
#define TypeForNonTerm 126
#define TypeEqualNonTerm 127
#define TypeExprNonTerm 128
#define TypeEndofExprNonTerm 129
#define TypeXorBitNonTerm 130
#define TypeEndofXorNonTerm 131
#define TypeAndBitNonTerm 132
#define TypeEndofAndNonTerm 133
#define TypeComparisonNonTerm 134
#define TypeEndofComparisonNonTerm 135
#define TypeCompSignNonTerm 136
#define TypeAdditierNonTerm 137
#define TypeEndofAdditierNonTerm 138
#define TypeMultiplierNonTerm 139
#define TypeEndofMultiplierNonTerm 140
#define TypeBasicExprNonTerm 141
#define TypePreIdentNonTerm 142
#define TypeEndofIdentNonTerm 143

#define TypeConstTypeNonTerm 150

#define TypeErr 100
#define TypeEnd 200

#endif //ANALYZATOR_DEFS_H
