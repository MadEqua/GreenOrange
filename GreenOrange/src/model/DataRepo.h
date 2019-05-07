#pragma once

//CSG types repository
enum class CsgType {
    Union = 0,
    Intersection,
    Subtraction,

    Size
};
extern const char* CsgTypeStrings[];


//Object types repository
enum class ObjectType {
    Sphere,
    Box,

    Size
};
extern const char* ObjectTypeStrings[];


void initDataRepo();