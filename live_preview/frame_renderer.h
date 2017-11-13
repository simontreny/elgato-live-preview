#include <memory>
#include "gl_includes.h"
#include "frame.h"
#include "screen_aligned_quad.h"

#define YUV_PLANES 3
#define YUV_Y_PLANE_INDEX 0
#define YUV_U_PLANE_INDEX 1
#define YUV_V_PLANE_INDEX 2

class FrameRenderer {
public:
    FrameRenderer();
    ~FrameRenderer();

    void render(const Frame& frame, float windowRatio);

private:
    std::shared_ptr<ScreenAlignedQuad> m_quad;
    GLuint m_uyvyTexture = 0;
    GLuint m_yuvPlanarTextures[YUV_PLANES] = { 0 };
    bool m_setupDone = false;

    void createTextures(const Frame& frame);
    void updateTextures(const Frame& frame);
    void getPlanarTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight);
};
