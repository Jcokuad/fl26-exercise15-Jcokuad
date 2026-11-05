#pragma once
// thread_safe_vector.hpp: starter code -- please refer to in-class example for ThreadSafeQueue

#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeVector {
public:
    void push_back(const T& value);
    bool empty() const;
    bool try_pop_back(T& popped_value);
    void wait_and_pop_back(T& popped_value);
private:
    std::vector<T> the_vector;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;
};

template<typename T>
bool ThreadSafeVector<T>::empty() const {
    std::lock_guard<std::mutex> lock(the_mutex); // locks now
    return the_vector.empty();
}   // unlocks here when the lock goes out of scope

template<typename T>
void ThreadSafeVector<T>::push_back(const T& value) {
    std::unique_lock<std::mutex> lock(the_mutex); // locks now
    the_vector.push_back(value);
    lock.unlock();
    the_condition_variable.notify_one(); // notify one waiting thread
}

template<typename T>
bool ThreadSafeVector<T>::try_pop_back(T& popped_value) {
    std::lock_guard<std::mutex> lock(the_mutex);
    if (the_vector.empty()) {
        return false;
    }
    popped_value = the_vector.back(); // vector takes from the back
    the_vector.pop_back();
    return true;
}

template<typename T>
void ThreadSafeVector<T>::wait_and_pop_back(T& popped_value) {
    std::unique_lock<std::mutex> lock(the_mutex);
    while (the_vector.empty()) {
        the_condition_variable.wait(lock); // unlocks and waits
    }
    popped_value = the_vector.back(); // vector takes from the back
    the_vector.pop_back();
}
