//
// Created by kolya on 12/1/2021.
//

#include "Semantic.h"
#define max(a, b) a > b ? a : b

#define isShort(num) (num >= -(short)32767 - 1 && num <= (short)32767)
#define isInt(num) (num >= -2147483647 - 1 && num <= 2147483647)
#define isLong(num) (num >= -(long)2147483647 - 1 && num <= (long)2147483647)


Tree* Tree::cur = (Tree*)nullptr;
std::vector<std::string> types = { "none", "short", "int", "long", "double", "type of struct" };

Tree::Tree() {
    this->node = new Node();
    this->up = nullptr; this->left = nullptr; this->right = nullptr;
    memcpy(this->node, &("------"), sizeof(Node));
}

Tree::Tree(Tree *l, Tree *r, Tree *up, Node *data) {
    this->node = new Node();
    this->up = up; this->left = l; this->right = r;
    memcpy(this->node, data, sizeof(Node));
}

// Создание левого потомка от текущей вершины
void Tree::setLeft(Node *data) {
    Tree* a = new Tree(nullptr, nullptr, this, data);
    this->left = a;
}

// Создание правого потомка от текущей вершины
void Tree::setRight(Node *data) {
    Tree* a = new Tree(nullptr, nullptr, this, data);
    this->right = a;
}

// Поиск данных в дереве от заданной вершины from до корня вверх по связям
Tree *Tree::findUp(Tree *from, TypeLex id) {
    Tree* i = from; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(id, i->node->id, max(strlen(i->node->id), strlen(id))) != 0))
        i = i->up; // поднимаемся вверх по связям
    return i;
}

Tree *Tree::findUpOnLevel(Tree *from, TypeLex id) {
    Tree* i = from;
    while ((i->up != nullptr) && (i->up->right != i))
    {
        if (memcmp(id, i->node->id, max(strlen(i->node->id), strlen(id))) == 0)
            return i; // нaшли совпадающий идентификатор
        i = i->up; // поднимаемся наверх по связям
    }
    return nullptr;
}

void Tree::print() {
    std::string nType = types[node->dataType];

    printf("Node with data %s [type %s", this->node->id, nType.c_str());
    if (this->node->init) {
        if (this->node->dataType == TYPE_INTEGER || this->node->dataType == TYPE_SHORT || this->node->dataType == TYPE_LONG)
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
    if (this->right != nullptr){
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
    printf("Error: %s on line %d, identifier %s", error.c_str(), sc->getLine() + 1, a);
    exit(-440);
}

int Tree::findDuplicate(Tree *addr, TypeLex a) {
    if (findUpOnLevel(addr, a) == nullptr) return 0;
    return 1;
}

void Tree::setCur(Tree *a) {
    Tree::cur = a;
}

void Tree::setScaner(Scaner *scaner) {
    sc = scaner;
}

Tree* Tree::semanticInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType) {
    if (findDuplicate(cur, a))
        printError("Redescription of an identifier", a);

    Tree *v;
    Node b;

    if (objType == TYPE_VAR)
    {
        memcpy(b.id, a, strlen(a) + 1); b.objectType = objType;
        b.dataType = dataType; b.isConst = false;
        cur->setLeft(&b); // сделали вершину - переменную
        cur = cur->left;
        return cur;
    }
    else
    {
        memcpy(b.id, a, strlen(a) + 1); b.objectType = objType; b.dataType = dataType;
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
    addr->node->init = flag;
}

void Tree::semanticSetConst(Tree *addr, bool flag) {
    addr->node->isConst = flag;
}

void Tree::semanticSetStruct(Tree* &addr, Tree* data) {
    if (addr->node->objectType == TYPE_VAR)
        addr->node->objectType = TYPE_STRUCT;
    addr->node->dataStruct = copyTree(data, addr);
}

int Tree::isStruct(Tree* addr, TypeLex a) {
    Tree* v = findUp(addr, a);
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
    Tree* v = semanticGetVar(a);
    checkInit(v);
}

void Tree::checkInit(Tree *a) {
    if (!a->node->init)
        printError("Used uninitialized variable", a->node->id);
}

void Tree::checkConst(TypeLex a) {
    Tree* v = semanticGetVar(a);
    checkConst(v);
}

void Tree::checkConst(Tree *a) {
    if (a->node->isConst)
        printError("Cannot change constant", a->node->id);
}

void Tree::semanticTypeCastCheck(TypeLex a, TypeLex b) {
    Tree* v = semanticGetVar(a);
    DATA_TYPE constDataType = getType(b);

    if (v->node->dataType < constDataType) {
        printf("Semantic error, line %d: Impossible cast %s to %s, identifier %s", sc->getLine() + 1,
               types[constDataType].c_str(), types[v->node->dataType].c_str(), a);
        exit(-440);
    }
}

void Tree::semanticTypeCastCheck(DATA_TYPE a, DATA_TYPE b) {
    if ((a == DATA_TYPE::TYPE_DATASTRUCT && a > b) || (b == DATA_TYPE::TYPE_DATASTRUCT && b > a)) {
        printf("Semantic error, line %d: Impossible casting in type %s, expected %s", sc->getLine() + 1,
               types[b].c_str(), types[a].c_str());
        exit(-440);
    }
    else if (a < b) {
        printf("Semantic error, line %d: Impossible cast type %s, expected %s", sc->getLine() + 1,
               types[b].c_str(), types[a].c_str());
        exit(-440);
    }
}

Tree* Tree::semanticGetVar(TypeLex a) {
    Tree* v = findUp(cur, a);
    if (v == nullptr)
        printError("Identifier description missing", a);
    return v;
}

Tree* Tree::semanticGetStructData(TypeLex a) {
    Tree* v = findUp(cur, a);
    if (v == nullptr)
        printError("Structure description missing", a);
    if (v->node->dataType != DATA_TYPE::TYPE_DATASTRUCT)
        printError("Identifier is not a structure", a);

    if (v->node->objectType == OBJECT_TYPE::TYPE_STRUCT)
        return v->node->dataStruct;
    else
        return v->right;
}

Tree* Tree::semanticGetStructData(Tree *a) {
    return a->node->dataStruct;
}

Tree* Tree::semanticGetIdentifierInStruct(Tree *from, TypeLex a) {
    Tree* i = from; // Текущая вершина поиска
    while ((i != nullptr) && (memcmp(a, i->node->id, max(strlen(i->node->id), strlen(a))) != 0))
        i = i->left; // ищем дальше по левой ветке
    return i;
}

DATA_TYPE Tree::getType(TypeLex a) {
    Tree* v;
    v = findUp(cur, a);
    if (v != nullptr)
        return v->node->dataType;

    errno = 0;
    double num = strtod(a, nullptr);
    char* isDouble = strchr(a, '.');
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
    return a->node->dataType;
}

Tree *Tree::compoundOperator() {
    Tree* v;
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

void Tree::semanticSetData(Tree* a, DATA_TYPE dt, char* data) {
    if (dt == TYPE_LONG || dt == TYPE_SHORT || dt == TYPE_INTEGER)
        a->node->dataValue.vInt = atoi(data);
//        memcpy(&a->node->dataValue.vInt, data, strlen(data) + 1);
    else if (dt == DATA_TYPE::TYPE_DOUBLE)
        a->node->dataValue.vDouble = atof(data);
//        memcpy(&a->node->dataValue.vDouble, data, strlen(data) + 1);

}

Tree* Tree::copyTree(Tree* from, Tree* up) {
    Tree* newTree = nullptr;
    if (from != nullptr) {
        newTree = new Tree(nullptr, nullptr, up, from->node);
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

Tree* Tree::deleteCompound(Tree* compoundOperator) {
    Tree* buf = compoundOperator->up;
    //так как деструктор может удалить объект когда захочет
    // - принудительно затираем связи и отправляем это в утиль на очередь
    buf->left = nullptr;
    buf->right = nullptr;
    delete compoundOperator;
    setCur(buf);
    return buf;
}

