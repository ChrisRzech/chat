#pragma once

#include <memory>
#include <mutex>

namespace chat::common
{

//TODO Should be replaced with boost::synchronized_value if Boost is used
template<typename T>
class SynchronizedValue
{
public:
    SynchronizedValue() = default;

    explicit SynchronizedValue(T&& value)
      : m_value{std::forward<T>(value)}
    {}

    SynchronizedValue(const SynchronizedValue& other) = delete;

    SynchronizedValue& operator=(const SynchronizedValue& other) = delete;

    SynchronizedValue(SynchronizedValue&& other) = delete;

    SynchronizedValue& operator=(SynchronizedValue&& other) = delete;

    ~SynchronizedValue() = default;

    void lock()
    {
        m_mutex.lock();
    }

    T& get()
    {
        return m_value;
    }

    const T& get() const
    {
        return m_value;
    }

    void unlock()
    {
        m_mutex.unlock();
    }

private:
    std::mutex m_mutex;
    T m_value;
};

}
