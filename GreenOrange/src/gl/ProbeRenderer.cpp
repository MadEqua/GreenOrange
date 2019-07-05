#include "ProbeRenderer.h"

#include "../Constants.h"
#include "../glsl/generated/fallback.frag.h"


ProbeRenderer::ProbeRenderer(uint32 size) :
    shader(true),
    fbo(size),
    size(size) {

    fallbackFragShader.append(GLSL_VERSION).append("\n").append(fallback_frag);

    glGenVertexArrays(1, &dummyVao);

    glGenTextures(6, textureIds);
    for(int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8, size, size);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

ProbeRenderer::~ProbeRenderer() {
    glDeleteVertexArrays(1, &dummyVao);
}

void ProbeRenderer::render(const char *fs) {
    bool result = shader.setFragmentShader(fs);
    if(result) {
        shader.addUniform(UNIFORM_TIME);
    }
    else {
        shader.setFragmentShader(fallbackFragShader.c_str());
    }
    shader.addUniform(UNIFORM_DIMENSIONS);
    shader.setUniformVec2(UNIFORM_DIMENSIONS, size, size);

    fbo.bind();
    shader.bind();

    glBindVertexArray(dummyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Copy the CubeMap data to 2D textures.
    glFinish(); //This is painfully bad.
    for(int i = 0; i < 6; ++i) {
        glCopyImageSubData(fbo.getCubeTextureId(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, i,
                           textureIds[i], GL_TEXTURE_2D, 0, 0, 0, 0,
                           size, size, 1);

    }
    FBO::bindDefault();
}
