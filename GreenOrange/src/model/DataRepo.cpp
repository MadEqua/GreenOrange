#include "DataRepo.h"

const char* CsgTypeStrings[static_cast<int>(CsgType::Size)];
const char* ObjectTypeStrings[static_cast<int>(ObjectType::Size)];

void initDataRepo() {
    CsgTypeStrings[static_cast<int>(CsgType::Union)] = "Union";
    CsgTypeStrings[static_cast<int>(CsgType::Intersection)] = "Intersection";
    CsgTypeStrings[static_cast<int>(CsgType::Subtraction)] = "Subtraction";

    ObjectTypeStrings[static_cast<int>(ObjectType::Sphere)] = "Sphere";
    ObjectTypeStrings[static_cast<int>(ObjectType::Box)] = "Box";
}