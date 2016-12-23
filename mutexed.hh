#ifndef HEADER_GUARD_MUTEXED_H
#define HEADER_GUARD_MUTEXED_H

#include <utility>
#include <mutex>

template <class T>
class mutexed {
    T value;
    std::mutex mut;
public:
    class lock_guard {
        mutexed& m;
    public:
        explicit lock_guard(mutexed& m) noexcept(false) : m(m) {
            m.mut.lock();
        }
        ~lock_guard()
            noexcept(noexcept(std::declval<std::mutex>().unlock())) {
            m.mut.unlock();
        }
        T& get() noexcept { return m.value; }
        T& operator*() noexcept { return m.value; }
        T* operator->() noexcept { return std::addressof(m.value); }
    };

    lock_guard lock() noexcept(false) { return lock_guard(*this); }
};

#endif
