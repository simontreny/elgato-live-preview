#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <memory>
#include "gl_includes.h"
#include "non_copyable.h"

class Shader : public NonCopyable {
public:
    static std::shared_ptr<Shader> fromSource(GLenum shaderType, const std::string& source);
    static std::shared_ptr<Shader> fromFile(GLenum shaderType, const std::string& path);

    Shader(GLenum shaderType, const char* source);
    ~Shader();

    GLuint getHandle() const;

private:
    GLuint m_handle;
};

class ShaderProgram : public NonCopyable {
public:
    ShaderProgram(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    ~ShaderProgram();

    GLuint getHandle() const;
    void use() const;

private:
    std::shared_ptr<Shader> m_vertexShader;
    std::shared_ptr<Shader> m_fragmentShader;
    GLuint m_handle;
};

#endif
