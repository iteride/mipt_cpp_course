#include "file_source.h"

#include <fstream>
#include <print>

#include "parse.h"

namespace nano_edr {
FileSource::FileSource(const std::string& path) : path_(path) {}

void FileSource::Run(IEventHandler* handler) {
    std::string line;
    std::ifstream log(path_);
    if (!log) {
        throw std::runtime_error("The file could not be opened");
    }
    while (std::getline(log, line)) {
        EventParts out;
        bool is_event_valid = ParseEventParts(line, &out);
        if (!IsBlankOrComment(line)) {
            if (is_event_valid) {
                try {
                    Event event(out);
                    handler->HandleEvent(event);
                } catch (const std::exception& e) {
                    handler->OnBroken();
                    std::print(stderr, "broken_event: {}\n", e.what());
                }
            } else {
                handler->OnBroken();
            }
        } else {
            handler->OnSkipped();
        }
    }
}
}  // namespace nano_edr