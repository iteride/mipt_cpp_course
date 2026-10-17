#include "conditions.h"

#include "fields.h"
#include "text.h"

namespace nano_edr {

bool FieldEquals::operator()(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return EqualNoCase(*value, value_);
}

bool FieldContains::operator()(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return ContainsNoCase(*value, needle_);
}

bool FieldEndsWith::operator()(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return EndsWithNoCase(*value, suffix_);
}

bool FieldInList::operator()(const Event& event) const {
    if (values_.empty()) {
        return false;
    }
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    for (size_t i = 0; i < values_.size(); i++) {
        if (EqualNoCase(*value, values_[i])) {
            return true;
        }
    }
    return false;
}

Condition Condition::Equals(const std::string& key, const std::vector<std::string>& values) {
    return Condition(Match::kEquals, key, values);
}

Condition Condition::Contains(const std::string& key, const std::vector<std::string>& values) {
    return Condition(Match::kContains, key, values);
}
Condition Condition::EndsWith(const std::string& key, const std::vector<std::string>& values) {
    return Condition(Match::kEndsWith, key, values);
}

bool Condition::operator()(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    switch (match_) {
        case Match::kEquals:
            for (size_t i = 0; i < values_.size(); i++) {
                if (EqualNoCase(*value, values_[i])) {
                    return true;
                }
            }
            return false;
        case Match::kContains:
            for (size_t i = 0; i < values_.size(); i++) {
                if (ContainsNoCase(*value, values_[i])) {
                    return true;
                }
            }
            return false;

        case Match::kEndsWith:
            for (size_t i = 0; i < values_.size(); i++) {
                if (EndsWithNoCase(*value, values_[i])) {
                    return true;
                }
            }
            return false;
    }
    return false;
}

}  // namespace nano_edr