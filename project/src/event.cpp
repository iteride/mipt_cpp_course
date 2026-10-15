#include "event.h"

#include <charconv>
#include <stdexcept>

namespace nano_edr {

std::string ToString(const Event& event) {
    std::string result = "Event: ts=" + event.raw_ts() + " type=" + event.type();
    if (event.pid() != "") {
        result += " pid=" + event.pid();
    }
    for (size_t i = 0; i < event.fields().size(); i++) {
        result += " " + event.fields()[i].key + "=" + event.fields()[i].value;
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << ToString(event);
    return out;
}

Event::Event(const EventParts& parts) {
    if (parts.type == "") {
        throw std::invalid_argument("Event don't have type");
    }
    uint64_t tmp = 0;
    auto [ptr, ec] = std::from_chars(parts.ts.data(), parts.ts.data() + parts.ts.size(), tmp);
    if (ec != std::errc() || ptr != parts.ts.data() + parts.ts.size() || parts.ts.size() == 0) {
        throw std::invalid_argument("Not correct ts:" + parts.ts);
    }
    pid_ = parts.pid;
    type_ = parts.type;
    ts_ = Timestamp{tmp};
    raw_ts_ = parts.ts;
    fields_ = parts.fields;
}
}  // namespace nano_edr