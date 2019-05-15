#include "GlslGenerator.h"

#include <stdio.h>
#include <stack>
#include <vector>
#include <forward_list>
#include <sstream>

#include "model/SceneEntity.h"
#include "model/Project.h"
#include "model/Scene.h"
#include "Constants.h"

#include "glsl/generated/template.frag.h"


GlslGenerator::RpnElement::RpnElement(SceneEntity &sceneEntity) :
    sceneEntity(&sceneEntity),
    isGeneratedCode(false) {
}

GlslGenerator::RpnElement::RpnElement(std::string &&generatedCode) :
    generatedCode(std::move(generatedCode)),
    isGeneratedCode(true) {
}


void GlslGenerator::initGeneration() {
    glslCode.clear();
    glslCode.append(GLSL_VERSION).append("\n").append(template_frag);
}

void GlslGenerator::generateIfNeeded(Project &project) {
    if(needToGenerate) {
        std::stringstream sstream;
        initGeneration();

        for(uint32 i = 0; i < project.getSceneCount(); ++i) {
            Scene &scene = project.getSceneByIndex(i);
            sstream << "float " << scene.getName() << "(vec3 p) {" << std::endl;
            sstream << "return " << generateScene(scene) << ";";
            sstream << std::endl << "}" << std::endl;
        }

        replace(glslCode, REPLACE_SCENES, sstream.str());
        needToGenerate = false;
        currentCodeId++;
    }
}

std::string GlslGenerator::generateScene(Scene &scene) {

    //RPN list of operations and operators (ex: AB+CD-+)
    std::forward_list<RpnElement> rpnList;

    //The list is initialized by doing a DFS and reversing the visit order
    scene.traverseTreeDfs(scene.getRootCsgOperator(), [&rpnList](SceneEntity &ent) -> bool {
        rpnList.emplace_front(ent);
        return false;
    });

    //Stack for the RPN algorithm
    std::stack<RpnElement> rpnStack;

    for(RpnElement &rpnElem : rpnList) {

        //If operand push into stack
        if(rpnElem.isGeneratedCode || rpnElem.sceneEntity->isObject()) {
            rpnStack.push(std::move(rpnElem));
        }
        //If operator pop related operands, generate code and push it into the stack as a new operand
        else {
            CsgOperator &op = dynamic_cast<CsgOperator&>(*rpnElem.sceneEntity);
            
            //Find out how many operands the operator has
            uint32 operandCount = static_cast<uint32>(op.getChildCount());
            
            //Create a list of operands to send to the operator code generator
            std::vector<RpnElement> operandList;
            for(uint32 i = 0; i < operandCount; ++i) {
                operandList.push_back(std::move(rpnStack.top()));
                rpnStack.pop();
            }

            //TODO: this should never happen if we cleanup the Scene of empty nodes
            if(operandList.empty()) {
                rpnStack.push(std::string("0.0"));
                continue;
            }

            std::string code = generateOperator(op, operandList, 0, operandCount);
            rpnStack.push(std::move(code));
        }
    }

    //At the end we should have one only RpnElement with the generated code
    if(rpnStack.size() == 1 && rpnStack.top().isGeneratedCode && !rpnStack.top().generatedCode.empty())
        return rpnStack.top().generatedCode;
    else {
        printf("generateScene(). Something went wrong, check the Scene tree.\n");
        return "0.0"; //TODO: what to do in case of errors?
    }
}

std::string GlslGenerator::generateOperator(const CsgOperator &csgOperator, const std::vector<RpnElement> &operands, uint32 startIdx, uint32 endIdx) {
    std::stringstream sstream;

    int size = endIdx - startIdx;

    //TODO: make pretty
    switch(csgOperator.getType()) {
    case CsgType::Union:
        if(size == 1) {
            sstream << generateOperand(operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "min(";
            sstream << generateOperand(operands[startIdx]);
            sstream << ", ";
            sstream << generateOperand(operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "min(";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ", ";
            sstream << generateOperator(csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Intersection:
        if(size == 1) {
            sstream << generateOperand(operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "max(";
            sstream << generateOperand(operands[startIdx]);
            sstream << ", ";
            sstream << generateOperand(operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ", ";
            sstream << generateOperator(csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Subtraction:
        if(size == 1) {
            sstream << generateOperand(operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "max(";
            sstream << generateOperand(operands[startIdx]);
            sstream << ", -";
            sstream << generateOperand(operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ", -";
            sstream << generateOperator(csgOperator, operands, 2, size);
            sstream << ")";
         }
        break;
    default:
        printf("generateOperator() trying replacement generate code for an unknown type.\n");
        break;
    }

    return sstream.str();
}

std::string GlslGenerator::generateOperand(const RpnElement &stackElement) {
    std::stringstream sstream;

    //This function relies on stackElements not being CsgOperators, only operands (Objects or generated code).
    if(stackElement.isGeneratedCode)
        sstream << stackElement.generatedCode;
    else {
        Object &obj = dynamic_cast<Object&>(*stackElement.sceneEntity);
        
        switch(obj.getType()) {
        case ObjectType::Box:
        {
            sstream << "box(p, vec3(";
            Box &box = dynamic_cast<Box&>(obj);
            float *dims = box.getDimensions();
            sstream << dims[0] << ", " << dims[1] << ", " << dims[2] << "))";
            break;
        }

        case ObjectType::Sphere:
        {
            sstream << "sphere(p, ";
            Sphere &sphere = dynamic_cast<Sphere&>(obj);
            float r = sphere.getRadius();
            sstream << r << ")";
            break;
        }
        default:
            printf("generateOperand() trying replacement generate code for an unknown type.\n");
            break;
        }
    }

    return sstream.str();
}

bool GlslGenerator::replace(std::string& str, const std::string& toReplace, const std::string& replacement) {
    size_t start_pos = str.find(toReplace);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, toReplace.length(), replacement);
    return true;
}