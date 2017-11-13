#ifndef _SCREEN_ALIGNED_QUAD_H_
#define _SCREEN_ALIGNED_QUAD_H_

#include <memory>
#include "shader.h"

class ScreenAlignedQuad {
public:
    ScreenAlignedQuad(std::shared_ptr<Shader> fragmentShader, bool invertY = false);
    ~ScreenAlignedQuad();

    std::shared_ptr<ShaderProgram> getShaderProgram() const;
    void bind() const;
    void draw() const;

private:
    std::shared_ptr<ShaderProgram> m_program;
    GLuint m_vertexArray;
    GLuint m_vertexBuffer;
    bool m_invertY;

    void createVertexArray();
    void updateVertexArray() const;
};

#endif
