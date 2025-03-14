#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;
using namespace Catch::Matchers;

TEST_CASE("FixedString default init", "[FixedString]")
{
	FixedString<16> s1;
	REQUIRE(s1.empty());
	REQUIRE(s1.size() == 0);
	REQUIRE_THAT(s1.data(), Equals(""));
	REQUIRE_THAT(s1.c_str(), Equals(""));
}

TEST_CASE("FixedString init", "[FixedString]")
{
	FixedString<16> s1 = "0123";
	REQUIRE(!s1.empty());
	REQUIRE(s1.size() == 4);
	REQUIRE_THAT(s1.data(), Equals("0123"));
	REQUIRE_THAT(s1.c_str(), Equals("0123"));

	FixedString<16> s2("0123");
	REQUIRE(!s2.empty());
	REQUIRE(s2.size() == 4);
	REQUIRE_THAT(s2.data(), Equals("0123"));
	REQUIRE_THAT(s2.c_str(), Equals("0123"));
}

TEST_CASE("FixedString assign", "[FixedString]")
{
	FixedString<16> s1;
	s1 = "0123";
	REQUIRE(!s1.empty());
	REQUIRE(s1.size() == 4);
	REQUIRE_THAT(s1.data(), Equals("0123"));
	REQUIRE_THAT(s1.c_str(), Equals("0123"));
}

TEST_CASE("FixedString self assign", "[FixedString]")
{
	FixedString<16> s1 = "0123";
	s1 = s1;
	REQUIRE(!s1.empty());
	REQUIRE(s1.size() == 4);
	REQUIRE_THAT(s1.data(), Equals("0123"));
	REQUIRE_THAT(s1.c_str(), Equals("0123"));
}

TEST_CASE("FixedString init oversized", "[FixedString]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	FixedString<8> s1 = "0123456789";
	REQUIRE(assertCount == 1);
	REQUIRE(s1.empty());
	REQUIRE_THAT(s1.data(), Equals(""));
	REQUIRE_THAT(s1.c_str(), Equals(""));
}

TEST_CASE("FixedString clear", "[FixedString]")
{
	FixedString<16> s1 = "0123";
	s1.clear();
	REQUIRE(s1.empty());
	REQUIRE_THAT(s1.data(), Equals(""));
	REQUIRE_THAT(s1.c_str(), Equals(""));
}

TEST_CASE("FixedString full", "[FixedString]")
{
	FixedString<8> s1 = "0123456";
	REQUIRE(s1.full());
}

TEST_CASE("FixedString begin-end", "[FixedString]")
{
	FixedString<8> s1 = "0123";
	auto begin = s1.begin();
	auto end = s1.end();

	REQUIRE(*begin++ == '0');
	REQUIRE(*begin++ == '1');
	REQUIRE(*begin++ == '2');
	REQUIRE(*begin++ == '3');
	REQUIRE(begin == end);
}

TEST_CASE("FixedString subscript", "[FixedString]")
{
	FixedString<8> s1 = "0123";
	REQUIRE(*s1[0] == '0');
	REQUIRE(*s1[1] == '1');
	REQUIRE(*s1[2] == '2');
	REQUIRE(*s1[3] == '3');
}

TEST_CASE("FixedString subscript out-of-bounds", "[FixedString]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	FixedString<8> s1 = "0123";
	REQUIRE(s1[4] == nullptr);
	REQUIRE(assertCount == 1);
}

constinit const FixedString<8> SomeFixedString1;
constinit const FixedString<8> SomeFixedString2 = "0123";
constinit const FixedString<8> SomeFixedString3("0123");
