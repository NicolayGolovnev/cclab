//
// Created by kolya on 12/1/2021.
//

#include "Semantic.h"

#define max(a, b) a > b ? a : b

#define isShort(num) (num >= -(short)32767 - 1 && num <= (short)32767)
#define isInt(num) (num >= -2147483647 - 1 && num <= 2147483647)
#define isLong(num) (num >= -(long)2147483647 - 1 && num <= (long)2147483647)


Tree *Tree::cur = (Tree *) nullptr;
bool Tree::flagInterpret = true;
std::vector<std::string> types = {"none", "short", "int", "long", "double", "type of struct"};

int numGenAsm = 0; // Икнрементируемый номер для генерации уникальных идентификаторов (для asmId)

Tree::Tree() {
    this->node = new Node();
    this->up = nullptr;
    this->left = nullptr;
    this->right = nullptr;
    memcpy(this->node, &("------"), sizeof(Node));
}

Tree::Tree(Tree *l, Tree *r, Tree *up, Node *data) {
    this->node = new Node();
    this->up = up;
    this->left = l;
    this->right = r;
    memcpy(this->node, data, sizeof(Node));
}

// Создание левого потомка от текущей вершины
void Tree::setLeft(Node *data) {
    if (!flagInterpret)
        return ;

    Tree *a = new Tree(nullptr, nullptr, this, data);
    this->left = a;
}

// Создание правого потомка от текущей вершины
void Tree::setRight(Node *data) {
    if (!flagInterpret)
        return ;

    Tree *a = new Tree(nullptr, nullptr, this, data);
    this->right = a;
}

// Поиск данных в дереве от заданной вершины from до корня вверх по связям
Tree *Tree::findUp(Tree *from, TypeLex id) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *i = from; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(id, i->node->id, max(strlen(i->node->id), strlen(id))) != 0))
        i = i->up; // поднимаемся вверх по связям
    return i;
}

Tree *Tree::findUpOnLevel(Tree *from, TypeLex id) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *i = from;
    while ((i->up != nullptr) && (i->up->right != i)) {
        if (memcmp(id, i->node->id, max(strlen(i->node->id), strlen(id))) == 0)
            return i; // нaшли совпадающий идентификатор
        i = i->up; // поднимаемся наверх по связям
    }
    return nullptr;
}

void Tree::print() {
    if (!flagInterpret)
        return ;

    std::string nType = types[node->dataType];

    printf("Node with data %s [type %s", this->node->id, nType.c_str());
    if (this->node->init) {
        if (this->node->dataType == TYPE_INTEGER || this->node->dataType == TYPE_SHORT ||
            this->node->dataType == TYPE_LONG)
            printf(", value: %d", this->node->dataValue.vInt);
        else if (this->node->dataType == TYPE_DOUBLE)
            printf(", value: %f", this->node->dataValue.vDouble);
    }
    printf("] ----->");
    if (this->left != nullptr) {
        std::string lType = types[left->node->dataType];
        printf(" left child %s [type %s", this->left->node->id, lType.c_str());
        if (this->left->node->init) {
            if (this->left->node->dataType == TYPE_INTEGER || this->left->node->dataType == TYPE_SHORT
                || this->left->node->dataType == TYPE_LONG)
                printf(", value: %d", this->left->node->dataValue.vInt);
            else if (this->left->node->dataType == TYPE_DOUBLE)
                printf(", value: %f", this->left->node->dataValue.vDouble);
        }
        printf("]");
    }
    if (this->right != nullptr) {
        std::string rType = types[right->node->dataType];
        printf(" right child %s [type %s", this->right->node->id, rType.c_str());
        if (this->right->node->init) {
            if (this->right->node->dataType == TYPE_INTEGER || this->right->node->dataType == TYPE_SHORT
                || this->right->node->dataType == TYPE_LONG)
                printf(", value: %d", this->right->node->dataValue.vInt);
            else if (this->right->node->dataType == TYPE_DOUBLE)
                printf(", value: %f", this->right->node->dataValue.vDouble);
        }
        printf("]");
    }
    printf("\n");
    if (this->left != nullptr) this->left->print();
    if (this->right != nullptr) this->right->print();
}

void Tree::printError(std::string error, TypeLex a) {
    if (!flagInterpret)
        return ;

    printf("Error: %s on line %d, identifier %s", error.c_str(), sc->getLine() + 1, a);
    exit(-440);
}

int Tree::findDuplicate(Tree *addr, TypeLex a) {
    if (!flagInterpret)
        return 0;

    if (findUpOnLevel(addr, a) == nullptr) return 0;
    return 1;
}

void Tree::setCur(Tree *a) {
    if (!flagInterpret)
        return ;

    Tree::cur = a;
}

void Tree::setScaner(Scaner *scaner) {
    if (!flagInterpret)
        return ;

    sc = scaner;
}

Tree *Tree::semanticInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    if (findDuplicate(cur, a))
        printError("Redescription of an identifier", a);

    Tree *v;
    Node b;

    memcpy(b.id, a, strlen(a) + 1);
    b.objectType = objType;
    b.dataType = dataType;

    // Генерация нового имени в asm
    // TODO
    std::string newAsmNameStr = this->generateAsmName(b);
    char* newAsmName = const_cast<char *>(newAsmNameStr.c_str());
    newAsmName[newAsmNameStr.size()] = '\0';
    memset(&b.asmId[0], 0, sizeof(b.asmId));
    memcpy(b.asmId, newAsmName, newAsmNameStr.size());
    // Тип переменной
    if (b.dataType == DATA_TYPE::TYPE_DOUBLE)
        b.declarationType = TypeDecl::DQ;
    else if (b.dataType == DATA_TYPE::TYPE_INTEGER)
        b.declarationType = TypeDecl::DD;
    else if (b.dataType == DATA_TYPE::TYPE_SHORT)
        b.declarationType = TypeDecl::DW;
    else if (b.dataType == DATA_TYPE::TYPE_LONG)
        b.declarationType = TypeDecl::DD;
    else if (b.dataType == DATA_TYPE::TYPE_DATASTRUCT)
        b.declarationType = TypeDecl::DB;
    b.len = 1;

    if (objType == TYPE_VAR) {
        b.isConst = false;
        cur->setLeft(&b); // сделали вершину - переменную
        cur = cur->left;
        return cur;
    } else {
        cur->setLeft(&b); // сделали вершину - функцию или структуру
        cur = cur->left;
        v = cur; // это точка возврата после выхода из функции

        b.dataType = TYPE_NONE;
        b.objectType = TYPE_UNDEFINED;
        memcpy(&b.id, &"<r>", 4);
        cur->setRight(&b); // сделали пустую вершину
        cur = cur->right;
        return v;
    }
}

void Tree::semanticSetInit(Tree *addr, bool flag) {
    if (!flagInterpret)
        return ;

    addr->node->init = flag;
    if (flag == true)
        addr->node->len = addr->node->declarationType;
}

void Tree::semanticSetConst(Tree *addr, bool flag) {
    if (!flagInterpret)
        return ;

    addr->node->isConst = flag;
}

void Tree::semanticSetStruct(Tree *&addr, Tree *data) {
    if (!flagInterpret)
        return ;

    if (addr->node->objectType == TYPE_VAR)
        addr->node->objectType = TYPE_STRUCT;

    memcpy(addr->node->structLex, data->up->node->id, sizeof(data->up->node->id));
    addr->node->dataStruct = copyTree(data, addr);
}

int Tree::isStruct(Tree *addr, TypeLex a) {
    if (!flagInterpret)
        return 0;

    Tree *v = findUp(addr, a);
    if (v == nullptr) {
        printError("Identifier-struct is not exist", a);
        return 0;
    }
    if (v->node->dataType == TYPE_DATASTRUCT && v->node->objectType == TYPE_STRUCT) {
        //printError("Identifier is not a structure", a);
        return 0;
    }
    return 1;
}

void Tree::checkInit(TypeLex a) {
    if (!flagInterpret)
        return ;

    Tree *v = semanticGetVar(a);
    checkInit(v);
}

void Tree::checkInit(Tree *a) {
    if (!flagInterpret)
        return ;

    if (!a->node->init)
        printError("Used uninitialized variable", a->node->id);
}

void Tree::checkConst(TypeLex a) {
    if (!flagInterpret)
        return ;

    Tree *v = semanticGetVar(a);
    checkConst(v);
}

void Tree::checkConst(Tree *a) {
    if (!flagInterpret)
        return ;

    if (a->node->isConst)
        printError("Cannot change constant", a->node->id);
}

void Tree::semanticTypeCastCheck(TypeLex a, TypeLex b) {
    if (!flagInterpret)
        return ;

    Tree *v = semanticGetVar(a);
    DATA_TYPE constDataType = getType(b);

    if (v->node->dataType < constDataType) {
        printf("Semantic error, line %d: Impossible cast %s to %s, identifier %s", sc->getLine() + 1,
               types[constDataType].c_str(), types[v->node->dataType].c_str(), a);
        exit(-440);
    }
}

void Tree::semanticTypeCastCheck(DATA_TYPE a, DATA_TYPE b) {
    if (!flagInterpret)
        return ;

    if ((a == DATA_TYPE::TYPE_DATASTRUCT && a > b) || (b == DATA_TYPE::TYPE_DATASTRUCT && b > a)) {
        printf("Semantic error, line %d: Impossible casting in type %s, expected %s", sc->getLine() + 1,
               types[b].c_str(), types[a].c_str());
        exit(-440);
    } else if (a < b) {
        printf("Semantic error, line %d: Impossible cast type %s, expected %s", sc->getLine() + 1,
               types[b].c_str(), types[a].c_str());
        exit(-440);
    }
}

Tree *Tree::semanticGetVar(TypeLex a) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *v = findUp(cur, a);
    if (v == nullptr)
        printError("Identifier description missing", a);
    return v;
}

Tree *Tree::semanticGetStructData(TypeLex a) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *v = findUp(cur, a);
    if (v == nullptr)
        printError("Structure description missing", a);
    if (v->node->dataType != DATA_TYPE::TYPE_DATASTRUCT)
        printError("Identifier is not a structure", a);

    if (v->node->objectType == OBJECT_TYPE::TYPE_STRUCT)
        return v->node->dataStruct;
    else
        return v->right;
}

Tree *Tree::semanticGetStructData(Tree *a) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    return a->node->dataStruct;
}

Tree *Tree::semanticGetIdentifierInStruct(Tree *from, TypeLex a) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *i = from; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(a, i->node->id, max(strlen(i->node->id), strlen(a))) != 0))
        i = i->left; // ищем дальше по левой ветке
    return i;
}

DATA_TYPE Tree::getType(TypeLex a) {
    if (!flagInterpret)
        return (DATA_TYPE) 0;

    Tree *v;
    v = findUp(cur, a);
    if (v != nullptr)
        return v->node->dataType;

    errno = 0;
    double num = strtod(a, nullptr);
    char *isDouble = strchr(a, '.');
    bool rangeError = errno == ERANGE;
    if (rangeError)
        printf("Line %d: number %s is very large\n", sc->getLine() + 1, a);

    if (isDouble != nullptr)
        return TYPE_DOUBLE;
    else if (isShort(num))
        return TYPE_SHORT;
    else if (isInt(num))
        return TYPE_INTEGER;
    else if (isLong(num))
        return TYPE_LONG;
}

DATA_TYPE Tree::getType(Tree *a) {
    if (!flagInterpret)
        return (DATA_TYPE) 0;

    return a->node->dataType;
}

char* Tree::getIdentifier(Tree* from) {
    if (!flagInterpret)
        return nullptr;

    return from->node->id;
}

Tree *Tree::compoundOperator() {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *v;
    Node b;

    memcpy(b.id, &"<l>", 4);
    b.dataType = TYPE_NONE;
    b.objectType = TYPE_UNDEFINED;

    cur->setLeft(&b);
    cur = cur->left;
    v = cur;

    memcpy(&b.id, &"<r>", 4);
    cur->setRight(&b); // сделали пустую вершину
    cur = cur->right;
    return v;
}

void Tree::semanticSetData(Tree *a, DATA_TYPE dt, char *data) {
    if (!flagInterpret)
        return ;

    if (dt == TYPE_LONG || dt == TYPE_SHORT || dt == TYPE_INTEGER)
        a->node->dataValue.vInt = (int) strtoul(data, nullptr, 0);
//        memcpy(&a->node->dataValue.vInt, data, strlen(data) + 1);
    else if (dt == DATA_TYPE::TYPE_DOUBLE)
        a->node->dataValue.vDouble = (double) strtoul(data, nullptr, 0);
//        memcpy(&a->node->dataValue.vDouble, data, strlen(data) + 1);

}

Tree *Tree::copyTree(Tree *from, Tree *up) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *newTree = nullptr;
    if (from != nullptr) {
        newTree = new Tree(nullptr, nullptr, up, from->node);
        if (from->node->dataType == DATA_TYPE::TYPE_DATASTRUCT)
            newTree->node->dataStruct = this->copyTree(
                    this->semanticGetStructData(from->node->structLex),
                    newTree
                    );
        if (from->right)
            newTree->right = copyTree(from->right, newTree);
        if (from->left)
            newTree->left = copyTree(from->left, newTree);
    }
    return newTree;
}

Tree::~Tree() {
    //удаляем ноду + левого и правого
    delete left;
    delete right;
    delete node;
}

Tree *Tree::deleteTreeFrom(Tree *a) {
    if (!flagInterpret)
        return (Tree*) nullptr;

    Tree *buf = a->up;
    //так как деструктор может удалить объект когда захочет
    // - принудительно затираем связи и отправляем это в утиль на очередь
    buf->left = nullptr;
    buf->right = nullptr;
    delete a;
    setCur(buf);
    return buf;
}

void Tree::semanticSetValue(Tree *a, ExpresData *data) {
    if (!flagInterpret)
        return ;

    if (data->dataType == TYPE_LONG || data->dataType == TYPE_SHORT || data->dataType == TYPE_INTEGER)
        data->dataType = TYPE_INTEGER;
    else if (data->dataType == TYPE_DOUBLE)
        data->dataType = TYPE_DOUBLE;

    switch (data->dataType) {
        case TYPE_INTEGER:
            switch (a->node->dataType) {
                case TYPE_INTEGER:
                case TYPE_SHORT:
                case TYPE_LONG:
                    a->node->dataValue.vInt = data->dataValue.vInt;
                    break;
                case TYPE_DOUBLE:
                    a->node->dataValue.vDouble = data->dataValue.vInt;
                    break;
                default:
                    break;
            }
            break;

        case TYPE_DOUBLE: {
            switch (a->node->dataType) {
                case TYPE_INTEGER:
                    a->node->dataValue.vInt = data->dataValue.vDouble;
                    break;
                case TYPE_DOUBLE:
                    a->node->dataValue.vDouble = data->dataValue.vDouble;
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }
}

void Tree::semanticGetData(Tree *a, ExpresData *data) {
    if (!flagInterpret)
        return ;

    data->dataType = a->node->dataType;
    data->dataValue = a->node->dataValue;
}

void Tree::semanticGetStringValue(TypeLex value, ExpresData *data, int type = 0) {
    if (!flagInterpret)
        return ;
    double val = (double) strtoul(value, nullptr, 0);

    if (isInt(val)) {
        data->dataValue.vInt = (int) val;
        data->dataType = TYPE_INTEGER;
    } else {
        data->dataValue.vDouble = val;
        data->dataType = TYPE_DOUBLE;
    }
}

void Tree::semanticMakeBiOperation(ExpresData *data1, ExpresData *data2, int type) {
    if (!flagInterpret)
        return ;

    DATA_TYPE maxType = max(data1->dataType, data2->dataType);
    DataValue value1, value2;

    if (maxType == data1->dataType)
        value1 = data1->dataValue;
    else
        switch (data1->dataType) {
            case TYPE_INTEGER:
            case TYPE_SHORT:
            case TYPE_LONG:
                value1.vDouble = data1->dataValue.vInt;
                break;
            case TYPE_DOUBLE:
                value1.vDouble = data1->dataValue.vDouble;
                break;
        }

    if (maxType == data2->dataType)
        value2 = data2->dataValue;
    else
        switch (data2->dataType) {
            case TYPE_INTEGER:
            case TYPE_SHORT:
            case TYPE_LONG:
                value2.vDouble = data2->dataValue.vInt;
                break;
            case TYPE_DOUBLE:
                value2.vDouble = data2->dataValue.vDouble;
                break;
        }
    if (maxType == TYPE_SHORT || maxType == TYPE_LONG)
        maxType = TYPE_INTEGER;

    switch (type) {
        case TypeBitOr:
            switch (maxType) {
                case TYPE_INTEGER:
                    data1->dataValue.vInt = value1.vInt | value2.vInt;
                    break;
                case TYPE_DOUBLE: {
//                    data1->dataValue.vDouble = value1.vDouble | value2.vDouble;
                    printf("Error: impossible make bitwise operation OR of two double numbers");
                    exit(-440);
                    break;
                }
                default: break;
            }
            break;
        case TypeBitXor:
            switch (maxType) {
                case TYPE_INTEGER:
                    data1->dataValue.vInt = value1.vInt ^ value2.vInt;
                    break;
                case TYPE_DOUBLE: {
//                    data1->dataValue.vDouble = value1.vDouble ^ value2.vDouble;
                    printf("Error: impossible make bitwise operation XOR of two double numbers");
                    exit(-440);
                    break;
                }
                default: break;
            }
            break;
        case TypeBitAnd:
            switch (maxType) {
                case TYPE_INTEGER:
                    data1->dataValue.vInt = value1.vInt & value2.vInt;
                    break;
                case TYPE_DOUBLE: {
//                    data1->dataValue.vDouble = value1.vDouble & value2.vDouble;
                    printf("Error: impossible make bitwise operation AND of two double numbers");
                    exit(-440);
                    break;
                }
                default: break;
            }
            break;
        case TypeEqual:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt == value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble == value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeNotEqual:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt != value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble != value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeLess:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt < value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble < value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeLessOrEqual:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt <= value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble <= value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeMore:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt > value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble > value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeMoreOrEqual:
            switch (maxType) {
                case TYPE_INTEGER:
                    if (value1.vInt >= value2.vInt)
                        data1->dataValue.vInt = 1;
                    else
                        data1->dataValue.vInt = 0;
                    break;
                case TYPE_DOUBLE:
                    if (value1.vDouble >= value2.vDouble)
                        data1->dataValue.vDouble = 1;
                    else
                        data1->dataValue.vDouble = 0;
                    break;
                default: break;
            }
            break;
        case TypeAdd:
            switch (maxType) {
                case TYPE_INTEGER: {
                    double buf = (double) value1.vInt + (double) value2.vInt;
                    if (isInt(buf))
                        data1->dataValue.vInt = value1.vInt + value2.vInt;
                    else {
                        data1->dataType = TYPE_DOUBLE;
                        data1->dataValue.vDouble = buf;
                    }
                    break;
                }
                case TYPE_DOUBLE:
                    data1->dataValue.vDouble = value1.vDouble + value2.vDouble;
                    break;
                default: break;
            }
            break;
        case TypeSub:
            switch (maxType) {
                case TYPE_INTEGER: {
                    double buf = (double) value1.vInt - (double) value2.vInt;
                    if (isInt(buf))
                        data1->dataValue.vInt = value1.vInt - value2.vInt;
                    else {
                        data1->dataType = TYPE_DOUBLE;
                        data1->dataValue.vDouble = buf;
                    }
                    break;
                }
                case TYPE_DOUBLE:
                    data1->dataValue.vDouble = value1.vDouble - value2.vDouble;
                    break;
                default: break;
            }
            break;
        case TypeMul:
            switch (maxType) {
                case TYPE_INTEGER: {
                    double buf = (double) value1.vInt * (double) value2.vInt;
                    if (isInt(buf))
                        data1->dataValue.vInt = value1.vInt * value2.vInt;
                    else {
                        data1->dataType = TYPE_DOUBLE;
                        data1->dataValue.vDouble = buf;
                    }
                    break;
                }
                case TYPE_DOUBLE:
                    data1->dataValue.vDouble = value1.vDouble * value2.vDouble;
                    break;
                default: break;
            }
            break;
        case TypeDiv:
            switch (maxType) {
                case TYPE_INTEGER: {
                    double buf = (double) value1.vInt / (double) value2.vInt;
                    if (isInt(buf))
                        data1->dataValue.vInt = value1.vInt / value2.vInt;
                    else {
                        data1->dataType = TYPE_DOUBLE;
                        data1->dataValue.vDouble = buf;
                    }
                    break;
                }
                case TYPE_DOUBLE:
                    data1->dataValue.vDouble = value1.vDouble / value2.vDouble;
                    break;
                default: break;
            }
            break;
        case TypeDivPart:
            switch (maxType) {
                case TYPE_INTEGER: {
                    double buf = value1.vInt % value2.vInt;
                    if (isInt(buf))
                        data1->dataValue.vInt = value1.vInt % value2.vInt;
                    else {
                        data1->dataType = TYPE_DOUBLE;
                        data1->dataValue.vDouble = buf;
                    }
                    break;
                }
                case TYPE_DOUBLE: {
                    // data1->dataValue.vDouble = value1.vDouble % value2.vDouble;
                    printf("Error: impossible take a part of division from two double numbers");
                    exit(-440);
                    break;
                }
                default: break;
            }
            break;
        default: break;
    }
}

void Tree::printInfo(Tree* a, std::string beforeText) {
    if (!flagInterpret)
        return ;

    printf("%s", beforeText.c_str());
    if (a->node->dataType == TYPE_SHORT || a->node->dataType == TYPE_INTEGER || a->node->dataType == TYPE_LONG)
        printf("var \'%s\', value: %d\n", a->node->id, a->node->dataValue.vInt);
    else if (a->node->dataType == TYPE_DOUBLE)
        printf("var \'%s\', value: %f\n", a->node->id, a->node->dataValue.vDouble);
}

std::string Tree::generateFullLexFromStruct(Tree *structLevel, char *structLex) {
    std::string fullLex = structLevel->node->asmId;
    structLevel = structLevel->up;

    if (structLevel == nullptr)
        return fullLex;
    else {
        do {
            fullLex = "." + fullLex;
            while(strcmp(structLevel->node->id, (char*)"<r>") != 0)
                structLevel = structLevel->up;
            structLevel = structLevel->up;
            fullLex = structLevel->node->asmId + fullLex;
        } while (structLevel->up != nullptr && (strcmp(structLevel->node->id, structLex) != 0));
    }
    return fullLex;
}

// Вычисление занимаемой длины идентификатора типы структуры
int Tree::computeLengthOfStruct(Tree *structAddr) {
    int totalLength = 0;

    Tree *i = structAddr; // Текущая вершина структуры
    while(i != nullptr) {
        if (i->node->dataType == DATA_TYPE::TYPE_DATASTRUCT)
            totalLength += this->computeLengthOfStruct(i->node->dataStruct);
        else
            totalLength += i->node->declarationType;

        i = i->left;
    }

    if (totalLength % 8 != 0)
        totalLength += 8 - totalLength % 8;

    return (totalLength == 0) ? 1 : totalLength;
}

int Tree::computeMemorySizeOfFunc(Tree *funcAddr) {
    int totalShift = 0;

    Tree *i = funcAddr; // Текущая вершина функции
    while(i != nullptr) {
        // Если справа есть ветка - значит это сложный оператор
        if (i->right != nullptr)
            totalShift -= this->computeMemorySizeOfFunc(i->right);
        else {
            // Если переменная с типом структуры
            if (i->node->dataType == DATA_TYPE::TYPE_DATASTRUCT)
                totalShift -= this->computeLengthOfStruct(i->node->dataStruct);
            // Если переменная не инициализирована
            else if (!i->node->isConst && !i->node->init)
                totalShift -= i->node->declarationType;
        }

        i = i->left;
    }

    if (totalShift % 8 != 0)
        totalShift += totalShift % 8;

    return totalShift;
}

std::string Tree::generateAsmName(Node node) {
    std::string name = node.id;
    if (node.dataType == DATA_TYPE::TYPE_DOUBLE)
        name = "double_" + name;
    else if (node.dataType == DATA_TYPE::TYPE_INTEGER)
        name = "int_" + name;
    else if (node.dataType == DATA_TYPE::TYPE_LONG)
        name = "long_" + name;
    else if (node.dataType == DATA_TYPE::TYPE_SHORT)
        name = "short_" + name;
    else if (node.dataType == DATA_TYPE::TYPE_DATASTRUCT)
        name = "datastruct_" + name;
    else
        name = "none_" + name;
    name += "_";
    name += std::to_string(numGenAsm++);

    return name;
}

char* Tree::getAsmIdentifier(Tree *from) {
    return from->node->asmId;
}

void Tree::insertLevel(Tree* from, int level) {
    Tree* i = from;

    while (i != nullptr) {
        // Здесь высчитываем размер структуры
        if (i->node->dataType == DATA_TYPE::TYPE_DATASTRUCT/* && i->node->objectType == OBJECT_TYPE::TYPE_STRUCT*/)
            i->node->len = this->computeLengthOfStruct(i->node->dataStruct);

        i->node->level = level;
        if (i->right != nullptr) {
            i->right->node->level = level + 1;
            this->insertLevel(i->right->left, level + 2);
        }

        i = i->left;
    }
}

void Tree::computeStackAddrInMain(Tree* root) {
    Tree* from = root;
    while (from->node->objectType != OBJECT_TYPE::TYPE_FUNC)
        from = from->left;

    Tree* mainTree = from;
    int mainLocalShift = 0; // Сдвиг адреса для функции main
    from = from->right->left;
    while (from != nullptr) {
        if (from->right == nullptr) {
            if (from->node->dataStruct != nullptr) {
                mainLocalShift -= from->node->len;
            }
            else if (!from->node->isConst && !from->node->init)
                mainLocalShift -= from->node->declarationType;
        } else
            mainLocalShift = this->computeStackAddr(from->right->left, mainLocalShift);
        from->node->stackAddr = mainLocalShift;

        from = from->left;
    }

    mainTree->node->stackAddr = mainLocalShift;
}

int Tree::computeStackAddr(Tree *from, int stackAddr) {
    Tree* i = from;
    int localStackAddr = stackAddr;
    while (i != nullptr) {
        if (i->right != nullptr)
            localStackAddr = this->computeStackAddr(i->right->left, localStackAddr);
        else {
            localStackAddr -= i->node->declarationType;
            i->node->stackAddr = localStackAddr;
        }

        i = i->left;
    }

    return localStackAddr;
}

Tree *Tree::goNext(Tree *from) {
    Tree* returnTree;
    if (from != nullptr) {
        if (from->node->objectType != OBJECT_TYPE::TYPE_FUNC)
            returnTree = from->left;
        else
            returnTree = from->right->left;
    }
    else
        returnTree = nullptr;

    return returnTree;
}

Node *Tree::getNode(Tree *from) {
    if (from != nullptr)
        return from->node;
    else
        return nullptr;
}

Tree *Tree::findInTree(Tree* from, TypeLex asmId) {
    Tree *i = from; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(asmId, i->node->asmId, max(strlen(i->node->asmId), strlen(asmId))) != 0)) {
        Tree* right = nullptr;
        if (i->right != nullptr)
            right = this->findInTree(i->right, asmId);

        if (right == nullptr)
            i = i->left;
    }

    return i;
}

Tree *Tree::findUpByAsmId(TypeLex asmId) {
    Tree *i = cur; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(asmId, i->node->asmId, max(strlen(i->node->asmId), strlen(asmId))) != 0)) {
        Tree* right = nullptr;
        if (i->right != nullptr)
            right = this->findInTree(i->right, asmId);

        if (right == nullptr)
            i = i->up; // поднимаемся вверх по связям
        else
            return right;
    }

    return i;
}

bool Tree::hasRightPart(Tree *addr) {
    if (addr->right != nullptr)
        return true;
    else
        return false;
}

Tree *Tree::goNextLeft(Tree *from) {
    return from->left;
}

Tree *Tree::goNextRight(Tree *from) {
    return from->right->left;
}
