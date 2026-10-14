#include "agent.h"

#include <print>
#include <string>

#include "agent_rules.h"
#include "event.h"
#include "event_list.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet) : window_(window_size), quiet_(quiet) {
}

void Agent::PrintSummary() const {
    if (!quiet_) {
        std::print("total number of events: {}\n", summary_);
        for (const auto& [type, count] : types) {
            std::print("{} : {}\n", type, count);
        }
    }
}

Event ToEvent(const os_event* ev) {
    EventParts part;
    part.ts = std::to_string(ev->ts);
    if (ev->pid == 0) {
        part.pid = "";
    } else {
        part.pid = std::to_string(ev->pid);
    }
    if (ev->type == nullptr) {
        part.type = "";
    } else {
        part.type = ev->type;
    }
    for (size_t i = 0; i < ev->field_count; i++) {
        part.fields.push_back({ev->fields[i].key, ev->fields[i].value});
    }
    Event event(part);
    return event;
}

void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    try {
        const Event event = ToEvent(ev);
        static_cast<Agent*>(ctx)->HandleEvent(event);
    } catch (const std::exception& e) {
        std::print(stderr, "exception in trampoline {}\n", e.what());
    }
}

void Agent::HandleEvent(const Event& event) {
    size_t context_size = 2;
    if (window_.capacity()) {
        context_size = std::min(window_.capacity(), context_size);
    }
    if (!window_.empty()) {
        context_.push_back(window_.back());
    }
    if (context_.size() > context_size) {
        context_.pop_front();
    }
    size_t detects = CheckRules(event, AgentRules(), AgentRuleCount());
    types[event.type()]++;
    summary_++;
    if (detects > 0 && !quiet_) {
        for (size_t i = 0; i < context_.size(); i++) {
            std::print("[CTX] -{}: ts={} type={} pid={}\n", context_.size() - i, context_[i]->event.raw_ts(), context_[i]->event.type(), context_[i]->event.pid());
        }
    }
    window_.PushBack(event);
}
}  // namespace nano_edr