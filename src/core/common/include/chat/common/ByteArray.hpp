#pragma once

#include <array>
#include <cstddef>

namespace chat::common
{

/**
 * @brief A container for a fixed number of bytes.
 *
 * @tparam N The number of elements in the array.
 */
template<size_t N>
using ByteArray = std::array<std::byte, N>;

}
