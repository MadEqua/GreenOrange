#include "PreviewRenderer.h"

#include "../Constants.h"


PreviewRenderer::PreviewRenderer() {
    glGenVertexArrays(1, &dummyVao);

    const char* const FS_FALLBACK =
        "out vec4 fragColor;"
        "uniform vec2 dimensions;"
        "void main() {"
        "fragColor = vec4(gl_FragCoord.xy / dimensions, 0.0f, 1.0f);"
        "}";

    fallbackFragShader = GLSL_VERSION;
    fallbackFragShader.append("\n").append(FS_FALLBACK);
}

PreviewRenderer::~PreviewRenderer() {
    glDeleteVertexArrays(1, &dummyVao);
}

bool PreviewRenderer::setFragmentShader(const char *fs) {
    bool result = shader.setFragmentShader(fs);
    if(result) {
        shader.addUniform(UNIFORM_TIME);
    }
    else {
        shader.setFragmentShader(fallbackFragShader.c_str());
    }

    shader.addUniform(UNIFORM_DIMENSIONS);
    return result;
}

void PreviewRenderer::setDimensions(float width, float height) {
    shader.setUniformVec2(UNIFORM_DIMENSIONS, width,  height);
    fbo.setDimensions(static_cast<uint32>(width), static_cast<uint32>(height));
}
 
void PreviewRenderer::render() const {
    fbo.bind();
    shader.bind();

    glBindVertexArray(dummyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    fbo.bindDefault();
}
