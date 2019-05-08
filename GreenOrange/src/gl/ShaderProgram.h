#pragma once

#include <glad/glad.h>


class ShaderProgram
{
public:
    ShaderProgram(const char *fs);
    ~ShaderProgram();

    void bind() const;
    void updateFragmentShader(const char *fs);

private:
    GLuint id;
    GLuint vsId;
    GLuint fsId;

    bool checkCompilation(GLuint id) const;
    bool checkLinking(GLuint id) const;
};

