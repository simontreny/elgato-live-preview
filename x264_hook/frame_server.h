#ifndef _FRAME_SERVER_H_
#define _FRAME_SERVER_H_

#include <stdint.h>
#include <stdbool.h>

void frame_server_start();
void frame_server_stop();
bool frame_server_client_connected();
bool frame_server_accept_incoming_client();
void frame_server_send_frame(int width, int height, int colorspace, uint8_t* planes);

#endif
