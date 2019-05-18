#include "GlslGenerator.h"

#include <stdio.h>
#include <stack>
#include <vector>
#include <forward_list>
#include <sstream>

#include "model/SceneEntity.h"
#include "model/Object.h"
#include "model/CsgOperator.h"
#include "model/Transform.h"
#include "model/Project.h"
#include "model/Scene.h"
#include "Constants.h"

#include "glsl/generated/template.frag.h"


GlslGenerator::RpnElement::RpnElement(TreeNode<SceneEntity> &sceneEntityNode) :
    sceneEntityNode(&sceneEntityNode),
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
            sstream << generateScene(scene) << std::endl;
        }

        replace(glslCode, REPLACE_SCENES, sstream.str());
        needToGenerate = false;
        currentCodeId++;
    }
}


std::string GlslGenerator::generateScene(Scene &scene) {
    std::stringstream sstream;
    sstream << "float " << scene.getName() << "(vec3 p) {" << std::endl;

    //Transformed Ps
    //TODO: only put transforms that are actually in use
    for(uint32 i = 0; i < scene.getTransformTreeCount(); ++i) {
        scene.getTransformTreeRootNodeByIndex(i).traverseBfs([&sstream](TreeNode<Transform> &node, TreeNode<Transform> *parent) -> bool {
            if(node->isTransform())
                sstream << "    " << generateTransform(node, parent) << ";" << std::endl;
            return false;
        });
    }

    sstream << "    return " << generateSceneTree(scene) << ";" << std::endl;
    sstream << "}" << std::endl;
    return sstream.str();
}


std::string GlslGenerator::generateSceneTree(Scene &scene) {
    //RPN list of operations and operators (ex: AB+CD-+)
    std::forward_list<RpnElement> rpnList;

    //The list is initialized by doing a DFS and reversing the visit order
    scene.getCsgTreeRootNode().traverseDfs([&rpnList](TreeNode<SceneEntity> &ent) -> bool {
        rpnList.emplace_front(ent);
        return false;
    });

    //Stack for the RPN algorithm
    std::stack<RpnElement> rpnStack;

    for(RpnElement &rpnElem : rpnList) {

        //If is operand push into stack
        if(rpnElem.isGeneratedCode || rpnElem.sceneEntityNode->getPayload().isObject()) {
            rpnStack.push(std::move(rpnElem));
        }
        //If is operator pop related operands, generate code and push it into the stack as a new operand
        else {
            TreeNode<SceneEntity> &node = *rpnElem.sceneEntityNode;
            CsgOperator &op = static_cast<CsgOperator&>(*node);

            //Find out how many *non-empty* operands the operator has. Basically we ignore all empty child CSG operators
            uint32 operandCount = countNonEmptyOperands(node);
            if(operandCount) {

                //Create a list of operands to send to the operator code generator
                std::vector<RpnElement> operandList;
                for(uint32 i = 0; i < operandCount; ++i) {
                    operandList.push_back(std::move(rpnStack.top()));
                    rpnStack.pop();
                }

                std::string code = generateOperator(scene, op, operandList, 0, operandCount);
                rpnStack.push(std::move(code));
            }
        }
    }

    //At the end we should have one only RpnElement with the generated code
    if(rpnStack.size() == 1 && rpnStack.top().isGeneratedCode && !rpnStack.top().generatedCode.empty())
        return rpnStack.top().generatedCode;
    else
        printf("generateScene(). Something went wrong, check the Scene tree.\n");
    return "";
}

uint32 GlslGenerator::countNonEmptyOperands(TreeNode<SceneEntity> &root) {
    uint32 count = 0;

    for(uint32 i = 0; i < root.getChildCount(); ++i) {
        TreeNode<SceneEntity> &child = root.getChildByIndex(i);
        
        if(child->isCsgOperator()) {
            if(countNonEmptyOperands(child))
                count++;
        }
        else if(child->isObject()) {
            count++;
        }
    }
    return count;
}

std::string GlslGenerator::generateOperator(Scene &scene, const CsgOperator &csgOperator, const std::vector<RpnElement> &operands, uint32 startIdx, uint32 endIdx) {
    std::stringstream sstream;

    int size = endIdx - startIdx;

    //TODO: make pretty
    switch(csgOperator.getType()) {
    case CsgType::Union:
        if(size == 1) {
            sstream << generateOperand(scene, operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "min(";
            sstream << generateOperand(scene, operands[startIdx]);
            sstream << ", ";
            sstream << generateOperand(scene, operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "min(";
            sstream << generateOperator(scene, csgOperator, operands, 0, 2);
            sstream << ", ";
            sstream << generateOperator(scene, csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Intersection:
        if(size == 1) {
            sstream << generateOperand(scene, operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "max(";
            sstream << generateOperand(scene, operands[startIdx]);
            sstream << ", ";
            sstream << generateOperand(scene, operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(";
            sstream << generateOperator(scene, csgOperator, operands, 0, 2);
            sstream << ", ";
            sstream << generateOperator(scene, csgOperator, operands, 2, size);
            sstream << ")";
        }
        break;
    case CsgType::Subtraction:
        if(size == 1) {
            sstream << generateOperand(scene, operands[startIdx]);
        }
        else if(size == 2) {
            sstream << "max(";
            sstream << generateOperand(scene, operands[startIdx]);
            sstream << ", -";
            sstream << generateOperand(scene, operands[startIdx + 1]);
            sstream << ")";
        }
        else if(size > 2) {
            sstream << "max(";
            sstream << generateOperator(scene, csgOperator, operands, 0, 2);
            sstream << ", -";
            sstream << generateOperator(scene, csgOperator, operands, 2, size);
            sstream << ")";
         }
        break;
    default:
        GO_ASSERT_ALWAYS();
        break;
    }

    return sstream.str();
}

std::string GlslGenerator::generateOperand(Scene &scene, const RpnElement &rpnElement) {
    std::stringstream sstream;

    if(rpnElement.isGeneratedCode)
        sstream << rpnElement.generatedCode;
    else {
        std::string p = "p";
        Object &obj = static_cast<Object&>(rpnElement.sceneEntityNode->getPayload());
        if(obj.isAttachedToTransform()) {
            SceneEntity *transform = scene.findSceneEntity(obj.getTransformId());
            if(transform)
                p = transform->getName();
        }

        switch(obj.getType()) {
        case ObjectType::Box:
        {
            sstream << "box(" << p << ", vec3(";
            Box &box = static_cast<Box&>(obj);
            float *dims = box.getDimensions();
            sstream << dims[0] << ", " << dims[1] << ", " << dims[2] << "))";
            break;
        }

        case ObjectType::Sphere:
        {
            sstream << "sphere(" << p << ", ";
            Sphere &sphere = static_cast<Sphere&>(obj);
            float r = sphere.getRadius();
            sstream << r << ")";
            break;
        }
        default:
            GO_ASSERT_ALWAYS();
            break;
        }
    }

    return sstream.str();
}

std::string GlslGenerator::generateTransform(TreeNode<Transform> &transformNode, TreeNode<Transform> *parentTransformNode) {
    std::stringstream sstream;

    Transform &transform = static_cast<Transform&>(*transformNode);
    std::string parentP = parentTransformNode ? parentTransformNode->getPayload().getName() : "p";
    
    sstream << "vec3 " << transform.getName() << " = ";
    switch(transform.getType()) {
    case TransformType::Translation:
    {
        Translation &translation = static_cast<Translation&>(transform);
        sstream << parentP << " - vec3(" << translation.getAmmount()[0] << "," << translation.getAmmount()[1] << "," << translation.getAmmount()[2] << ")";
        break;
    }
    case TransformType::Rotation:
    {
        //TODO
        break;
    }
    case TransformType::Custom:
    {
        CustomTransform &custom = static_cast<CustomTransform&>(transform);
        sstream << custom.getCode();
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
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