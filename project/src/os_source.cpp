#include "os_source.h"

#include <os.h>

#include <stdexcept>

#include "os_handle.h"

namespace nano_edr {
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