#include <map>

#include "event.h"
#include "event_list.h"
#include "os.h"

namespace nano_edr {
class Agent {
 public:
    Agent(size_t window_size, bool quiet);  // из --window-size и --quiet
    static void Trampoline(const os_event* ev, void* ctx) noexcept;
    void HandleEvent(const Event& event);
    void PrintSummary() const;  // сводка в конце прогона
                                // ...
 private:
    EventList window_;  // последние события, не больше window_size
    bool quiet_;
    int summary;
    std::map<std::string, int> types;
    // счётчики для сводки
};
}  // namespace nano_edr