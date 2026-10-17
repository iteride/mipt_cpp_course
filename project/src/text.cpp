#include "text.h"

#include <cctype>

namespace nano_edr {

std::string ToLower(const std::string& str) {
    std::string result = "";
    for (size_t i = 0; i < str.size(); i++) {
        result += tolower(static_cast<unsigned char>(str[i]));
    }
    return result;
}

bool EndsWithNoCase(const std::string& value, const std::string& suffix) {
    return ToLower(value).ends_with(ToLower(suffix));
}

bool ContainsNoCase(const std::string& value, const std::string& needle) {
    return ToLower(value).find(ToLower(needle)) != std::string::npos;
}

bool EqualNoCase(const std::string& value1, const std::string& value2) {
    return ToLower(value1) == ToLower(value2);
}
}  // namespace nano_edr
