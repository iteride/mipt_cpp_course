
#include <cstdio>
#include <exception>
#include <print>
#include <string>

#include "agent.h"
#include "edr_handle.h"
#include "field_traits.h"
#include "file_source.h"
#include "os_source.h"

using namespace nano_edr;

struct options {
    size_t context_size = 2;
    std::string path = "";
    std::string out = "";
    bool quiet = false;
    bool file = false;
};

int main(int argc, char** argv) {
    options option;
    std::string args = "";
    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }
    for (int i = 1; i < argc; ++i) {
        args = std::string(argv[i]);
        if (std::string(argv[i]) == "--quiet") {
            option.quiet = true;
            continue;
        }
        if (args == "--file") {
            option.file = true;
            if (i + 1 < argc) {
                if (option.path.empty()) {
                    option.path = argv[i + 1];
                } else {
                    std::print(stderr, "Double path\n");
                    return 2;
                }
            } else {
                std::print(stderr, "There is no path after --file.\n");
                return 2;
            }
            i++;
            continue;
        }
        if (args == "--out") {
            if (i + 1 < argc) {
                option.out = argv[i + 1];
            } else {
                std::print(stderr, "There is no path after --out.\n");
                return 2;
            }
            i++;
            continue;
        }
        if (args.starts_with("--")) {
            std::print(stderr, "Invalid argument\n");
            return 2;
        }
        if (option.path.empty()) {
            option.path = args;
        } else {
            std::print(stderr, "Double path\n");
            return 2;
        }
    }
    std::unique_ptr<IEventSource> source;
    EdrHandle edr(option.path);
    const std::string text = edr.Config("dedup_period_ms", "10000");
    uint64_t dedup_period_ms = 0;
    if (!FieldTraits<uint64_t>::Parse(text, &dedup_period_ms)) {
        std::print(stderr, "[WARN] dedup_period_ms не разбирается: {}\n", text);
        dedup_period_ms = 10000;
    }
    Agent agent{option.quiet, option.context_size, dedup_period_ms};

    FanOutSink targets;
    targets.Add(ConsoleSink());
    if (!option.out.empty()) {
        targets.Add(FileSink(option.out));
    }
    targets.Add(EdrSink(&edr));
    agent.Subscribe(std::move(targets));
    try {
        if (!option.file) {
            source = std::make_unique<OsSource>(option.path);
        } else {
            source = std::make_unique<FileSource>(option.path);
        }
        source->Run(&agent);
        if (edr.Flush() != EDR_OK) {
            std::print(stderr, "[WARN] детекты не отправлены\n");
        }
        agent.PrintSummary();
    } catch (const std::exception& e) {
        std::print(stderr, "{}\n", e.what());
        return 1;
    }
    return 0;
}