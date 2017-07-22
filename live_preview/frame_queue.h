#ifndef _FRAME_QUEUE_H_
#define _FRAME_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include "frame.h"

class FrameQueue {
public:
	FrameQueue(size_t maxSize);

	size_t size() const;
	void enqueue(const Frame& frame);
	Frame dequeue();
	bool tryDequeue(Frame* outFrame, int timeoutMs);

private:
	size_t m_maxSize;
	std::queue<Frame> m_frames;
	mutable std::mutex m_mutex;
	std::condition_variable m_hasFrames;
};

#endif
