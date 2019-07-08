#include "ProbeRenderer.h"

#include "../model/Probe.h"
#include "../Constants.h"
#include "../glsl/generated/fallback.frag.h"
#include "../glsl/generated/computeIrradiance.frag.h"


ProbeRenderer::ProbeRenderer(uint32 size) :
    probeShader(true),
    fboCube(size),
    size(size),
    computeIrradianceShader(false),
    fbo1d(9) {

    fallbackFragShaderCode.append(GLSL_VERSION).append("\n").append(fallback_frag);
    computeIrradianceShaderCode.append(GLSL_VERSION).append("\n").append(computeIrradiance_frag);
    
    computeIrradianceShader.setFragmentShaderCode(computeIrradianceShaderCode.c_str());
    computeIrradianceShader.addUniform("radianceMapSize");

    glGenVertexArrays(1, &dummyVao);

    glGenTextures(6, copyTextureIds);
    for(int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, copyTextureIds[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, size, size); //Floating point to be a automatic conversion from the original FBO texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

ProbeRenderer::~ProbeRenderer() {
    glDeleteVertexArrays(1, &dummyVao);
}

void ProbeRenderer::render(const char *fs, Probe &probe) {
    bool result = probeShader.setFragmentShaderCode(fs);
    if(result) {
        probeShader.addUniform(UNIFORM_TIME);
    }
    else {
        probeShader.setFragmentShaderCode(fallbackFragShaderCode.c_str());
    }
    probeShader.addUniform(UNIFORM_DIMENSIONS);
    probeShader.setUniformVec2(UNIFORM_DIMENSIONS, size, size);

    fboCube.bind();
    probeShader.bind();

    glBindVertexArray(dummyVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Copy the CubeMap data to 2D textures for UI display.
    glFinish(); //This is painfully bad.
    for(int i = 0; i < 6; ++i) {
        glCopyImageSubData(fboCube.getCubeTextureId(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, i,
                           copyTextureIds[i], GL_TEXTURE_2D, 0, 0, 0, 0,
                           size, size, 1);
    }

    //Compute the irradiance map from the radiance map. This is done using spherical harmonics on a fragment shader.
    //The results will be 9 spherical harmonics coeficients stored on a 9*1 texture.
    fbo1d.bind();
    computeIrradianceShader.bind();
    computeIrradianceShader.setUniformInt("radianceMapSize", size);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glFinish();

    glGetTexImage(GL_TEXTURE_1D, 0, GL_RGB, GL_FLOAT, coeficients);
    glFinish();

    for(int i = 0; i < 9; ++i) {
        float *ptr = coeficients + (i * 3);
        probe.setIrradianceCoeff(i, *ptr, *(ptr + 1), *(ptr + 2));
    }

    FBO::bindDefault();
}
