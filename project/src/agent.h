#pragma once

#include <deque>
#include <map>

#include "event.h"
#include "event_list.h"
#include "event_source.h"
#include "rule_engine.h"

namespace nano_edr {
class Agent : public IEventHandler {
 public:
    Agent(size_t window_size, bool quiet, size_t context_size);  // из --window-size и --quiet
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;
    void CountBrokenEvent() { broken_events_++; }
    void CountSkipedEvent() { skiped_events_++; }

 private:
    EventList window_;
    bool quiet_ = false;
    size_t summary_ = 0;
    size_t skiped_events_ = 0;
    size_t broken_events_ = 0;
    size_t context_size_ = 2;
    std::map<std::string, size_t> types_;
    std::deque<const EventNode*> context_;
    RuleEngine engine_;
    // счётчики для сводки
};
}  // namespace nano_edr