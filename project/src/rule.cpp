#include "rule.h"

namespace nano_edr {

RuleBase::RuleBase(const std::string& id, Severity severity) : id_(id), severity_(severity) {}

void RuleBase::AddAction(const std::string& action) {
    actions_.push_back(action);
}

MatchRule::MatchRule(const std::string& id, Severity severity) : RuleBase(id, severity) {}

void MatchRule::AddCondition(const Condition& condition) {
    conditions_.push_back(condition);
}

bool MatchRule::Check(const Event& event) {
    for (size_t i = 0; i < conditions_.size(); i++) {
        if (!conditions_[i](event)) {
            return false;
        }
    }
    CountHit();
    return true;
}

}  // namespace nano_edr