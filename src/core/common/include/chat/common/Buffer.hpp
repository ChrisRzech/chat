#pragma once

#include <cstddef>
#include <vector>

namespace chat::common
{

/**
 * @brief A container for a dynamic number of bytes.
 */
using Buffer = std::vector<std::byte>;

}
