#include "agent.h"

#include <print>

#include "agent_rules.h"
#include "event.h"
#include "event_list.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(std::size_t window_size, bool quiet, size_t context_size) : window_(window_size), quiet_(quiet), context_size_(context_size), engine_(BuildAgentRules()) {
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
        for (size_t i = 0; i < engine_.size(); i++) {
            std::print("{}: {}\n", engine_.rule(i).id(), engine_.rule(i).hits());
        }

        std::print("\nInvalid events: {}\n", broken_events_);
    }
}

void Agent::HandleEvent(const Event& event) {
    if (!window_.empty()) {
        context_.push_back(window_.back());
    }
    if (context_.size() > context_size_) {
        context_.pop_front();
    }
    std::vector<const IRule*> fired;
    engine_.ProcessEvent(event, &fired);
    bool context = false;
    for (size_t i = 0; i < fired.size(); i++) {
        if (!quiet_ && !context) {
            for (size_t j = 0; j < context_.size(); j++) {
                std::print("[CTX] -{}: ts={} type={} pid={}\n", context_.size() - j, context_[j]->event.raw_ts(), context_[j]->event.type(), context_[j]->event.pid());
                context = true;
            }
        }
        std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(fired[i]->severity()), fired[i]->id(), event.raw_ts(),
                   event.pid());
    }
    types_[event.type()]++;
    summary_++;
    window_.PushBack(event);
}

void Agent::OnSkipped() {
    skiped_events_++;
}

void Agent::OnBroken() {
    broken_events_++;
}

void Agent::OnTick() {
}

}  // namespace nano_edr