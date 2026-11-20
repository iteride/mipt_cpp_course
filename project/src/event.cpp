#include "event.h"

#include <charconv>
#include <stdexcept>

#include "copy_stats.h"

namespace nano_edr {

Timestamp Event::Validate(const std::string& raw_ts,
                          const std::string& type) {
    if (type.empty()) {
        throw std::invalid_argument("Event don't have type");
    }
    uint64_t tmp = 0;
    auto [ptr, ec] = std::from_chars(raw_ts.data(), raw_ts.data() + raw_ts.size(), tmp);
    if (ec != std::errc() || ptr != raw_ts.data() + raw_ts.size() || raw_ts.size() == 0) {
        throw std::invalid_argument("Not correct ts:" + raw_ts);
    }
    return Timestamp{tmp};
}

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
    ts_ = Validate(parts.ts, parts.type);
    pid_ = parts.pid;
    type_ = parts.type;
    raw_ts_ = parts.ts;
    fields_ = parts.fields;
    event_type_ = EventTypeFromString(type_);
    copy_stats().event_strings_copied += 3 + 2 * parts.fields.size();
}

Event::Event(EventParts&& parts) {
    ts_ = Validate(parts.ts, parts.type);
    copy_stats().event_strings_moved += 3 + 2 * parts.fields.size();
    pid_ = std::move(parts.pid);
    type_ = std::move(parts.type);
    raw_ts_ = std::move(parts.ts);
    fields_ = std::move(parts.fields);
    event_type_ = EventTypeFromString(type_);
}

EventType EventTypeFromString(const std::string& type) {
    if (type == "process_start") {
        return EventType::kProcessStart;
    }
    if (type == "process_end") {
        return EventType::kProcessEnd;
    }
    if (type == "file_create") {
        return EventType::kFileCreate;
    }
    if (type == "file_write") {
        return EventType::kFileWrite;
    }
    if (type == "file_delete") {
        return EventType::kFileDelete;
    }
    if (type == "file_move") {
        return EventType::kFileMove;
    }
    if (type == "net_connect") {
        return EventType::kNetConnect;
    }
    return EventType::kOther;
}
}  // namespace nano_edr