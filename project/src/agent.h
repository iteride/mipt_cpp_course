#pragma once

#include <deque>
#include <map>

#include "event.h"
#include "event_list.h"
#include "os.h"
#include "rule.h"

namespace nano_edr {
class Agent {
 public:
    Agent(size_t window_size, bool quiet, size_t context_size);  // из --window-size и --quiet
    static void Trampoline(const os_event* ev, void* ctx) noexcept;
    void HandleEvent(const Event& event);
    void PrintSummary() const;

 private:
    EventList window_;
    bool quiet_ = false;
    size_t summary_ = 0;
    size_t context_size_ = 2;
    std::map<std::string, size_t> types;
    std::deque<const EventNode*> context_;
    std::vector<MatchRule> rules_;
    // счётчики для сводки
};
}  // namespace nano_edr