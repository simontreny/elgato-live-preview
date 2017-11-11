#include <GLFW/glfw3.h>
#include <memory>
#include "frame.h"
#include "shader.h"

#define YUV_PLANES 3
#define YUV_Y_PLANE_INDEX 0
#define YUV_U_PLANE_INDEX 1
#define YUV_V_PLANE_INDEX 2

class FrameRenderer {
public:
    ~FrameRenderer();

    void render(const Frame& frame, float windowRatio);

private:
    std::shared_ptr<ShaderProgram> m_shaderProgram;

    GLuint m_vertexBuffer = 0;
    GLuint m_posLocation;
    GLuint m_yuvPlanarTextures[YUV_PLANES] = { 0 };
    GLuint m_yuvPlanarTextureLocations[YUV_PLANES];
    bool m_setupDone = false;

    void createShaders(int colorspace);
    void createVertexBuffer();
    void updateVertexBuffer(float frameRatio, float windowRatio);
    void createTextures(const Frame& frame);
    void updateTextures(const Frame& frame);
    void getPlanarTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight);
};
