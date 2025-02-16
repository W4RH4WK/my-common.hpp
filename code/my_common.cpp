#include "my_common.hpp"

#include <cstdio>
#include <mutex>

namespace MY {

// Assertions

OnAssert onAssert = abort;

// Logging

thread_local char g_logBuffer[MY_LOG_BUFFER_SIZE];

OnLog onLog = +[](Severity severity, const char* msg, const char* file, long line) {
	static std::mutex g_logMutex;
	std::lock_guard guard(g_logMutex);
	std::printf("%c [%s:%ld] %s\n", toChar(severity), file, line, msg);
	if (severity >= Severity::Warning) {
		std::fflush(stdout);
	}
};

} // namespace MY
