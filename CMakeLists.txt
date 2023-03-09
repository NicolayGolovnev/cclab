cmake_minimum_required(VERSION 3.15)
project(mag_analyzator)

set(CMAKE_CXX_STANDARD 14)

add_executable(mag_analyzator defs.h main.cpp
        Scaner.h Scaner.cpp TestsScanner/TestScanner.cpp
        Magazine.cpp Magazine.h)