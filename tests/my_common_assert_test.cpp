#include <my_common.hpp>

#include "catch_amalgamated.hpp"
using namespace Catch::Matchers;

namespace MY {

TEST_CASE("Assert returns from current function", "[assert]")
{
	[]() {
		MY_ASSERT(21 == 42);
		REQUIRE(false);
	}();

	// with return value
	int x = []() -> int {
		MY_ASSERT(21 == 42, -1);
		REQUIRE(false);
		return 0;
	}();
	REQUIRE(x == -1);
}

TEST_CASE("Assert invoke onAssert callback", "[assert]")
{
	static const char* lastCondition;
	onAssert = +[](const char* condition, const char*, long) noexcept { lastCondition = condition; };

	[]() { MY_ASSERT(21 == 42); }();

	REQUIRE_THAT(lastCondition, Equals("21 == 42"));
}

TEST_CASE("Assert emits a log message", "[assert]")
{
	static const char* lastMsg;
	static LogSeverity lastSeverity;
	onLog = +[](LogSeverity severity, const char* msg, const char*, long) noexcept {
		lastMsg = msg;
		lastSeverity = severity;
	};

	[]() { MY_ASSERT(21 == 42); }();

	REQUIRE_THAT(lastMsg, Equals("Assertion failed: 21 == 42"));
	REQUIRE(lastSeverity == LogSeverity::Error);
}

TEST_CASE("onAssert callback disabled in tests by default", "[assert]")
{
	REQUIRE(onAssert == nullptr);
}

} // namespace MY
