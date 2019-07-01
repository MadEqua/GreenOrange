#pragma once

#include <string>
#include <vector>

#include "Types.h"
#include "Tree.h"

class Project;
class CsgOperator;
class Object;
class Scene;
class Entity;
class Transform;


#define GEN_SET_DIRTY() GlslGenerator::getInstance().setNeedToGenerate();

class GlslGenerator
{
public:
    static GlslGenerator& getInstance() {
        static GlslGenerator instance;
        return instance;
    }

    GlslGenerator(GlslGenerator const&) = delete;
    void operator=(GlslGenerator const&) = delete;

    void setNeedToGenerate() { needToGenerate = true; }

    //Each time we generate new code, it will have a new id. Useful for clients to identify changes.
    uint64 getCurrentCodeId() const { return currentCodeId; }

    //Call once per frame for the PreviewPanel
    void generateForPreview(Project &project);
   
    void generate(Project &project, bool onlyStaticObjects = false);

    const std::string& getGlslCode() const { return glslCode; }

private:
    GlslGenerator();

    //Element on the code generation RPN list. It will be either a SceneElement (CsgOperator or Object)
    //or some already generated code to be composed with other operators/operations.
    class RpnElement {
    public:
        RpnElement(TreeNode<Entity> &sceneEntityNode);
        RpnElement(std::string &&generatedCode);

        TreeNode<Entity> *sceneEntityNode;
        std::string generatedCode;

        bool isGeneratedCode;
    };

    bool needToGenerate = true;
    uint64 currentCodeId = 0;
    std::string glslCode;

    void initGeneration();

    void generateLights(Project &project, bool onlyStaticLights);
    void generateScenes(Project &project, bool onlyStaticObjects);
    void generateMaterials(Project &project);

    static std::string generateScene(Project &project, Scene &scene, bool onlyStaticObjects);
    static std::string generateSceneTree(Project &project, Scene &scene, bool onlyStaticObjects);
    static std::string generateOperator(Project &project, Scene &scene, const CsgOperator &csgOperator, const std::vector<RpnElement> &operands, uint32 startIdx, uint32 endIdx);
    static std::string generateOperand(Project &project, Scene &scene, const RpnElement &rpnElement);
    static std::string generateTransform(TreeNode<Transform> &transformNode, TreeNode<Transform> *parentTransformNode);

    static std::string generateTransformName(const Transform &transform);
    static uint32 countNonEmptyOperands(TreeNode<Entity> &node, bool onlyStaticObjects);
    
    static bool replace(std::string& str, const std::string& toReplace, const std::string& replacement);
};