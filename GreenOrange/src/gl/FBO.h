#pragma once

#include <glad/glad.h>

#include "../Types.h"

/*
 * FBO that renders to a 2D texture. Supports frequent redimensioning.
 * To use, first call setDimensions() with the correct dimensions.
 */
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


//FBO that renders to a floating point CubeTexture.
class FBOCube
{
public:
    FBOCube(uint32 size);
    ~FBOCube();

    void bind() const;

    GLuint getCubeTextureId() const { return cubeTexId; }

private:
    GLuint id;
    GLuint cubeTexId;
    uint32 size;
};


//FBO that renders to a floating point 1D texture.
class FBO1D
{
public:
    FBO1D(uint32 length);
    ~FBO1D();

    void bind() const;

    GLuint getTextureId() const { return texId; }

private:
    GLuint id;
    GLuint texId;
    uint32 length;
};

