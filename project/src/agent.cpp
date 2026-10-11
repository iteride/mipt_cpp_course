#include "agent.h"

#include <print>
#include <string>

#include "agent_rules.h"
#include "event.h"
#include "event_list.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet) : window_(window_size), quiet_(quiet), summary(0) {
}

void Agent::PrintSummary() const {
    if (!this->quiet_) {
        std::print("total number of events: {}\n", this->summary);
        for (const auto& [type, count] : this->types) {
            std::print("{} : {}\n", type, count);
        }
    }
}

void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    try {
        EventParts part;
        part.ts = std::to_string(ev->ts);
        if (ev->pid == 0) {
            part.pid = "";
        } else {
            part.pid = std::to_string(ev->pid);
        }
        part.type = ev->type;
        for (size_t i = 0; i < ev->field_count; i++) {
            part.fields.push_back({ev->fields[i].key, ev->fields[i].value});
        }
        try {
            const Event event(part);
            static_cast<Agent*>(ctx)->HandleEvent(event);
        } catch (const std::exception& e) {
        }
    } catch (...) {
        // ...
    }
}

void Agent::HandleEvent(const Event& event) {
    size_t context_size;
    size_t ind_t;
    const EventNode* prevtail;
    window_.PushBack(event);
    size_t detects = CheckRules(event, AgentRules(), AgentRuleCount());
    //std::cout << event << std::endl;
    this->types[event.type()]++;
    this->summary++;
    if (detects > 0 && !this->quiet_) {
        context_size = std::min(window_.size() - 1, window_.capacity());
        ind_t = window_.size() - 1;
        prevtail = window_.head();
        while (prevtail != nullptr) {
            if (ind_t <= context_size && ind_t > 0) {
                std::print("[CTX] -{}: ts={} type={} pid={}\n", ind_t, prevtail->event.raw_ts(), prevtail->event.type(), prevtail->event.pid());
            }
            ind_t--;
            prevtail = prevtail->next;
        }
    }
}
}  // namespace nano_edr