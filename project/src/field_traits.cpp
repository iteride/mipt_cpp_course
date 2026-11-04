#include "field_traits.h"

#include <charconv>

#include "text.h"

namespace nano_edr {

bool FieldTraits<std::string>::Parse(const std::string& text, std::string* out) {
    *out = text;
    return true;
}
bool FieldTraits<uint64_t>::Parse(const std::string& text, uint64_t* out) {
    uint64_t temp;
    auto [ptr, errc] = std::from_chars(text.data(), text.data() + text.size(), temp);

    if (ptr == text.data() + text.size() && errc == std::errc()) {
        *out = temp;
        return true;
    }
    return false;
}
bool FieldTraits<bool>::Parse(const std::string& text, bool* out) {
    if (EqualNoCase(text, "yes") || EqualNoCase(text, "1") || EqualNoCase(text, "true")) {
        *out = true;
        return true;
    }
    if (EqualNoCase(text, "no") || EqualNoCase(text, "0") || EqualNoCase(text, "false")) {
        *out = false;
        return true;
    }
    return false;
}
}  // namespace nano_edr