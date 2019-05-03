#pragma once

class Scene;
class Operator;

class ScenePanel
{
public:
    void drawGui(Scene &scene);

private:
    void doOperatorNode(Operator *op) const;
};

