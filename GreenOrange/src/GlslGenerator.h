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

    void generate(Project &project);

    const std::string& getGlslCode() const { return glslCode; }

private:
    GlslGenerator();

    //Element on the code generation RPN list. It will be an Entity or some already generated code.
    class RpnElement {
    public:
        RpnElement(TreeNode<Entity> &treeNode);
        RpnElement(Entity &entity);
        RpnElement(std::string &&generatedCode);

        TreeNode<Entity> *csgTreeNode;
        Entity *entity;
        std::string generatedCode;

        bool containsGeneratedCode() const { return type == Type::Code; }
        bool containsCsgTreeNode() const { return type == Type::CsgTreeNode; }
        bool containsEntity() const { return type == Type::Entity; }
    private:
        enum class Type {
            CsgTreeNode, Entity, Code
        };

        Type type;
    };

    bool needToGenerate = true;
    uint64 currentCodeId = 0;
    std::string glslCode;

    void initGeneration();

    void generateLights(Project &project);
    void generateScenes(Project &project);
    void generateMaterials(Project &project);

    static std::string generateScene(Project &project, Scene &scene);
    static std::string generateSceneTree(Project &project, Scene &scene);
    static std::string generateOperator(Project &project, Scene &scene, const CsgOperator &csgOperator, const std::vector<RpnElement> &operands, uint32 startIdx, uint32 endIdx);
    static std::string generateOperand(Project &project, Scene &scene, const RpnElement &rpnElement);
    static std::string generateTransform(TreeNode<Transform> &transformNode, TreeNode<Transform> *parentTransformNode);

    static std::string generateTransformName(const Transform &transform);
    static uint32 countNonEmptyOperands(TreeNode<Entity> &node, bool onlyStaticObjects);

    static bool replace(std::string& str, const std::string& toReplace, const std::string& replacement);
};