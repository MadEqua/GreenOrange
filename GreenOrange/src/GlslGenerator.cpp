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
#include "model/Light.h"
#include "Constants.h"

#include "glsl/generated/template.frag.h"
#include "glsl/generated/objects.frag.h"
#include "glsl/generated/transforms.frag.h"


GlslGenerator::RpnElement::RpnElement(TreeNode<SceneEntity> &sceneEntityNode) :
    sceneEntityNode(&sceneEntityNode),
    isGeneratedCode(false) {
}

GlslGenerator::RpnElement::RpnElement(std::string &&generatedCode) :
    generatedCode(std::move(generatedCode)),
    isGeneratedCode(true) {
}


GlslGenerator::GlslGenerator() {
    glslCode.reserve(1024 * 8);
}

void GlslGenerator::initGeneration() {
    glslCode.clear();
    glslCode.append(GLSL_VERSION).append("\n").append(template_frag);
}

void GlslGenerator::generateIfNeeded(Project &project) {
    if(needToGenerate) {
        initGeneration();

        //TODO: only add actually needed/used parts
        replace(glslCode, REPLACE_OBJECTS, objects_frag);
        replace(glslCode, REPLACE_TRANSFORMS, transforms_frag);

        generateLights(project);
        generateScenes(project);

        needToGenerate = false;
        currentCodeId++;
    }
}

void GlslGenerator::generateLights(Project &project) {
    uint32 totalDirLightCount = 0;
    uint32 totalPointLightCount = 0;

    std::stringstream sstreamDirLightsSet;
    std::stringstream sstreamPointLightsSet;

    sstreamDirLightsSet << "switch(currentSceneIndex) {" << std::endl;
    sstreamPointLightsSet << "switch(currentSceneIndex) {" << std::endl;

    for(uint32 i = 0; i < project.getSceneCount(); ++i) {
        Scene &scene = project.getSceneByIndex(i);

        uint32 sceneDirLightCount = scene.getDirLightCount();
        uint32 scenePointLightCount = scene.getPointLightCount();

        sstreamDirLightsSet << "case " << i << ":" << std::endl;
        sstreamDirLightsSet << "return ivec2(" << totalDirLightCount << "," << (totalDirLightCount + sceneDirLightCount - 1) << ");" << std::endl;

        sstreamPointLightsSet << "case " << i << ":" << std::endl;
        sstreamPointLightsSet << "return ivec2(" << totalPointLightCount << "," << (totalPointLightCount + scenePointLightCount - 1) << ");" << std::endl;

        totalDirLightCount += sceneDirLightCount;
        totalPointLightCount += scenePointLightCount;
    }

    sstreamDirLightsSet << "}" << std::endl;
    sstreamPointLightsSet << "}" << std::endl;

    replace(glslCode, REPLACE_DIR_LIGHTS_SET, sstreamDirLightsSet.str());
    replace(glslCode, REPLACE_POINT_LIGHTS_SET, sstreamPointLightsSet.str());

    replace(glslCode, REPLACE_DIR_LIGHT_COUNT, std::to_string(totalDirLightCount));
    replace(glslCode, REPLACE_POINT_LIGHT_COUNT, std::to_string(totalPointLightCount));

    std::stringstream sstreamLightInit;
    if(totalDirLightCount) {
        sstreamLightInit << "for(int i = 0; i < " << totalDirLightCount << "; ++i) {" << std::endl;
        for(uint32 s = 0; s < project.getSceneCount(); ++s) {
            Scene &scene = project.getSceneByIndex(s);

            for(uint32 l = 0; l < scene.getLightCount(); ++l) {
                Light &light = scene.getLightByIndex(l);
                auto &col = light.getColor();

                if(light.isDirectional()) {
                    auto &dir = static_cast<DirectionalLight&>(light).getDirection();
                    sstreamLightInit << "    dirLights[i].color = vec3(" << col[0] << "," << col[1] << "," << col[2] << ");" << std::endl;
                    sstreamLightInit << "    dirLights[i].direction = vec3(" << dir[0] << "," << dir[1] << "," << dir[2] << ");" << std::endl;
                }
            }
        }
        sstreamLightInit << "}" << std::endl;
    }

    if(totalPointLightCount) {
        sstreamLightInit << "for(int i = 0; i < " << totalPointLightCount << "; ++i) {" << std::endl;
        for(uint32 s = 0; s < project.getSceneCount(); ++s) {
            Scene &scene = project.getSceneByIndex(s);

            for(uint32 l = 0; l < scene.getLightCount(); ++l) {
                Light &light = scene.getLightByIndex(l);
                auto &col = light.getColor();
                if(light.isPoint()) {
                    auto &pos = static_cast<PointLight&>(light).getPosition();
                    sstreamLightInit << "    pointLights[i].color = vec3(" << col[0] << "," << col[1] << "," << col[2] << ");" << std::endl;
                    sstreamLightInit << "    pointLights[i].position = vec3(" << pos[0] << "," << pos[1] << "," << pos[2] << ");" << std::endl;
                }
            }
        }
        sstreamLightInit << "}" << std::endl;
    }

    replace(glslCode, REPLACE_LIGHTS_INIT, sstreamLightInit.str());
}

void GlslGenerator::generateScenes(Project &project) {
    std::stringstream sstream;
    for(uint32 i = 0; i < project.getSceneCount(); ++i) {
        Scene &scene = project.getSceneByIndex(i);
        sstream << generateScene(scene) << std::endl;
    }
    replace(glslCode, REPLACE_SCENE_SDFS, sstream.str());
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
    std::string templateString;
    templateString.reserve(512);

    switch(csgOperator.getType()) {
    case CsgType::Union:
        templateString = "min(#OP1,#OP2)";
        break;
    case CsgType::Intersection:
        templateString = "max(#OP1,#OP2)";
        break;
    case CsgType::Subtraction:
        templateString = "max(#OP1,-#OP2)";
        break;
    default:
        GO_ASSERT_ALWAYS();
    }

    int size = endIdx - startIdx;
    if(size == 1) {
        return generateOperand(scene, operands[startIdx]);
    }
    else if(size == 2) {
        replace(templateString, "#OP1", generateOperand(scene, operands[startIdx]));
        replace(templateString, "#OP2", generateOperand(scene, operands[startIdx + 1]));
    }
    else {
        replace(templateString, "#OP1", generateOperator(scene, csgOperator, operands, 0, 2));
        replace(templateString, "#OP2", generateOperator(scene, csgOperator, operands, 2, size));
    }

    return templateString;
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
                p = generateTransformName(static_cast<Transform&>(*transform));
        }

        switch(obj.getType()) {
        case ObjectType::Box:
        {
            sstream << "box(" << p << ", vec3(";
            Box &box = static_cast<Box&>(obj);
            auto &dims = box.getDimensions();
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
        }
    }

    return sstream.str();
}

std::string GlslGenerator::generateTransform(TreeNode<Transform> &transformNode, TreeNode<Transform> *parentTransformNode) {
    std::stringstream sstream;

    Transform &transform = static_cast<Transform&>(*transformNode);
    std::string parentP = parentTransformNode ? generateTransformName(parentTransformNode->getPayload()) : "p";
    
    sstream << "vec3 " << generateTransformName(transform) << " = ";
    switch(transform.getType()) {
    case TransformType::Translation:
    {
        Translation &translation = static_cast<Translation&>(transform);
        sstream << parentP << " - vec3(" << translation.getAmmount()[0] << "," << translation.getAmmount()[1] << "," << translation.getAmmount()[2] << ")";
        break;
    }
    case TransformType::Rotation:
    {
        Rotation &rotation = static_cast<Rotation&>(transform);
        sstream << "rotateEuler(" << rotation.getAmmount()[0] << "," << rotation.getAmmount()[1] << "," << rotation.getAmmount()[2] << ") * " << parentP;
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
    }

    return sstream.str();
}

std::string GlslGenerator::generateTransformName(const Transform &transform) {
    return transform.getName() + "_" + std::to_string(transform.getId());
}

bool GlslGenerator::replace(std::string& str, const std::string& toReplace, const std::string& replacement) {
    size_t index = 0;
    bool replaced = false;
    while(true) {
        index = str.find(toReplace, index);
        if(index == std::string::npos) break;
        str.replace(index, toReplace.length(), replacement);
        index += toReplace.length();
        replaced = true;
    }
    return replaced;
}