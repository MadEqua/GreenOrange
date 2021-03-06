#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class PreviewRenderer
{
public:
    PreviewRenderer();
    ~PreviewRenderer();

    void setDimensions(float width, float height);
    bool setFragmentShaderCode(const char *fs);
    void render() const;
    
    GLuint getRenderedImageId() const { return fbo.getColorTextureId(); }

private:
    FBO fbo;
    ShaderProgram shader;

    GLuint dummyVao;

    std::string fallbackFragShader;
};

