#include "ShaderProgram.h"

#include <stdio.h>
#include <string>

#include "../Constants.h"

#include "../glsl/generated/fullScreen.vert.h"


ShaderProgram::ShaderProgram() {
    vsHandle = glCreateShader(GL_VERTEX_SHADER);
    fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    //Full screen triangle
    std::string vs;
    vs.append(GLSL_VERSION).append("\n").append(fullScreen_vert);
    const char * const vsc = vs.c_str();

    glShaderSource(vsHandle, 1, &vsc, nullptr);
    glCompileShader(vsHandle);
    if(!checkCompilation(vsHandle))
        return;

    handle = glCreateProgram();
    glAttachShader(handle, vsHandle);
    glAttachShader(handle, fsHandle);
}

ShaderProgram::~ShaderProgram() {
    glDeleteShader(vsHandle);
    glDeleteShader(fsHandle);
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
