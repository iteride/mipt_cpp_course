#include "../kit/include/l1.2/parse.h"

#include <print>
#include <string>

bool nano_edr::IsBlankOrComment(const std::string* line) {
    int ind = (*line).find_first_not_of(" \t");
    if (ind == std::string::npos) {
        return true;
    }

    return (*line).empty() || (*line)[ind] == '#' || (*line)[ind] == ';';
}

bool nano_edr::ParseEventLine(const std::string* line, Event* out) {
    if (IsBlankOrComment(line)) {
        return false;
    }
    std::string str = *line + '\n';

    int ind_ts = 0, ind_tp = 0, ind_pid = 0, ind = 0, end = 0;
    std::string tims = "";
    std::string key, value;

    ind = str.find("ts=");

    if (ind == std::string::npos) {
        return false;
    } else {
        ind_ts = ind;
        ind += 3;
        end = str.find_first_of(" \n", ind);
        out->ts = str.substr(ind, end - ind);
        str.erase(ind_ts, end - ind_ts);
    }

    ind = str.find("type=");
    if (ind == std::string::npos) {
        return false;
    } else {
        ind_tp = ind;
        ind += 5;
        end = str.find_first_of(" \n", ind);
        out->type = str.substr(ind, end - ind);
        str.erase(ind_tp, end - ind_tp);
    }

    ind = str.find("pid=");
    if (ind != std::string::npos) {
        ind_pid = ind;
        ind += 4;
        end = str.find_first_of(" \n", ind);
        out->pid = str.substr(ind, end - ind);
        str.erase(ind_pid, end - ind_pid);
    } else {
        out->pid = "";
    }
    ind = 0;
    while (ind < str.size() - 1) {
        ind = str.find_first_not_of(' ', ind);
        if (ind >= str.size() - 1) {
            break;
        }
        if (str[ind] == '=') {
            return false;
        }
        end = str.find_first_of("= \n", ind);
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
            if (str[ind] != ' ' && ind < str.size() - 1) {
                return false;
            }
        } else {
            end = str.find_first_of(" \n", ind);
            value = str.substr(ind, end - ind);
            ind = end;
        }

        out->fields.push_back({key, value});
    }
    return true;
}