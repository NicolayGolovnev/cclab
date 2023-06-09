//
// Created by kolya on 08.06.2023.
//

#ifndef MAG_ANALYZATOR_GENERASM_H
#define MAG_ANALYZATOR_GENERASM_H


#include "../semantic/Semantic.h"

class GenerAsm {
private:
    Tree* tree;
    GlobalData* global;

    std::deque<std::string> globalDataDeclaration;
    std::deque<std::string> constDataDeclaration;
    std::deque<std::string> asmCode;
public:
    GenerAsm();
    void setTree(Tree* tree);
    void setGlobal(GlobalData* global);

    void printAllData();

    void preInitializeTree();
    void generateGlobalData();
    void generateMainFunc();
};


#endif //MAG_ANALYZATOR_GENERASM_H
