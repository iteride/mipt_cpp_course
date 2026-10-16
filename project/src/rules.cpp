#include "rules.h"

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
}  // namespace nano_edr