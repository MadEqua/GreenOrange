#pragma once

#include <string>
#include <vector>

class Project;
class CsgOperator;
class Object;
class Scene;
class SceneEntity;


class CodeGenerator
{
public:
    std::string generate(Project &project);

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

    std::string generateScene(Scene &scene);
    std::string generateOperator(const CsgOperator &csgOperator, const std::vector<StackElement> &operands, int startIdx, int endIdx);
    std::string generateOperand(const StackElement &stackElement);
};