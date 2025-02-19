#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;
using namespace Catch::Matchers;

TEST_CASE("Log messages invoke onLog callback", "[log]")
{
	static const char* lastMsg;
	static LogSeverity lastSeverity;
	onLog = +[](LogSeverity severity, const char* msg, const char*, long) noexcept {
		lastMsg = msg;
		lastSeverity = severity;
	};

	MY_INFO("Hello World %d", 42);

	REQUIRE_THAT(lastMsg, Equals("Hello World 42"));
	REQUIRE(lastSeverity == LogSeverity::Info);
}

TEST_CASE("Trace logs should be emitted through a use case specific macro", "[log]")
{
#define MY_TRACE_INPUT(...) MY_LOG(::MY::LogSeverity::Trace, __VA_ARGS__) // <-- this one is active
#define MY_TRACE_AUDIO(...) // MY_LOG(::MY::LogSeverity::Trace, __VA_ARGS__) // <-- this one is disabled

	MY_TRACE_INPUT("Input %d", 42);
	MY_TRACE_AUDIO("Audio %d", 42);
}

TEST_CASE("onLog callback disabled in tests by default", "[log]")
{
	REQUIRE(onLog == nullptr);
}
