
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include "agent_rules.h"
#include "event.h"
#include "event_list.h"
#include "parse.h"
#include "rules.h"

using namespace nano_edr;

int main(int argc, char** argv) {
    std::string line;
    size_t ind_t = 0;
    size_t context_size = 0;
    bool quiet = false;
    Event out;
    EventList list;
    EventNode* prevtail = nullptr;
    size_t ctx_count = 2;
    size_t detects = 0;

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
                    list.capacity = std::stoi(std::string(argv[i + 1])) + 1;
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
        bool is_event_valid = ParseEventLine(&line, &out);
        if (is_event_valid) {
            ListPushBack(&list, &out);
            size_t rule_count = AgentRuleCount();
            const Rule* rules = AgentRules();
            try {
                detects = CheckRules(out, rules, rule_count);
            } catch (const std::exception& error) {
                std::print("{}\n", error.what());
            }
            if (!quiet) {
                if (detects) {
                    context_size = std::min(list.size - 1, ctx_count);
                    ind_t = list.size - 1;
                    prevtail = list.head;
                    /*while (prevtail != nullptr) {
                        std::print("{}\n", prevtail->event.ts);
                        prevtail = prevtail->next;
                    }*/
                    while (prevtail != nullptr) {
                        if (ind_t <= context_size && ind_t > 0) {
                            std::print("[CTX] -{}: ts={} type={} pid={}\n", ind_t, prevtail->event.ts, prevtail->event.type, prevtail->event.pid);
                        }
                        ind_t--;
                        prevtail = prevtail->next;
                    }
                }
            }
            out.pid = "";
            out.fields.clear();
            detects = 0;
        }
    }
    return 0;
}