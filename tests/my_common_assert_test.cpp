#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;
using namespace Catch::Matchers;

TEST_CASE("Assert returns from current function", "[Assert]")
{
	[]() {
		MY_ASSERT(false);
		REQUIRE(false);
	}();

	// with return value
	int x = []() -> int {
		MY_ASSERT(false, -1);
		REQUIRE(false);
		return 0;
	}();
	REQUIRE(x == -1);
}

TEST_CASE("Assert invoke onAssert callback", "[Assert]")
{
	static const char* lastCondition;
	onAssert = +[](const char* condition, const char*, long) noexcept { lastCondition = condition; };

	[]() { MY_ASSERT(false); }();

	REQUIRE_THAT(lastCondition, Equals("false"));
}

TEST_CASE("Assert emits a log message", "[Assert]")
{
	static const char* lastMsg;
	static LogSeverity lastSeverity;
	onLog = +[](LogSeverity severity, const char* msg, const char*, long) noexcept {
		lastMsg = msg;
		lastSeverity = severity;
	};

	[]() { MY_ASSERT(false); }();

	REQUIRE_THAT(lastMsg, Equals("Assertion failed: false"));
	REQUIRE(lastSeverity == LogSeverity::Error);
}

TEST_CASE("onAssert callback disabled in tests by default", "[Assert]")
{
	REQUIRE(onAssert == nullptr);
}
