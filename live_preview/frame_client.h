#ifndef _FRAME_CLIENT_H_
#define _FRAME_CLIENT_H_

#include <functional>
#include <thread>
#include "frame.h"

typedef void (*FrameReceivedCallback)(const Frame& frame);

class FrameClient {
public:
    FrameClient();

    void start();
    void stop();
    void setFrameReceivedHandler(std::function<void(const Frame&)> handler);

private:
    std::thread m_thread;
    bool m_running;

    std::function<void(const Frame&)> m_frameReceivedHandler;

    void run();
    int connectToServer();
    bool readFrame(int fd);
    void readFromSocket(int fd, void* buf, size_t len);
};

#endif
