#include "fields.h"

#include <charconv>
#include <stdexcept>

#include "text.h"

namespace nano_edr {

const std::string* FindField(const Event& event, const std::string& key) {
    if (key == "type") {
        return &event.type();
    }
    if (key == "ts") {
        return &event.raw_ts();
    }
    if (key == "pid") {
        return &event.pid();
    }
    for (size_t i = 0; i < event.fields().size(); ++i) {
        if (event.fields()[i].key == key) {
            return &event.fields()[i].value;
        }
    }
    return nullptr;
}

const std::string& GetRequiredField(const Event& event, const std::string& key) {
    const std::string* value = FindField(event, key);
    if (value != nullptr) {
        return *value;
    }
    throw std::invalid_argument("field " + key + " not found");
}

bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    const std::string* value = FindField(event, key);
    if (value != nullptr) {
        auto [ptr, ec] = std::from_chars(value->data(), value->data() + value->size(), *out);
        if (ec != std::errc()) {
            return false;
        }
        if (ptr != value->data() + value->size()) {
            return false;
        }
        return true;
    }
    return false;
}

uint64_t GetIntField(const Event& event, const std::string& key, uint64_t fallback) {
    const std::string* value = FindField(event, key);
    if (value != nullptr) {
        uint64_t out;
        auto [ptr, ec] = std::from_chars(value->data(), value->data() + value->size(), out);
        if (ec != std::errc()) {
            return fallback;
        }
        if (ptr != value->data() + value->size()) {
            return fallback;
        }
        return out;
    }
    return fallback;
}

bool IsProcessStart(const Event& event) {
    return event.type() == "process_start";
}
bool IsFileWrite(const Event& event) {
    return event.type() == "file_write";
}
bool IsNetConnect(const Event& event) {
    return event.type() == "net_connect";
}
bool PathEndsWith(const Event& event, const std::string& suffix) {
    const std::string* value = FindField(event, "path");
    if (value != nullptr) {
        if (suffix.size() <= value->size()) {
            return EndsWithNoCase(*value, suffix);
        } else {
            return false;
        }
    }
    return false;
}

bool CommandLineContains(const Event& event, const std::string& needle) {
    const std::string* value = FindField(event, "cmdline");
    return value != nullptr && ContainsNoCase(*value, needle);
}

std::string NormalizePath(const std::string& path) {
    std::string normalize_path = "";
    size_t ind = 0;
    while (ind < path.size()) {
        while (ind < path.size() && path[ind] != '/' && path[ind] != '\\') {
            normalize_path += std::tolower(static_cast<unsigned char>(path[ind]));
            ind++;
        }
        if (ind < path.size() && (path[ind] == '/' || path[ind] == '\\')) {
            normalize_path += '\\';
        }
        while (ind < path.size() && (path[ind] == '/' || path[ind] == '\\')) {
            ind++;
        }
    }
    ind = normalize_path.find("%temp%");
    if (ind != std::string::npos) {
        normalize_path.erase(ind, 6);
        normalize_path = normalize_path.substr(0, ind) + "\\appdata\\local\\temp" + normalize_path.substr(ind, normalize_path.size() - ind);
    }
    ind = normalize_path.find("%tmp%");
    if (ind != std::string::npos) {
        normalize_path.erase(ind, 5);
        normalize_path = normalize_path.substr(0, ind) + "\\appdata\\local\\temp" + normalize_path.substr(ind, normalize_path.size() - ind);
    }
    return normalize_path;
}
}  // namespace nano_edr