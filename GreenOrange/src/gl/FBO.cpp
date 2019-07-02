#include "FBO.h"


FBO::FBO() {
    glGenFramebuffers(1, &id);
    glGenTextures(1, &colorTexId);

    glBindTexture(GL_TEXTURE_2D, colorTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

FBO::~FBO() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTexId);
}

void FBO::bind() const {
    glBindTexture(GL_TEXTURE_2D, 0); //Avoid rendering to a bound texture
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, width, height);
}

void FBO::bindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

void FBO::setDimensions(uint32 width, uint32 height) { 
    this->width = width; 
    this->height = height;
    glBindTexture(GL_TEXTURE_2D, colorTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexId, 0);
    bindDefault();
}


FBOCube::FBOCube(uint32 width, uint32 height) :
    width(width), height(height) {
    glGenFramebuffers(1, &id);
    glGenTextures(1, &colorTexId);

    glBindTexture(GL_TEXTURE_CUBE_MAP, colorTexId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for(int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexId, 0);
    bindDefault();
}

FBOCube::~FBOCube() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTexId);
}

void FBOCube::bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Avoid rendering to a bound texture
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, width, height);
}

void FBOCube::bindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}