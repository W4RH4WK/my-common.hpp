#include <my_common.hpp>

#include "catch_amalgamated.hpp"

class TestRunListener : public Catch::EventListenerBase {
  public:
	using Catch::EventListenerBase::EventListenerBase;

	void testCaseStarting(Catch::TestCaseInfo const&) override
	{
		MY::onAssert = nullptr;
		MY::onLog = nullptr;
	}
};
CATCH_REGISTER_LISTENER(TestRunListener)
