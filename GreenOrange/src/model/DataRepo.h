#pragma once


//CSG types repository
enum class CsgType {
    Union = 0,
    Intersection,
    Subtraction,

    Size
};
extern const char* CsgTypeStrings[];


void initDataRepo();