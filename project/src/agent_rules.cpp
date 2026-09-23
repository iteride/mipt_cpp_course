#include "agent_rules.h"

#include "fields.h"

namespace nano_edr {

bool ScriptHostFromTemp(const Event& event) {
    if (IsProcessStart(event)) {
        for (size_t i = 0; i < event.fields.size(); i++) {
            if (event.fields[i].key == "image") {
                if (event.fields[i].value.find("wscript.exe") != std::string::npos || event.fields[i].value.find("cscript.exe") != std::string::npos) {
                    if (CommandLineContains(event, "\\appdata\\local\\temp\\") || CommandLineContains(event, "\\windows\\temp\\")) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }
        return false;
    }

    bool LolblinDownload(const Event& event) {
        if (IsProcessStart(event) && (CommandLineContains(event, "certutil.exe") || CommandLineContains(event, "bitsadmin.exe")) && (CommandLineContains(event, "urlcache") || CommandLineContains(event, "transfer") || CommandLineContains(event, "http:") || CommandLineContains(event, "https:"))) {
            return true;
        }
        return false;
    }
    bool HiddenPowershell(const Event& event) {
        if (IsProcessStart(event)) {
            for (size_t i = 0; i < event.fields.size(); i++) {
                if (event.fields[i].key == "image" && event.fields[i].value.find(""))
            }
            return true;
        }
        return false;
    }
    bool AutostartWrite(const Event& event);
    bool RansomExtension(const Event& event);

}  // namespace nano_edr