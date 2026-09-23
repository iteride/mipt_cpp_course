#include "rules.h"

namespace nano_edr {

const char* SeverityName(Severity severity) {
    if (severity == Severity::kLow) {
        return "low";
    }
    if (severity == Severity::kHigh) {
        return "hight";
    }
    if (severity == Severity::kCritical) {
        return "critical";
    }
    if (severity == Severity::kMedium) {
        return "Medium";
    }
    return "?";
}

size_t CheckRules(const Event& event, const Rule* rules, size_t rule_count) {
}

}  // namespace nano_edr