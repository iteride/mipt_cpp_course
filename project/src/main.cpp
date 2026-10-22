
#include <charconv>
#include <cstdio>
#include <exception>
#include <print>
#include <string>

#include "agent.h"
#include "file_source.h"
#include "os_source.h"

using namespace nano_edr;

int main(int argc, char** argv) {
    bool quiet = false;
    size_t ctx_count = 0;
    bool file = false;
    std::string path;
    std::string args = "";
    size_t context_size = 2;
    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }
    for (int i = 1; i < argc; ++i) {
        args = std::string(argv[i]);
        if (std::string(argv[i]) == "--quiet") {
            quiet = true;
            continue;
        }
        if (std::string(argv[i]) == "--window-size") {
            if (i + 1 < argc) {
                std::from_chars(std::string(argv[i + 1]).data(), std::string(argv[i + 1]).data() + std::string(argv[i + 1]).size(), ctx_count);
            } else {
                std::print(stderr, "There is no size after --window-size.\n");
                return 2;
            }
            i++;
            continue;
        }
        if (args == "--file") {
            file = true;
            if (i + 1 < argc) {
                if (path.empty()) {
                    path = argv[i + 1];
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
        if (args.starts_with("--")) {
            std::print(stderr, "Invalid argument\n");
            return 2;
        }
        if (path.empty()) {
            path = args;
        } else {
            std::print(stderr, "Double path\n");
            return 2;
        }
    }
    std::unique_ptr<IEventSource> source;
    Agent agent{ctx_count, quiet, context_size};
    try {
        if (!file) {
            source = std::make_unique<OsSource>(path);
        } else {
            source = std::make_unique<FileSource>(path);
        }
        source->Run(&agent);
        agent.PrintSummary();
    } catch (const std::exception& e) {
        std::print(stderr, "{}\n", e.what());
        return 1;
    }
    return 0;
}