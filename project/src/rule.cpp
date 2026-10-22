#include "rule.h"

#include <cstddef>

namespace nano_edr {

RuleBase::RuleBase(const std::string& id, Severity severity) : id_(id), severity_(severity) {}

void RuleBase::AddAction(const std::string& action) {
    actions_.push_back(action);
}

MatchRule::MatchRule(const std::string& id, Severity severity) : RuleBase(id, severity) {}

void MatchRule::AddCondition(std::unique_ptr<ICondition> condition) {
    conditions_.push_back(std::move(condition));
}

bool MatchRule::Check(const Event& event) {
    if (conditions_.empty()) {
        CountHit();
        return true;
    }
    for (size_t i = 0; i < conditions_.size(); i++) {
        if (!(*conditions_[i])(event)) {
            return false;
        }
    }
    CountHit();
    return true;
}

SequenceRule::SequenceRule(const std::string& id, Severity severity,
                           uint64_t window_ms)
    : RuleBase(id, severity), window_ms_(window_ms) {}

void SequenceRule::SetFirst(std::unique_ptr<ICondition> condition) {
    first_ = std::move(condition);
}
void SequenceRule::SetSecond(std::unique_ptr<ICondition> condition) {
    second_ = std::move(condition);
}
bool SequenceRule::Check(const Event& event) {
    if (first_ == nullptr || second_ == nullptr) {
        return false;
    }
    const uint64_t now_ms = event.ts().ms;
    for (size_t i = 0; i < pending_.size();) {
        if (now_ms - pending_[i].ts_ms > window_ms_) {
            pending_.erase(pending_.begin() + i);
        } else {
            i++;
        }
    }
    if ((*second_)(event)) {
        if (same_pid_ == false && !pending_.empty()) {
            pending_.erase(pending_.begin());
            CountHit();
            return true;
        }
        for (size_t i = 0; i < pending_.size(); i++) {
            if (event.pid() == pending_[i].pid) {
                CountHit();
                pending_.erase(pending_.begin() + i);
                return true;
            }
        }
    }
    if ((*first_)(event)) {
        pending_.push_back({event.ts().ms, event.pid()});
    }
    return false;
}

}  // namespace nano_edr