#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>

// Implementation of a concurrent queue supporting multiple produces/consumers taken from
// https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

namespace Acidrain {

    using namespace std;

    template<typename T>
    class ConcurrentQueue {
    public:
        void push(T const& data) {
            unique_lock<mutex> lock(mutex_);
            storage.push(data);
            lock.unlock();
            waitForDataCondition.notify_one();
        }

        bool empty() const {
            unique_lock<mutex> lock(mutex_);
            return storage.empty();
        }

        bool try_pop(T& popped_value) {
            unique_lock<mutex> lock(mutex_);
            if (storage.empty()) {
                return false;
            }

            popped_value = storage.front();
            storage.pop();
            return true;
        }

        void wait_and_pop(T& popped_value) {
            unique_lock<mutex> lock(mutex_);
            while (storage.empty()) {
                waitForDataCondition.wait(lock);
            }

            popped_value = storage.front();
            storage.pop();
        }
    private:
        queue<T> storage;
        mutable mutex mutex_;
        condition_variable waitForDataCondition;
    };

} // end of namespace Acidrain