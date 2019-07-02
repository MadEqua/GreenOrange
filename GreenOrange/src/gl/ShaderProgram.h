#pragma once

#include <glad/glad.h>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>


/*
 * This creates a program with a fixed full screen triangle vertex shader.
 * Optional Geometry shading for layered rendering (cube map generation).
 * Call setFragmentShader() with fragment shader code to be able to use.
 */
class ShaderProgram
{
public:
    ShaderProgram(bool layeredRendering = false);
    ~ShaderProgram();

    void bind() const;
    bool setFragmentShader(const char *fs);
    
    bool addUniform(const char *name);
    bool setUniformFloat(const char *name, float v) const;
    bool setUniformVec2(const char *name, float x, float y) const;
    bool setUniformVec3(const char *name, float x, float y, float z) const;
    bool setUniformMat3(const char *name, const glm::mat3 &mat) const;

private:
    GLuint handle;
    GLuint vsHandle;
    GLuint fsHandle;
    GLuint gsHandle = -1;

    std::unordered_map<std::string, GLuint> uniformsByName;

    bool checkCompilation(GLuint id) const;
    bool checkLinking(GLuint id) const;
};

