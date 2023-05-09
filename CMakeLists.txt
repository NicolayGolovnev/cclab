cmake_minimum_required(VERSION 3.15)
project(mag_analyzator)

set(CMAKE_CXX_STANDARD 14)

add_executable(mag_analyzator defs.h main.cpp
        scaner/Scaner.h scaner/Scaner.cpp scaner/TestScanner.cpp
        magazine/Magazine.cpp magazine/Magazine.h magazine/TestMagazine.cpp
        semantic/Semantic.h semantic/Semantic.cpp semantic/TestSemantic.cpp
        translator/Translate.cpp translator/Translate.h translator/TranslatorHelper.h
        triad/GenerTriad.cpp triad/GenerTriad.h triad/TriadHelper.h)