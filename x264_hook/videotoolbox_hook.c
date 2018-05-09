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
    CVPixelBufferRef pixelBuffer = (CVPixelBufferRef)imageBuffer;
    OSType pixelFormatType = CVPixelBufferGetPixelFormatType(pixelBuffer);
    printf("VTCompressionSessionEncodeFrame_hook (%d)\n", (int)pixelFormatType);

    if (pixelFormatType == '2vuy') {
        if (!_started) {
            _started = true;
            frame_server_start();
        }

        frame_server_accept_incoming_clients();

        size_t width = CVPixelBufferGetWidth(pixelBuffer);
        size_t height = CVPixelBufferGetHeight(pixelBuffer);
        size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
        CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
        void* baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
        frame_server_send_frame(width, height, pixelFormatType, height * bytesPerRow, baseAddress);
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }

    return VTCompressionSessionEncodeFrame(session, imageBuffer, presentationTimeStamp, duration, frameProperties, sourceFrameRefCon, infoFlagsOut);
}

DYLD_INTERPOSE(VTCompressionSessionEncodeFrame_hook, VTCompressionSessionEncodeFrame);
