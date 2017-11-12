#include "frame_renderer.h"
#include <cassert>

#define YUV420P_COLORSPACE 0x0001    // YUV 4:2:0 planar as defined in x264.h

FrameRenderer::FrameRenderer(/*int colorspace*/) {
    m_quad = std::make_shared<ScreenAlignedQuad>(Shader::fromFile(GL_FRAGMENT_SHADER, "shaders/yuvPlanarToRgb.glsl"));
}

FrameRenderer::~FrameRenderer() {
    glDeleteTextures(YUV_PLANES, m_yuvPlanarTextures);
}

void FrameRenderer::render(const Frame& frame, float windowRatio) {
    if (!m_setupDone) {
        createTextures(frame);
        m_setupDone = true;
    }

    updateTextures(frame);

    for (int i = 0; i < YUV_PLANES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
        // glUniform1i(m_yuvPlanarTextureLocations[i], i);
    }

    m_quad->bind();
    glUniform1i(m_quad->getShaderProgram()->getUniformLocation("yTex"), 0);
    glUniform1i(m_quad->getShaderProgram()->getUniformLocation("uTex"), 1);
    glUniform1i(m_quad->getShaderProgram()->getUniformLocation("vTex"), 2);
    m_quad->draw();
}

void FrameRenderer::createTextures(const Frame& frame) {
    assert(frame.colorspace == YUV420P_COLORSPACE);

    glGenTextures(YUV_PLANES, m_yuvPlanarTextures);
    for (int i = 0; i < YUV_PLANES; i++) {
        int textureWidth, textureHeight;
        getPlanarTextureSize(i, frame.width, frame.height, &textureWidth, &textureHeight);
        glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void FrameRenderer::updateTextures(const Frame& frame) {
    assert(frame.colorspace == YUV420P_COLORSPACE);

    int planeOffset = 0;
    for (int i = 0; i < YUV_PLANES; i++) {
        int textureWidth, textureHeight;
        getPlanarTextureSize(i, frame.width, frame.height, &textureWidth, &textureHeight);
        glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, frame.pixels->data() + planeOffset);
        planeOffset += textureWidth * textureHeight;
    }
}

void FrameRenderer::getPlanarTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight) {
    *textureWidth = (planeIndex == YUV_Y_PLANE_INDEX) ? frameWidth : frameWidth / 2;
    *textureHeight = (planeIndex == YUV_Y_PLANE_INDEX) ? frameHeight : frameHeight / 2;
}

