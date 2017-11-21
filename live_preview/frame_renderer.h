#include <memory>
#include "gl_includes.h"
#include "frame.h"
#include "render_to_texture.h"
#include "screen_aligned_quad.h"

enum YUVPlanes {
    YUV_Y,
    YUV_U,
    YUV_V,
    YUV_NUM_PLANES
};

enum FrameColorspace {
    YUV420P = 0x0001,
    UYVY422 = '2vuy'
};

class FrameRenderer {
public:
    FrameRenderer(int frameWidth, int frameHeight, int colorspace);
    ~FrameRenderer();

    void render(const Frame& frame);

private:
    int m_frameWidth;
    int m_frameHeight;
    int m_colorspace;
    std::shared_ptr<ScreenAlignedQuad> m_quad;
    GLuint m_uyvyTexture = 0;
    GLuint m_yuvPlanarTextures[YUV_NUM_PLANES] = { 0 };

    void createQuad(int frameWidth, int frameHeight, int colorspace);
    void createTextures(int frameWidth, int frameHeight, int colorspace);
    void updateTextures(const Frame& frame);
    void getUyvyTextureSize(int frameWidth, int frameHeight, int *textureWidth, int *textureHeight);
    void getPlanarTextureSize(int planeIndex, int frameWidth, int frameHeight, int *textureWidth, int *textureHeight);
};
