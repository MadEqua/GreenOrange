#pragma once

//Place to declare enum types that will need a string representation

enum class SceneEntityType {
    CsgOperator,
    Object,

    Size
};
extern const char* SceneEntityTypeStrings[];


enum class CsgType {
    Union = 0,
    Intersection,
    Subtraction,

    Size
};
extern const char* CsgTypeStrings[];


enum class ObjectType {
    Sphere,
    Box,

    Size
};
extern const char* ObjectTypeStrings[];


enum class PanelType {
    Project,
    Scene,
    Inspector,
    Preview,
    GeneratedGlsl,
    Console,

    Size
};
extern const char* PanelTypeStrings[];


void initDataRepo();