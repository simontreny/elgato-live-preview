#define _GNU_SOURCE
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include <CoreVideo/CoreVideo.h>
#include <VideoToolbox/VideoToolbox.h>
#include "frame_server.h"

static bool _started = false;

#define DYLD_INTERPOSE(_replacement,_replacee) \
    __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
    __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

OSStatus VTCompressionSessionEncodeFrame_hook(VTCompressionSessionRef session, CVImageBufferRef imageBuffer, CMTime presentationTimeStamp, CMTime duration, CFDictionaryRef frameProperties, void* sourceFrameRefCon, VTEncodeInfoFlags* infoFlagsOut) {
    return VTCompressionSessionEncodeFrame(session, imageBuffer, presentationTimeStamp, duration, frameProperties, sourceFrameRefCon, infoFlagsOut);
}

CVReturn CVPixelBufferCreateWithBytes_hook(CFAllocatorRef allocator, size_t width, size_t height, OSType pixelFormatType, void *baseAddress, size_t bytesPerRow, CVPixelBufferReleaseBytesCallback releaseCallback, void *releaseRefCon, CFDictionaryRef pixelBufferAttributes, CVPixelBufferRef  _Nullable *pixelBufferOut) {
    if (pixelFormatType == '2vuy') {
        if (!_started) {
            _started = true;
            frame_server_start();
        }

        frame_server_accept_incoming_clients();
        frame_server_send_frame(width, height, pixelFormatType, height * bytesPerRow, baseAddress);
    }

    return CVPixelBufferCreateWithBytes(allocator, width, height, pixelFormatType, baseAddress, bytesPerRow, releaseCallback, releaseRefCon, pixelBufferAttributes, pixelBufferOut);
}

DYLD_INTERPOSE(VTCompressionSessionEncodeFrame_hook, VTCompressionSessionEncodeFrame);
DYLD_INTERPOSE(CVPixelBufferCreateWithBytes_hook, CVPixelBufferCreateWithBytes);