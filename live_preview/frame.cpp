#include "frame.h"

static void _addPixelBufferToPool(PixelBuffer* pixels);
static PixelBuffer* _getExistingPixelBufferFromPool(int dataLength);
static PixelBuffer* _getOrAllocatePixelBufferFromPool(int dataLength);
static std::vector<PixelBuffer*> _pixelBufferPool;

Frame::Frame(int width, int height, int colorspace, int dataLength): width(width), height(height), colorspace(colorspace) {
    this->pixels = std::shared_ptr<PixelBuffer>(_getOrAllocatePixelBufferFromPool(dataLength), _addPixelBufferToPool);
}

static void _addPixelBufferToPool(PixelBuffer* pixels) {
    _pixelBufferPool.push_back(pixels);
}

static PixelBuffer* _getExistingPixelBufferFromPool(int dataLength) {
    for (auto it = _pixelBufferPool.begin(); it != _pixelBufferPool.end(); it++) {
        PixelBuffer* pixelBuffer = *it;
        if (pixelBuffer->size() >= dataLength) {
            _pixelBufferPool.erase(it);
            return pixelBuffer;
        }
    }
    return NULL;
}

static PixelBuffer* _getOrAllocatePixelBufferFromPool(int dataLength) {
    PixelBuffer* pixelBuffer = _getExistingPixelBufferFromPool(dataLength);
    if (pixelBuffer == NULL) {
        pixelBuffer = new std::vector<uint8_t>(dataLength);
    }
    return pixelBuffer;
}