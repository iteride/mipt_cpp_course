#pragma once

#include "agent.h"

namespace nano_edr {
class FileSource {
 public:
    explicit FileSource(const std::string& path);
    void Run(Agent* agent);  // строка → EventParts → Event → HandleEvent
                             // ...
 private:
    std::string path_;
};
}  // namespace nano_edr