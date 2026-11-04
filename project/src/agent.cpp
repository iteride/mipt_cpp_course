#include "agent.h"

#include <print>

#include "agent_rules.h"
#include "event.h"
#include "rules.h"

namespace nano_edr {
Agent::Agent(bool quiet, size_t context_size) : quiet_(quiet), context_size_(context_size), engine_(BuildAgentRules()) {
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
        std::print("Skipped events: {}\n", skiped_events_);

        std::print("[STAT] Windows filling - {} out of {}\n", window_.size(), kWindowCapacity);
    }
}

void Agent::HandleEvent(const Event& event) {
    std::vector<const IRule*> fired;
    engine_.ProcessEvent(event, &fired);
    bool context = false;
    for (size_t i = 0; i < fired.size(); i++) {
        if (!quiet_ && !context) {
            size_t start = 0;
            size_t w_size = window_.size();
            if (context_size_ < w_size) {
                start = w_size - context_size_;
            }
            for (size_t j = start; j < w_size; j++) {
                std::print("[CTX] -{}: {} {} pid={}\n", w_size - j, window_[j].ts.ms, window_[j].type, window_[j].pid);
            }
            context = true;
        }
        std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(fired[i]->severity()), fired[i]->id(), event.raw_ts(),
                   event.pid());
    }
    types_[event.type()]++;
    summary_++;
    window_.PushBack({event.ts(), event.type(), event.pid()});
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