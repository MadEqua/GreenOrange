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

    GLuint getCopyTextureId(int face) const { return copyTextureIds[face]; }

private:
    uint32 size;

    FBOCube fboCube;
    ShaderProgram probeShader;
    GLuint dummyVao;
    std::string fallbackFragShaderCode;

    //Copy of the CubeMap from the FBOCube. Useful for ImGui display.
    GLuint copyTextureIds[6];

    FBO1D fbo1d;
    ShaderProgram spHarmonicsShader;
    std::string spHarmonicsShaderCode;
    float coeficients[9 * 3];
};