#include "DataRepo.h"

const char* SceneEntityTypeStrings[static_cast<int>(SceneEntityType::Size)];
const char* CsgTypeStrings[static_cast<int>(CsgType::Size)];
const char* ObjectTypeStrings[static_cast<int>(ObjectType::Size)];
const char* PanelTypeStrings[static_cast<int>(PanelType::Size)];

void initDataRepo() {
    SceneEntityTypeStrings[static_cast<int>(SceneEntityType::Object)] = "Object";
    SceneEntityTypeStrings[static_cast<int>(SceneEntityType::CsgOperator)] = "CSG Operator";


    CsgTypeStrings[static_cast<int>(CsgType::Union)] = "Union";
    CsgTypeStrings[static_cast<int>(CsgType::Intersection)] = "Intersection";
    CsgTypeStrings[static_cast<int>(CsgType::Subtraction)] = "Subtraction";

    ObjectTypeStrings[static_cast<int>(ObjectType::Sphere)] = "Sphere";
    ObjectTypeStrings[static_cast<int>(ObjectType::Box)] = "Box";

    PanelTypeStrings[static_cast<int>(PanelType::Project)] = "Project";
    PanelTypeStrings[static_cast<int>(PanelType::Scene)] = "Scene";
    PanelTypeStrings[static_cast<int>(PanelType::Preview)] = "Preview";
    PanelTypeStrings[static_cast<int>(PanelType::Inspector)] = "Inspector";
    PanelTypeStrings[static_cast<int>(PanelType::GeneratedGlsl)] = "Generated GLSL";
    PanelTypeStrings[static_cast<int>(PanelType::Console)] = "Console";
}