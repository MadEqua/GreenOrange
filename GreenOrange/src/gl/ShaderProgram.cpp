#include "ShaderProgram.h"

#include <stdio.h>
#include <string>

#include "../Constants.h"


ShaderProgram::ShaderProgram() {
    vsHandle = glCreateShader(GL_VERTEX_SHADER);
    fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    //Full screen triangle
    const char * const DEFAULT_VS =
        "void main() {"
            "vec2 c = vec2(gl_VertexID << 1 & 2, gl_VertexID & 2);"
            "gl_Position = vec4(c * vec2(2, -2) + vec2(-1, 1), 0.0f, 1.0f);"
        "}";

    std::string vs(GLSL_VERSION);
    vs.append("\n").append(DEFAULT_VS);
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

bool ShaderProgram::setUniformFloat(const char *name, float v) {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniform1f(uniformsByName[name], v);
        return true;
    }
    return false;
}

bool ShaderProgram::setUniformVec2(const char *name, float x, float y) {
    if(uniformsByName.find(name) != uniformsByName.end()) {
        bind();
        glUniform2f(uniformsByName[name], x, y);
        return true;
    }
    return false;
}
