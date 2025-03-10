#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;

TEST_CASE("Span default constructor", "[Span]")
{
	Span<int> s;
	REQUIRE(s.data == nullptr);
	REQUIRE(s.size == 0);
	REQUIRE(s.empty());
	REQUIRE(!s);
}

TEST_CASE("Span constructor with data and size", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr, 5);
	REQUIRE(s.data == arr);
	REQUIRE(s.size == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Span constructor with begin and end", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr, arr + 5);
	REQUIRE(s.data == arr);
	REQUIRE(s.size == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Span constructor with native array", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	REQUIRE(s.data == arr);
	REQUIRE(s.size == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Span constructor with another span", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s1(arr);
	Span<const int> s2(s1);
	REQUIRE(s2.data == arr);
	REQUIRE(s2.size == 5);
	REQUIRE(!s2.empty());
	REQUIRE(s2);
}

TEST_CASE("Span operator[]", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	REQUIRE(*s[0] == 1);
	REQUIRE(*s[1] == 2);
	REQUIRE(*s[2] == 3);
	REQUIRE(*s[3] == 4);
	REQUIRE(*s[4] == 5);
}

TEST_CASE("Span operator[] asserts", "[Span]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	REQUIRE(s[5] == nullptr);
	REQUIRE(assertCount == 1);
}

TEST_CASE("Span front and back", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	REQUIRE(*s.front() == 1);
	REQUIRE(*s.back() == 5);
}

TEST_CASE("Span front and back asserts", "[Span]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	Span<int> s;
	REQUIRE(s.front() == nullptr);
	REQUIRE(assertCount == 1);
	REQUIRE(s.back() == nullptr);
	REQUIRE(assertCount == 2);
}

TEST_CASE("Span Span function", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	auto subSpan = s.subspan(1, 3);
	REQUIRE(subSpan.size == 3);
	REQUIRE(*subSpan[0] == 2);
	REQUIRE(*subSpan[1] == 3);
	REQUIRE(*subSpan[2] == 4);
}

TEST_CASE("Span Span function out of bounds", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);

	SECTION("Offset out of bounds")
	{
		auto subSpan = s.subspan(10, 3);
		REQUIRE(subSpan.size == 0);
		REQUIRE(subSpan.data == arr + 5);
	}

	SECTION("size out of bounds")
	{
		auto subSpan = s.subspan(1, 10);
		REQUIRE(subSpan.size == 4);
		REQUIRE(*subSpan[0] == 2);
		REQUIRE(*subSpan[1] == 3);
		REQUIRE(*subSpan[2] == 4);
		REQUIRE(*subSpan[3] == 5);
	}

	SECTION("Offset and size out of bounds")
	{
		auto subSpan = s.subspan(10, 10);
		REQUIRE(subSpan.size == 0);
		REQUIRE(subSpan.data == arr + 5);
	}
}

TEST_CASE("Span first and last", "[Span]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Span<int> s(arr);
	auto firstSpan = s.first(3);
	REQUIRE(firstSpan.size == 3);
	REQUIRE(*firstSpan[0] == 1);
	REQUIRE(*firstSpan[1] == 2);
	REQUIRE(*firstSpan[2] == 3);

	auto lastSpan = s.last(3);
	REQUIRE(lastSpan.size == 3);
	REQUIRE(*lastSpan[0] == 3);
	REQUIRE(*lastSpan[1] == 4);
	REQUIRE(*lastSpan[2] == 5);
}

TEST_CASE("Span reinterpretation", "[Span]")
{
	int arr[4] = {1, 2, 3, 4};
	Span<int> s(arr);
	auto byteSpan = s.as<u8>();
	REQUIRE(byteSpan.size == 16);
	REQUIRE(reinterpret_cast<int*>(byteSpan.data) == arr);
}

TEST_CASE("Span reinterpretation bigger stride", "[Span]")
{
	u8 bytes[] = {1, 2, 3, 4, 5, 6, 7};
	Span<u8> s = bytes;

	auto ints = s.as<u32>();
	REQUIRE(ints.size == 1);
}
