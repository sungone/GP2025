#pragma once
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>

class RWLock {
private:
    std::mutex mutex;
    std::condition_variable condition;
    int readerCount = 0;
    bool writerActive = false;

public:
    class ReadGuard {
    private:
        RWLock& lock;
    public:
        ReadGuard(RWLock& lock) : lock(lock) {
            lock.lockRead();
        }
        ~ReadGuard() {
            lock.unlockRead();
        }
    };

    class WriteGuard {
    private:
        RWLock& lock;
    public:
        WriteGuard(RWLock& lock) : lock(lock) {
            lock.lockWrite();
        }
        ~WriteGuard() {
            lock.unlockWrite();
        }
    };

    void lockRead() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]() { return !writerActive; });
        ++readerCount;
    }

    void unlockRead() {
        std::lock_guard<std::mutex> lock(mutex);
        if (--readerCount == 0) {
            condition.notify_all();
        }
    }

    void lockWrite() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]() { return !writerActive && readerCount == 0; });
        writerActive = true;
    }

    void unlockWrite() {
        std::lock_guard<std::mutex> lock(mutex);
        writerActive = false;
        condition.notify_all();
    }
};