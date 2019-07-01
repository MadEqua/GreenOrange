#pragma once

//Place to declare enum types that will need a string representation

enum class EntityType {
    CsgOperator,
    Object,
    Transform,
    Light,
    Material,
    Probe,

    Size
};
extern const char* EntityTypeStrings[];


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


enum class TransformType {
    Translation,
    Rotation,
    Custom,

    Size
};
extern const char* TransformTypeStrings[];


enum class LightType {
    Directional,
    Point,

    Size
};
extern const char* LightTypeStrings[];


void initDataRepo();