#include "fields.h"

#include <stdexcept>

namespace nano_edr {

const std::string* FindField(const Event& event, const std::string& key) {
    for (size_t i = 0; i < event.fields.size(); ++i) {
        if (event.fields[i].key == key) {
            return &event.fields[i].value;
        }
    }
    return nullptr;
}

const std::string& GetRequiredField(const Event& event, const std::string& key) {
    for (size_t i = 0; i < event.fields.size(); ++i) {
        if (event.fields[i].key == key) {
            return event.fields[i].value;
        }
    }
    throw std::invalid_argument("field " + key + " not found");
}

bool is_number(const std::string* value) {
    for (size_t i = 0; i < value->size(); i++) {
        if ((*value)[i] < '0' || (*value)[i] > '9') {
            return false;
        }
    }
    return true;
}

bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    const std::string* value = FindField(event, key);
    if (value != nullptr) {
        if (is_number(value)) {
            try {
                *out = std::stoi(*value);
                return true;
            } catch (const std::exception& error) {
                return false;
            }
        } else {
            return false;
        }
    }
    return false;
}

uint64_t GetIntField(const Event& event, const std::string& key, uint64_t fallback) {
    const std::string* value = FindField(event, key);
    if (value != nullptr) {
        if (is_number(value)) {
            return std::stoi(*value);
        } else {
            return fallback;
        }
    }
    return fallback;
}

bool IsProcessStart(const Event& event) {
    return event.type == "process_start";
}
bool IsFileWrite(const Event& event) {
    return event.type == "file_write";
}
bool IsNetConnect(const Event& event) {
    return event.type == "net_connect";
}
bool PathEndsWith(const Event& event, const std::string& suffix) {
    std::string path_end;
    const std::string* value = FindField(event, "path");
    if (value != nullptr) {
        if (suffix.size() <= value->size()) {
            path_end = value->substr(value->size() - suffix.size(), suffix.size());
            for (size_t j = 0; j < path_end.size(); j++) {
                if (std::tolower(static_cast<unsigned char>(path_end[j])) != std::tolower(static_cast<unsigned char>(suffix[j]))) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    } else {
        const std::string* value = FindField(event, "to");
        if (value != nullptr) {
            if (suffix.size() <= value->size()) {
                path_end = value->substr(value->size() - suffix.size(), suffix.size());
                for (size_t j = 0; j < path_end.size(); j++) {
                    if (std::tolower(static_cast<unsigned char>(path_end[j])) != std::tolower(static_cast<unsigned char>(suffix[j]))) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool CommandLineContains(const Event& event, const std::string& needle) {
    std::string needle_no_reg = "";
    const std::string* value = FindField(event, "cmdline");
    if (value != nullptr) {
        const std::string no_reg = NormalizePath(*value);
        for (size_t j = 0; j < needle.size(); j++) {
            needle_no_reg += std::tolower(static_cast<unsigned char>(needle[j]));
        }
        if (no_reg.find(needle_no_reg) == std::string::npos) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

std::string NormalizePath(const std::string& path) {
    std::string normalize_path = "";
    size_t ind = 0;
    while (ind < path.size()) {
        while (ind < path.size() && path[ind] != '/' && path[ind] != '\\') {
            normalize_path += std::tolower(static_cast<unsigned char>(path[ind]));
            ind++;
        }
        if (path[ind] == '/' || path[ind] == '\\') {
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