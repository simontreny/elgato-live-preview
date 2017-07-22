#include "frame_client.h"
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <exception>

#define SOCKET_PATH "/tmp/elgato_raw_frames"

FrameClient::FrameClient(): m_running(false) {
}

void FrameClient::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread([this]() { this->run(); });
    }
}

void FrameClient::stop() {
    if (m_running) {
        m_running = false;
        m_thread.join();
    }
}

void FrameClient::setFrameReceivedHandler(std::function<void(const Frame&)> handler) {
    m_frameReceivedHandler = handler;
}

void FrameClient::run() {
    int fd = -1;

    while (m_running) {
        if (fd < 0) {
            fd = this->connectToServer();
            if (fd < 0) {
                sleep(1);
            }
        }

        if (fd >= 0) {
            if (!this->readFrame(fd)) {
                close(fd);
                fd = -1;
            }
        }
    }
}

int FrameClient::connectToServer() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Unable to create frame-client socket");
        return -1;
    }

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to connect to frame-server");
        return -1;
    }

    printf("Connected to frame-server\n");
    return fd;
}

bool FrameClient::readFrame(int fd) {
    try {
        int width, height, colorspace, dataLength;
        readFromSocket(fd, &width, sizeof(width));
        readFromSocket(fd, &height, sizeof(height));
        readFromSocket(fd, &colorspace, sizeof(colorspace));
        readFromSocket(fd, &dataLength, sizeof(dataLength));

        Frame frame(width, height, colorspace, dataLength);
        readFromSocket(fd, frame.pixels->data(), dataLength);

        printf("Received %dx%d frame\n", width, height);

        if (m_frameReceivedHandler != NULL) {
            m_frameReceivedHandler(frame);
        }

        return true;
    } catch (std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
        return false;
    }
}

void FrameClient::readFromSocket(int fd, void* buf, size_t len) {
    size_t recvLen = 0;
    while (recvLen < len) {
        ssize_t l = recv(fd, ((uint8_t*)buf + recvLen), len - recvLen, 0);
        if (l < 0) {
            perror("Unable to receive data from server");
            throw std::runtime_error("Unable to receive data from server");
        } else if (l == 0) {
            throw std::runtime_error("Connection-lost with frame-server");
        }
        recvLen += l;
    }
}
