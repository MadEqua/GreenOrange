#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class PreviewRenderer
{
public:
    PreviewRenderer(uint32 width, uint32 height, const char *fs);
    ~PreviewRenderer();

    void render() const;
    void setDimensions(uint32 width, uint32 height) { fbo.setDimensions(width, height); }
    GLuint getRenderedImageId() const { return fbo.getColorTextureId(); }

private:
    FBO fbo;
    ShaderProgram shader;

    GLuint dummyVao;
};

