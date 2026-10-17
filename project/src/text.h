#pragma once

#include <string>

namespace nano_edr {

std::string ToLower(const std::string& str);

bool EndsWithNoCase(const std::string& value, const std::string& suffix);

bool ContainsNoCase(const std::string& value, const std::string& needle);

bool EqualNoCase(const std::string& value1, const std::string& value2);
}  // namespace nano_edr