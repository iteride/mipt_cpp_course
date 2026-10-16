#include "text.h"

#include <cctype>

namespace nano_edr {

std::string To_Lower(const std::string& str) {
    std::string result = "";
    for (size_t i = 0; i < str.size(); i++) {
        result += tolower(static_cast<unsigned char>(str[i]));
    }
    return result;
}
}  // namespace nano_edr