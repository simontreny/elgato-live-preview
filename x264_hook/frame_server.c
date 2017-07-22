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
#define MAX_CLIENTS 16

static bool _frame_server_send_data(int fd, void* buf, size_t len);

static int _socket = -1;
static int _client_sockets[MAX_CLIENTS];
static int _num_clients = 0;

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

    for (int i = 0; i < _num_clients; i++) {
        close(_client_sockets[i]);
    }
    _num_clients = 0;

    close(_socket);
    _socket = -1;

    printf("Frame-server stopped\n");
}

bool frame_server_accept_incoming_clients() {
    if (_socket < 0) {
        fprintf(stderr, "Warning: cannot accept incoming clients, frame-server not started\n");
        return false;
    }

    struct pollfd fd;
    fd.fd = _socket;
    fd.events = POLLIN;

    bool has_accepted_clients = false;
    while (true) {
        if (poll(&fd, 1, 0) < 0) {
            perror("Unable to poll for incoming clients to frame-server");
            break;
        }

        if (fd.revents & POLLIN) {
            int client_socket = accept(_socket, NULL, NULL);
            if (client_socket < 0) {
                perror("Unable to accept incoming client to frame-server");
                break;
            }

            if (_num_clients >= MAX_CLIENTS) {
                fprintf(stderr, "Cannot accept incoming client, frame-server is full\n");
                close(client_socket);
                continue;
            }

            _client_sockets[_num_clients++] = client_socket;
            has_accepted_clients = true;
            printf("Client connected to frame-server\n");
        } else {
            break;
        }
    }

    return has_accepted_clients;
}

void frame_server_send_frame(int width, int height, int colorspace, int dataLen, uint8_t* data) {
    printf("Sending %dx%d frame (colorspace: %d) to %d clients\n", width, height, colorspace, _num_clients);
    for (int i = _num_clients - 1; i >= 0; i--) {
        int client_socket = _client_sockets[i];
        if (!_frame_server_send_data(client_socket, &width, sizeof(width))) goto error;
        if (!_frame_server_send_data(client_socket, &height, sizeof(height))) goto error;
        if (!_frame_server_send_data(client_socket, &colorspace, sizeof(colorspace))) goto error;
        if (!_frame_server_send_data(client_socket, &dataLen, sizeof(dataLen))) goto error;
        if (!_frame_server_send_data(client_socket, data, dataLen)) goto error;
        continue;
    error:
        close(client_socket);
        memmove(_client_sockets + i, _client_sockets + i + 1, _num_clients - i - 1);
        _num_clients--;
    }
}

static bool _frame_server_send_data(int fd, void* buf, size_t len) {
    if (send(fd, buf, len, 0) < 0) {
        perror("Unable to send data to client");
        return false;
    }
    return true;
}
