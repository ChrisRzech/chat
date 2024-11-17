#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <variant>

namespace chat::common
{
/**
 * @brief A strong type to represent an error when used on @c Result.
 *
 * @tparam T The type of the error.
 */
template<typename T>
struct Error
{
    /**
     * @brief Construct an @c Error.
     *
     * @tparam Args The types to construct the value with.
     *
     * @param args The values to construct the value with.
     */
    template<typename... Args>
    explicit Error(Args&&... args)
      : value{std::forward<Args>(args)...}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Error(const Error& other) = delete;
    Error& operator=(const Error& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Error(Error&& other) noexcept = default;
    Error& operator=(Error&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the @c Error.
     */
    ~Error() = default;

    T value;
};

/**
 * @brief Deduction guide to deduce the template parameter of @c Error for when
 * a single argument is provided to the constructor of @c Error.
 */
template<typename T>
Error(T) -> Error<T>;

/**
 * @brief A class that represents either a result value or an error value.
 *
 * @details The @c Result class can either represent a result value or an error
 * value. It allows functions to indicate to callers whether it succeeded in its
 * operation or not. If the function does not fail, a result value is returned;
 * otherwise, an error is returned.
 *
 * To store a result value, simply use a constructor or assignment operator. To
 * store an error value, an @c Error must be filled with the value before being
 * passed to a constructor or assignment operator.
 *
 * Requiring the explicit use of @c Error prevents mistaking whether a result
 * value or error value is being stored.
 *
 * If the error type is `void` or is not provided, a partial template
 * specialization of @c Result is used.
 *
 * @tparam T The type of the result value.
 *
 * @tparam E The type of the error value. Defaults to `void`.
 */
template<typename T, typename E = void>
class Result
{
private:
    static constexpr std::size_t resultIndex = 0;
    static constexpr std::size_t errorIndex = 1;

public:
    /**
     * @brief Construct a @c Result that holds a result value.
     *
     * @tparam Args The types to construct the result value with.
     *
     * @param args The values to construct the result value with.
     */
    template<typename... Args>
    explicit Result(Args&&... args)
      : m_value{std::in_place_index<resultIndex>, std::forward<Args>(args)...}
    {}

    /**
     * @brief Construct a @c Result that holds an error value.
     *
     * @param error An @c Error that holds the error value to store.
     */
    explicit Result(Error<E> error)
      : m_value{std::in_place_index<errorIndex>, std::move(error.value)}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Result(const Result& other) = delete;
    Result& operator=(const Result& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Result(Result&& other) noexcept = default;
    Result& operator=(Result&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the @c Result.
     */
    ~Result() = default;

    /**
     * @brief Store a new result value.
     *
     * @details The current value is destroyed, and the new result value is
     * stored.
     *
     * @tparam Arg The type to construct the new result value with.
     *
     * @param arg The value to construct the new result value with.
     *
     * @return This object.
     */
    template<typename Arg>
    Result& operator=(Arg&& arg)
    {
        m_value.template emplace<resultIndex>(std::forward<Arg>(arg));
        return *this;
    }

    /**
     * @brief Store a new error value.
     *
     * @details The current value is destroyed, and the new error value is
     * stored.
     *
     * @param error An @c Error that holds the new error value.
     *
     * @return This object.
     */
    Result& operator=(Error<E> error)
    {
        m_value.template emplace<errorIndex>(std::move(error.value));
        return *this;
    }

    /**
     * @brief Check if a result value is held.
     *
     * @return True if a result value is held; otherwise, false.
     */
    [[nodiscard]] bool hasValue() const
    {
        return m_value.index() == resultIndex;
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] T& getValue() &
    {
        return std::get<resultIndex>(m_value);
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] const T& getValue() const&
    {
        return std::get<resultIndex>(m_value);
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] T&& getValue() &&
    {
        return std::move(std::get<resultIndex>(m_value));
    }

    /**
     * @brief Get the error value.
     *
     * @return The error value.
     */
    [[nodiscard]] E& getError() &
    {
        return std::get<errorIndex>(m_value);
    }

    /**
     * @brief Get the error value.
     *
     * @return The error value.
     */
    [[nodiscard]] const E& getError() const&
    {
        return std::get<errorIndex>(m_value);
    }

    /**
     * @brief Get the error value.
     *
     * @return The error value.
     */
    [[nodiscard]] E&& getError() &&
    {
        return std::move(std::get<errorIndex>(m_value));
    }

private:
    std::variant<T, E> m_value;
};

/**
 * @brief A class that represents either a result value or an error value.
 *
 * @details This is a partial template specialization of @c Result where the
 * error type does not exist and instead can only indicate that a result value
 * exists or not. This form of @c Result essentially acts as an
 * @c std::optional.
 *
 * @tparam T The type of the result value.
 */
template<typename T>
class Result<T, void>
{
public:
    /**
     * @brief Construct a @c Result that does not hold a result value.
     */
    Result() = default;

    /**
     * @brief Construct a @c Result that holds a result value.
     *
     * @tparam Args The types to construct the result value with.
     *
     * @param args The values to construct the result value with.
     */
    template<typename... Args>
    explicit Result(Args&&... args)
      : m_value{std::forward<Args>(args)...}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Result(const Result& other) = delete;
    Result& operator=(const Result& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Result(Result&& other) noexcept = default;
    Result& operator=(Result&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the @c Result.
     */
    ~Result() = default;

    /**
     * @brief Store a new result value.
     *
     * @details The current value is destroyed, and the new result value is
     * stored.
     *
     * @tparam Arg The type to construct the new result value with.
     *
     * @param arg The value to construct the new result value with.
     *
     * @return This object.
     */
    template<typename Arg>
    Result& operator=(Arg&& arg)
    {
        m_value = std::forward<Arg>(arg);
        return *this;
    }

    /**
     * @brief Check if a result value is held.
     *
     * @return True if a result value is held; otherwise, false.
     */
    [[nodiscard]] bool hasValue() const
    {
        return m_value.has_value();
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] T& getValue() &
    {
        return m_value.value();
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] const T& getValue() const&
    {
        return m_value.value();
    }

    /**
     * @brief Get the result value.
     *
     * @return The result value.
     */
    [[nodiscard]] T&& getValue() &&
    {
        return std::move(m_value.value());
    }

private:
    std::optional<T> m_value;
};
}
