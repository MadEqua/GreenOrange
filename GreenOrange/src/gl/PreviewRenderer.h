#pragma once

#include <glad/glad.h>

#include "FBO.h"
#include "ShaderProgram.h"


class PreviewRenderer
{
public:
    PreviewRenderer(const char *fs);
    ~PreviewRenderer();

    void render() const;
    GLuint getRenderedImageId() const { return fbo.getColorTextureId(); }

private:
    FBO fbo;
    ShaderProgram shader;

    GLuint dummyVao;
};

