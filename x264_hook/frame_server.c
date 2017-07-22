#include "frame_server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#define SOCKET_PATH "/tmp/elgato_raw_frames"

static void _frame_server_send_data(void* buf, size_t len);

static int _socket = -1;
static int _clientSocket = -1;

void frame_server_start() {
    if (_socket >= 0) {
        fprintf(stderr, "Warning: frame-server already started\n");
        return;
    }

    _socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_socket < 0) {
        perror("Unable to create frame-server socket");
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);
    if (bind(_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind frame-server socket to path " SOCKET_PATH);
        return;
    }

    if (listen(_socket, 5)) {
        perror("Unable to listen on frame-server socket");
        return;
    }

    printf("Frame-server started successfully\n");
}

void frame_server_stop() {
    if (_socket < 0) {
        fprintf(stderr, "Warning: frame-server already stopped\n");
        return;
    }

    close(_clientSocket);
    close(_socket);
    _socket = -1;
    _clientSocket = -1;
    printf("Frame-server stopped\n");
}

bool frame_server_client_connected() {
    return _clientSocket >= 0;
}

bool frame_server_accept_incoming_client() {
    if (_socket < 0) {
        fprintf(stderr, "Warning: cannot accept incoming client, frame-server not started\n");
        return false;
    }

    struct pollfd fd;
    fd.fd = _socket;
    fd.events = POLLIN;

    if (poll(&fd, 1, 0) < 0) {
        perror("Unable to poll for incoming client to frame-server");
        return false;
    }

    if (fd.revents & POLLIN) {
        _clientSocket = accept(_socket, NULL, NULL);
        if (_clientSocket < 0) {
            perror("Unable to accept incoming client to frame-server");
            return false;
        }

        printf("Client connected to frame-server\n");
        return true;
    } else {
        return false;
    }
}

void frame_server_send_frame(int width, int height, int colorspace, int dataLen, uint8_t* data) {
    if (_clientSocket < 0) {
        fprintf(stderr, "Warning: cannot send frame as no client connected to frame-server\n");
        return;
    }

    printf("Sending %dx%d frame (colorspace: %d) to client\n", width, height, colorspace);
    _frame_server_send_data(&width, sizeof(width));
    _frame_server_send_data(&height, sizeof(height));
    _frame_server_send_data(&colorspace, sizeof(colorspace));
    _frame_server_send_data(&dataLen, sizeof(dataLen));
    _frame_server_send_data(data, dataLen);
}

static void _frame_server_send_data(void* buf, size_t len) {
    if (_clientSocket >= 0) {
        if (send(_clientSocket, buf, len, 0) < 0) {
            perror("Unable to send data to client");
            _clientSocket = -1;
        }
    }
}
