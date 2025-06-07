#pragma once

#include <cstddef>
#include <span>

namespace chat::common
{
using BufferView = std::span<const std::byte>;
}
