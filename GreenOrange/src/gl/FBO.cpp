#include "FBO.h"


FBO::FBO() {
    glGenFramebuffers(1, &id);

    glGenTextures(1, &colorTexId);
    glBindTexture(GL_TEXTURE_2D, colorTexId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); //TODO size

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexId, 0);
    bindDefault();

}

FBO::~FBO() {
    glDeleteFramebuffers(1, &id);
}

void FBO::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, 512, 512); //TODO size
}

void FBO::bindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}