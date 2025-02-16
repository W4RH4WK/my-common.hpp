#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <initializer_list>

namespace MY {

////////////////////////////////////////////////////////////
// Macros

#define MY_EXPAND(x) x

#define MY_CONCAT(x, y) x##y
#define MY_XCONCAT(x, y) MY_CONCAT(x, y)

#define MY_TEMPORARY(prefix) MY_XCONCAT(prefix, __COUNTER__)

#if _MSC_VER
#define MY_ATTR_PRINTF(x, y)
#define MY_ATTR_PRINTF_PARAM(x) _Printf_format_string_ x
#else
#define MY_ATTR_PRINTF(x, y) [[gnu::format(printf, x, y)]]
#define MY_ATTR_PRINTF_PARAM(x) x
#endif

////////////////////////////////////////////////////////////
// Assertion

#define MY_ASSERT1(cond) \
	do { \
		if (!(cond)) [[unlikely]] { \
			MY_ERROR("Assertion failed: %s", #cond); \
			if (::MY::onAssert) { \
				::MY::onAssert(); \
			} \
			return; \
		} \
	} while (0)

#define MY_ASSERT2(cond, ret) \
	do { \
		if (!(cond)) [[unlikely]] { \
			MY_ERROR("Assertion failed: %s", #cond); \
			if (::MY::onAssert) { \
				::MY::onAssert(); \
			} \
			return (ret); \
		} \
	} while (0)

#define MY_ASSERT(...) MY_EXPAND(MY_ASSERT_OVERLOAD(__VA_ARGS__, MY_ASSERT2, MY_ASSERT1)(__VA_ARGS__))
#define MY_ASSERT_OVERLOAD(_1, _2, NAME, ...) NAME

using OnAssert = void (*)();
extern OnAssert onAssert;

////////////////////////////////////////////////////////////
// Logging

#define MY_INFO(...) MY_LOG(::MY::Severity::Info, __VA_ARGS__)
#define MY_WARN(...) MY_LOG(::MY::Severity::Warning, __VA_ARGS__)
#define MY_ERROR(...) MY_LOG(::MY::Severity::Error, __VA_ARGS__)

// To minimize dynamic allocation, a thread-local buffer is used for formatting.
#define MY_LOG_BUFFER_SIZE 1024
extern thread_local char g_logBuffer[MY_LOG_BUFFER_SIZE];

#define MY_LOG(severity, ...) \
	do { \
		if (::MY::onLog) { \
			snprintf(::MY::g_logBuffer, MY_LOG_BUFFER_SIZE, __VA_ARGS__); \
			::MY::onLog(severity, ::MY::g_logBuffer, __FILE__, __LINE__); \
		} \
	} while (0)

enum class Severity { Trace, Info, Warning, Error };

inline constexpr char toChar(Severity severity)
{
	switch (severity) {
	case Severity::Trace: return 'T';
	case Severity::Info: return 'I';
	case Severity::Warning: return 'W';
	case Severity::Error: return 'E';
	}
	return '?';
}

using OnLog = void (*)(Severity, const char* message, const char* file, long line);
extern OnLog onLog;

////////////////////////////////////////////////////////////
// Defer

#define MY_DEFER(code) const auto MY_TEMPORARY(deferer) = ::MY::Deferer([&]() { code; });

template <typename T>
struct Deferer {
	explicit Deferer(T&& function) : function(function) {}
	~Deferer() noexcept { function(); }
	T function;
};

////////////////////////////////////////////////////////////
// Type Utilities

template <typename...>
constexpr bool AlwaysFalse = false;

template <typename...>
constexpr bool AlwaysTrue = true;

template <class... Ts>
struct overload : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

////////////////////////////////////////////////////////////
// Primitives

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using usize = size_t;

using f32 = float;
using f64 = double;

////////////////////////////////////////////////////////////
// Span

// Span refers to a contiguous sequence of objects.
//
// This implementation is far more basic than std::span, but it should cover
// the relevant use cases. Functions are implemented in a safe manner where
// possible.
//
// Note the additional conversion functions asBytes and asSpan below, easing
// the handling of binary data.
template <typename T>
struct Span {
	constexpr Span() = default;
	constexpr Span(T* data, usize count) : data(data), count(count) {}
	constexpr Span(T* begin, T* end) : data(begin), count(end - begin) {}

	// Construct from a native array, deriving the statically known size.
	template <usize count>
	constexpr Span(T (&data)[count]) : data(data), count(count)
	{
	}

	// Construct from another span, assuming the underlying pointer is
	// convertible. Commonly used for Span<T> -> Span<const T>.
	template <typename U>
	constexpr Span(Span<U> span) : data(span.data), count(span.count)
	{
	}

	// The sub-script operator always does bounds checking. If this is not
	// desired, use .data[index] instead.
	constexpr T* operator[](usize index) const
	{
		MY_ASSERT(index < count, nullptr);
		return data + index;
	}

	constexpr bool empty() const { return count == 0; }
	constexpr usize byteCount() const { return sizeof(T) * count; }

	constexpr T* front() const { return operator[](0); }
	constexpr T* back() const { return operator[](count - 1); }

	constexpr Span<T> subspan(usize offset, usize subCount = -1) const
	{
		offset = std::min(offset, count);
		subCount = std::min(subCount, count - offset);
		return Span(data + offset, subCount);
	}

	constexpr Span<T> first(usize subCount) const { return subspan(0, subCount); }
	constexpr Span<T> last(usize subCount) const { return subspan(count - subCount); }

	constexpr T* begin() const { return data; }
	constexpr T* end() const { return data + count; }

	T* data = nullptr;
	usize count = 0;
};

template <typename T>
constexpr Span<u8> asBytes(Span<T> span)
{
	return Span(reinterpret_cast<u8*>(span.data), span.byteCount());
}

template <typename T>
constexpr Span<const u8> asBytes(Span<const T> span)
{
	return Span(reinterpret_cast<const u8*>(span.data), span.byteCount());
}

template <typename T>
constexpr Span<T> asSpan(Span<u8> span)
{
	return Span(reinterpret_cast<T*>(span.data), span.count / sizeof(T));
}

template <typename T>
constexpr Span<const T> asSpan(Span<const u8> span)
{
	return Span(reinterpret_cast<const T*>(span.data), span.count / sizeof(T));
}

} // namespace MY
