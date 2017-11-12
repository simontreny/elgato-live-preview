#ifndef _RENDER_TO_TEXTURE_H_
#define _RENDER_TO_TEXTURE_H_

#include <vector>
#include <memory>
#include "gl_includes.h"
#include "non_copyable.h"

class RenderToTexture : public NonCopyable {
public:
    RenderToTexture(int width, int height);
    ~RenderToTexture();

    GLuint addTargetTexture(GLenum format);
    GLuint addTargetTexture(int width, int height, GLenum format);
    GLuint getTargetTexture(int index) const;

    void bind() const;
    void unbind() const;

private:
    static const RenderToTexture* s_boundInstance;
    static GLint s_initialViewport[4];

    int m_width;
    int m_height;
    GLuint m_framebuffer;
    std::vector<GLuint> m_targetTextures;
};

#endif
