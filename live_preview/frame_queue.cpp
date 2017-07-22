#include "frame_queue.h"
#include <cstdio>
#include <chrono>

FrameQueue::FrameQueue(size_t maxSize): m_maxSize(maxSize) {
}

size_t FrameQueue::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_frames.size();
}

void FrameQueue::enqueue(const Frame& frame) {
	std::lock_guard<std::mutex> lock(m_mutex);
    while (m_maxSize > 0 && m_frames.size() >= m_maxSize) {
        printf("Frame dropped from queue\n");
        m_frames.pop();
    }
	m_frames.push(frame);
	m_hasFrames.notify_one();
}

Frame FrameQueue::dequeue() {
	std::unique_lock<std::mutex> lock(m_mutex);
    while (m_frames.empty()) {
    	m_hasFrames.wait(lock);
    }
    Frame frame = m_frames.front();
    m_frames.pop();
    return frame;
}

bool FrameQueue::tryDequeue(Frame* outFrame, int timeoutMs) {
    auto absTimeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeoutMs);
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_frames.empty()) {
        if (m_hasFrames.wait_until(lock, absTimeout) == std::cv_status::timeout) {
            return false;
        }
    }
    *outFrame = m_frames.front();
    m_frames.pop();
    return true;
}
