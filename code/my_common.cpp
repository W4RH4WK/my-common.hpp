// MIT License
//
// Copyright (c) 2025 Alexander Hirsch
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "my_common.hpp"

#include <stdarg.h>
#include <stdio.h>

#include <mutex>

namespace MY {

////////////////////////////////////////////////////////////
// String Interpolation

usize sFormat(Slice<char> dst, MY_ATTR_PRINTF_PARAM(const char* fmt), ...)
{
	va_list args;
	va_start(args, fmt);
	int n = vsnprintf(dst.data, dst.count, fmt, args);
	va_end(args);
	if (n < 0)
		return 0;
	if (usize(n) > dst.count)
		return dst.count;
	return usize(n) + 1 /* terminator */;
}

////////////////////////////////////////////////////////////
// Assertion

constinit OnAssert onAssert = +[](const char*, const char*, long) noexcept { abort(); };

////////////////////////////////////////////////////////////
// Logging

thread_local char g_logBuffer[MY_LOG_BUFFER_SIZE];

static constinit std::mutex g_logMutex;

OnLog onLog = +[](LogSeverity severity, const char* msg, const char* file, long line) noexcept {
	std::lock_guard guard(g_logMutex);
	printf("%c [%s:%ld] %s\n", toChar(severity), file, line, msg);
	if (severity >= LogSeverity::Warning) {
		fflush(stdout);
	}
};

////////////////////////////////////////////////////////////
// Allocator

constinit Allocator g_defaultAllocator = {
    .alloc_ = +[](void*, usize size, usize alignment) noexcept -> void* {
	    MY_ASSERT(alignment == 1, nullptr);
	    return malloc(size);
    },
    .dealloc_ = +[](void*, void* ptr) noexcept { return free(ptr); },
};

} // namespace MY
