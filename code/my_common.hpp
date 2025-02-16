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

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

namespace MY {

////////////////////////////////////////////////////////////
// Macros

#define MY_EXPAND(x) x

#define MY_CONCAT(x, y) x##y
#define MY_XCONCAT(x, y) MY_CONCAT(x, y)

#define MY_TEMPORARY(prefix) MY_XCONCAT(prefix, __COUNTER__)

#if defined _MSC_VER
#define MY_FILENAME __FILE__
#else
#define MY_FILENAME __FILE_NAME__
#endif

#define MY_ARRAYSIZE(x) (sizeof(x) / sizeof(0 [x]))

#if defined _MSC_VER
#define MY_ATTR_PRINTF(x, y)
#define MY_ATTR_PRINTF_PARAM(x) _Printf_format_string_ x
#else
#define MY_ATTR_PRINTF(x, y) [[gnu::format(printf, x, y)]]
#define MY_ATTR_PRINTF_PARAM(x) x
#endif

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
// Assertion
//
// These assertions are always enabled, the condition is always checked. An
// error is logged on failure, followed by a call to onAssert (if set). onAssert
// usually terminates the program; however, if control-flow is supposed to
// continue, we return from the current function with the given return value.

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
//
// This provides a very basic logging infrastructure where log messages are
// emitted via a onLog callback function (if set). An internal buffer is used
// for formatting the log message.
//
// The Trace severity is meant for sub-system specific logging that can be
// enabled / disabled at compile-time via the preprocessor (e.g. MY_TRACE_INPUT,
// MY_TRACE_AUDIO).
//
// Note that onLog's implementation as to cover thread-safety.

#define MY_INFO(...) MY_LOG(::MY::Severity::Info, __VA_ARGS__)
#define MY_WARN(...) MY_LOG(::MY::Severity::Warning, __VA_ARGS__)
#define MY_ERROR(...) MY_LOG(::MY::Severity::Error, __VA_ARGS__)

// To minimize dynamic allocation, a thread-local buffer is used for formatting.
#define MY_LOG_BUFFER_SIZE 1024
extern thread_local char g_logBuffer[MY_LOG_BUFFER_SIZE];

#define MY_LOG(severity, ...) \
	do { \
		if (::MY::onLog) { \
			::snprintf(::MY::g_logBuffer, MY_LOG_BUFFER_SIZE, __VA_ARGS__); \
			::MY::onLog(severity, ::MY::g_logBuffer, MY_FILENAME, __LINE__); \
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
//
// The defer utility allows code to be executed automatically at the end of the
// enclosing scope.

#define MY_DEFER(code) const auto MY_TEMPORARY(MY_deferer) = ::MY::Deferer([&]() { code; });

template <typename T>
struct Deferer {
	explicit Deferer(T&& function) : function(function) {}
	~Deferer() noexcept { function(); }
	T function;
};

////////////////////////////////////////////////////////////
// Scalar Math

constexpr auto Pi = 3.14159265358979323846264338327950288;

template <typename T>
constexpr T toRad(T deg)
{
	return deg * T(Pi) / T(180);
}

template <typename T>
constexpr T toDeg(T rad)
{
	return rad * T(180) / T(Pi);
}

template <typename T>
constexpr T min(T a, T b)
{
	return (b < a) ? b : a;
}

template <typename T>
constexpr T max(T a, T b)
{
	return (a < b) ? b : a;
}

template <typename T>
constexpr T clamp(T v, T lo, T hi)
{
	if (v < lo)
		return lo;
	else if (v > hi)
		return hi;
	else
		return v;
}

template <typename T>
constexpr T clamp01(T v)
{
	return clamp(v, T(0), T(1));
}

template <typename T>
constexpr T lerp(T l, T lo, T hi)
{
	return lo + (hi - lo) * l;
}

template <typename T>
constexpr T invLerp(T v, T lo, T hi)
{
	return (v - lo) / (hi - lo);
}

////////////////////////////////////////////////////////////
// Vector 2D

template <typename T>
struct Vec2T {
	constexpr Vec2T() = default;
	constexpr Vec2T(T v) : x(v), y(v) {}
	constexpr Vec2T(T x, T y) : x(x), y(y) {}

	template <typename TT>
	explicit constexpr operator Vec2T<TT>() const
	{
		return {TT(x), TT(y)};
	}

	constexpr double length() const { return sqrt(lengthSq()); }
	constexpr double lengthSq() const { return double(x) * double(x) + double(y) * double(y); }

	constexpr double ratio() const { return double(x) / double(y); }

	constexpr void normalize() { clampLength(1.0); }

	constexpr void clampLength(double max)
	{
		if (double len = length(); len > max) {
			x = T(max * double(x) / len);
			y = T(max * double(y) / len);
		}
	}

	// friend constexpr auto operator<=>(Vec2T, Vec2T) = default;

	T x = 0;
	T y = 0;

	static constexpr Vec2T Up, Down, Left, Right;
};

template <typename T>
constexpr Vec2T<T> Vec2T<T>::Up{0, -1};
template <typename T>
constexpr Vec2T<T> Vec2T<T>::Down{0, 1};
template <typename T>
constexpr Vec2T<T> Vec2T<T>::Left{-1, 0};
template <typename T>
constexpr Vec2T<T> Vec2T<T>::Right{1, 0};

template <typename T>
constexpr T dot(Vec2T<T> a, Vec2T<T> b)
{
	return a.x * b.x + a.y * b.y;
}

template <typename T>
constexpr Vec2T<T>& operator+=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

template <typename T>
constexpr Vec2T<T>& operator-=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

template <typename T, typename C>
constexpr Vec2T<T>& operator*=(Vec2T<T>& v, C c)
{
	v.x *= c;
	v.y *= c;
	return v;
}

template <typename T>
constexpr Vec2T<T>& operator*=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

template <typename T, typename C>
constexpr Vec2T<T>& operator/=(Vec2T<T>& v, C c)
{
	v.x /= c;
	v.y /= c;
	return v;
}

template <typename T>
constexpr Vec2T<T>& operator/=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

template <typename T>
constexpr Vec2T<T> operator-(Vec2T<T> v)
{
	return {-v.x, -v.y};
}

template <typename T>
constexpr Vec2T<T> operator+(Vec2T<T> a, Vec2T<T> b)
{
	return a += b;
}

template <typename T>
constexpr Vec2T<T> operator-(Vec2T<T> a, Vec2T<T> b)
{
	return a -= b;
}

template <typename T, typename C>
constexpr Vec2T<T> operator*(C c, Vec2T<T> v)
{
	return v *= c;
}

template <typename T, typename C>
constexpr Vec2T<T> operator*(Vec2T<T> v, C c)
{
	return v *= c;
}

template <typename T>
constexpr Vec2T<T> operator*(Vec2T<T> a, Vec2T<T> b)
{
	return a *= b;
}

template <typename T, typename C>
constexpr Vec2T<T> operator/(Vec2T<T> v, C c)
{
	return v /= c;
}

template <typename T, typename C>
constexpr Vec2T<T> operator/(C c, Vec2T<T> v)
{
	return {c / v.x, c / v.y};
}

template <typename T>
constexpr Vec2T<T> operator/(Vec2T<T> a, Vec2T<T> b)
{
	return a /= b;
}

using Vec2 = Vec2T<f32>;
using Vec2d = Vec2T<f64>;
using Vec2i = Vec2T<i32>;

////////////////////////////////////////////////////////////
// Span
//
// A Span refers to a contiguous sequence of objects.
//
// This implementation is far more basic than std::span, but it should cover the
// relevant use cases. Functions are implemented in a safe manner where
// possible.

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
	template <typename TT>
	constexpr Span(Span<TT> span) : data(span.data), count(span.count)
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
		offset = min(offset, count);
		subCount = min(subCount, count - offset);
		return Span(data + offset, subCount);
	}

	constexpr Span<T> first(usize subCount) const { return subspan(0, subCount); }
	constexpr Span<T> last(usize subCount) const { return subspan(count - subCount); }

	constexpr T* begin() const { return data; }
	constexpr T* end() const { return data + count; }

	template <typename TT>
	constexpr Span<TT> as() const
	{
		return Span<TT>(reinterpret_cast<TT*>(data), byteCount() / sizeof(TT));
	}

	T* data = nullptr;
	usize count = 0;
};

////////////////////////////////////////////////////////////
// Unmanaged Storage
//
// The UnmanagedStorage wrapper is used to disable dynamic construction and
// destruction for global variables. The wrapped object is stored inline and
// needs to be created and destroyed manually.

template <typename T>
struct UnmanagedStorage {
	template <typename... Args>
	constexpr T* emplace(Args&&... args)
	{
		reset();
		new (instance) T(args...);
		hasInstance = true;
		return get();
	}

	constexpr void reset()
	{
		if (hasInstance) {
			hasInstance = false;
			get()->~T();
		}
	}

	constexpr T* get()
	{
		MY_ASSERT(hasInstance, nullptr);
		return reinterpret_cast<T*>(&instance);
	}
	constexpr const T* get() const { const_cast<UnmanagedStorage<T>*>(this)->get(); }

	constexpr T* operator->() { return get(); }
	constexpr const T* operator->() const { return get(); }

	constexpr explicit operator bool() const { return hasInstance; }

	bool hasInstance = false;
	alignas(T) u8 instance[sizeof(T)] = {};
};

} // namespace MY
