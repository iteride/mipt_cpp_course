#include "../kit/include/l1.3/fields.h"

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

bool is_number(const std::string& value) {
    for (size_t i = 0; i < value.size(); i++) {
        if (value[i] < '0' || value[i] > '9') {
            return false;
        }
    }
    return true;
}

bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    for (size_t i = 0; i < event.fields.size(); ++i) {
        if (event.fields[i].key == key) {
            if (is_number(event.fields[i].value)) {
                *out = std::stoi(event.fields[i].value);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

uint64_t GetIntField(const Event& event, const std::string& key, uint64_t fallback) {
    for (size_t i = 0; i < event.fields.size(); ++i) {
        if (event.fields[i].key == key) {
            if (is_number(event.fields[i].value)) {
                return std::stoi(event.fields[i].value);
            } else {
                return fallback;
            }
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
    for (size_t i = 0; i < event.fields.size(); i++) {
        if (event.fields[i].key == "path") {
            if (suffix.size() <= event.fields[i].value.size()) {
                path_end = event.fields[i].value.substr(event.fields[i].value.size() - suffix.size(), suffix.size());
                for (size_t j = 0; j < path_end.size(); j++) {
                    if (std::tolower(static_cast<unsigned char>(path_end[j])) != suffix[j]) {
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
    std::string no_reg = "";
    for (size_t i = 0; i < event.fields.size(); i++) {
        if (event.fields[i].key == "cmdline") {
            for (size_t j = 0; j < event.fields[i].value.size(); j++) {
                no_reg += std::tolower(static_cast<unsigned char>(event.fields[i].value[j]));
            }
            if (no_reg.find(needle) == std::string::npos) {
                return false;
            } else {
                return true;
            }
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

/*using namespace nano_edr;

int main() {
    std::string test;
    std::cin >> test;
    std::cout << NormalizePath(test) << std::endl;
}
// namespace nano_edr*/