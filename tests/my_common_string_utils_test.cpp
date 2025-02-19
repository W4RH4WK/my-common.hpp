#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;

TEST_CASE("sCmp", "[string_utils]")
{
	REQUIRE(sCmp("abc", "abc") == 0);
	REQUIRE(sCmp("abc", "abd") < 0);
	REQUIRE(sCmp("abd", "abc") > 0);
	REQUIRE(sCmp("abc", "abcd") < 0);
	REQUIRE(sCmp("abcd", "abc") > 0);
	REQUIRE(sCmp("", "") == 0);
	REQUIRE(sCmp("a", "") > 0);
	REQUIRE(sCmp("", "a") < 0);
}

TEST_CASE("sEq", "[string_utils]")
{
	REQUIRE(sEq("abc", "abc"));
	REQUIRE_FALSE(sEq("abc", "abd"));
	REQUIRE_FALSE(sEq("abd", "abc"));
	REQUIRE_FALSE(sEq("abc", "abcd"));
	REQUIRE_FALSE(sEq("abcd", "abc"));
	REQUIRE(sEq("", ""));
	REQUIRE_FALSE(sEq("a", ""));
	REQUIRE_FALSE(sEq("", "a"));
}

TEST_CASE("sLess", "[string_utils]")
{
	REQUIRE_FALSE(sLess("abc", "abc"));
	REQUIRE(sLess("abc", "abd"));
	REQUIRE_FALSE(sLess("abd", "abc"));
	REQUIRE(sLess("abc", "abcd"));
	REQUIRE_FALSE(sLess("abcd", "abc"));
	REQUIRE_FALSE(sLess("", ""));
	REQUIRE_FALSE(sLess("a", ""));
	REQUIRE(sLess("", "a"));
}

TEST_CASE("sFormat", "[string_utils]")
{
	char buffer[64];
	auto n = sFormat(buffer, "Hello World");
	REQUIRE(n == 12);
}

TEST_CASE("sFormat exceeds buffer", "[string_utils]")
{
	char buffer[10];
	auto n = sFormat(buffer, "Hello World");
	REQUIRE(buffer[9] == '\0');
	REQUIRE(n == 10);
}
