#ifndef _SCREEN_ALIGNED_QUAD_H_
#define _SCREEN_ALIGNED_QUAD_H_

#include <memory>
#include "shader.h"

class ScreenAlignedQuad {
public:
    ScreenAlignedQuad(std::shared_ptr<Shader> fragmentShader);
    ~ScreenAlignedQuad();

    std::shared_ptr<ShaderProgram> getShaderProgram() const;
    void render() const;

private:
    std::shared_ptr<ShaderProgram> m_program;
    GLuint m_vertexArray;
    GLuint m_vertexBuffer;

    void createVertexArray();
    void updateVertexArray() const;
};

#endif
