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
    if (value.size() < suffix.size()) {
        return false;
    }
    const size_t offset = value.size() - suffix.size();
    for (size_t i = 0; i < suffix.size(); i++) {
        if (std::tolower(static_cast<unsigned char>(value[offset + i])) !=
            std::tolower(static_cast<unsigned char>(suffix[i]))) {
            return false;
        }
    }
    return true;
}

bool ContainsNoCase(const std::string& value, const std::string& needle) {
    if (needle.empty()) {
        return true;
    }
    if (value.size() < needle.size()) {
        return false;
    }
    for (size_t i = 0; i + needle.size() <= value.size(); i++) {
        bool match = true;
        for (size_t j = 0; j < needle.size(); j++) {
            if (std::tolower(static_cast<unsigned char>(value[i + j])) !=
                std::tolower(static_cast<unsigned char>(needle[j]))) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}

bool EqualNoCase(const std::string& value1, const std::string& value2) {
    if (value1.size() != value2.size()) {
        return false;
    }
    for (size_t i = 0; i < value1.size(); i++) {
        if (std::tolower(static_cast<unsigned char>(value1[i])) !=
            std::tolower(static_cast<unsigned char>(value2[i]))) {
            return false;
        }
    }
    return true;
}
}  // namespace nano_edr
