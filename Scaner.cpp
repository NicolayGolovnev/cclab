#include "defs.h"
#include "Scaner.h"

#define isNumber (text[pos] <= '9') && (text[pos] >= '0')
#define isHexCase ((text[pos] >= 'a') && (text[pos] <= 'f')) || ((text[pos] >= 'A') && (text[pos] <= 'F'))
#define lowerCase (text[pos] >= 'a') && (text[pos] <= 'z')
#define upperCase (text[pos] >= 'A') && (text[pos] <= 'Z')

TypeLex keyword[MAX_KEYWORDS] = { "main", "for", "struct", "const",
                              "int", "short", "long", "double", "return"};

int indexKeyword[MAX_KEYWORDS] = {TypeMain, TypeFor, TypeStruct, TypeConst,
                              TypeInt, TypeShort, TypeLong, TypeDouble, TypeReturn};

Scaner::Scaner(char* fileName)
{
    getData(fileName);
    setPos(0);
    newLinePos = 0;
    line = 0;
}

void Scaner::printError(char* error, char* lex, int _line, int _pos) {
    if ((line == -1) && (pos == -1)) {
        if (lex[0] == '\0')
            printf("Error: %s\n", error);
        else
            printf("Error: %s. Wrong symbol %s\n", error, lex);
    }
    else {
        if (lex[0] == '\0')
            printf("Error: %s, symbol %s in line %d, position %d\n", error, lex, _line + 1, _pos + 1);
        else
            printf("Error: %s. Wrong symbol %s in line %d, position %d\n", error, lex, _line + 1, _pos + 1);
    }
    exit(-220);
}

int Scaner::scan(TypeLex lex) {
    // Текущая длина лексемы
    int i;
    // Сообщение об ошибке
    char* errMessage;

    // Чистим лексему
    for (i = 0; i < MAX_LEX; i++)
        lex[i] = 0;
    i = 0;

    ignorable:
    // Игнорируемые символы
    while ((text[pos] == ' ') || (text[pos] == '\n') || (text[pos] == '\t')) {
        if (text[pos] == '\n') {
            line++;
            newLinePos = pos + 1;
        }
        pos++;
    }

    // Комментарий
    if ((text[pos] == '/') && (text[pos + 1] == '/')) {
        pos += 2;
        while (text[pos] != '\n') {
            if (text[pos] == '\0')
                goto exitScan;
            pos++;
        }
        goto ignorable;
    }
exitScan:
    // Конец программы
    if (text[pos] == '\0') {
        lex[0] = '#';
        return TypeEnd;
    }

    // Константы
    if (isNumber) {
        lex[i++] = text[pos++];
        // hex
        if ((text[pos - 1] == '0') && (text[pos] == 'x')) {
            lex[i++] = text[pos++];
            if (!(isNumber || isHexCase)) {
                errMessage = const_cast<char*>("Wrong const in hex-format");
                lex[i++] = text[pos++];
                printError(errMessage, lex, line, pos - newLinePos);
                return TypeErr;
            }
            while (isNumber || isHexCase)
                if (i < MAX_LEX - 1)
                    lex[i++] = text[pos++];
                else {
                    errMessage = const_cast<char*>("Hex const is too long");
                    printError(errMessage, lex, line, pos - newLinePos);
                    while (isNumber || isHexCase)
                        pos++;
                    return TypeErr;
                }
            return TypeConstHex;
        }
        while(isNumber)
            if (i < MAX_LEX - 1)
                lex[i++] = text[pos++];
            else {
                errMessage = const_cast<char *>("Int const is too long");
                printError(errMessage, lex, line, pos - newLinePos);
                while (isNumber)
                    pos++;
                return TypeErr;
            }
        // expNum
        if ((text[pos + 1] == 'E') || (text[pos + 1] == 'e')) {
            lex[i++] = text[pos++];
            goto expConst;
        }
        else // const int
            return TypeConstInt;
    }
    // Идентификатор или ключевое слово
    else if (lowerCase || upperCase || text[pos] == '_') {
        lex[i++] = text[pos++];
        while (isNumber || lowerCase || upperCase || text[pos] == '_')
            if (i < MAX_LEX - 1)
                lex[i++] = text[pos++];
            else
                pos++;

        // Проверяем ключевые слова
        int j;
        for (j = 0; j < MAX_KEYWORDS; j++)
            if (strcmp(lex, keyword[j]) == 0)
                return indexKeyword[j];
        return TypeIdent;
    }
    else if (text[pos] == '|') {
        lex[i++] = text[pos++];
        return TypeBitOr;
    }
    else if (text[pos] == '^') {
        lex[i++] = text[pos++];
        return TypeBitXor;
    }
    else if (text[pos] == '&') {
        lex[i++] = text[pos++];
        return TypeBitAnd;
    }
    else if (text[pos] == '!' && text[pos + 1] == '=') {
        lex[i++] = text[pos++];
        lex[i++] = text[pos++];
        return TypeNotEqual;
    }
    else if (text[pos] == '=') {
        lex[i++] = text[pos++];
        if (text[pos] == '=') {
            lex[i++] = text[pos++];
            return TypeEqual;
        }
        else
            return TypeAssign;
    }
    else if (text[pos] == '<') {
        lex[i++] = text[pos++];
        if (text[pos] == '=') {
            lex[i++] = text[pos++];
            return TypeLessOrEqual;
        }
        else
            return TypeLess;
    }
    else if (text[pos] == '>') {
        lex[i++] = text[pos++];
        if (text[pos] == '=') {
            lex[i++] = text[pos++];
            return TypeMoreOrEqual;
        }
        else
            return TypeMore;
    }
    else if (text[pos] == '+') {
        lex[i++] = text[pos++];
        return TypeAdd;
    }
    else if (text[pos] == '-') {
        lex[i++] = text[pos++];
        return TypeSub;
    }
    else if (text[pos] == '*') {
        lex[i++] = text[pos++];
        return TypeMul;
    }
    else if (text[pos] == '/') {
        lex[i++] = text[pos++];
        return TypeDiv;
    }
    else if (text[pos] == '%') {
        lex[i++] = text[pos++];
        return TypeDivPart;
    }
    else if (text[pos] == ',') {
        lex[i++] = text[pos++];
        return TypeComma;
    }
    else if (text[pos] == ';') {
        lex[i++] = text[pos++];
        return TypeEndComma;
    }
    else if (text[pos] == '(') {
        lex[i++] = text[pos++];
        return TypeLeftRB;
    }
    else if (text[pos] == ')') {
        lex[i++] = text[pos++];
        return TypeRightRB;
    }
    else if (text[pos] == '{') {
        lex[i++] = text[pos++];
        return TypeLeftFB;
    }
    else if (text[pos] == '}'){
        lex[i++] = text[pos++];
        return TypeRightFB;
    }
    else if (text[pos] == '.'){
        lex[i++] = text[pos++];
        return TypeDot;
    }
    else {
        errMessage = const_cast<char*>("Wrong symbol");
        lex[i] = text[pos];
        printError(errMessage, lex, line, pos - newLinePos);
        pos++;
        return TypeErr;
    }
expConst:
    if ((text[pos] == '+') || (text[pos] == '-')) {
        lex[i++] = text[pos++];
        if (isNumber)
        {
            while (isNumber)
                if (i < MAX_LEX - 1)
                    lex[i++] = text[pos++];
                else
                    pos++;
            return TypeConstExp;
        }
        else {
            errMessage = const_cast<char*>("Wrong symbol in exp const");
            printError(errMessage, lex, line, pos - newLinePos);
            return TypeErr;
        }
    }
    else {
        errMessage = const_cast<char*>("Wrong symbol in exp const");
        printError(errMessage, lex, line, pos - newLinePos);
        return TypeErr;
    }
}

void Scaner::getData(char* fileName) {
    // Ввод названия файла, из которого будет читать анализируемый текст
    char sym;
    char* empty = const_cast<char*>("");
    char* errMessage;

    FILE* in = fopen(fileName, "r");

    if (in == NULL) {
        errMessage = const_cast<char*>("Couldn't find an input file");
        printError(errMessage, empty, -1, -1);
        exit(1);
    }

    int i = 0;

    while (!feof(in))
    {
        fscanf(in, "%c", &sym);
        if (!feof(in))
            text[i++] = sym;

        if (i >= MAX_TEXT - 1)
        {
            errMessage = const_cast<char*>("Exceeding the size of the program text");
            printError(errMessage, empty, -1, -1);
            break;
        }
    }
    text[i] = '\0'; // Дописываем '\0' в конец текста
    fclose(in);
}

// Устанавливаем указатель на позицию i
void Scaner::setPos(int i) { pos = i; }

// Получаем значение указателя
int Scaner::getPos() { return pos; }

int Scaner::getLine() { return line; }

void Scaner::setLine(int inputLine) { line = inputLine; }

int Scaner::getNewLinePos() { return newLinePos; }

void Scaner::setNewLinePos(int inputLine) { newLinePos = inputLine; }
