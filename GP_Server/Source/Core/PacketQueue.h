#pragma once

class PacketQueue
{
public:
    void Push(std::vector<BYTE>&& packet)
    {
        RWLock::WriteGuard guard(_qlock);
        _queue.push(std::move(packet));
    }

    bool Pop(std::vector<BYTE>& outPacket)
    {
        RWLock::WriteGuard guard(_qlock);
        if (_queue.empty()) return false;

        outPacket = std::move(_queue.front());
        _queue.pop();
        return true;
    }

private:
	std::queue<std::vector<BYTE>> _queue;
	RWLock _qlock;
};
