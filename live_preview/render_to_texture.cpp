#include "render_to_texture.h"
#include <cassert>

const RenderToTexture* RenderToTexture::s_boundInstance = NULL;
GLint RenderToTexture::s_initialViewport[4];

RenderToTexture::RenderToTexture(int width, int height)
    : m_width(width)
    , m_height(height) {

    glGenFramebuffers(1, &m_framebuffer);
}

RenderToTexture::~RenderToTexture() {
    glDeleteTextures(m_targetTextures.size(), &m_targetTextures.front());
    glDeleteFramebuffers(1, &m_framebuffer);
}

GLuint RenderToTexture::addTargetTexture(GLenum format) {
    return addTargetTexture(m_width, m_height, format);
}

GLuint RenderToTexture::addTargetTexture(int width, int height, GLenum format) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureIndex = m_targetTextures.size();
    m_targetTextures.push_back(texture);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textureIndex, texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return texture;
}

GLuint RenderToTexture::getTargetTexture(int index) const {
    return m_targetTextures[index];
}

void RenderToTexture::bind() const {
    if (s_boundInstance == NULL) {
        glGetIntegerv(GL_VIEWPORT, s_initialViewport);
    }
    s_boundInstance = this;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
}

void RenderToTexture::unbind() const {
    assert(s_boundInstance == this);
    s_boundInstance = NULL;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(s_initialViewport[0], s_initialViewport[1], s_initialViewport[2], s_initialViewport[3]);
}

