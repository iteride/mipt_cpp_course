#include "os_source.h"

#include <os.h>

#include <print>
#include <stdexcept>

#include "event.h"
#include "os_handle.h"

namespace nano_edr {

OsSource::OsSource(const std::string& config_path) : handle_(config_path) {}  // создаёт OsHandle

namespace {
Event ToEvent(const os_event* ev) {
    EventParts part;
    part.ts = std::to_string(ev->ts);
    if (ev->pid == 0) {
        part.pid = "";
    } else {
        part.pid = std::to_string(ev->pid);
    }
    if (ev->type == nullptr) {
        part.type = "";
    } else {
        part.type = ev->type;
    }
    for (size_t i = 0; i < ev->field_count; i++) {
        part.fields.push_back({ev->fields[i].key, ev->fields[i].value});
    }
    Event event(part);
    return event;
}
}  // namespace

void OsSource::Trampoline(const os_event* ev, void* ctx) noexcept {
    IEventHandler* handler = static_cast<IEventHandler*>(ctx);
    try {
        const Event event = ToEvent(ev);
        handler->HandleEvent(event);
    } catch (const std::exception& e) {
        handler->OnBroken();
        std::print(stderr, "exception in trampoline {}\n", e.what());
    } catch (...) {
        handler->OnBroken();
        std::print(stderr, "unknown exception in trampoline\n");
    }
}

void OsSource::Run(IEventHandler* handler) {
    handle_.Subscribe(&OsSource::Trampoline, handler);
    handle_.Start();
    os_status st;
    while ((st = handle_.Wait(1000)) == OS_TIMEOUT) {
        handler->OnTick();
    }
    if (st != OS_OK) {
        throw std::runtime_error(
            std::string("os_wait: ") + os_status_str(st));
    }
}

}  // namespace nano_edr