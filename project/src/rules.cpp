#include "rules.h"

#include <iostream>
#include <vector>
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
    std::vector<std::string> detect;
    try {
        for (size_t i = 0; i < rule_count; i++) {
            if (rules[i].check(event) == true) {
                detect.push_back(std::format("[DETECT] {}  {}  ts={} pid={}",
                                             SeverityName(rules[i].severity),
                                             rules[i].id,
                                             event.raw_ts(),
                                             event.pid()));
                detects++;
            }
        }
        for (size_t i = 0; i < detect.size(); i++) {
            std::cout << detect[i] << std::endl;
        }
        return detects;
    } catch (const std::exception& e) {
        return detects;
    }
}

}  // namespace nano_edr