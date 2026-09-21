#include <algorithm>
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include "event.h"
#include "event_list.h"
#include "parse.h"

using namespace nano_edr;

int main(int argc, char** argv) {
    long long lines = 0;
    std::string line;
    std::vector<std::string> warnings{"wscript.exe", ".locked", "certutil.exe", "\\Startup\\"};
    size_t ind_t = 0;
    size_t context_size = 0;
    bool quiet = false;
    Event out;
    EventList list;
    EventNode* prevtail = nullptr;
    size_t ctx_count = 2;

    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }
    if (argc > 2) {
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]).find("--quiet") != std::string::npos) {
                quiet = true;
            }
            if (std::string(argv[i]).find("--window-size") != std::string::npos) {
                if (i + 1 < argc) {
                    list.capacity = std::stoi(std::string(argv[i + 1]));
                    ctx_count = std::min(list.capacity, size_t{2});
                }
            }
        }
    }
    std::ifstream log(argv[1]);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", argv[1]);
        return 2;
    }

    while (std::getline(log, line)) {
        ++lines;
        bool fof = ParseEventLine(&line, &out);
        if (fof) {
            for (size_t i = 0; i < warnings.size(); i++) {
                if (line.find(warnings[i]) != std::string::npos) {
                    if (!quiet) {
                        context_size = std::max(list.size - ctx_count, size_t{0});
                        ind_t = size_t{0};
                        prevtail = list.head;
                        while (prevtail != nullptr) {
                            if (ind_t >= context_size) {
                                std::print("[CTX] -{}: ts={} type={} pid={}\n", list.size - ind_t, prevtail->event.ts, prevtail->event.type, prevtail->event.pid);
                            }
                            ind_t += 1;
                            prevtail = prevtail->next;
                        }
                    }
                    std::print("[DETECT] строка {}, признак {}: {}\n", lines, warnings[i], line);
                }
            }
            ListPushBack(&list, &out);
            out.pid = "";
            out.fields.clear();
        }
    }
    return 0;
}