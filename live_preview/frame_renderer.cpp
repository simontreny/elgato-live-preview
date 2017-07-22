#include "frame_renderer.h"
#include "frame_renderer_shader.h"
#include <cassert>
#include "linmath.h"

#define YUV420P_COLORSPACE 0x0001    // YUV 4:2:0 planar as defined in x264.h

FrameRenderer::~FrameRenderer() {
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteTextures(YUV_PLANES, m_yuvTextures);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteShader(m_shaderProgram);
}

void FrameRenderer::render(const Frame& frame, float windowRatio) {
    if (!m_setupDone) {
        createVertexBuffer();
        createTextures(frame);
        createShaders(frame.colorspace);
        m_setupDone = true;
    }

    glUseProgram(m_shaderProgram);

    glEnableVertexAttribArray(m_posLocation);
    glVertexAttribPointer(m_posLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(m_uvLocation);
    glVertexAttribPointer(m_uvLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    mat4x4 mvp;
    float frameRatio = (float)frame.width / frame.height;
    if (windowRatio > frameRatio) {
        mat4x4_ortho(mvp, -windowRatio / frameRatio, windowRatio / frameRatio, -1.0f, 1.0f, 1.0f, -1.0f);
    } else {
        mat4x4_ortho(mvp, -1.0f, 1.0f, -frameRatio / windowRatio, frameRatio / windowRatio, 1.0f, -1.0f);
    }
    glUniformMatrix4fv(m_mvpLocation, 1, GL_FALSE, (const GLfloat*)mvp);

    updateTextures(frame);
    for (int i = 0; i < YUV_PLANES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_yuvTextures[i]);
        glUniform1i(m_yuvTextureLocations[i], i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FrameRenderer::createVertexBuffer() {
    static struct { float x, y, u, v; } vertices[4] = {
        { -1.0f,  1.0f, 0.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f, 1.0f },
        {  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f, 1.0f, 1.0f }
    };

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void FrameRenderer::createShaders(int colorspace) {
    assert(colorspace == YUV420P_COLORSPACE);

    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &YUV420P_VERTEX_SHADER, NULL);
    glCompileShader(m_vertexShader);

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &YUV420P_FRAGMENT_SHADER, NULL);
    glCompileShader(m_fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);

    m_mvpLocation = glGetUniformLocation(m_shaderProgram, "mvp");
    m_posLocation = glGetAttribLocation(m_shaderProgram, "pos");
    m_uvLocation = glGetAttribLocation(m_shaderProgram, "uv");
    m_yuvTextureLocations[0] = glGetUniformLocation(m_shaderProgram, "yTexture");
    m_yuvTextureLocations[1] = glGetUniformLocation(m_shaderProgram, "uTexture");
    m_yuvTextureLocations[2] = glGetUniformLocation(m_shaderProgram, "vTexture");
}

void FrameRenderer::createTextures(const Frame& frame) {
    assert(frame.colorspace == YUV420P_COLORSPACE);
    
    glGenTextures(YUV_PLANES, m_yuvTextures);
    for (int i = 0; i < YUV_PLANES; i++) {
        int textureWidth, textureHeight;
        getTextureSize(i, frame.width, frame.height, &textureWidth, &textureHeight);
        glBindTexture(GL_TEXTURE_2D, m_yuvTextures[i]);
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
        getTextureSize(i, frame.width, frame.height, &textureWidth, &textureHeight);
        glBindTexture(GL_TEXTURE_2D, m_yuvTextures[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, frame.pixels->data() + planeOffset);
        planeOffset += textureWidth * textureHeight;
    }
}

void FrameRenderer::getTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight) {
    *textureWidth = (planeIndex == YUV_Y_PLANE_INDEX) ? frameWidth : frameWidth / 2;
    *textureHeight = (planeIndex == YUV_Y_PLANE_INDEX) ? frameHeight : frameHeight / 2;
}

