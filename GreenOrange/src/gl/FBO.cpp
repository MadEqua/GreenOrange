#include "FBO.h"


FBO::FBO(uint32 width, uint32 height) {
    glGenFramebuffers(1, &id);

    glGenTextures(1, &colorTexId);
    setDimensions(width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexId, 0);
    bindDefault();

}

FBO::~FBO() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &colorTexId);
}

void FBO::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void FBO::bindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::setDimensions(uint32 width, uint32 height) { 
    this->width = width; 
    this->height = height;
    glBindTexture(GL_TEXTURE_2D, colorTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}