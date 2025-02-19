#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;

TEST_CASE("Slice default constructor", "[slice]")
{
	Slice<int> s;
	REQUIRE(s.data == nullptr);
	REQUIRE(s.count == 0);
	REQUIRE(s.empty());
	REQUIRE(!s);
}

TEST_CASE("Slice constructor with data and count", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr, 5);
	REQUIRE(s.data == arr);
	REQUIRE(s.count == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Slice constructor with begin and end", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr, arr + 5);
	REQUIRE(s.data == arr);
	REQUIRE(s.count == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Slice constructor with native array", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	REQUIRE(s.data == arr);
	REQUIRE(s.count == 5);
	REQUIRE(!s.empty());
	REQUIRE(s);
}

TEST_CASE("Slice constructor with another span", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s1(arr);
	Slice<const int> s2(s1);
	REQUIRE(s2.data == arr);
	REQUIRE(s2.count == 5);
	REQUIRE(!s2.empty());
	REQUIRE(s2);
}

TEST_CASE("Slice operator[]", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	REQUIRE(*s[0] == 1);
	REQUIRE(*s[1] == 2);
	REQUIRE(*s[2] == 3);
	REQUIRE(*s[3] == 4);
	REQUIRE(*s[4] == 5);
}

TEST_CASE("Slice operator[] asserts", "[slice]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	REQUIRE(s[5] == nullptr);
	REQUIRE(assertCount == 1);
}

TEST_CASE("Slice front and back", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	REQUIRE(*s.front() == 1);
	REQUIRE(*s.back() == 5);
}

TEST_CASE("Slice front and back asserts", "[slice]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	Slice<int> s;
	REQUIRE(s.front() == nullptr);
	REQUIRE(assertCount == 1);
	REQUIRE(s.back() == nullptr);
	REQUIRE(assertCount == 2);
}

TEST_CASE("Slice slice function", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	auto subSlice = s.slice(1, 3);
	REQUIRE(subSlice.count == 3);
	REQUIRE(*subSlice[0] == 2);
	REQUIRE(*subSlice[1] == 3);
	REQUIRE(*subSlice[2] == 4);
}

TEST_CASE("Slice slice function out of bounds", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);

	SECTION("Offset out of bounds")
	{
		auto subSlice = s.slice(10, 3);
		REQUIRE(subSlice.count == 0);
		REQUIRE(subSlice.data == arr + 5);
	}

	SECTION("Count out of bounds")
	{
		auto subSlice = s.slice(1, 10);
		REQUIRE(subSlice.count == 4);
		REQUIRE(*subSlice[0] == 2);
		REQUIRE(*subSlice[1] == 3);
		REQUIRE(*subSlice[2] == 4);
		REQUIRE(*subSlice[3] == 5);
	}

	SECTION("Offset and count out of bounds")
	{
		auto subSlice = s.slice(10, 10);
		REQUIRE(subSlice.count == 0);
		REQUIRE(subSlice.data == arr + 5);
	}
}

TEST_CASE("Slice first and last", "[slice]")
{
	int arr[5] = {1, 2, 3, 4, 5};
	Slice<int> s(arr);
	auto firstSlice = s.first(3);
	REQUIRE(firstSlice.count == 3);
	REQUIRE(*firstSlice[0] == 1);
	REQUIRE(*firstSlice[1] == 2);
	REQUIRE(*firstSlice[2] == 3);

	auto lastSlice = s.last(3);
	REQUIRE(lastSlice.count == 3);
	REQUIRE(*lastSlice[0] == 3);
	REQUIRE(*lastSlice[1] == 4);
	REQUIRE(*lastSlice[2] == 5);
}

TEST_CASE("Slice reinterpretation", "[slice]")
{
	int arr[4] = {1, 2, 3, 4};
	Slice<int> s(arr);
	auto byteSlice = s.as<u8>();
	REQUIRE(byteSlice.count == 16);
	REQUIRE(reinterpret_cast<int*>(byteSlice.data) == arr);
}

TEST_CASE("Slice reinterpretation bigger stride", "[slice]")
{
	u8 bytes[] = {1, 2, 3, 4, 5, 6, 7};
	Slice<u8> s = bytes;

	auto ints = s.as<u32>();
	REQUIRE(ints.count == 1);
}
