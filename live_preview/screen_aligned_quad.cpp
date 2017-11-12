#include "screen_aligned_quad.h"

ScreenAlignedQuad::ScreenAlignedQuad(std::shared_ptr<Shader> fragmentShader) {
    m_program = std::make_shared<ShaderProgram>(
        Shader::fromFile(GL_VERTEX_SHADER, "shaders/screenAlignedQuad.glsl"),
        fragmentShader
    );

    createVertexArray();
}

ScreenAlignedQuad::~ScreenAlignedQuad() {
    glDeleteVertexArrays(1, &m_vertexArray);
    glDeleteBuffers(1, &m_vertexBuffer);
}

std::shared_ptr<ShaderProgram> ScreenAlignedQuad::getShaderProgram() const {
    return m_program;
}

void ScreenAlignedQuad::render() const {
    m_program->use();

    updateVertexArray();

    glBindVertexArray(m_vertexArray);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScreenAlignedQuad::createVertexArray() {
    glGenVertexArrays(1, &m_vertexArray);
    glBindVertexArray(m_vertexArray);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void ScreenAlignedQuad::updateVertexArray() const {
    static struct { float x, y; } vertices[4] = {
        { -1.0f,  1.0f },
        { -1.0f, -1.0f },
        {  1.0f,  1.0f },
        {  1.0f, -1.0f }
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}
