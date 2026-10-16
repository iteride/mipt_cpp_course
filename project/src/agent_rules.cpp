#include "agent_rules.h"

#include <string>

#include "conditions.h"
#include "rule.h"
#include "rules.h"

namespace nano_edr {

MatchRule Make_script_host_from_temp() {
    MatchRule rule("script_host_from_temp", Severity::kHigh);
    rule.AddCondition(Condition::Equals("type", {"process_start"}));
    rule.AddCondition(
        Condition::EndsWith("image", {"wscript.exe", "cscript.exe"}));
    rule.AddCondition(Condition::Contains("cmdline", {"\\appdata\\local\\temp\\", "\\windows\\temp\\"}));
    rule.AddAction("kill_process");
    return rule;
}

MatchRule Make_lolblin_download() {
    MatchRule rule("lolbin_download", Severity::kHigh);
    rule.AddCondition(Condition::Equals("type", {"process_start"}));
    rule.AddCondition(Condition::EndsWith("image", {"certutil.exe", "bitsadmin.exe"}));
    rule.AddCondition(Condition::Contains("cmdline", {"urlcache", "transfer",
                                                      "http:", "https:"}));
    rule.AddAction("kill_process");
    return rule;
}

MatchRule Make_hidden_powershell() {
    MatchRule rule("hidden_powershell", Severity::kMedium);
    rule.AddCondition(Condition::Equals("type", {"process_start"}));
    rule.AddCondition(Condition::EndsWith("image", {"powershell.exe", "pwsh.exe"}));
    rule.AddCondition(Condition::Contains("cmdline", {"-w hidden", "-windowstyle hidden", "-enc", "-encodedcommand"}));
    rule.AddAction("kill_process");
    return rule;
}

MatchRule Make_autostart_write_path() {
    MatchRule rule("autostart_write", Severity::kHigh);
    rule.AddCondition(Condition::Equals("type", {"file_create", "file_write"}));
    rule.AddCondition(Condition::Contains("path", {"\\start menu\\programs\\startup\\"}));
    rule.AddAction("quarantine_file");
    return rule;
}

MatchRule Make_autostart_write_to() {
    MatchRule rule("autostart_write", Severity::kHigh);
    rule.AddCondition(Condition::Equals("type", {"file_move"}));
    rule.AddCondition(Condition::Contains("to", {"\\start menu\\programs\\startup\\"}));
    rule.AddAction("quarantine_file");
    return rule;
}

MatchRule Make_ransom_extension_path() {
    MatchRule by_path("ransom_extension", Severity::kCritical);
    by_path.AddCondition(Condition::Equals("type", {"file_create", "file_write"}));
    by_path.AddCondition(Condition::EndsWith("path", {".locked"}));
    by_path.AddAction("quarantine_file");
    return by_path;
}
MatchRule Make_ransom_extension_to() {
    MatchRule by_to("ransom_extension", Severity::kCritical);
    by_to.AddCondition(Condition::Equals("type", {"file_move"}));
    by_to.AddCondition(Condition::EndsWith("to", {".locked"}));
    by_to.AddAction("quarantine_file");
    return by_to;
}

std::vector<MatchRule> BuildAgentRules() {
    std::vector<MatchRule> rules;
    rules.push_back(Make_script_host_from_temp());
    rules.push_back(Make_lolblin_download());
    rules.push_back(Make_hidden_powershell());
    rules.push_back(Make_autostart_write_path());
    rules.push_back(Make_autostart_write_to());
    rules.push_back(Make_ransom_extension_path());
    rules.push_back(Make_ransom_extension_to());
    return rules;
}
}  // namespace nano_edr