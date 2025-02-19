#include <my_common.hpp>

#include "catch_amalgamated.hpp"

using namespace MY;

TEST_CASE("UnmanagedStorage does not construct automatically", "[unmanaged_storage]")
{
	struct FailOnConstruction {
		FailOnConstruction() { REQUIRE(false); }
	};

	UnmanagedStorage<FailOnConstruction> storage;
}

TEST_CASE("UnmanagedStorage does not destruct automatically", "[unmanaged_storage]")
{
	struct FailOnDestruction {
		~FailOnDestruction() { REQUIRE(false); }
	};

	UnmanagedStorage<FailOnDestruction> storage;
	storage.init();
}

TEST_CASE("UnmanagedStorage allows manual con- / destruction", "[unmanaged_storage]")
{
	struct Foo {
		Foo(int* sentinel) : sentinel_(sentinel) { *sentinel = 1; }
		~Foo() { *sentinel_ = 2; }
		int* sentinel_ = nullptr;
	};

	int sentinel = 0;

	UnmanagedStorage<Foo> storage;
	REQUIRE(!storage);
	REQUIRE(sentinel == 0);

	storage.init(&sentinel);
	REQUIRE(storage);
	REQUIRE(sentinel == 1);

	storage.deinit();
	REQUIRE(!storage);
	REQUIRE(sentinel == 2);
}

TEST_CASE("UnmanagedStorage allows re-initialization", "[unmanaged_storage]")
{
	struct Foo {
		Foo(int* initCount, int* deinitCount) : deinitCount_(deinitCount) { (*initCount)++; }
		~Foo() { (*deinitCount_)++; }
		int* deinitCount_ = nullptr;
	};

	int initCount = 0;
	int deinitCount = 0;

	UnmanagedStorage<Foo> storage;

	storage.init(&initCount, &deinitCount);
	REQUIRE(storage);
	REQUIRE(initCount == 1);
	REQUIRE(deinitCount == 0);

	storage.init(&initCount, &deinitCount); // re-initialize
	REQUIRE(storage);
	REQUIRE(initCount == 2);
	REQUIRE(deinitCount == 1);

	storage.deinit();
	REQUIRE(!storage);
	REQUIRE(initCount == 2);
	REQUIRE(deinitCount == 2);
}

TEST_CASE("UnmanagedStorage get asserts", "[unmanaged_storage]")
{
	static int assertCount = 0;
	onAssert = +[](const char*, const char*, long) noexcept { assertCount++; };

	UnmanagedStorage<int> storage;

	REQUIRE(storage.get() == nullptr);
	REQUIRE(assertCount == 1);
}
