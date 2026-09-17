#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <print>
#include <string>
#include <vector>

#include "../kit/include/l1.2/event.h"
#include "../kit/include/l1.2/event_list.h"
#include "../kit/include/l1.2/parse.h"

int main(int argc, char** argv) {
    long long lines = 0;
    long long comments = 0;
    std::string line;
    std::map<std::string, int> types;
    std::vector<std::string> warnings{"wscript.exe", ".locked", "certutil.exe", "\\Startup\\"};
    int ind_t, j = 0;
    std::string type;
    bool flag = true;
    nano_edr::Event* out = new nano_edr::Event;
    nano_edr::EventList* list = new nano_edr::EventList;
    std::string cap = "";
    nano_edr::EventNode* prevtail = nullptr;
    int ctx_count = 2;
    std::string args;
    std::string qcopy;

    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }
    if (argc > 2) {
        for (int i = 1; i < argc; ++i) {
            if (i > 1) args += ' ';
            args += argv[i];
        }
        if (args.find("--quiet") != std::string::npos) {
            flag = false;
        }

        ind_t = args.find("--window-size");
        if (ind_t != std::string::npos) {
            ind_t = ind_t + 14;
            while (ind_t < args.size() && args[ind_t] != ' ') {
                cap += args[ind_t];
                ind_t++;
            }
            list->capacity = std::stoi(cap);
        }
    }
    std::ifstream log(argv[1]);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", argv[1]);
        return 2;
    }

    while (std::getline(log, line)) {
        ++lines;
        qcopy = line;
        bool fof = nano_edr::ParseEventLine(&line, out);
        if (fof) {
            for (int i = 0; i < warnings.size(); i++) {
                if (line.find(warnings[i]) != std::string::npos) {
                    if (flag) {
                        j = std::max(int(list->size - ctx_count), 0);
                        ind_t = 0;
                        prevtail = list->head;
                        while (prevtail != nullptr) {
                            if (ind_t >= j) {
                                std::print("[CTX] {}: ts={} type={} pid={}\n", ind_t - int(list->size), prevtail->event.ts, prevtail->event.type, prevtail->event.pid);
                            }
                            ind_t += 1;
                            prevtail = prevtail->next;
                        }
                    } else {
                        std::print("[DETECT] строка {}, признак {}: {}\n", lines, warnings[i], qcopy);
                    }
                }
            }
            nano_edr::ListPushBack(list, out);
        }
        nano_edr::EventClear(out);
    }
    return 0;
}