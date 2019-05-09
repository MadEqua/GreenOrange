#pragma once

#include <glad/glad.h>

#include "../Types.h"

class FBO
{
public:
    FBO(uint32 width, uint32 height);
    ~FBO();

    void bind() const;
    static void bindDefault();

    void setDimensions(uint32 width, uint32 height);
    GLuint getColorTextureId() const { return colorTexId; }

private:
    GLuint id;
    GLuint colorTexId;
    uint32 width, height;
};

