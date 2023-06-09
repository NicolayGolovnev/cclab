//
// Created by kolya on 08.06.2023.
//

#include <iomanip>
#include "GenerAsm.h"
#include "sstream"

std::string intToHex(int i) {
    std::stringstream stream;
    stream << std::hex << i;
    return stream.str();
}

GenerAsm::GenerAsm() = default;

void GenerAsm::setTree(Tree *tree) {
    this->tree = tree;
}

void GenerAsm::setGlobal(GlobalData *global) {
    this->global = global;
}

void GenerAsm::printAllData() {
    printf("Global data:\n");
    for (int i = 0; i < this->globalDataDeclaration.size(); i++)
        printf("%s\n", this->globalDataDeclaration[i].c_str());
    printf("\n");

    printf("Global const data:\n");
    for (int i = 0; i < this->constDataDeclaration.size(); i++)
        printf("%s\n", this->constDataDeclaration[i].c_str());
    printf("\n");

    printf("Main func:\n");
    for (int i = 0; i < this->asmCode.size(); i++)
        printf("%s\n", this->asmCode[i].c_str());
    printf("\n");
}

void GenerAsm::preInitializeTree() {
    this->tree->insertLevel(this->tree, 0);
    this->tree->computeStackAddrInMain(this->tree);
}

void GenerAsm::generateGlobalData() {
    Tree* root = this->tree;
    Tree* from = this->tree->goNext(root);
    Node* node = this->tree->getNode(from);

    std::string outGlobalData;
    while (from != nullptr && node != nullptr && node->level == 0) {
        // Если встретилась функция main - выходим (она запишется как идентификатор)
        if (node->objectType == OBJECT_TYPE::TYPE_FUNC)
            break;

        if (node->isConst)
            this->constDataDeclaration.push_back("CONST SEGMENT");
        else {
            std::string asmName = node->asmId;
            std::string normalName = node->id;
            outGlobalData = "PUBLIC " + asmName + "\t; " + normalName;
            this->globalDataDeclaration.push_back(outGlobalData);
        }


        // asmName DD 0xxH DUP(?); name
        outGlobalData = node->asmId;
        outGlobalData += " ";

        if (node->declarationType == TypeDecl::DB)
            outGlobalData += "DB ";
        else if (node->declarationType == TypeDecl::DW)
            outGlobalData += "DW ";
        else if (node->declarationType == TypeDecl::DQ)
            outGlobalData += "DQ ";
        else if (node->declarationType == TypeDecl::DD)
            outGlobalData += "DD ";

        std::string hexLen = "0" + intToHex(node->len) + "H";
        outGlobalData += hexLen;
        if (!node->init)
            outGlobalData += " DUP(?)";
        // else -> asmName DD 0xxH ; name

        outGlobalData += "\t";
        outGlobalData += "; ";
        std::string normalName = node->id;
        outGlobalData += normalName;

        if (node->isConst) {
            this->constDataDeclaration.push_back(outGlobalData);
            this->constDataDeclaration.push_back("CONST ENDS");
        }
        else
            this->globalDataDeclaration.push_back(outGlobalData);

        from = this->tree->goNext(from);
        node = this->tree->getNode(from);
    }
}

void GenerAsm::generateMainFunc() {
    Tree* root = this->tree;
    Tree* from = this->tree->goNext(root);
    Node* node = this->tree->getNode(from);
    // Доходим до функции main
    while (from != nullptr && node != nullptr) {
        if (node->objectType == OBJECT_TYPE::TYPE_FUNC)
            break;

        from = this->tree->goNext(from);
        node = this->tree->getNode(from);
    }

    std::string mainProc = node->asmId;
    mainProc += " PROC";
    this->asmCode.push_back(mainProc);
    this->asmCode.push_back("\tpush ebp");
    this->asmCode.push_back("\tpush mov ebp, esp");

    std::string andEsp = "\tsub exp, ";
    andEsp += intToHex(abs(node->stackAddr));
    this->asmCode.push_back(andEsp);

    this->asmCode.push_back("\t...");
    this->asmCode.push_back("\tTRIAD CODE");
    this->asmCode.push_back("\t...");

    this->asmCode.push_back("\tmov esp, ebp");
    this->asmCode.push_back("\tpop ebp");
    this->asmCode.push_back("\tret 0");
    std::string mainEnd = node->asmId;
    mainEnd += " ENDP";
    this->asmCode.push_back(mainEnd);
}


