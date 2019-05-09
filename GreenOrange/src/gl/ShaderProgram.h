#pragma once

#include <glad/glad.h>


//This creates a program with a fixed full screen triangle vertex shader.
//Call setFragmentShader() with fragment shader code to be able to use.
class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void bind() const;
    bool setFragmentShader(const char *fs);

private:
    GLuint id;
    GLuint vsId;
    GLuint fsId;

    bool checkCompilation(GLuint id) const;
    bool checkLinking(GLuint id) const;
};

