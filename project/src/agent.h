#pragma once

#include <map>

#include "event.h"
#include "event_source.h"
#include "ring_buffer.h"
#include "rule_engine.h"

namespace nano_edr {

struct WindowEntry {
    Timestamp ts;
    std::string type;
    std::string pid;
};

class Agent : public IEventHandler {
 public:
    explicit Agent(bool quiet, size_t context_size);  // из --window-size и --quiet
    static constexpr std::size_t kWindowCapacity = 256;
    void HandleEvent(const Event& event) override;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;
    void PrintSummary() const;
    void CountBrokenEvent() { broken_events_++; }
    void CountSkipedEvent() { skiped_events_++; }

 private:
    bool quiet_ = false;
    size_t summary_ = 0;
    size_t skiped_events_ = 0;
    size_t broken_events_ = 0;
    size_t context_size_ = 2;
    std::map<std::string, size_t> types_;
    RuleEngine engine_;
    RingBuffer<WindowEntry, kWindowCapacity> window_;
    // счётчики для сводки
};
}  // namespace nano_edr