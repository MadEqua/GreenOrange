#pragma once

#include <string>
#include <vector>

#include "Types.h"

class Project;
class CsgOperator;
class Object;
class Scene;
class SceneEntity;


class GlslGenerator
{
public:
    const std::string& generate(Project &project) const;
    const std::string& getLastGenerated() const { return glslCode; }

private:
    //Element on the code generation Stack. It will be either a SceneElement (CsgOperator or Object)
    //or some already generated code pushed onto the stack to be composed with other operators/operations.
    class StackElement {
    public:
        StackElement(SceneEntity &sceneElement);
        StackElement(std::string &&generatedCode);

        SceneEntity *sceneEntity;
        std::string generatedCode;

        bool isGeneratedCode;
    };

    mutable std::string glslCode;

    void initGeneration() const;

    static bool replace(std::string& str, const std::string& toReplace, const std::string& replacement);

    static std::string generateScene(Scene &scene);
    static std::string generateOperator(const CsgOperator &csgOperator, const std::vector<StackElement> &operands, uint32 startIdx, uint32 endIdx);
    static std::string generateOperand(const StackElement &stackElement);
};