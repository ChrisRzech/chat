#include "chat/common/utility.hpp"

#include <iomanip>

namespace chat::common::utility
{

void hexdump(std::ostream& out, const std::byte* bytes, std::size_t size)
{
    constexpr std::size_t BYTES_PER_CHUNK = 2;
    constexpr std::size_t BYTES_PER_LINE = 16;
    // Make sure to have null-terminator
    std::array<char, BYTES_PER_LINE + 1> chars = {'\0'};
    std::size_t linePaddingCount = BYTES_PER_LINE - (size % BYTES_PER_LINE);

    for(std::size_t i = 0; i < size + linePaddingCount; i++) {
        if(i % BYTES_PER_LINE == 0) {
            constexpr int ADDRESS_DIGIT_COUNT = 8;
            out << std::setw(ADDRESS_DIGIT_COUNT) << std::setfill('0')
                << std::hex << i << std::dec << std::setfill(' ') << "  ";
        }

        if(i < size) {
            out << std::setw(2) << std::setfill('0') << std::hex
                << static_cast<int>(bytes[i]) << std::dec << std::setfill(' ');
            chars.at(i % BYTES_PER_LINE) =
                std::isprint(static_cast<int>(bytes[i])) != 0
                    ? static_cast<char>(bytes[i])
                    : '.';
        } else {
            out << "  ";
            chars.at(i % BYTES_PER_LINE) = ' ';
        }

        if((i + 1) % BYTES_PER_LINE == 0) {
            out << " |" << chars.data() << "|\n";
        } else if((i + 1) % BYTES_PER_CHUNK == 0) {
            out << ' ';
        }
    }
}

}
