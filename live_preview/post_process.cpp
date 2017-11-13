#include "post_process.h"


PostProcess::PostProcess(int width, int height, std::shared_ptr<Shader> fragmentShader) {
    m_renderToTexture = std::make_shared<RenderToTexture>(width, height);
    m_renderToTexture->addTargetTexture(GL_RGB);
    m_quad = std::make_shared<ScreenAlignedQuad>(fragmentShader, true);
}

void PostProcess::bind() const {
    m_renderToTexture->bind();
}

void PostProcess::draw() const {
    m_renderToTexture->unbind();

    m_quad->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderToTexture->getTargetTexture(0));
    glUniform1i(m_quad->getShaderProgram()->getUniformLocation("tex"), 0);
    m_quad->draw();
}