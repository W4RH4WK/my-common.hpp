#include <my_common.hpp>

#include "catch_amalgamated.hpp"

TEST_CASE("Defer is executed at end of scope", "[Defer]")
{
	int i = 0;
	{
		REQUIRE(i == 0);
		MY_DEFER(i = 1);
		REQUIRE(i == 0);
	}
	REQUIRE(i == 1);
}

TEST_CASE("Defer is executed in reverse order", "[Defer]")
{
	int i = 0;
	{
		MY_DEFER(i = 1);
		MY_DEFER(i = 2);
	}
	REQUIRE(i == 1);
}
