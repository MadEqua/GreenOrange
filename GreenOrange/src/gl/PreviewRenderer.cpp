#include "PreviewRenderer.h"


PreviewRenderer::PreviewRenderer(uint32 width, uint32 height, const char *fs) :
    fbo(width, height),
    shader(fs) {
    glGenVertexArrays(1, &dummyVao);
}

PreviewRenderer::~PreviewRenderer() {
    glDeleteVertexArrays(1, &dummyVao);
}
 
void PreviewRenderer::render() const {
    fbo.bind();
    shader.bind();

    glBindVertexArray(dummyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    fbo.bindDefault();
}
