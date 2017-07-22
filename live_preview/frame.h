#ifndef _FRAME_H_
#define _FRAME_H_

#include <vector>
#include <memory>

typedef std::vector<uint8_t> PixelBuffer;

class Frame {
public:
    Frame() = default;
    Frame(int width, int height, int colorspace, int dataLength);

    int width;
    int height;
    int colorspace;
    std::shared_ptr<PixelBuffer> pixels;
};

#endif
