#include "ShaderProgram.h"

#include <stdio.h>
#include <string>

#include "../Constants.h"


ShaderProgram::ShaderProgram(const char *fs) {
    vsId = glCreateShader(GL_VERTEX_SHADER);
    fsId = glCreateShader(GL_FRAGMENT_SHADER);

    //Full screen triangle
    const char *DEFAULT_VS =
        "void main() {"\
            "vec2 c = vec2(gl_VertexID << 1 & 2, gl_VertexID & 2);"
            "gl_Position = vec4(c * vec2(2, -2) + vec2(-1, 1), 0.0f, 1.0f);"\
        "}";

    std::string vs(GLSL_VERSION);
    vs.append("\n").append(DEFAULT_VS);
    const char *vsc = vs.c_str();

    glShaderSource(vsId, 1, &vsc, nullptr);
    glCompileShader(vsId);
    if(!checkCompilation(vsId))
        return;

    glShaderSource(fsId, 1, &fs, nullptr);
    glCompileShader(fsId);
    if(!checkCompilation(fsId))
        return;

    id = glCreateProgram();
    glAttachShader(id, vsId);
    glAttachShader(id, fsId);
    glLinkProgram(id);
    if(!checkLinking(id))
        return;
}

ShaderProgram::~ShaderProgram() {
    glDeleteShader(vsId);
    glDeleteShader(fsId);
    glDeleteProgram(id);
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
    glUseProgram(id);
}

void ShaderProgram::updateFragmentShader(const char *fs) {
    glShaderSource(fsId, 1, &fs, nullptr);
    glCompileShader(fsId);
    glLinkProgram(id);
}