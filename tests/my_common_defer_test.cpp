#include <my_common.hpp>

#include "catch_amalgamated.hpp"

namespace MY {

TEST_CASE("Defer is executed at end of scope", "[defer]")
{
	int i = 0;
	{
		REQUIRE(i == 0);
		MY_DEFER(i = 1);
		REQUIRE(i == 0);
	}
	REQUIRE(i == 1);
}

TEST_CASE("Defer is executed in reverse order", "[defer]")
{
	int i = 0;
	{
		MY_DEFER(i = 1);
		MY_DEFER(i = 2);
	}
	REQUIRE(i == 1);
}

} // namespace MY
