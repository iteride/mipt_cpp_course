#include "parse.h"

#include <string>

namespace nano_edr {

bool IsBlankOrComment(const std::string* line) {
    const auto ind = line->find_first_not_of(" \t");
    if (ind == std::string::npos) {
        return true;
    }

    return (*line).empty() || (*line)[ind] == '#' || (*line)[ind] == ';';
}

bool ParseEventLine(const std::string* line, Event* out) {
    if (IsBlankOrComment(line)) {
        return false;
    }
    std::string str = *line + '\n';
    size_t ind = 0;
    size_t end = 0;
    std::string tims = "";
    std::string key, value;
    bool type = false;
    bool ts = false;
    bool pid = false;
    while (ind < str.size() - size_t{1}) {
        ind = str.find_first_not_of(" \t", ind);
        if (ind >= str.size() - size_t{1}) {
            break;
        }
        if (str[ind] == '=') {
            return false;
        }
        end = str.find_first_of("= \t\n", ind);
        if (str[end] != '=') {
            return false;
        }

        key = str.substr(ind, end - ind);
        ind = end + 1;

        if (str[ind] == '"') {
            ind++;
            end = str.find('"', ind);
            if (end == std::string::npos) {
                return false;
            }
            value = str.substr(ind, end - ind);
            ind = end + 1;
            if ((str[ind] != ' ' && str[ind] != '\t') && ind < str.size() - size_t{1}) {
                return false;
            }
        } else {
            end = str.find_first_of(" \t\n", ind);
            value = str.substr(ind, end - ind);
            ind = end;
        }
        if (key == "type" && !type) {
            out->type = value;
            type = true;
            continue;
        }
        if (key == "ts" && !ts) {
            out->ts = value;
            ts = true;
            continue;
        }
        if (key == "pid" && !pid) {
            out->pid = value;
            pid = true;
            continue;
        }
        out->fields.push_back({key, value});
    }
    if (!type || !ts) {
        return false;
    }
    return true;
}

}  // namespace nano_edr