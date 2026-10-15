#include "agent_rules.h"

#include <string>

#include "event.h"
#include "fields.h"
#include "rules.h"

namespace nano_edr {
constexpr Rule kRules[] = {
    {"script_host_from_temp", ScriptHostFromTemp, Severity::kHigh},
    {"lolbin_download", LolblinDownload, Severity::kHigh},
    {"hidden_powershell", HiddenPowershell, Severity::kMedium},
    {"autostart_write", AutostartWrite, Severity::kHigh},
    {"ransom_extension", RansomExtension, Severity::kCritical}};

const Rule* AgentRules() { return kRules; }
size_t AgentRuleCount() { return sizeof(kRules) / sizeof(kRules[0]); }

bool ScriptHostFromTemp(const Event& event) {
    if (IsProcessStart(event)) {
        std::string value = NormalizePath(GetRequiredField(event, "image"));
        if (value.ends_with("wscript.exe") || value.ends_with("cscript.exe")) {
            if (CommandLineContains(event, "\\appdata\\local\\temp\\") || CommandLineContains(event, "\\windows\\temp\\")) {
                return true;
            }
        }
    }
    return false;
}

bool LolblinDownload(const Event& event) {
    if (IsProcessStart(event)) {
        const std::string& value = NormalizePath(GetRequiredField(event, "image"));
        if (value.ends_with("certutil.exe") || value.ends_with("bitsadmin.exe")) {
            if (CommandLineContains(event, "urlcache") || CommandLineContains(event, "transfer") || CommandLineContains(event, "http:") || CommandLineContains(event, "https:")) {
                return true;
            }
        }
    }
    return false;
}
bool HiddenPowershell(const Event& event) {
    if (IsProcessStart(event)) {
        const std::string& value = NormalizePath(GetRequiredField(event, "image"));
        if (value.ends_with("powershell.exe") || value.ends_with("pwsh.exe")) {
            if (CommandLineContains(event, "-w hidden") || CommandLineContains(event, "-windowstyle hidden") || CommandLineContains(event, "-enc") || CommandLineContains(event, "-encodedcommand")) {
                return true;
            }
        }
    }
    return false;
}
bool AutostartWrite(const Event& event) {
    const std::string* value;
    if (event.type() == "file_create" || event.type() == "file_write" || event.type() == "file_move") {
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
    if (event.type() == "file_create" || event.type() == "file_write" || event.type() == "file_move") {
        if (PathEndsWith(event, ".locked")) {
            return true;
        }
    }
    return false;
}
}  // namespace nano_edr