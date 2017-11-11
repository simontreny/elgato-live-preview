#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <memory>
#include <GLFW/glfw3.h>

class Shader {
public:
    static std::shared_ptr<Shader> fromString(GLenum shaderType, const std::string& source);
    static std::shared_ptr<Shader> fromFile(GLenum shaderType, const std::string& path);

    Shader(GLenum shaderType, const char* source);
    Shader(const Shader&) = delete;
    ~Shader();
    Shader& operator=(const Shader&) = delete;

    GLuint getHandle() const;

private:
    GLuint m_handle;
};

class ShaderProgram {
public:
    static std::shared_ptr<ShaderProgram> create(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);

    ShaderProgram(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    ShaderProgram(const ShaderProgram&) = delete;
    ~ShaderProgram();
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    GLuint getHandle() const;
    void use() const;

private:
    std::shared_ptr<Shader> m_vertexShader;
    std::shared_ptr<Shader> m_fragmentShader;
    GLuint m_handle;
};

#endif
