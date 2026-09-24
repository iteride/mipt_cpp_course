#include "agent_rules.h"

#include "fields.h"

namespace nano_edr {

const Rule* AgentRules() { return kRules; }
size_t AgentRuleCount() { return sizeof(kRules) / sizeof(kRules[0]); }

bool ScriptHostFromTemp(const Event& event) {
    if (IsProcessStart(event)) {
        std::string value = NormalizePath(GetRequiredField(event, "image"));
        if (value.find("wscript.exe") != std::string::npos || value.find("cscript.exe") != std::string::npos) {
            if (CommandLineContains(event, "\\appdata\\local\\temp\\") || CommandLineContains(event, "\\windows\\temp\\")) {
                return true;
            }
        }
    }
    return false;
}

bool LolblinDownload(const Event& event) {
    if (IsProcessStart(event)) {
        std::string value = NormalizePath(GetRequiredField(event, "image"));
        if (value.find("certutil.exe") != std::string::npos || value.find("bitsadmin.exe") != std::string::npos) {
            if (CommandLineContains(event, "urlcache") || CommandLineContains(event, "transfer") || CommandLineContains(event, "http:") || CommandLineContains(event, "https:")) {
                return true;
            }
        }
    }
    return false;
}
bool HiddenPowershell(const Event& event) {
    if (IsProcessStart(event)) {
        std::string value = GetRequiredField(event, "image");
        if (value.find("powershell.exe") != std::string::npos || value.find("pwsh.exe") != std::string::npos) {
            if (CommandLineContains(event, "-w hidden") || CommandLineContains(event, "-windowstyle hidden") || CommandLineContains(event, "-enc") || CommandLineContains(event, "-encodedcommand")) {
                return true;
            }
        }
    }
    return false;
}
bool AutostartWrite(const Event& event) {
    const std::string* value;
    if (event.type == "file_create" || event.type == "file_write" || event.type == "file_move") {
        value = FindField(event, "path");
        if (value != nullptr) {
            if (NormalizePath(*value).find("\\start menu\\programs\\startup\\") != std::string::npos) {
                return true;
            }
        } else {
            value = FindField(event, "to");
            if (value != nullptr) {
                if (NormalizePath(*value).find("\\start menu\\programs\\startup\\") != std::string::npos) {
                    return true;
                }
            }
        }
    }
    return false;
}
bool RansomExtension(const Event& event) {
    if (event.type == "file_create" || event.type == "file_write" || event.type == "file_move") {
        if (PathEndsWith(event, ".locked")) {
            return true;
        }
    }
    return false;
}

}  // namespace nano_edr