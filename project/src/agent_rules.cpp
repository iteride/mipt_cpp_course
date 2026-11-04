#include "agent_rules.h"

#include <memory>
#include <string>

#include "combinators.h"
#include "conditions.h"
#include "event.h"
#include "make_rule.h"
#include "rule.h"
#include "rule_engine.h"
#include "rules.h"

namespace nano_edr {

namespace {

auto MakeScriptHostFromTemp() {
    auto rule = std::make_unique<MatchRule>("script_host_from_temp", Severity::kHigh);
    rule->AddCondition(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
    temp->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));
    rule->AddCondition(std::move(temp));

    temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\appdata\\local\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/appdata/local/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\windows\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/windows/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%temp%"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%tmp%"));
    rule->AddCondition(std::move(temp));
    rule->AddAction("kill_process");
    return rule;
}

auto MakeLolbinDownload() {
    auto rule = std::make_unique<MatchRule>("lolbin_download", Severity::kHigh);
    rule->AddCondition(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldEndsWith>("image", "certutil.exe"));
    temp->Add(std::make_unique<FieldEndsWith>("image", "bitsadmin.exe"));
    rule->AddCondition(std::move(temp));

    temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("cmdline", "urlcache"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "transfer"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "http:"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "https:"));
    rule->AddCondition(std::move(temp));
    rule->AddAction("kill_process");
    return rule;
}

auto MakeHiddenPowershell() {
    auto rule = std::make_unique<MatchRule>("hidden_powershell", Severity::kMedium);
    rule->AddCondition(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kProcessStart}));
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldEndsWith>("image", "powershell.exe"));
    temp->Add(std::make_unique<FieldEndsWith>("image", "pwsh.exe"));
    rule->AddCondition(std::move(temp));

    temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("cmdline", "-w hidden"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "-windowstyle hidden"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "-enc"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "-encodedcommand"));
    rule->AddCondition(std::move(temp));
    rule->AddAction("kill_process");
    return rule;
}

auto MakeAutostartWrite() {
    auto rule = std::make_unique<MatchRule>("autostart_write", Severity::kHigh);
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("path", "\\start menu\\programs\\startup\\"));
    temp->Add(std::make_unique<FieldContains>("path", "/start menu/programs/startup/"));
    temp->Add(std::make_unique<FieldContains>("to", "\\start menu\\programs\\startup\\"));
    temp->Add(std::make_unique<FieldContains>("to", "/start menu/programs/startup/"));
    rule->AddCondition(std::move(temp));
    rule->AddAction("quarantine_file");
    return rule;
}

auto MakeRansomExtension() {
    auto rule = std::make_unique<MatchRule>("ransom_extension", Severity::kCritical);
    rule->AddCondition(std::make_unique<EventTypeIs>(std::vector<EventType>{EventType::kFileWrite, EventType::kFileCreate, EventType::kFileMove}));
    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldEndsWith>("path", ".locked"));
    temp->Add(std::make_unique<FieldEndsWith>("to", ".locked"));
    rule->AddCondition(std::move(temp));
    rule->AddAction("quarantine_file");
    return rule;
}

auto MakeScriptHostBeacon() {
    auto beacon = std::make_unique<SequenceRule>("script_host_beacon",
                                                 Severity::kCritical, 10000);
    auto first = std::make_unique<AllOf>();
    first->Add(std::make_unique<EventTypeIs>(std::vector<EventType>({EventType::kProcessStart})));

    auto temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldEndsWith>("image", "wscript.exe"));
    temp->Add(std::make_unique<FieldEndsWith>("image", "cscript.exe"));

    first->Add(std::move(temp));

    temp = std::make_unique<AnyOf>();
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\appdata\\local\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/appdata/local/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "\\windows\\temp\\"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "/windows/temp/"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%temp%"));
    temp->Add(std::make_unique<FieldContains>("cmdline", "%tmp%"));

    first->Add(std::move(temp));

    beacon->SetFirst(std::move(first));

    beacon->SetSecond(std::make_unique<EventTypeIs>(
        std::vector<EventType>{EventType::kNetConnect}));
    beacon->AddAction("kill_process");
    return beacon;
}

auto MakeRansomwareMassWrite() {
    auto mass_write = MakeRule<ThresholdRule<128>>(
        "ransomware_mass_write", Severity::kCritical,
        20,      // сколько событий
        10000,   // за сколько миллисекунд
        "pid");  // порог считается для каждого pid отдельно
    mass_write->SetCondition(MakeCondition(AllOfStatic(
        EventTypeIs({EventType::kFileCreate, EventType::kFileWrite,
                     EventType::kFileMove}),
        AnyOfStatic(FieldContains("path", "\\Documents\\"),
                    FieldContains("path", "\\Desktop\\"),
                    FieldContains("to", "\\Documents\\"),
                    FieldContains("to", "\\Desktop\\")))));
    mass_write->AddAction("kill_process");
    mass_write->AddAction("quarantine_file");
    return mass_write;
}

}  // namespace

RuleEngine BuildAgentRules() {
    RuleEngine engine;
    engine.AddRule(MakeScriptHostFromTemp());
    engine.AddRule(MakeLolbinDownload());
    engine.AddRule(MakeHiddenPowershell());
    engine.AddRule(MakeAutostartWrite());
    engine.AddRule(MakeRansomExtension());
    engine.AddRule(MakeScriptHostBeacon());
    engine.AddRule(MakeRansomwareMassWrite());
    return engine;
}
}  // namespace nano_edr