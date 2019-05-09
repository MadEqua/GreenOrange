#pragma once

#include <glad/glad.h>

#include "../Types.h"

//FBO that contains only a color texture.
//To use first call setDimensions() with the correct dimensions.
class FBO
{
public:
    FBO();
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

