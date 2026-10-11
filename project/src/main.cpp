
#include <cstdio>
#include <exception>
#include <print>
#include <string>

#include "event.h"
#include "source.h"

using namespace nano_edr;

int main(int argc, char** argv) {
    std::string line;
    bool quiet = false;
    EventParts out;
    size_t ctx_count = 2;
    bool file = false;
    std::string path = "";

    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--quiet") {
            quiet = true;
            continue;
        }
        if (std::string(argv[i]) == "--window-size") {
            if (i + 1 < argc) {
                ctx_count = std::stoi(std::string(argv[i + 1])) + 1;
            } else {
                std::print(stderr, "There is no size after --window-size.\n");
                return 2;
            }
            i++;
            continue;
        }
        if (std::string(argv[i]) == "--file") {
            file = true;
            if (i + 1 < argc) {
                if (path == "") {
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
        if (std::string(argv[i]).starts_with("--")) {
            std::print(stderr, "Invalid argument\n");
            return 2;
        }
        if (path == "") {
            path = std::string(argv[i]);
        } else {
            std::print(stderr, "Double path\n");
            return 2;
        }
    }
    Agent agent{ctx_count, quiet};
    try {
        if (!file) {
            OsSource src(path);
            src.Run(&agent);
        } else {
            FileSource src(path);
            src.Run(&agent);
        }
    } catch (const std::exception& e) {
        std::print(stderr, "{}\n", e.what());
        return 1;
    }
    return 0;
}