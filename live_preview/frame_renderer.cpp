#include "frame_renderer.h"
#include <cassert>

FrameRenderer::FrameRenderer(int frameWidth, int frameHeight, int colorspace)
    : m_frameWidth(frameWidth)
    , m_frameHeight(frameHeight)
    , m_colorspace(colorspace) {

    this->createQuad(frameWidth, frameHeight, colorspace);
    this->createTextures(frameWidth, frameHeight, colorspace);
}

FrameRenderer::~FrameRenderer() {
    glDeleteTextures(1, &m_uyvyTexture);
    glDeleteTextures(YUV_NUM_PLANES, m_yuvPlanarTextures);
}

void FrameRenderer::render(const Frame& frame) {
    assert(frame.width == m_frameWidth);
    assert(frame.height == m_frameHeight);
    assert(frame.colorspace == m_colorspace);

    this->updateTextures(frame);

    m_quad->bind();

    if (m_colorspace == YUV420P) {
        for (int i = 0; i < YUV_NUM_PLANES; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
        }
        glUniform1i(m_quad->getShaderProgram()->getUniformLocation("yTex"), 0);
        glUniform1i(m_quad->getShaderProgram()->getUniformLocation("uTex"), 1);
        glUniform1i(m_quad->getShaderProgram()->getUniformLocation("vTex"), 2);
    }

    if (m_colorspace == UYVY422) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_uyvyTexture);
        glUniform1i(m_quad->getShaderProgram()->getUniformLocation("uyvyTex"), 0);
    }

    m_quad->draw();
}

void FrameRenderer::createQuad(int frameWidth, int frameHeight, int colorspace) {
    switch (colorspace) {
        case YUV420P:
            m_quad = std::make_shared<ScreenAlignedQuad>(Shader::fromFile(GL_FRAGMENT_SHADER, "shaders/yuvPlanarToRgb.glsl"));
            break;
        case UYVY422:
            m_quad = std::make_shared<ScreenAlignedQuad>(Shader::fromFile(GL_FRAGMENT_SHADER, "shaders/uyvyToRgb.glsl"));
            break;
        default:
            assert(false);
    }
}

void FrameRenderer::createTextures(int frameWidth, int frameHeight, int colorspace) {
    if (colorspace == YUV420P) {
        glGenTextures(YUV_NUM_PLANES, m_yuvPlanarTextures);
        for (int planeIndex = 0; planeIndex < YUV_NUM_PLANES; planeIndex++) {
            int textureWidth, textureHeight;
            this->getPlanarTextureSize(planeIndex, frameWidth, frameHeight, &textureWidth, &textureHeight);
            glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[planeIndex]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }

    if (colorspace == UYVY422) {
        int textureWidth, textureHeight;
        this->getUyvyTextureSize(frameWidth, frameHeight, &textureWidth, &textureHeight);
        glGenTextures(1, &m_uyvyTexture);
        glBindTexture(GL_TEXTURE_2D, m_uyvyTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void FrameRenderer::updateTextures(const Frame& frame) {
    if (m_colorspace == YUV420P) {
        int planeOffset = 0;
        for (int i = 0; i < YUV_NUM_PLANES; i++) {
            int textureWidth, textureHeight;
            getPlanarTextureSize(i, frame.width, frame.height, &textureWidth, &textureHeight);
            glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, frame.pixels->data() + planeOffset);
            planeOffset += textureWidth * textureHeight;
        }
    }

    if (m_colorspace == UYVY422) {
        int textureWidth, textureHeight;
        getUyvyTextureSize(frame.width, frame.height, &textureWidth, &textureHeight);
        glBindTexture(GL_TEXTURE_2D, m_uyvyTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, frame.pixels->data());
    }
}

void FrameRenderer::getUyvyTextureSize(int frameWidth, int frameHeight, int *textureWidth, int *textureHeight) {
    *textureWidth = frameWidth / 2;
    *textureHeight = frameHeight;
}

void FrameRenderer::getPlanarTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight) {
    *textureWidth = (planeIndex == YUV_Y) ? frameWidth : frameWidth / 2;
    *textureHeight = (planeIndex == YUV_Y) ? frameHeight : frameHeight / 2;
}

