#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class ProbeRenderer
{
public:
    ProbeRenderer(const char *fs, uint32 width, uint32 height);
    ~ProbeRenderer();

    void render() const;

private:
    FBOCube fbo;
    ShaderProgram shader;

    uint32 width, height;

    GLuint dummyVao;
};

