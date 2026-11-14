#include <iostream>
#include <stdexcept>
#include <string>

#include "alert_sink.h"
#include "dedup_sink.h"
#include "detection.h"

namespace nano_edr {

// --- ConsoleSink -----------------------------------------------------------

void ConsoleSink::operator()(const Detection& detection) const {
    std::cout << ToString(detection) << '\n';
}

// --- FileSink --------------------------------------------------------------

FileSink::FileSink(const std::string& path) : out_(path) {
    if (!out_.is_open()) {
        throw std::runtime_error("не удалось открыть файл: " + path);
    }
}

void FileSink::operator()(const Detection& detection) {
    out_ << ToString(detection) << '\n';
}

// --- CountingSink ----------------------------------------------------------

void CountingSink::operator()(const Detection& detection) const {
    counter_->total++;
    counter_->seen.push_back(detection);
}

// --- FanOutSink ------------------------------------------------------------

void FanOutSink::Add(AlertSink sink) {
    sinks_.push_back(std::move(sink));
}

void FanOutSink::operator()(const Detection& detection) const {
    for (auto sink : sinks_) {
        sink(detection);
    }
}

// --- DedupSink -------------------------------------------------------------

DedupSink::DedupSink(AlertSink inner, uint64_t period_ms, std::size_t* suppressed) : inner_(inner), period_ms_(period_ms), suppressed_(suppressed) {}

void DedupSink::operator()(const Detection& detection) {
    for (auto& record : history_) {
        if (detection.pid == record.pid && detection.rule == record.rule) {
            if (detection.ts.ms - record.first_ts <= period_ms_) {
                if (suppressed_) {
                    (*suppressed_)++;
                }
                return;
            } else {
                record.first_ts = detection.ts.ms;
                inner_(detection);
                return;
            }
        }
    }
    history_.push_back({detection.rule, detection.pid, detection.ts.ms});
    inner_(detection);
}

}  // namespace nano_edr
