#include "agent.h"

#include <print>
#include <string>

#include "agent_rules.h"
#include "event.h"
#include "event_list.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet, size_t context_size) : window_(window_size), quiet_(quiet), context_size_(context_size), rules_(BuildAgentRules()) {
    if (window_size) {
        context_size_ = std::min(context_size_, window_size);
    }
}

void Agent::PrintSummary() const {
    if (!quiet_) {
        std::print("\ntotal number of events: {}\n", summary_);
        for (const auto& [type, count] : types_) {
            std::print("{} : {}\n", type, count);
        }

        std::print("\ndetects on rules:\n");
        for (size_t i = 0; i < rules_.size(); i++) {
            std::print("{}: {}\n", rules_[i].id(), rules_[i].hits());
        }

        std::print("\nInvalid events: {}\n", invalid_events_);
    }
}

namespace {
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
}  // namespace

void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    auto self = static_cast<Agent*>(ctx);
    try {
        const Event event = ToEvent(ev);
        self->HandleEvent(event);
    } catch (const std::exception& e) {
        self->invalid_events_++;
        std::print(stderr, "exception in trampoline {}\n", e.what());
    } catch (...) {
        self->invalid_events_++;
        std::print(stderr, "unknown exception in trampoline\n");
    }
}

void Agent::HandleEvent(const Event& event) {
    if (!window_.empty()) {
        context_.push_back(window_.back());
    }
    if (context_.size() > context_size_) {
        context_.pop_front();
    }
    bool context = false;
    for (size_t i = 0; i < rules_.size(); i++) {
        if (rules_[i].Check(event)) {
            if (!quiet_ && !context) {
                for (size_t j = 0; j < context_.size(); j++) {
                    std::print("[CTX] -{}: ts={} type={} pid={}\n", context_.size() - j, context_[j]->event.raw_ts(), context_[j]->event.type(), context_[j]->event.pid());
                }
                context = true;
            }
            std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(rules_[i].severity()), rules_[i].id(), event.raw_ts(),
                       event.pid());
        }
    }
    types_[event.type()]++;
    summary_++;
    window_.PushBack(event);
}
}  // namespace nano_edr