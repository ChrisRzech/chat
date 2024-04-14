#pragma once

#include <cstddef>
#include <vector>

namespace chat::common
{

/**
 * @brief A container for a dynamic number of bytes.
 */
using ByteString = std::vector<std::byte>;

}
