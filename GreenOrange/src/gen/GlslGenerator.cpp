#include "GlslGenerator.h"

#include <stdio.h>
#include <stack>
#include <sstream>
#include <fstream>

#include "../model/SceneEntity.h"
#include "../model/Project.h"
#include "../model/Scene.h"
#include "../Constants.h"


GlslGenerator::StackElement::StackElement(SceneEntity &sceneEntity) :
    sceneEntity(&sceneEntity),
    isGeneratedCode(false) {
}

GlslGenerator::StackElement::StackElement(std::string &&generatedCode) :
    generatedCode(std::move(generatedCode)),
    isGeneratedCode(true) {
}

GlslGenerator::GlslGenerator() {
    readTemplateFile();
}

const std::string& GlslGenerator::generate(Project &project) {
    std::stringstream sstream;

    for(uint32 i = 0; i < project.getSceneCount(); ++i) {
        Scene &scene = project.getSceneByIndex(i);
        sstream << "float " << scene.getName() << "(vec3 p) {" << std::endl;
        sstream << "return " << generateScene(scene) << ";";
        sstream << std::endl << "}" << std::endl;
    }

    replace(templateGlsl, REPLACE_SCENES, sstream.str());
    return templateGlsl;
}

std::string GlslGenerator::generateScene(Scene &scene) {
    
    //This stack will contain the Scene data in reverse polish notation.
    std::stack<StackElement> stack;

    //It's important that this traversal visits the Object nodes first and the CsgOperators second.
    scene.traverseTreeDfs(scene.getRootCsgOperator(), [&stack](SceneEntity &ent, CsgOperator* parent) -> bool {
        stack.emplace(ent);
        return false;
    });

    while(stack.size() > 1) {
        
        std::vector<StackElement> operands;

        //Pop all the Objects and already generated code to the next batch of operands
        while(true)  {
            StackElement &stackElement = stack.top();
            if(!stackElement.isGeneratedCode && !stackElement.sceneEntity->isObject())
                break;

            operands.emplace_back(std::move(stackElement));
            stack.pop();
        }

        //At this point we know that the next stackElement is a CsgOperator
        if(!operands.empty()) {
            std::string code = generateOperator(dynamic_cast<CsgOperator&>(*stack.top().sceneEntity), operands, 0, operands.size());
            stack.pop();

            //The result of the operator will go back to the stack as generated code
            stack.emplace(std::move(code));
        }
        else {
            printf("generateScene(). A CsgOperator does not have enough operands.\n");
        }
    }

    //At the end we should have one only StackElement with the generated code
    if(stack.size() == 1 && stack.top().isGeneratedCode)
        return stack.top().generatedCode;
    else {
        printf("generateScene(). Something went wrong, check the Scene tree.\n");
        return "";
    }
}

std::string GlslGenerator::generateOperator(const CsgOperator &csgOperator, const std::vector<StackElement> &operands, int startIdx, int endIdx) {
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
            sstream << ",";
            sstream << generateOperand(operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "min(";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ",";
            sstream << generateOperator(csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Intersection:
        if(size == 1) {
            sstream << generateOperand(operands[0]);
        }
        else if(size == 2) {
            sstream << "max(";
            sstream << generateOperand(operands[0]);
            sstream << ",";
            sstream << generateOperand(operands[1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ",";
            sstream << generateOperator(csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Subtraction:
        if(size == 1) {
            sstream << generateOperand(operands[0]);
        }
        else if(size == 2) {
            sstream << "max(-";
            sstream << generateOperand(operands[0]);
            sstream << ",";
            sstream << generateOperand(operands[1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(-";
            sstream << generateOperator(csgOperator, operands, 0, 2);
            sstream << ",";
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

std::string GlslGenerator::generateOperand(const StackElement &stackElement) {
    std::stringstream sstream;

    //This function relies on stackElements not being CsgOperators, only operands (Objects or generated code).
    if(stackElement.isGeneratedCode)
        sstream << stackElement.generatedCode;
    else {
        switch(dynamic_cast<Object&>(*stackElement.sceneEntity).getType()) {
        case ObjectType::Box:
            sstream << "box(p, vec3(1., .5, .5))";
            break;
        case ObjectType::Sphere:
            sstream << "sphere(p, .9)";
            break;
        default:
            printf("generateOperand() trying replacement generate code for an unknown type.\n");
            break;
        }
    }

    return sstream.str();
}

void GlslGenerator::readTemplateFile() {
    std::ifstream file;
    file.open("../../src/glsl/template.frag"); //TODO: figure out folder
    if(!file.is_open()) {
        printf("readTemplateFile(). Error opening file.");
    }

    std::stringstream ss;
    ss << GLSL_VERSION << '\n' << file.rdbuf();
    templateGlsl = ss.str();
    file.close();
}

bool GlslGenerator::replace(std::string& str, const std::string& toReplace, const std::string& replacement) {
    size_t start_pos = str.find(toReplace);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, toReplace.length(), replacement);
    return true;
}