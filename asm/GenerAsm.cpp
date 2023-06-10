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

std::string getRegisterName(RegisterType registerType) {
    if (registerType == RegisterType::eax)
        return "eax";
    else if (registerType == RegisterType::ebx)
        return "ebx";
    else if (registerType == RegisterType::ecx)
        return "ecx";
    else if (registerType == RegisterType::edx)
        return "edx";
    else if (registerType == RegisterType::esi)
        return "esi";
    else if (registerType == RegisterType::edi)
        return "edi";
    else
        return "some_register";
}

GenerAsm::GenerAsm() {
    this->registerLoad.push_back(true);
    this->registerLoad.push_back(true);
    this->registerLoad.push_back(true);
    this->registerLoad.push_back(true);
    this->registerLoad.push_back(true);
    this->registerLoad.push_back(true);
}

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

RegisterType GenerAsm::getRegister() {
    for (int i = 0; i < this->registerLoad.size(); i++) {
        if (!this->registerLoad[i])
            // регистр занят
            continue;

        // Свободен - забираем его
        this->registerLoad[i] = false;
        return static_cast<RegisterType>(i);
    }
}

void GenerAsm::freeRegister(RegisterType registerType) {
    this->registerLoad[registerType] = true;
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

void GenerAsm::insertLocalData(Tree* from) {
    while (from != nullptr) {
        Node* node = this->tree->getNode(from);
        // Могут быть <r> - сложные операторы
        if (node->dataType != DATA_TYPE::TYPE_NONE) {
            std::string localSegment = node->asmId;
            localSegment += " = ";
            localSegment += std::to_string(node->stackAddr);
            localSegment += "\t; size = ";
            localSegment += std::to_string(node->declarationType);

            this->localData.push(localSegment);
        }
        else if (this->tree->hasRightPart(from)) {
            this->insertLocalData(this->tree->goNextRight(from));
        }

        from = this->tree->goNextLeft(from);
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
    // Генерируем локальные данные
    this->insertLocalData(this->tree->goNext(from));

    this->asmCode.push_back("_TEXT SEGMENT");
    std::string mainProc = node->asmId;
    mainProc += " PROC";
    while (!this->localData.empty()) {
        this->asmCode.push_back(this->localData.top());
        this->localData.pop();
    }
    this->asmCode.push_back(mainProc);
    this->asmCode.push_back("\tpush ebp");
    this->asmCode.push_back("\tpush mov ebp, esp");
    this->asmCode.push_back("\tand esp, -8");

    std::string andEsp = "\tsub exp, ";
    andEsp += intToHex(abs(node->stackAddr));
    this->asmCode.push_back(andEsp);

    this->asmCode.push_back("");
    for (int i = 0; i < this->asmTriads.size(); i++)
        this->asmCode.push_back(this->asmTriads[i]);
    this->asmCode.push_back("");

    this->asmCode.push_back("\tmov esp, ebp");
    this->asmCode.push_back("\tpop ebp");
    this->asmCode.push_back("\tret 0");
    std::string mainEnd = node->asmId;
    mainEnd += " ENDP";
    this->asmCode.push_back(mainEnd);
    this->asmCode.push_back("_TEXT ENDS");
}

void GenerAsm::generateTriadAsm() {
    std::deque<Triad> triads = this->global->resultTriads;

    std::string ifJump = "";
    for (int i = 0; i < triads.size(); i++) {
        Triad triad = triads[i];

        if (triad.isGotoMark)
            // MarkName:
            this->asmTriads.push_back(triad.gotoMark + ":");

        if (strcmp(triad.operation, (char*)"nop") == 0)
            continue;
        // TODO пропускаем присвоение в глобальных данных

        // заходим только в триады с операциями + и =
        if (strcmp(triad.operation, (char*)"=") == 0 || strcmp(triad.operation, (char*)"+") == 0) {
            if (strcmp(triad.operation, (char*)"+") == 0) {
                // + a b
                if (!triad.firstOperand.isLink && !triad.secondOperand.isLink) {
                    RegisterType freeRegister = this->getRegister();
                    triad.registerType = freeRegister;
                    // mov reg, a
                    std::string asmStr = "\tmov " + getRegisterName(freeRegister) + ", ";
                    if (triad.firstOperand.isConst) {
                        std::string lex = triad.firstOperand.lex;
                        asmStr += lex;
                    }
                    else {
                        Tree* firstOperandPtr = this->tree->findUpByAsmId(triad.firstOperand.lex);
                        Node* firstNodeOperand = this->tree->getNode(firstOperandPtr);
                        if (firstNodeOperand == nullptr)
                            continue;
                        else if (firstNodeOperand->level == 0) {
                            std::string lex = firstNodeOperand->asmId;
                            asmStr += lex;
                        }
                        else {
                            if (firstNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(firstNodeOperand->stackAddr) + ")";
                            asmStr += lex;
                        }
                    }

                    // add reg, b
                    std::string asmStr2 = "\tadd " + getRegisterName(freeRegister) + ", ";
                    if (triad.secondOperand.isConst) {
                        std::string lex = triad.secondOperand.lex;
                        asmStr2 += lex;
                    }
                    else {
                        Tree* secondOperandPtr = this->tree->findUpByAsmId(triad.secondOperand.lex);
                        Node* secondNodeOperand = this->tree->getNode(secondOperandPtr);
                        if (secondNodeOperand == nullptr)
                            continue;
                        else if (secondNodeOperand->level == 0) {
                            std::string lex = secondNodeOperand->asmId;
                            asmStr2 += lex;
                        }
                        else {
                            if (secondNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(secondNodeOperand->stackAddr) + ")";
                            asmStr2 += lex;
                        }
                    }
                    asmTriads.push_back(asmStr);
                    asmTriads.push_back(asmStr2);
                }
                // + (i) b
                else if (triad.firstOperand.isLink && !triad.secondOperand.isLink) {
                    triad.registerType = triads[triad.firstOperand.number].registerType;
                    // add reg_i, b
                    std::string asmStr = "\tmov " + getRegisterName(triad.registerType) + ", ";
                    if (triad.secondOperand.isConst) {
                        std::string lex = triad.secondOperand.lex;
                        asmStr += lex;
                    }
                    else {
                        Tree* secondOperandPtr = this->tree->findUpByAsmId(triad.secondOperand.lex);
                        Node* secondNodeOperand = this->tree->getNode(secondOperandPtr);
                        if (secondNodeOperand == nullptr)
                            continue;
                        else if (secondNodeOperand->level == 0) {
                            std::string lex = secondNodeOperand->asmId;
                            asmStr += lex;
                        }
                        else {
                            if (secondNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(secondNodeOperand->stackAddr) + ")";
                            asmStr += lex;
                        }
                    }
                    asmTriads.push_back(asmStr);
                }
            }
            else if (strcmp(triad.operation, (char*)"=") == 0) {
                // = a b
                if (!triad.firstOperand.isLink && !triad.secondOperand.isLink) {
                    RegisterType freeRegister = this->getRegister();
//                    triad.registerType = freeRegister;
                    // mov reg, b
                    std::string asmStr = "\tmov " + getRegisterName(freeRegister) + ", ";
                    if (triad.secondOperand.isConst) {
                        std::string lex = triad.secondOperand.lex;
                        asmStr += lex;
                    }
                    else {
                        Tree* secondOperandPtr = this->tree->findUpByAsmId(triad.secondOperand.lex);
                        Node* secondNodeOperand = this->tree->getNode(secondOperandPtr);
                        if (secondNodeOperand == nullptr)
                            continue;
                        else if (secondNodeOperand->level == 0) {
                            std::string lex = secondNodeOperand->asmId;
                            asmStr += lex;
                        }
                        else {
                            if (secondNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(secondNodeOperand->stackAddr) + ")";
                            asmStr += lex;
                        }
                    }

                    // mov a, reg
                    std::string asmStr2 = "\tmov ";
                    if (triad.firstOperand.isConst) {
                        std::string lex = triad.firstOperand.lex;
                        asmStr2 += lex;
                    }
                    else {
                        Tree* firstOperandPtr = this->tree->findUpByAsmId(triad.firstOperand.lex);
                        Node* firstNodeOperand = this->tree->getNode(firstOperandPtr);
                        if (firstNodeOperand == nullptr)
                            continue;
                        else if (firstNodeOperand->level == 0) {
                            std::string lex = firstNodeOperand->asmId;
                            asmStr2 += lex;
                        }
                        else {
                            if (firstNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(firstNodeOperand->stackAddr) + ")";
                            asmStr2 += lex;
                        }
                    }
                    asmStr2 += ", ";
                    asmStr2 += getRegisterName(freeRegister);

                    asmTriads.push_back(asmStr);
                    asmTriads.push_back(asmStr2);

                    this->freeRegister(freeRegister);
                }
                // = a (i)
                else if (triad.firstOperand.isLink && !triad.secondOperand.isLink) {
                    RegisterType registerType = triad.registerType;
                    // mov a, reg_i
                    std::string asmStr = "\tmov ";
                    if (triad.firstOperand.isConst) {
                        std::string lex = triad.firstOperand.lex;
                        asmStr += lex;
                    }
                    else {
                        Tree* firstOperandPtr = this->tree->findUpByAsmId(triad.firstOperand.lex);
                        Node* firstNodeOperand = this->tree->getNode(firstOperandPtr);
                        if (firstNodeOperand == nullptr)
                            continue;
                        else if (firstNodeOperand->level == 0) {
                            std::string lex = firstNodeOperand->asmId;
                            asmStr += lex;
                        }
                        else {
                            if (firstNodeOperand->stackAddr == 0)
                                continue;
                            std::string lex = "(ebp" + std::to_string(firstNodeOperand->stackAddr) + ")";
                            asmStr += lex;
                        }
                    }
                    asmStr += ", ";
                    asmStr += getRegisterName(registerType);
                    asmTriads.push_back(asmStr);

                    this->freeRegister(registerType);
                }
            }
        }

        // Операции управления кодом (for-цикл)
        if (strcmp(triad.operation, (char*)"<") == 0 || strcmp(triad.operation, (char*)">") == 0) {
            if (strcmp(triad.operation, (char*)">") == 0)
            // > i 5 - при сравнении cmp i, 5
                ifJump = "\tjg SHORT ";
            else if (strcmp(triad.operation, (char*)"<") == 0)
            // < i 5 - при сравнении cmp i, 5
                ifJump = "\tjl SHORT ";

            // cmp a, b
            std::string asmStr = "\tcmp ";
            if (triad.firstOperand.isConst) {
                std::string lex = triad.firstOperand.lex;
                asmStr += lex;
            }
            else {
                Tree* firstOperandPtr = this->tree->findUpByAsmId(triad.firstOperand.lex);
                Node* firstNodeOperand = this->tree->getNode(firstOperandPtr);
                if (firstNodeOperand == nullptr)
                    continue;
                else if (firstNodeOperand->level == 0) {
                    std::string lex = firstNodeOperand->asmId;
                    asmStr += lex;
                }
                else {
                    if (firstNodeOperand->stackAddr == 0)
                        continue;
                    std::string lex = "(ebp" + std::to_string(firstNodeOperand->stackAddr) + ")";
                    asmStr += lex;
                }
            }
            asmStr += ", ";
            if (triad.secondOperand.isConst) {
                std::string lex = triad.secondOperand.lex;
                asmStr += lex;
            }
            else {
                Tree* secondOperandPtr = this->tree->findUpByAsmId(triad.secondOperand.lex);
                Node* secondNodeOperand = this->tree->getNode(secondOperandPtr);
                if (secondNodeOperand == nullptr)
                    continue;
                else if (secondNodeOperand->level == 0) {
                    std::string lex = secondNodeOperand->asmId;
                    asmStr += lex;
                }
                else {
                    if (secondNodeOperand->stackAddr == 0)
                        continue;
                    std::string lex = "(ebp" + std::to_string(secondNodeOperand->stackAddr) + ")";
                    asmStr += lex;
                }
            }

            asmTriads.push_back(asmStr);
        }

        if (strcmp(triad.operation, (char*)"if") == 0) {
            // if (i) (j)
            // Получается, что у нас в ifJump есть правильный прыжок, если условие соблюдено
            // а в триадах по ссылке находится gotoMark - собираем все вместе
            int gotoTriadLinkFirst = triad.firstOperand.number;
            int gotoTriadLinkSecond = triad.secondOperand.number;
            ifJump += triads[gotoTriadLinkFirst].gotoMark;
            std::string jmpAsm = "\tjmp " + triads[gotoTriadLinkSecond].gotoMark;

            asmTriads.push_back(ifJump);
            ifJump = "";
            asmTriads.push_back(jmpAsm);
        }

        if (strcmp(triad.operation, (char*)"goto") == 0) {
            // goto (i)
            // Получается в триаде по ссылке i лежит метка, куда нужно прыгнуть
            int gotoTriadLink = triad.firstOperand.number;
            std::string jmpAsm = "\tjmp " + triads[gotoTriadLink].gotoMark;

            asmTriads.push_back(jmpAsm);
        }
    }
}




