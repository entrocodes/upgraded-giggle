#pragma once

enum class SystemExecResult {
    Ran,
    EarlyExit
};

struct SystemExec {
    SystemExecResult result = SystemExecResult::Ran;
    const char* earlyExitReason = nullptr;
};
