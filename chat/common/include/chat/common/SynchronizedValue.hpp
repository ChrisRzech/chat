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

    class ConstLocked
    {
    public:
        ConstLocked(const ConstLocked& other) = delete;

        ConstLocked& operator=(const ConstLocked& other) = delete;

        ConstLocked(ConstLocked&& other) = delete;

        ConstLocked& operator=(ConstLocked&& other) = delete;

        ~ConstLocked() = default;

        const T& get() const
        {
            return m_value;
        }

    private:
        friend class SynchronizedValue;

        ConstLocked(const SynchronizedValue& synchronizedValue)
          : m_lock{synchronizedValue.m_mutex},
            m_value{synchronizedValue.m_value}
        {}

        std::unique_lock<decltype(SynchronizedValue::m_mutex)> m_lock;
        const T& m_value;
    };

    class Locked
    {
    public:
        Locked(const Locked& other) = delete;

        Locked& operator=(const Locked& other) = delete;

        Locked(Locked&& other) = delete;

        Locked& operator=(Locked&& other) = delete;

        ~Locked() = default;

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

    ConstLocked lock() const
    {
        return ConstLocked(*this);
    }

    Locked lock()
    {
        return Locked{*this};
    }

private:
    mutable std::mutex m_mutex;
    T m_value;
};

}
