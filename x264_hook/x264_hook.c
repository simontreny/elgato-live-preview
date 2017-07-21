#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include "x264.h"
#include "frame_server.h"

#define DYLD_INTERPOSE(_replacement,_replacee) \
    __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
    __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

x264_t* x264_encoder_open_hook(x264_param_t* param) {
    frame_server_start();
    return x264_encoder_open(param);
}

void x264_encoder_close_hook(x264_t* h) {
    frame_server_stop();
    x264_encoder_close(h);
}

int x264_encoder_encode_hook(x264_t* h, x264_nal_t** pp_nal, int* pi_nal, x264_picture_t* pic_in, x264_picture_t* pic_out) {
    if (frame_server_client_connected() || frame_server_accept_incoming_client()) {
        x264_param_t param;
        x264_encoder_parameters(h, &param);
        frame_server_send_frame(param.i_width, param.i_height, param.i_csp, pic_in->img.plane[0]);
    }

    // Disable x264_encoder_encode() by default as software-encoding can use too much CPU
    if (getenv("X264_HOOK_ENCODE_ENABLED") != NULL) {
        return x264_encoder_encode(h, pp_nal, pi_nal, pic_in, pic_out);
    } else {
        return -1;
    }
}

DYLD_INTERPOSE(x264_encoder_open_hook, x264_encoder_open);
DYLD_INTERPOSE(x264_encoder_close_hook, x264_encoder_close);
DYLD_INTERPOSE(x264_encoder_encode_hook, x264_encoder_encode);
