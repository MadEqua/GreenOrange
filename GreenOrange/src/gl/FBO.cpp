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
    glBindTexture(GL_TEXTURE_2D, 0);
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
    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexId, 0);
    bindDefault();
}


FBOCube::FBOCube(uint32 size) :
    size(size) {
    glGenFramebuffers(1, &id);
    
    glGenTextures(1, &cubeTexId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB32F, size, size);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubeTexId, 0);
    FBO::bindDefault();
}

FBOCube::~FBOCube() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &cubeTexId);
}

void FBOCube::bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glViewport(0, 0, size, size);
}