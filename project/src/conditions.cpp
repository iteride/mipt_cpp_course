#include "conditions.h"

#include "fields.h"

namespace nano_edr {

bool FieldEquals::operator()(const Event& event) const {
    if (NormalizePath(key_) == "type" && NormalizePath(event.type()) == NormalizePath(value_)) {
        return true;
    }
    if (NormalizePath(key_) == "ts" && NormalizePath(event.raw_ts()) == NormalizePath(value_)) {
        return true;
    }
    if (NormalizePath(key_) == "pid" && NormalizePath(event.pid()) == NormalizePath(value_)) {
        return true;
    }
    for (size_t i = 0; i < event.fields().size(); i++) {
        if (NormalizePath(event.fields()[i].key) == NormalizePath(key_) && NormalizePath(event.fields()[i].value) == NormalizePath(value_)) {
            return true;
        }
    }
    return false;
}

bool FieldContains::operator()(const Event& event) const {
    if (NormalizePath(key_) == "type" && NormalizePath(event.type()).find(NormalizePath(needle_)) != std::string::npos) {
        return true;
    }
    if (NormalizePath(key_) == "ts" && NormalizePath(event.raw_ts()).find(NormalizePath(needle_)) != std::string::npos) {
        return true;
    }
    if (NormalizePath(key_) == "pid" && NormalizePath(event.pid()).find(NormalizePath(needle_)) != std::string::npos) {
        return true;
    }
    for (size_t i = 0; i < event.fields().size(); i++) {
        if (NormalizePath(event.fields()[i].key) == NormalizePath(key_) && NormalizePath(event.fields()[i].value).find(NormalizePath(needle_)) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool FieldEndsWith::operator()(const Event& event) const {
    if (NormalizePath(key_) == "type" && NormalizePath(event.type()).ends_with(NormalizePath(suffix_))) {
        return true;
    }
    if (NormalizePath(key_) == "ts" && NormalizePath(event.raw_ts()).ends_with(NormalizePath(suffix_))) {
        return true;
    }
    if (NormalizePath(key_) == "pid" && NormalizePath(event.pid()).ends_with(NormalizePath(suffix_))) {
        return true;
    }
    for (size_t i = 0; i < event.fields().size(); i++) {
        if (NormalizePath(event.fields()[i].key) == NormalizePath(key_) && NormalizePath(event.fields()[i].value).ends_with(NormalizePath(suffix_))) {
            return true;
        }
    }
    return false;
}

bool FieldInList::operator()(const Event& event) const {
    if (values_.empty()) {
        return false;
    }
    if (NormalizePath(key_) == "type") {
        const std::string lower = NormalizePath(event.type());
        for (size_t i = 0; i < values_.size(); i++) {
            if (lower == NormalizePath(values_[i])) {
                return true;
            }
        }
    }
    if (NormalizePath(key_) == "ts") {
        const std::string lower = NormalizePath(event.raw_ts());
        for (size_t i = 0; i < values_.size(); i++) {
            if (lower == NormalizePath(values_[i])) {
                return true;
            }
        }
    }
    if (NormalizePath(key_) == "pid") {
        const std::string lower = NormalizePath(event.pid());
        for (size_t i = 0; i < values_.size(); i++) {
            if (lower == NormalizePath(values_[i])) {
                return true;
            }
        }
    }
    for (size_t i = 0; i < event.fields().size(); i++) {
        if (NormalizePath(event.fields()[i].key) == NormalizePath(key_)) {
            const std::string lower = NormalizePath(event.fields()[i].value);
            for (size_t j = 0; j < values_.size(); j++) {
                if (lower == NormalizePath(values_[j])) {
                    return true;
                }
            }
        }
    }
    return false;
}

Condition Condition::Equals(const std::string& key, const std::vector<std::string>& values) {
    std::vector<std::string> temp = values;
    for (size_t i = 0; i < temp.size(); i++) {
        temp[i] = NormalizePath(temp[i]);
    }
    return Condition(Match::kEquals, NormalizePath(key), temp);
}

Condition Condition::Contains(const std::string& key, const std::vector<std::string>& values) {
    std::vector<std::string> temp = values;
    for (size_t i = 0; i < temp.size(); i++) {
        temp[i] = NormalizePath(temp[i]);
    }
    return Condition(Match::kContains, NormalizePath(key), temp);
}
Condition Condition::EndsWith(const std::string& key, const std::vector<std::string>& values) {
    std::vector<std::string> temp = values;
    for (size_t i = 0; i < temp.size(); i++) {
        temp[i] = NormalizePath(temp[i]);
    }
    return Condition(Match::kEndsWith, NormalizePath(key), temp);
}

bool Condition::operator()(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    const std::string lower_value = NormalizePath(*value);
    switch (match_) {
        case Match::kEquals:
            for (size_t i = 0; i < values_.size(); i++) {
                if (lower_value == values_[i]) {
                    return true;
                }
            }
            return false;
        case Match::kContains:
            for (size_t i = 0; i < values_.size(); i++) {
                if (lower_value.find(values_[i]) != std::string::npos) {
                    return true;
                }
            }
            return false;

        case Match::kEndsWith:
            for (size_t i = 0; i < values_.size(); i++) {
                if (lower_value.ends_with(values_[i])) {
                    return true;
                }
            }
            return false;
    }
    return false;
}

}  // namespace nano_edr