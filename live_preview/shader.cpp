#include "shader.h"
#include "utils.h"

Shader::Shader(GLenum shaderType, const char* source) {
    m_handle = glCreateShader(shaderType);
    glShaderSource(m_handle, 1, &source, NULL);
    glCompileShader(m_handle);

    GLint success;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[256];
        glGetShaderInfoLog(m_handle, sizeof(log), NULL, log);
        fprintf(stderr, "Unable to compile shader. %s\n", log);
    }
}

Shader::~Shader() {
    glDeleteShader(m_handle);
}

std::shared_ptr<Shader> Shader::fromString(GLenum shaderType, const std::string& source) {
    return std::make_shared<Shader>(shaderType, source.c_str());
}

std::shared_ptr<Shader> Shader::fromFile(GLenum shaderType, const std::string& path) {
    return Shader::fromString(shaderType, readFile(path).c_str());
}

GLuint Shader::getHandle() const {
    return m_handle;
}

std::shared_ptr<ShaderProgram> ShaderProgram::create(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader) {
    return std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
}

ShaderProgram::ShaderProgram(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader)
    : m_vertexShader(vertexShader)
    , m_fragmentShader(fragmentShader) {

    m_handle = glCreateProgram();
    glAttachShader(m_handle, m_vertexShader->getHandle());
    glAttachShader(m_handle, m_fragmentShader->getHandle());
    glLinkProgram(m_handle);

    GLint success;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);

    if (!success) {
        char log[256];
        glGetProgramInfoLog(m_handle, sizeof(log), NULL, log);
        fprintf(stderr, "Unable to link shader-program. %s\n", log);
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteShader(m_handle);
}

GLuint ShaderProgram::getHandle() const {
    return m_handle;
}

void ShaderProgram::use() const {
    glUseProgram(m_handle);
}
