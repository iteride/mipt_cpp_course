#include "file_source.h"

#include <fstream>
#include <print>

#include "parse.h"

namespace nano_edr {
FileSource::FileSource(const std::string& path) : path_(path) {}

void FileSource::Run(Agent* agent) {
    std::string line;
    std::ifstream log(path_);
    if (!log) {
        throw std::runtime_error("The file could not be opened");
    }
    while (std::getline(log, line)) {
        EventParts out;
        bool is_event_valid = ParseEventParts(line, &out);
        if (is_event_valid) {
            try {
                Event event(out);
                agent->HandleEvent(event);
            } catch (const std::exception& e) {
                agent->CountBrokenEvent();
                std::print(stderr, "broken_event: {}\n", e.what());
            }
        } else {
            agent->CountBrokenEvent();
        }
    }
}
}  // namespace nano_edr