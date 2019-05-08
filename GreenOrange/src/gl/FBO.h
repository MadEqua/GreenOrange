#pragma once

#include <glad/glad.h>

//TODO: define and change texture dimensions
class FBO
{
public:
    FBO();
    ~FBO();

    void bind() const;
    static void bindDefault();
    GLuint getColorTextureId() const { return colorTexId; }

private:
    GLuint id;
    GLuint colorTexId;
};

