#pragma once

#include <mutex>

namespace chat::common
{

template<typename T>
class SynchronizedValue
{
public:
    template<typename... Args>
    SynchronizedValue(Args&&... args)
      : m_value{std::forward<Args>(args)...}
    {}

    SynchronizedValue(const SynchronizedValue& other) = delete;

    SynchronizedValue& operator=(const SynchronizedValue& other) = delete;

    SynchronizedValue(SynchronizedValue&& other) = delete;

    SynchronizedValue& operator=(SynchronizedValue&& other) = delete;

    ~SynchronizedValue() = default;

    class Locked
    {
    public:
        Locked(const Locked& other) = delete;

        Locked& operator=(const Locked& other) = delete;

        Locked(Locked&& other) = delete;

        Locked& operator=(Locked&& other) = delete;

        ~Locked() = default;

        const T& get() const
        {
            return m_value;
        }

        T& get()
        {
            return m_value;
        }

    private:
        friend class SynchronizedValue;

        Locked(SynchronizedValue& synchronizedValue)
          : m_lock{synchronizedValue.m_mutex},
            m_value{synchronizedValue.m_value}
        {}

        std::unique_lock<decltype(SynchronizedValue::m_mutex)> m_lock;
        T& m_value;
    };

    Locked lock()
    {
        return Locked{*this};
    }

private:
    std::mutex m_mutex;
    T m_value;
};

}
