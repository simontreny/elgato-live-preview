#include <GLFW/glfw3.h>
#include "frame.h"

#define YUV_PLANES 3
#define YUV_Y_PLANE_INDEX 0
#define YUV_U_PLANE_INDEX 1
#define YUV_V_PLANE_INDEX 2

class FrameRenderer {
public:
    ~FrameRenderer();

    void render(const Frame& frame, float windowRatio);

private:
    GLuint m_vertexBuffer = 0;
    GLuint m_yuvTextures[YUV_PLANES] = { 0 };
    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;
    GLuint m_shaderProgram = 0;
    GLuint m_mvpLocation;
    GLuint m_posLocation;
    GLuint m_uvLocation;
    GLuint m_yuvTextureLocations[YUV_PLANES];
    bool m_setupDone = false;

    void createVertexBuffer();
    void createShaders(int colorspace);
    void createTextures(const Frame& frame);
    void updateTextures(const Frame& frame);
    void getTextureSize(int planeIndex, int frameWidth, int frameHeight, int* textureWidth, int* textureHeight);
};
