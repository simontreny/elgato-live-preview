#ifndef _POST_PROCESS_H_
#define _POST_PROCESS_H_

#include <memory>
#include "render_to_texture.h"
#include "screen_aligned_quad.h"
#include "shader.h"

class PostProcess {
public:
    PostProcess(int width, int height, std::shared_ptr<Shader> fragmentShader);

    void bind() const;
    void draw() const;

private:
    std::shared_ptr<RenderToTexture> m_renderToTexture;
    std::shared_ptr<ScreenAlignedQuad> m_quad;
};

#endif
