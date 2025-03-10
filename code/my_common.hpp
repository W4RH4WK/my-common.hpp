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

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include <bit>
#include <initializer_list>
#include <memory>
#include <type_traits>

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

template <typename T>
struct Span;

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
// Hash

// clang-format off
inline constexpr u64 hash(i8  v) { return u64(v); }
inline constexpr u64 hash(u8  v) { return u64(v); }
inline constexpr u64 hash(i16 v) { return u64(v); }
inline constexpr u64 hash(u16 v) { return u64(v); }
inline constexpr u64 hash(i32 v) { return u64(v); }
inline constexpr u64 hash(u32 v) { return u64(v); }
inline constexpr u64 hash(i64 v) { return u64(v); }
inline constexpr u64 hash(u64 v) { return v; }
inline constexpr u64 hash(f32 v) { return hash(std::bit_cast<u32>(v)); }
inline constexpr u64 hash(f64 v) { return hash(std::bit_cast<u64>(v)); }
// clang-format on

template <typename T>
constexpr u64 hash(T* p)
{
	return u64(p);
}

inline constexpr u64 hashCombine(u64 seed, u64 v)
{
	seed ^= seed >> 32u;
	seed *= 0xE9846AF9B1A615Du;
	seed ^= seed >> 32u;
	seed *= 0xE9846AF9B1A615Du;
	seed ^= seed >> 28u;
	return seed + v;
}

inline constexpr u64 hashRange(const u8* data, usize size)
{
	u64 seed = 0;
	while (size >= sizeof(u64)) {
		seed = hashCombine(seed, *reinterpret_cast<const u64*>(data));
		data += sizeof(u64);
		size -= sizeof(u64);
	}
	while (size >= sizeof(u32)) {
		seed = hashCombine(seed, *reinterpret_cast<const u32*>(data));
		data += sizeof(u32);
		size -= sizeof(u32);
	}
	while (size > 0) {
		seed = hashCombine(seed, *data++);
		size--;
	}
	return seed;
}

////////////////////////////////////////////////////////////
// String Interpolation
//
// Some form of string interpolation is essential for the creation of log and
// error messages. For simplicity, we use a thin wrapper around snprintf, which
// writes the result into dst.
//
// The resulting string is null-terminated and sFormat returns the number of
// characters written (including the null-terminator).

MY_ATTR_PRINTF(2, 3)
usize sFormat(Span<char> dst, MY_ATTR_PRINTF_PARAM(const char* fmt), ...);

////////////////////////////////////////////////////////////
// Assertion
//
// These assertions are always enabled, the condition is always checked. An
// error is logged on failure, followed by a call to onAssert (if set). onAssert
// usually terminates the program; however, if execution continues, we return
// from the current function with the given return value.

#define MY_ASSERT1(cond) \
	do { \
		if (!(cond)) [[unlikely]] { \
			MY_ERROR("Assertion failed: %s", #cond); \
			if (::MY::onAssert) { \
				::MY::onAssert(#cond, MY_FILENAME, __LINE__); \
			} \
			return; \
		} \
	} while (0)

#define MY_ASSERT2(cond, ret) \
	do { \
		if (!(cond)) [[unlikely]] { \
			MY_ERROR("Assertion failed: %s", #cond); \
			if (::MY::onAssert) { \
				::MY::onAssert(#cond, MY_FILENAME, __LINE__); \
			} \
			return (ret); \
		} \
	} while (0)

#define MY_ASSERT(...) MY_EXPAND(MY_ASSERT_OVERLOAD(__VA_ARGS__, MY_ASSERT2, MY_ASSERT1)(__VA_ARGS__))
#define MY_ASSERT_OVERLOAD(_1, _2, NAME, ...) NAME

using OnAssert = void (*)(const char* condition, const char* file, long line) noexcept;
extern OnAssert onAssert;

////////////////////////////////////////////////////////////
// Logging
//
// This provides a very basic logging infrastructure where log messages are
// emitted via an onLog callback function (if set). An internal buffer is used
// for formatting log messages.
//
// The Trace severity is meant for sub-system specific logging that can be
// enabled / disabled at compile-time via the preprocessor (e.g. MY_TRACE_INPUT,
// MY_TRACE_AUDIO).
//
// Note that onLog's implementation as to cover thread-safety.

#define MY_INFO(...) MY_LOG(::MY::LogSeverity::Info, __VA_ARGS__)
#define MY_WARN(...) MY_LOG(::MY::LogSeverity::Warning, __VA_ARGS__)
#define MY_ERROR(...) MY_LOG(::MY::LogSeverity::Error, __VA_ARGS__)

// To minimize dynamic allocation, a thread-local buffer is used for formatting.
#define MY_LOG_BUFFER_SIZE 1024
extern thread_local char g_logBuffer[MY_LOG_BUFFER_SIZE];

#define MY_LOG(severity, ...) \
	do { \
		if (::MY::onLog) { \
			::MY::sFormat(::MY::g_logBuffer, __VA_ARGS__); \
			::MY::onLog(severity, ::MY::g_logBuffer, MY_FILENAME, __LINE__); \
		} \
	} while (0)

enum class LogSeverity { Trace, Info, Warning, Error };

inline constexpr char toChar(LogSeverity severity)
{
	switch (severity) {
	case LogSeverity::Trace: return 'T';
	case LogSeverity::Info: return 'I';
	case LogSeverity::Warning: return 'W';
	case LogSeverity::Error: return 'E';
	}
	return '?';
}

using OnLog = void (*)(LogSeverity, const char* message, const char* file, long line) noexcept;
extern OnLog onLog;

////////////////////////////////////////////////////////////
// Defer
//
// The defer utility allows code to be executed automatically at the end of the
// enclosing scope.

#define MY_DEFER(code) const auto MY_TEMPORARY(MY_deferer) = ::MY::Deferer([&]() { code; });

template <typename T>
struct Deferer {
	explicit Deferer(T&& function) : function_(function) {}
	~Deferer() noexcept { function_(); }
	T function_;
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
	constexpr explicit operator Vec2T<TT>() const
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

	friend constexpr auto operator<=>(Vec2T, Vec2T) = default;

	T x = 0;
	T y = 0;

	static const Vec2T Up, Down, Left, Right;
};

template <typename T>
constinit const Vec2T<T> Vec2T<T>::Up(0, -1);
template <typename T>
constinit const Vec2T<T> Vec2T<T>::Down(0, 1);
template <typename T>
constinit const Vec2T<T> Vec2T<T>::Left(-1, 0);
template <typename T>
constinit const Vec2T<T> Vec2T<T>::Right(1, 0);

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
	constexpr Span(T* data, usize size) : data(data), size(size) {}
	constexpr Span(T* begin, T* end) : data(begin), size(usize(end - begin)) { MY_ASSERT(begin <= end); }

	// Construct from a native array, deriving the statically known size.
	template <usize size>
	constexpr Span(T (&data)[size]) : data(data), size(size)
	{
	}

	// Construct from another span, assuming the underlying pointer is
	// convertible. Commonly used for Span<T> -> Span<const T>.
	template <typename TT>
	constexpr Span(Span<TT> span) : data(span.data), size(span.size)
	{
	}

	constexpr explicit operator bool() const { return !empty(); }

	constexpr bool empty() const { return size == 0; }
	constexpr usize sizeBytes() const { return sizeof(T) * size; }

	constexpr T* begin() const { return data; }
	constexpr T* end() const { return data + size; }

	constexpr Span<T> subspan(usize offset, usize subsize = usize(-1)) const
	{
		offset = min(offset, size);
		subsize = min(subsize, size - offset);
		return Span(data + offset, subsize);
	}

	constexpr Span<T> first(usize subsize) const { return subspan(0, subsize); }
	constexpr Span<T> last(usize subsize) const { return subspan(size - subsize); }

	// The subscript operator always does bounds checking. If this is not
	// desired, use .data[index] instead.
	constexpr T* operator[](usize index) const
	{
		MY_ASSERT(index < size, nullptr);
		return data + index;
	}

	constexpr T* front() const { return operator[](0); }
	constexpr T* back() const { return operator[](size - 1); }

	template <typename TT>
	constexpr Span<TT> as() const
	{
		return Span<TT>(reinterpret_cast<TT*>(data), sizeBytes() / sizeof(TT));
	}

	T* data = nullptr;
	usize size = 0;
};

template <typename T>
inline constexpr u64 hash(Span<T> span)
{
	auto bytes = span.template as<u8>();
	return hashRange(bytes.data, bytes.size);
}

////////////////////////////////////////////////////////////
// Container Utils

template <typename T>
T* relocateUninit(T* first, T* last, T* dstFirst)
{
	while (first != last) {
		std::construct_at(dstFirst, std::move(*first));
		std::destroy_at(first);
		first++;
		dstFirst++;
	}
	return dstFirst;
}

template <typename T>
T* relocateUninitBackward(T* first, T* last, T* dstLast)
{
	while (first != last) {
		last--;
		dstLast--;
		std::construct_at(dstLast, std::move(*last));
		std::destroy_at(last);
	}
	return dstLast;
}

////////////////////////////////////////////////////////////
// Allocator

struct Allocator {
	using OnAlloc = void* (*)(void* userdata, usize size, usize alignment) noexcept;
	using OnDealloc = void (*)(void* userdata, void* ptr) noexcept;

	constexpr Allocator(OnAlloc onAlloc, OnDealloc onDealloc, void* userdata)
	    : onAlloc_(onAlloc), onDealloc_(onDealloc), userdata_(userdata)
	{
	}

	void* alloc(usize size, usize alignment = 1) const noexcept { return onAlloc_(userdata_, size, alignment); }
	void dealloc(void* ptr) const noexcept { onDealloc_(userdata_, ptr); }

	OnAlloc onAlloc_ = nullptr;
	OnDealloc onDealloc_ = nullptr;
	void* userdata_ = nullptr;
};

extern Allocator g_defaultAllocator;

////////////////////////////////////////////////////////////
// Fixed Vector
//
// Note on constexpr: it's impossible to use FixedVector in a constexpr context
// as reinterpret_cast is not allowed in a constexpr context. reinterpret_cast
// is essential for the implementation.

template <typename T, usize Capacity>
struct FixedVector {
	FixedVector() noexcept = default;
	FixedVector(Span<T> span) { assignSpan(span); }
	FixedVector(Span<const T> span) { assignSpan(span); }
	FixedVector(std::initializer_list<T> init) { assignRange(init.begin(), init.end()); }

	~FixedVector() noexcept { clear(); }

	FixedVector(const FixedVector& other)
	    requires(std::is_copy_constructible_v<T>)
	{
		std::uninitialized_copy(other.begin(), other.end(), begin());
		size_ = other.size_;
	}

	FixedVector& operator=(const FixedVector& other)
	    requires(std::is_copy_constructible_v<T>)
	{
		if (&other != this) {
			clear();
			std::uninitialized_copy(other.begin(), other.end(), begin());
			size_ = other.size_;
		}
		return *this;
	}

	FixedVector(FixedVector&& other) noexcept
	    requires(std::is_nothrow_move_constructible_v<T>)
	{
		std::uninitialized_move(other.begin(), other.end(), begin());
		size_ = other.size_;
		other.clear();
	}

	FixedVector& operator=(FixedVector&& other) noexcept
	    requires(std::is_nothrow_move_constructible_v<T>)
	{
		if (&other != this) {
			clear();
			std::uninitialized_move(other.begin(), other.end(), begin());
			size_ = other.size_;
			other.clear();
		}
		return *this;
	}

	usize size() const { return size_; }
	usize sizeBytes() const { return sizeof(T) * size_; }
	usize capacity() const { return Capacity; }

	bool empty() const { return size_ == 0; }
	bool full() const { return size_ == Capacity; }

	T* data() { return reinterpret_cast<T*>(data_); }
	const T* data() const { return reinterpret_cast<const T*>(data_); }

	T* begin() { return data(); }
	const T* begin() const { return data(); }
	T* end() { return data() + size_; }
	const T* end() const { return data() + size_; }

	T* operator[](usize index)
	{
		MY_ASSERT(index < size_, nullptr);
		return data() + index;
	}
	const T* operator[](usize index) const
	{
		MY_ASSERT(index < size_, nullptr);
		return data() + index;
	}

	T* front() { return operator[](0); }
	const T* front() const { return operator[](0); }
	T* back() { return operator[](size_ - 1); }
	const T* back() const { return operator[](size_ - 1); }

	template <typename... Args>
	void emplace(T* pos, Args&&... args)
	{
		MY_ASSERT(!full());
		MY_ASSERT(begin() <= pos && pos <= end());
		relocateUninitBackward(pos, pos + 1, end() + 1);
		std::construct_at(pos, std::forward<Args>(args)...);
		size_++;
	}

	void insert(T* pos, const T& v) { emplace(pos, v); }

	template <typename It>
	void insertRange(T* pos, It first, It last)
	{
		usize insertSize = std::distance(first, last);
		MY_ASSERT(insertSize <= Capacity - size_);
		MY_ASSERT(begin() <= pos && pos <= end());
		relocateUninitBackward(pos, pos + insertSize, end() + insertSize);
		std::uninitialized_copy(first, last, pos);
		size_ += insertSize;
	}

	void insertSpan(T* pos, Span<T> span) { insertRange(pos, span.begin(), span.end()); }
	void insertSpan(T* pos, Span<const T> span) { insertRange(pos, span.begin(), span.end()); }

	void append(const T& v) { insert(end(), v); }

	template <typename... Args>
	void appendEmplace(Args&&... args)
	{
		emplace(end(), std::forward<Args>(args)...);
	}

	template <typename It>
	void appendRange(It first, It last)
	{
		insertRange(end(), first, last);
	}

	void appendSpan(Span<T> span) { insertRange(end(), span.begin(), span.end()); }
	void appendSpan(Span<const T> span) { insertRange(end(), span.begin(), span.end()); }

	void prepend(const T& v) { insert(begin(), v); }

	template <typename... Args>
	void prependEmplace(Args&&... args)
	{
		emplace(begin(), std::forward<Args>(args)...);
	}

	void prependSpan(Span<T> span) { insertRange(begin(), span.begin(), span.end()); }
	void prependSpan(Span<const T> span) { insertRange(begin(), span.begin(), span.end()); }

	template <typename It>
	void assignRange(It first, It last)
	{
		usize assignSize = std::distance(first, last);
		MY_ASSERT(assignSize <= Capacity);
		clear();
		std::uninitialized_copy(first, last, begin());
		size_ = assignSize;
	}

	void assignSpan(Span<const T> span) { assignRange(span.begin(), span.end()); }

	void resize(usize newSize)
	{
		MY_ASSERT(newSize <= Capacity);
		if (newSize < size_)
			removeRange(begin() + newSize, end());
		else
			std::uninitialized_default_construct(end(), end() + newSize);
		size_ = newSize;
	}

	void resizeWith(usize newSize, const T& v)
	{
		MY_ASSERT(newSize <= Capacity);
		if (newSize < size_)
			removeRange(begin() + newSize, end());
		else
			std::uninitialized_fill(end(), end() + newSize, v);
		size_ = newSize;
	}

	void remove(T* pos) { removeRange(pos, pos + 1); }

	void removeRange(T* first, T* last)
	{
		MY_ASSERT(first <= last);
		MY_ASSERT(begin() <= first && first <= end());
		MY_ASSERT(begin() <= last && last <= end());
		T* rem = relocateUninit(last, end(), first);
		std::destroy(rem, end());
		size_ -= last - first;
	}

	void clear()
	{
		std::destroy(begin(), end());
		size_ = 0;
	}

	operator Span<T>() { return Span(begin(), end()); }
	operator Span<const T>() const { return Span(begin(), end()); }

	usize size_ = 0;
	alignas(T) u8 data_[Capacity * sizeof(T)] = {};
};

////////////////////////////////////////////////////////////
// String Utilities

inline constexpr i32 sCmp(const char* a, const char* b)
{
	MY_ASSERT(a && b, 0);
	while (*a && (*a == *b)) {
		a++;
		b++;
	}
	return *reinterpret_cast<const u8*>(a) - *reinterpret_cast<const u8*>(b);
}

inline constexpr bool sEq(const char* a, const char* b)
{
	return sCmp(a, b) == 0;
}

inline constexpr bool sLess(const char* a, const char* b)
{
	return sCmp(a, b) < 0;
}

////////////////////////////////////////////////////////////
// Unmanaged Storage
//
// The UnmanagedStorage wrapper is used to disable dynamic construction and
// destruction for global variables. The wrapped object is stored inline and
// needs to be created and destroyed manually.

template <typename T>
struct UnmanagedStorage {
	template <typename... Args>
	constexpr T* init(Args&&... args)
	{
		deinit();
		std::construct_at(reinterpret_cast<T*>(instance_), std::forward<Args>(args)...);
		hasInstance_ = true;
		return get();
	}

	constexpr void deinit()
	{
		if (hasInstance_) {
			hasInstance_ = false;
			std::destroy_at(reinterpret_cast<T*>(instance_));
		}
	}

	constexpr T* get()
	{
		MY_ASSERT(hasInstance_, nullptr);
		return reinterpret_cast<T*>(&instance_);
	}
	constexpr const T* get() const { const_cast<UnmanagedStorage<T>*>(this)->get(); }

	constexpr T* operator->() { return get(); }
	constexpr const T* operator->() const { return get(); }

	constexpr explicit operator bool() const { return hasInstance_; }

	u8 instance_[sizeof(T)] = {};
	bool hasInstance_ = false;
};

} // namespace MY
