#include "DataRepo.h"

const char* CsgTypeStrings[static_cast<int>(CsgType::Size)];

void initDataRepo() {
    CsgTypeStrings[static_cast<int>(CsgType::Union)] = "Union";
    CsgTypeStrings[static_cast<int>(CsgType::Intersection)] = "Intersection";
    CsgTypeStrings[static_cast<int>(CsgType::Subtraction)] = "Subtraction";
}