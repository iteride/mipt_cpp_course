#include "agent.h"
#include "os_handle.h"

namespace nano_edr {
class FileSource {
 public:
    explicit FileSource(const std::string& path);
    void Run(Agent* agent);  // строка → EventParts → Event → HandleEvent
                             // ...
 private:
    std::string path_;
};

class OsSource {
 public:
    explicit OsSource(const std::string& config_path);  // создаёт OsHandle
    void Run(Agent* agent);                             // Subscribe, Start, Wait в цикле
                                                        // ...
 private:
    OsHandle handle_;
};
}  // namespace nano_edr