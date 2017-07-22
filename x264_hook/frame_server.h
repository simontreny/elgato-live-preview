#ifndef _FRAME_SERVER_H_
#define _FRAME_SERVER_H_

#include <stdint.h>
#include <stdbool.h>

void frame_server_start();
void frame_server_stop();
bool frame_server_accept_incoming_clients();
void frame_server_send_frame(int width, int height, int colorspace, int dataLen, uint8_t* data);

#endif
