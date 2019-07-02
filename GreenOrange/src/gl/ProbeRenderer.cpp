#include "ProbeRenderer.h"

#include "../Constants.h"


ProbeRenderer::ProbeRenderer(const char *fs, uint32 width, uint32 height) :
    shader(true),
    fbo(width, height),
    width(width), height(height) {

    glGenVertexArrays(1, &dummyVao);
    shader.setFragmentShader(fs);
    shader.addUniform(UNIFORM_DIMENSIONS);
    shader.setUniformVec2(UNIFORM_DIMENSIONS, width, height);
}

ProbeRenderer::~ProbeRenderer() {
    glDeleteVertexArrays(1, &dummyVao);
}

void ProbeRenderer::render() const {
    fbo.bind();
    shader.bind();

    glBindVertexArray(dummyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    fbo.bindDefault();
}
