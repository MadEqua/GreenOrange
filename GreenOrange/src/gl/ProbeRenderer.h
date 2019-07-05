#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class ProbeRenderer
{
public:
    ProbeRenderer(uint32 size);
    ~ProbeRenderer();

    void render(const char *fs);

    GLuint getCopyTextureId(int face) const { return textureIds[face]; }

private:
    FBOCube fbo;
    ShaderProgram shader;

    uint32 size;

    GLuint dummyVao;
    std::string fallbackFragShader;

    //Copy of the CubeMap from the FBO. Useful for ImGui display.
    GLuint textureIds[6];
};