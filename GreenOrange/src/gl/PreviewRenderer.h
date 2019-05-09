#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class PreviewRenderer
{
public:
    PreviewRenderer();
    ~PreviewRenderer();

    void setDimensions(uint32 width, uint32 height) { fbo.setDimensions(width, height); }
    bool setFragmentShader(const char *fs) { return shader.setFragmentShader(fs); };
    void render() const;
    
    GLuint getRenderedImageId() const { return fbo.getColorTextureId(); }

private:
    FBO fbo;
    ShaderProgram shader;

    GLuint dummyVao;
};

