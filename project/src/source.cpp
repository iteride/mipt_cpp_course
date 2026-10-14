#include "source.h"

#include <os.h>

#include <fstream>
#include <print>
#include <stdexcept>

#include "os_handle.h"
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
                std::print(stderr, "broken_event: {}\n", e.what());
            }
        }
    }
}

OsSource::OsSource(const std::string& config_path) : handle_(config_path) {}  // создаёт OsHandle
void OsSource::Run(Agent* agent) {
    handle_.Subscribe(&Agent::Trampoline, agent);
    handle_.Start();
    os_status st;
    while ((st = handle_.Wait(100)) == OS_TIMEOUT) {
    }
    if (st != OS_OK) {
        throw std::runtime_error(
            std::string("os_wait: ") + os_status_str(st));
    }
}

}  // namespace nano_edr