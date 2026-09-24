#include "rules.h"

#include <print>

namespace nano_edr {

const char* SeverityName(Severity severity) {
    if (severity == Severity::kLow) {
        return "low";
    }
    if (severity == Severity::kHigh) {
        return "high";
    }
    if (severity == Severity::kCritical) {
        return "critical";
    }
    if (severity == Severity::kMedium) {
        return "medium";
    }
    return "?";
}

size_t CheckRules(const Event& event, const Rule* rules, size_t rule_count) {
    size_t detects = 0;
    for (size_t i = 0; i < rule_count; i++) {
        if (rules[i].check(event) == true) {
            std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(rules[i].severity), rules[i].id, event.ts, event.pid);
            detects++;
        }
    }
    return detects;
}

}  // namespace nano_edr