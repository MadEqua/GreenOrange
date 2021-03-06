#include "GlslGenerator.h"

#include <stdio.h>
#include <stack>
#include <vector>
#include <forward_list>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include "model/Entity.h"
#include "model/Object.h"
#include "model/CsgOperator.h"
#include "model/Transform.h"
#include "model/Project.h"
#include "model/Scene.h"
#include "model/Light.h"
#include "model/Material.h"
#include "model/Probe.h"
#include "Constants.h"

#include "glsl/generated/template.frag.h"
#include "glsl/generated/objects.frag.h"
#include "glsl/generated/transforms.frag.h"
#include "glsl/generated/operators.frag.h"


GlslGenerator previewGenerator(GlslGenerator::GenerationType::Preview);

GlslGenerator::RpnElement::RpnElement(TreeNode<Entity> &treeNode) {
    if(treeNode->isCsgOperator()) {
        type = Type::CsgTreeNode;
        this->csgTreeNode = &treeNode;
    }
    else {
        type = Type::Entity;
        entity = &treeNode.getPayload();
    }
}

GlslGenerator::RpnElement::RpnElement(Entity &entity) :
    entity(&entity),
    type(Type::Entity) {
}

GlslGenerator::RpnElement::RpnElement(std::string &&generatedCode) :
    generatedCode(std::move(generatedCode)),
    type(Type::Code) {
}


GlslGenerator::GlslGenerator(GenerationType type) :
    type(type) {
    glslCode.reserve(1024 * 8);
}

GlslGenerator::GlslGenerator(const Probe &probe) :
    type(GlslGenerator::GenerationType::Probe),
    probe(&probe) {
    glslCode.reserve(1024 * 8);
}

void GlslGenerator::generate(Project &project) {
    switch(type) {
    case GenerationType::Preview:
        if(needToGenerate) {
            internalGenerate(project);
            needToGenerate = false;
        }
    break;
    case GenerationType::Probe:
    case GenerationType::Export:
        internalGenerate(project);
        break;
    }

}

void GlslGenerator::initGeneration() {
    glslCode.clear();
    glslCode.append(GLSL_VERSION).append("\n").append(template_frag);
}

void GlslGenerator::internalGenerate(Project &project) {
    initGeneration();

    //TODO: only add actually needed/used parts
    replace(glslCode, REPLACE_OBJECTS, objects_frag);
    replace(glslCode, REPLACE_TRANSFORMS, transforms_frag);
    replace(glslCode, REPLACE_OPERATORS, operators_frag);

    generateScenes(project);
    generateLights(project);
    generateMaterials(project);
    
    if(type == GenerationType::Probe)
        generateCamerasForProbe(project);
    else
        generateCameras(project);

    generateIrradianceCoeffs(project);

    replace(glslCode, REPLACE_RENDERING_PROBE, type == GenerationType::Probe ? "1" : "0");
    replace(glslCode, REPLACE_INDIRECT_LIGHT, type != GenerationType::Probe && project.getPreviewIndirectLight() ? "1" : "0");

    currentCodeId++;
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

        uint32 sceneDirLightCount = project.getOnlyPreviewStaticObjects() ? scene.getStaticDirLightCount() : scene.getDirLightCount();
        uint32 scenePointLightCount = project.getOnlyPreviewStaticObjects() ? scene.getStaticPointLightCount() : scene.getPointLightCount();

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
        for(uint32 s = 0; s < project.getSceneCount(); ++s) {
            Scene &scene = project.getSceneByIndex(s);

            int idx = 0;
            for(uint32 l = 0; l < scene.getLightCount(); ++l) {
                Light &light = scene.getLightByIndex(l);
                if(light.isDirectional()) {
                    auto &col = light.getColor();

                    bool useLight = false;
                    if(type == GenerationType::Preview && (!project.getOnlyPreviewStaticObjects() || light.isStatic())) {
                        useLight = true;
                    }
                    else if(type == GenerationType::Probe && light.isStatic()) {
                        useLight = true;
                    }

                    if(useLight) {
                        auto &dir = static_cast<DirectionalLight&>(light).getDirection();
                        sstreamLightInit << "dirLights[" << idx << "].color = vec3(" << col[0] << "," << col[1] << "," << col[2] << ");" << std::endl;
                        sstreamLightInit << "dirLights[" << idx << "].direction = vec3(" << dir[0] << "," << dir[1] << "," << dir[2] << ");" << std::endl;
                        sstreamLightInit << "dirLights[" << idx << "].intensity = " << light.getIntensity() << ";" << std::endl;
                        idx++;
                    }
                }
            }
        }
        sstreamLightInit << std::endl;
    }

    if(totalPointLightCount) {
        for(uint32 s = 0; s < project.getSceneCount(); ++s) {
            Scene &scene = project.getSceneByIndex(s);

            int idx = 0;
            for(uint32 l = 0; l < scene.getLightCount(); ++l) {
                Light &light = scene.getLightByIndex(l);
                if(light.isPoint()) {
                    auto &col = light.getColor();

                    bool useLight = false;
                    if(type == GenerationType::Preview && (!project.getOnlyPreviewStaticObjects() || light.isStatic())) {
                        useLight = true;
                    }
                    else if(type == GenerationType::Probe && light.isStatic()) {
                        useLight = true;
                    }

                    if(useLight) {
                        auto &pos = static_cast<PointLight&>(light).getPosition();
                        sstreamLightInit << "pointLights[" << idx << "].color = vec3(" << col[0] << "," << col[1] << "," << col[2] << ");" << std::endl;
                        sstreamLightInit << "pointLights[" << idx << "].position = vec3(" << pos[0] << "," << pos[1] << "," << pos[2] << ");" << std::endl;
                        sstreamLightInit << "pointLights[" << idx << "].intensity = " << light.getIntensity() << ";" << std::endl;
                        idx++;
                    }
                }
            }
        }
        sstreamLightInit << std::endl;
    }

    replace(glslCode, REPLACE_LIGHTS_INIT, sstreamLightInit.str());
}

void GlslGenerator::generateMaterials(Project &project) {
    replace(glslCode, REPLACE_MATERIAL_COUNT, std::to_string(project.getMaterialCount()));

    std::stringstream sstream;
    for(uint32 i = 0; i < project.getMaterialCount(); ++i) {
        Material &mat = project.getMaterialByIndex(i);
        auto &baseCol = mat.getBaseColor();
        auto &emissiveColor = mat.getEmissiveColor();
        sstream << "materials[" << i << "].baseColor = vec3(" << baseCol[0] << ", " << baseCol[1] << ", " << baseCol[2] << "); " << std::endl;
        sstream << "materials[" << i << "].metallic = " << mat.getMetallic() << ";" << std::endl;
        sstream << "materials[" << i << "].roughness = " << mat.getRoughness() << ";" << std::endl;
        sstream << "materials[" << i << "].emissiveColor = vec3(" << emissiveColor[0] << ", " << emissiveColor[1] << ", " << emissiveColor[2] << "); " << std::endl;
        sstream << "materials[" << i << "].emissiveIntensity = " << mat.getEmissiveIntensity() << ";" << std::endl;
    }
    replace(glslCode, REPLACE_MATERIALS_INIT, sstream.str());
}

void GlslGenerator::generateCameras(Project &project) {
    //TODO
    replace(glslCode, REPLACE_CAMERA_COUNT, std::to_string(1));
    replace(glslCode, REPLACE_CAMERA_CURRENT, "return 0;");

    //TODO
    std::stringstream sstream;
    glm::vec3 pos(0.0f, 0.0f, -10.0f);
    glm::mat3 axis = glm::mat3(1.0f);
    sstream << "cameras[0].pos = vec3(" << pos[0] << ", " << pos[1] << ", " << pos[2] << "); " << std::endl;
    sstream << "cameras[0].axis = mat3(" << axis[0][0] << ", " << axis[0][1] << ", " << axis[0][2] << ", " <<
                                            axis[1][0] << ", " << axis[1][1] << ", " << axis[1][2] << ", " <<
                                            axis[2][0] << ", " << axis[2][1] << ", " << axis[2][2] << "); " << std::endl;
    sstream << "cameras[0].fovy =" << glm::half_pi<float>() << ";" << std::endl;
    replace(glslCode, REPLACE_CAMERAS_INIT, sstream.str());
}

void GlslGenerator::generateCamerasForProbe(Project &project) {
    replace(glslCode, REPLACE_CAMERA_COUNT, "6");
    replace(glslCode, REPLACE_CAMERA_CURRENT, "return gl_Layer;");

    std::stringstream sstream;
    for(uint32 i = 0; i < 6; ++i) {
        auto &pos = probe->getPosition();
        sstream << "cameras[" << i << "].pos = vec3(" << pos[0] << ", " << pos[1] << ", " << pos[2] << "); " << std::endl;
        glm::mat3 axis;
        switch(i)
        {
        case 0: //X+
            axis = glm::mat3(0, 0, 1,  0, 1, 0,  1, 0, 0);
            break;
        case 1: //X-
            axis = glm::mat3(0, 0, -1,  0, 1, 0,  -1, 0, 0);
            break;
        case 2: //Y+
            axis = glm::mat3(1, 0, 0,  0, 0, -1,  0, 1, 0);
            break;
        case 3: //Y-
            axis = glm::mat3(1, 0, 0,  0, 0, -1,  0, -1, 0);
            break;
        case 4: //Z+
            axis = glm::mat3(-1, 0, 0,  0, 1, 0,  0, 0, 1);
            break;
        case 5: //Z-
            axis = glm::mat3(1, 0, 0,  0, 1, 0,  0, 0, -1);
            break;
        }
        sstream << "cameras[" << i << "].axis = mat3(" << axis[0][0] << ", " << axis[0][1] << ", " << axis[0][2] << ", " <<
                                                          axis[1][0] << ", " << axis[1][1] << ", " << axis[1][2] << ", " <<
                                                          axis[2][0] << ", " << axis[2][1] << ", " << axis[2][2] << "); " << std::endl;
        sstream << "cameras[" << i << "].fovy =" << glm::half_pi<float>() << ";" << std::endl;
    }
    replace(glslCode, REPLACE_CAMERAS_INIT, sstream.str());
}

void GlslGenerator::generateIrradianceCoeffs(Project &project) {
    std::stringstream sstream;

    //TODO multiple probes and multiple scenes
    if(project.getSceneByIndex(0).getProbeCount()) {
        auto coeffs = project.getSceneByIndex(0).getProbeByIndex(0).getIrradianceCoeffs();
        for(int i = 0; i < 9; ++i) {
            sstream << "irradianceCoeffs[0][" << i << "] = vec3(" << coeffs[i].x << "," << coeffs[i].y << "," << coeffs[i].z << ");" << std::endl;
        }
        replace(glslCode, REPLACE_IRRADIANCE_COEFFS, sstream.str());
    }
    else {
        replace(glslCode, REPLACE_IRRADIANCE_COEFFS, "");
    }
}


void GlslGenerator::generateScenes(Project &project) {
    std::stringstream sstream;
    for(uint32 i = 0; i < project.getSceneCount(); ++i) {
        Scene &scene = project.getSceneByIndex(i);
        sstream << generateScene(project, scene) << std::endl;
    }
    replace(glslCode, REPLACE_SCENE_SDFS, sstream.str());
}

std::string GlslGenerator::generateScene(Project &project, Scene &scene) {
    std::stringstream sstream;
    sstream << "SceneQuery " << scene.getName() << "(vec3 p) {" << std::endl;
    sstream << "SceneQuery sq;" << std::endl;

    //Transformed Ps
    //TODO: only put transforms that are actually in use
    for(uint32 i = 0; i < scene.getTransformTreeCount(); ++i) {
        scene.getTransformTreeRootNodeByIndex(i).traverseBfs([&sstream](TreeNode<Transform> &node, TreeNode<Transform> *parent) -> bool {
            if(node->isTransform())
                sstream << "    " << generateTransform(node, parent) << ";" << std::endl;
            return false;
        });
    }

    sstream << "vec2 result = " << generateSceneTree(project, scene) << ";" << std::endl;
    sstream << "sq.dist = result.x; sq.matIdx = int(result.y); return sq;" << std::endl;
    sstream << "}" << std::endl;
    return sstream.str();
}

std::string GlslGenerator::generateSceneTree(Project &project, Scene &scene) {
    //RPN list of operations and operators (ex: AB+CD-+)
    std::forward_list<RpnElement> rpnList;

    //The list is initialized by doing a DFS and reversing the visit order
    scene.getCsgTreeRootNode().traverseDfs([&rpnList](TreeNode<Entity> &ent) -> bool {
        rpnList.emplace_front(ent);
        return false;
    });

    //Add probe previews to the root union if needed.
    if(type == GenerationType::Preview && project.getPreviewProbes()) {
        for(int i = 0; i < scene.getProbeCount(); ++i) {
            rpnList.emplace_front(scene.getProbeByIndex(i));
        }
    }

    //Stack for the RPN algorithm
    std::stack<RpnElement> rpnStack;

    for(RpnElement &rpnElem : rpnList) {

        //If is operand push into stack
        if(rpnElem.containsGeneratedCode()) {
            rpnStack.push(std::move(rpnElem));
        }
        else if(rpnElem.containsEntity()) {
            if(rpnElem.entity->isObject()) {
                Object &obj = static_cast<Object&>(*rpnElem.entity);
                if(type == GenerationType::Preview && (!project.getOnlyPreviewStaticObjects() || obj.isStatic())) {
                    rpnStack.push(std::move(rpnElem));
                }
                else if(type == GenerationType::Probe && obj.isStatic()) {
                    rpnStack.push(std::move(rpnElem));
                }
            }
            else if(rpnElem.entity->isProbe() && project.getPreviewProbes()) {
                rpnStack.push(std::move(rpnElem));
            }
        }
        //If is operator pop related operands, generate code and push it into the stack as a new operand
        else if(rpnElem.containsCsgTreeNode()) {
            TreeNode<Entity> &csgNode = *rpnElem.csgTreeNode;
            CsgOperator &op = static_cast<CsgOperator&>(*csgNode);

            //Find out how many *non-empty* operands the operator has. Basically we ignore all empty child CSG operators
            //We also add the probe count for the root operator if previewing probes.
            uint32 operandCount = countNonEmptyOperands(csgNode, project.getOnlyPreviewStaticObjects());
            if(type == GenerationType::Preview && project.getPreviewProbes() && op == scene.getCsgRootOperator()) {
                operandCount += scene.getProbeCount();
            }

            if(operandCount) {
                //Create a list of operands to send to the operator code generator
                std::vector<RpnElement> operandList;
                for(uint32 i = 0; i < operandCount; ++i) {
                    operandList.push_back(std::move(rpnStack.top()));
                    rpnStack.pop();
                }

                std::string code = generateOperator(project, scene, op, operandList, 0, operandCount);
                rpnStack.push(std::move(code));
            }
        }
    }

    //At the end we should have one only RpnElement with the generated code
    if(rpnStack.size() == 1 && rpnStack.top().containsGeneratedCode() && !rpnStack.top().generatedCode.empty())
        return rpnStack.top().generatedCode;
    else
        printf("generateScene(). Something went wrong, check the Scene tree.\n");
    return "";
}

uint32 GlslGenerator::countNonEmptyOperands(TreeNode<Entity> &root, bool onlyStaticObjects) {
    uint32 count = 0;

    for(uint32 i = 0; i < root.getChildCount(); ++i) {
        TreeNode<Entity> &child = root.getChildByIndex(i);

        if(child->isCsgOperator()) {
            if(countNonEmptyOperands(child, onlyStaticObjects))
                count++;
        }
        else if(child->isObject()) {
            Object &obj = static_cast<Object&>(child.getPayload());
            if(type == GenerationType::Preview && (!onlyStaticObjects || obj.isStatic())) {
                count++;
            }
            else if(type == GenerationType::Probe && obj.isStatic()) {
                count++;
            }
        }
    }
    return count;
}

std::string GlslGenerator::generateOperator(Project &project, Scene &scene, const CsgOperator &csgOperator, const std::vector<RpnElement> &operands, uint32 startIdx, uint32 endIdx) {
    std::string templateString;
    templateString.reserve(512);

    switch(csgOperator.getType()) {
    case CsgType::Union:
        templateString = "union_(#OP1,#OP2)";
        break;
    case CsgType::Intersection:
        templateString = "intersection(#OP1,#OP2)";
        break;
    case CsgType::Subtraction:
        templateString = "subtraction(#OP1,#OP2)";
        break;
    default:
        GO_ASSERT_ALWAYS();
    }

    int size = endIdx - startIdx;
    if(size == 1) {
        return generateOperand(project, scene, operands[startIdx]);
    }
    else if(size == 2) {
        replace(templateString, "#OP1", generateOperand(project, scene, operands[startIdx]));
        replace(templateString, "#OP2", generateOperand(project, scene, operands[startIdx + 1]));
    }
    else {
        replace(templateString, "#OP1", generateOperator(project, scene, csgOperator, operands, startIdx, startIdx + 2));
        replace(templateString, "#OP2", generateOperator(project, scene, csgOperator, operands, startIdx + 2, endIdx));
    }

    return templateString;
}

std::string GlslGenerator::generateOperand(Project &project, Scene &scene, const RpnElement &rpnElement) {
    std::stringstream sstream;

    if(rpnElement.containsGeneratedCode())
        sstream << rpnElement.generatedCode;
    else {
        std::string p = "p";
        Entity &ent = *rpnElement.entity; //Guaranteed to be an entity

        if(ent.isObject()) {
            Object &obj = static_cast<Object&>(ent);
            if(obj.isAttachedToTransform()) {
                Entity *transform = scene.findSceneEntity(obj.getTransformId());
                if(transform)
                    p = generateTransformName(static_cast<Transform&>(*transform));
            }

            sstream << "vec2(";
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

            uint32 matIdx = 0;
            if(obj.isAttachedToMaterial()) {
                for(uint32 i = 0; i < project.getMaterialCount(); ++i) {
                    Material &mat = project.getMaterialByIndex(i);
                    if(obj.getMaterialId() == mat.getId()) {
                        matIdx = i;
                        break;
                    }
                }
            }
            sstream << "," << matIdx << ")";
        }
        else if(ent.isProbe()) {
            Probe &probe = static_cast<Probe&>(ent);
            sstream << "vec2(sphere(p-vec3(" << probe.getPosition().x << ", " << probe.getPosition().y << ", " << probe.getPosition().z << "), 0.1), 0)";
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