#include "conditions.h"

#include "event.h"
#include "fields.h"
#include "text.h"

namespace nano_edr {

bool FieldEquals::Matches(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return EqualNoCase(*value, value_);
}

bool FieldContains::Matches(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return ContainsNoCase(*value, needle_);
}

bool FieldEndsWith::Matches(const Event& event) const {
    const std::string* value = FindField(event, key_);
    if (value == nullptr) {
        return false;
    }
    return EndsWithNoCase(*value, suffix_);
}

bool FieldInList::Matches(const Event& event) const {
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

bool EventTypeIs::Matches(const Event& event) const {
    const EventType& event_type = event.event_type();
    for (const auto& type : types_) {
        if (type == event_type) {
            return true;
        }
    }
    return false;
}

void AllOf::Add(std::unique_ptr<ICondition> condition) {
    parts_.push_back(std::move(condition));
}

void AnyOf::Add(std::unique_ptr<ICondition> condition) {
    parts_.push_back(std::move(condition));
}

bool AllOf::Matches(const Event& event) const {
    if (parts_.size() == 0) {
        return true;
    }
    for (const auto& check : parts_) {
        if (!(*check)(event)) {
            return false;
        }
    }
    return true;
}

bool AnyOf::Matches(const Event& event) const {
    if (parts_.size() == 0) {
        return false;
    }
    for (const auto& check : parts_) {
        if ((*check)(event)) {
            return true;
        }
    }
    return false;
}
}  // namespace nano_edr