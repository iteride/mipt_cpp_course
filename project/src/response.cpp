#include "response.h"

#include <print>
#include <string>

#include "detection.h"
#include "entity_model.h"
#include "event.h"
#include "fields.h"
#include "os_handle.h"
#include "text.h"

namespace nano_edr {

// --- ResponsePlanner -------------------------------------------------------

namespace {
bool IsSystemFile(const std::string& path) {
    return StartWithNoCase(path, "C:\\Windows\\") || StartWithNoCase(path, "C:\\Program Files\\");
}
}  // namespace

ResponsePlanner::ResponsePlanner(const EntityModel* model,
                                 std::string quarantine_dir)
    : model_(model), quarantine_dir_(quarantine_dir) {}

ResponsePlan ResponsePlanner::Plan(const Detection& detection) const {
    ResponsePlan plan;

    const std::vector<std::string> tree =
        model_->GetProcessTree(detection.pid);
    for (const std::string& name : detection.actions) {
        if (name == "kill_process") {
            for (std::size_t i = tree.size(); i > 0; --i) {
                const std::string& pid = tree[i - 1];
                Action action;
                action.kind = ActionKind::kKillProcess;
                action.pid = pid;
                action.expected_start = model_->FindProcess(pid)->start;
                action.reason = detection.rule;
                if (pid != detection.pid) {
                    action.reason += ", потомок " + detection.pid;
                }
                plan.Add(std::move(action));
            }
        } else if (name == "quarantine_source") {
            const FileRecord* source = model_->FindSpawnSource(detection.pid);
            if (source != nullptr && !IsSystemFile(source->path)) {
                Action action;
                action.kind = ActionKind::kQuarantineFile;
                action.path = source->path;
                action.to = QuarantinePath(source->path, detection.pid);
                if (!action.to.empty()) {
                    action.reason = detection.rule + ", источник запуска";
                    plan.Add(std::move(action));
                }
            }
        } else if (name == "quarantine_image") {
            const ProcessRecord* source = model_->FindProcess(detection.pid);
            if (source != nullptr && !IsSystemFile(source->image)) {
                Action action;
                action.kind = ActionKind::kQuarantineFile;
                action.path = source->image;
                action.to = QuarantinePath(source->image, detection.pid);
                if (!action.to.empty()) {
                    action.reason = detection.rule + ", образ процесса";
                    plan.Add(std::move(action));
                }
            }
        } else if (name == "delete_dropped") {
            for (size_t i = 0; i < tree.size(); ++i) {
                std::string pid = tree[i];
                std::vector<std::string> files = model_->GetFilesCreatedBy(pid, true);
                for (size_t j = 0; j < files.size(); ++j) {
                    if (IsSystemFile(files[j])) {
                        continue;
                    }
                    Action action;
                    action.kind = ActionKind::kDeleteFile;
                    action.path = files[j];
                    action.reason = detection.rule + ", принесён " + pid;
                    plan.Add(std::move(action));
                }
            }
        } else if (name == "delete_evidence") {
            for (const auto& event : detection.evidence) {
                const std::string* path = nullptr;
                if (event->type() == "file_move") {
                    path = FindField(*event, "to");
                } else {
                    path = FindField(*event, "path");
                }
                if (path != nullptr && !IsSystemFile(*path)) {
                    Action action;
                    action.kind = ActionKind::kDeleteFile;
                    action.path = *path;
                    action.reason = detection.rule + ", файл из детекта";
                    plan.Add(std::move(action));
                }
            }
        }
    }
    return plan;
}

std::string ResponsePlanner::QuarantinePath(const std::string& path,
                                            const std::string& pid) const {
    if (quarantine_dir_.empty()) {
        return "";
    }
    std::string name = path.substr(path.find_last_of("\\") + 1);

    return quarantine_dir_ + '\\' + pid + '-' + name;
}

// --- Responder -------------------------------------------------------------

Responder::Responder(OsHandle* os, bool dry_run) : os_(os), dry_run_(dry_run) {}

void Responder::Execute(const ResponsePlan& plan) {
    for (const Action& action : plan.actions()) {
        ++stats_.planned;

        // цель, по которой уже отработали: на одном процессе срабатывает
        // несколько правил
        bool repeat = false;
        for (const std::string& target : done_) {
            if (target == action.target()) {
                repeat = true;
                break;
            }
        }
        if (repeat) {
            ++stats_.skipped_repeat;
            continue;
        }
        done_.push_back(action.target());

        if (dry_run_ || os_ == nullptr) {
            std::print("[PLAN] {}\n", ToString(action));
            continue;
        }
        if (action.kind == ActionKind::kKillProcess) {
            Kill(action);
        }
        if (action.kind == ActionKind::kQuarantineFile) {
            MoveToQuarantine(action);
        }
        if (action.kind == ActionKind::kDeleteFile) {
            Delete(action);
        }
        // на шаге 6 здесь действие исполняется
    }
}

}  // namespace nano_edr
