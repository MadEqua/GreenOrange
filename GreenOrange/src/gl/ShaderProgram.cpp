#include "ShaderProgram.h"

#include <stdio.h>
#include <string>

#include "../Constants.h"

#include "../glsl/generated/fullScreen.vert.h"
#include "../glsl/generated/layered.geo.h"

#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram(bool layeredRendering) {
    vsHandle = glCreateShader(GL_VERTEX_SHADER);
    fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
    if(layeredRendering) gsHandle = glCreateShader(GL_GEOMETRY_SHADER);

    //Full screen triangle.
    std::string vs;
    vs.append(GLSL_VERSION).append("\n").append(fullScreen_vert);
    const char * const vsc = vs.c_str();

    glShaderSource(vsHandle, 1, &vsc, nullptr);
    glCompileShader(vsHandle);
    if(!checkCompilation(vsHandle))
        return;

    if(layeredRendering) {
        //Geometry shader to do layered rendering for cubemap generation.
        std::string gs;
        gs.append(GLSL_VERSION).append("\n").append(layered_geo);
        const char * const gsc = gs.c_str();

        glShaderSource(gsHandle, 1, &gsc, nullptr);
        glCompileShader(gsHandle);
        if(!checkCompilation(gsHandle))
            return;
    }

    handle = glCreateProgram();
    glAttachShader(handle, vsHandle);
    glAttachShader(handle, fsHandle);
    if(layeredRendering) glAttachShader(handle, gsHandle);
}

ShaderProgram::~ShaderProgram() {
    glDeleteShader(vsHandle);
    glDeleteShader(fsHandle);
    if(gsHandle != -1) glDeleteShader(gsHandle);
    glDeleteProgram(handle);
}

bool ShaderProgram::checkCompilation(GLuint id) const {
    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if(!result) {
        GLint logLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

        auto *log = new GLchar[logLength];
        glGetShaderInfoLog(id, logLength, nullptr, log);

        printf("Error compiling shader:\n%s\n", log);

        delete[] log;
        return false;
    }

    printf("Shader compiled successfully.\n");
    return true;
}

bool ShaderProgram::checkLinking(GLuint id) const {
    GLint result;
    glGetProgramiv(id, GL_LINK_STATUS, &result);

    if(!result) {
        GLint logLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

        auto *log = new GLchar[logLength];
        glGetProgramInfoLog(id, logLength, nullptr, log);

        printf("Error linking shader:\n%s\n", log);

        delete[] log;
        return false;
    }

    printf("Shader linked successfully.\n");
    return true;
}

void ShaderProgram::bind() const {
    glUseProgram(handle);
}

bool ShaderProgram::setFragmentShader(const char *fs) {
    glShaderSource(fsHandle, 1, &fs, nullptr);
    glCompileShader(fsHandle);
    if(!checkCompilation(fsHandle))
        return false;

    glLinkProgram(handle);
    if(!checkLinking(handle))
        return false;

    return true;
}

bool ShaderProgram::addUniform(const char *name) {
    GLint id = glGetUniformLocation(handle, name);
    if(id >= 0) {
        uniformsByName[name] = id;
        return true;
    }
    return false;
}

bool ShaderProgram::setUniformFloat(const char *name, float v) const {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniform1f(uniformsByName.at(name), v);
        return true;
    }
    return false;
}

bool ShaderProgram::setUniformVec2(const char *name, float x, float y) const {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniform2f(uniformsByName.at(name), x, y);
        return true;
    }
    return false;
}

bool ShaderProgram::setUniformVec3(const char *name, float x, float y, float z) const {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniform3f(uniformsByName.at(name), x, y, z);
        return true;
    }
    return false;
}

bool ShaderProgram::setUniformMat3(const char *name, const glm::mat3 &mat) const {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniformMatrix3fv(uniformsByName.at(name), 1, GL_FALSE, glm::value_ptr(mat));
        return true;
    }
    return false;
}