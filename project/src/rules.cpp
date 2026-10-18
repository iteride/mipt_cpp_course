#include "rules.h"

namespace nano_edr {

const char* SeverityName(Severity severity) {
    switch (severity) {
        case Severity::kLow:
            return "low";
        case Severity::kHigh:
            return "high";
        case Severity::kCritical:
            return "critical";
        case Severity::kMedium:
            return "medium";
    }
    return "?";
}
}  // namespace nano_edr