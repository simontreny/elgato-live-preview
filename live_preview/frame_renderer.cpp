#include "frame_renderer.h"
#include <cassert>
#include "linmath.h"

#define YUV420P_COLORSPACE 0x0001    // YUV 4:2:0 planar as defined in x264.h

FrameRenderer::~FrameRenderer() {
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteTextures(YUV_PLANES, m_yuvPlanarTextures);
}

void FrameRenderer::render(const Frame& frame, float windowRatio) {
    if (!m_setupDone) {
        createShaders(frame.colorspace);
        createVertexBuffer();
        createTextures(frame);
        m_setupDone = true;
    }

    m_shaderProgram->use();
    updateVertexBuffer((float)frame.width / frame.height, windowRatio);
    updateTextures(frame);

    glEnableVertexAttribArray(m_posLocation);
    glVertexAttribPointer(m_posLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    for (int i = 0; i < YUV_PLANES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_yuvPlanarTextures[i]);
        glUniform1i(m_yuvPlanarTextureLocations[i], i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FrameRenderer::createShaders(int colorspace) {
    assert(colorspace == YUV420P_COLORSPACE);

    m_shaderProgram = ShaderProgram::create(
        Shader::fromFile(GL_VERTEX_SHADER, "shaders/screenAlignedQuad.glsl"),
        Shader::fromFile(GL_FRAGMENT_SHADER, "shaders/yuvPlanarToRgb.glsl"));

    m_posLocation = glGetAttribLocation(m_shaderProgram->getHandle(), "pos");
    m_yuvPlanarTextureLocations[YUV_Y_PLANE_INDEX] = glGetUniformLocation(m_shaderProgram->getHandle(), "yTex");
    m_yuvPlanarTextureLocations[YUV_U_PLANE_INDEX] = glGetUniformLocation(m_shaderProgram->getHandle(), "uTex");
    m_yuvPlanarTextureLocations[YUV_V_PLANE_INDEX] = glGetUniformLocation(m_shaderProgram->getHandle(), "vTex");
}

void FrameRenderer::createVertexBuffer() {
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
}

void FrameRenderer::updateVertexBuffer(float frameRatio, float windowRatio) {
    static struct { float x, y; } vertices[4] = {
        { -1.0f,  1.0f },
        { -1.0f, -1.0f },
        {  1.0f,  1.0f },
        {  1.0f, -1.0f }
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
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

