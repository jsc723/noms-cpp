#pragma once
#include <string>
#include <span>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "proxy.h"

using pro::proxy;

namespace noms {

PRO_DEF_MEM_DISPATCH(MemToString, toString);

struct Stringable : pro::facade_builder
	::support_copy<pro::constraint_level::nontrivial>
	::add_convention<MemToString, std::string()const>
	::build {};
std::string toString(proxy<Stringable> p) noexcept;


template <typename T>
class Channel {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cond_var;
    bool is_closed = false;

public:
    void send(T value) {
        std::unique_lock<std::mutex> lock(mtx);
        queue.push(std::move(value));
        cond_var.notify_one();
    }

    T receive() {
        std::unique_lock<std::mutex> lock(mtx);
        cond_var.wait(lock, [this] { return !queue.empty() || is_closed; });
        if (is_closed && queue.empty()) {
            throw std::runtime_error("Channel is closed");
        }
        T value = queue.front();
        queue.pop();
        return value;
    }

    void close() {
        std::unique_lock<std::mutex> lock(mtx);
        is_closed = true;
        cond_var.notify_all(); // Wake up all waiting threads
    }
};

}




