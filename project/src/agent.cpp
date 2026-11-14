#include "agent.h"

#include <print>

#include "agent_rules.h"
#include "dedup_sink.h"
#include "event.h"

namespace nano_edr {
Agent::Agent(bool quiet, size_t context_size, uint64_t dedup_period_ms) : quiet_(quiet), context_size_(context_size), dedup_period_ms_(dedup_period_ms), engine_(BuildAgentRules(&model_)) {
    engine_.SetModel(&model_);
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
        std::print("[STAT] In the process model, there are {} processes and {} files.\n", model_.process_count(), model_.file_count());
        if (suppressed_ > 0) {
            std::print("[STAT] {} duplicates suppressed (period {} ms)\n", suppressed_, dedup_period_ms_);
        }
    }
}

void Agent::HandleEvent(const Event& event) {
    model_.Observe(event);
    std::vector<const IRule*> fired;
    context_ = false;
    engine_.ProcessEvent(event, &fired);
    types_[event.type()]++;
    summary_++;
    window_.PushBack({event.ts(), event.type(), event.pid()});
}

void Agent::Subscribe(AlertSink sink) {
    FanOutSink all;
    if (!quiet_) {
        all.Add([this](const Detection&) { PrintContextOnce(); });
    }
    all.Add(std::move(sink));
    engine_.Subscribe(DedupSink(std::move(all), dedup_period_ms_, &suppressed_));
}

void Agent::PrintContextOnce() {
    if (context_) {
        return;
    }
    size_t start = 0;
    size_t w_size = window_.size();
    if (context_size_ < w_size) {
        start = w_size - context_size_;
    }
    for (size_t j = start; j < w_size; j++) {
        std::print("[CTX] -{}: {} {} pid={}\n", w_size - j, window_[j].ts.ms, window_[j].type, window_[j].pid);
    }
    context_ = true;
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