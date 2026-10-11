#include "os_handle.h"

#include <os.h>

#include <stdexcept>

namespace nano_edr {

OsHandle::OsHandle(const std::string& config_path) {
    os_handle* out = NULL;
    os_status st = os_init(config_path.c_str(), &out);
    if (st != OS_OK) {
        throw std::runtime_error(
            "os_init failed for '" + config_path + "': " + os_status_str(st));
    }
    handle_ = out;
}

OsHandle::~OsHandle() {
    os_free(this->handle_);
}

void OsHandle::Subscribe(os_event_cb callback, void* context) {
    const os_status st = os_event_subscribe(handle_, callback, context);

    if (st != OS_OK) {
        throw std::runtime_error(
            std::string("os_event_subscribe failed: ") + os_status_str(st));
    }
}

// Запуск доставки. Бросает std::runtime_error при отказе.
void OsHandle::Start() {
    const os_status st = os_start(this->handle_);
    if (st != OS_OK) {
        throw std::runtime_error(
            std::string("os_start failed: ") + os_status_str(st));
    }
}

// Ожидание событий. Возвращает код: OS_TIMEOUT означает «истёк бюджет
// модельного времени, событий пока нет» — это тик агента, а не ошибка.
// OS_OK означает, что поток кончился. Остальные коды — ошибки, например
// OS_IO_ERROR на испорченном журнале; что с ними делать, решает
// вызывающий. Не бросает.
os_status OsHandle::Wait(uint32_t timeout_ms) {
    const os_status st = os_wait(this->handle_, timeout_ms);
    return st;
}

// Остановка доставки. Звать можно всегда, в том числе повторно и после
// конца потока. Не бросает.
void OsHandle::Stop() {
    os_stop(this->handle_);
}

}  // namespace nano_edr