#pragma once

#include <map>

#include "event.h"
#include "event_source.h"
#include "response.h"
#include "ring_buffer.h"
#include "rule_engine.h"
#include "window_entry.h"

namespace nano_edr {

class Agent : public IEventHandler {
 public:
    explicit Agent(bool quiet, size_t context_size, uint64_t dedup_period_ms);  // из --window-size и --quiet
    static constexpr std::size_t kWindowCapacity = 256;
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;
    void CountBrokenEvent() { broken_events_++; }
    void CountSkipedEvent() { skiped_events_++; }
    void Subscribe(AlertSink sink);
    void EnableResponse(OsHandle* os, bool dry_run,
                        const std::string& quarantine_dir);

 private:
    void PrintContextOnce();
    bool quiet_ = false;
    size_t summary_ = 0;
    size_t skiped_events_ = 0;
    size_t broken_events_ = 0;
    size_t context_size_ = 2;
    uint64_t dedup_period_ms_{};
    std::size_t suppressed_{};
    std::map<std::string, size_t> types_;
    RingBuffer<WindowEntry, kWindowCapacity> window_;
    bool context_printed_ = false;
    EntityModel model_;
    RuleEngine engine_;
    std::unique_ptr<ResponsePlanner> planner_{};
    std::unique_ptr<Responder> responder_{};
    // счётчики для сводки
};
}  // namespace nano_edr