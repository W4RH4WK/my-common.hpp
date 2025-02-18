#include <my_common.hpp>

#include "catch_amalgamated.hpp"
using Catch::Approx;

namespace MY {

TEST_CASE("Vec2 initialization and conversion", "[vec2]")
{
	Vec2 v1;
	REQUIRE(v1.x == 0.0f);
	REQUIRE(v1.y == 0.0f);

	Vec2 v2(3.0f);
	REQUIRE(v2.x == 3.0f);
	REQUIRE(v2.y == 3.0f);

	Vec2 v3(2.0f, 4.0f);
	REQUIRE(v3.x == 2.0f);
	REQUIRE(v3.y == 4.0f);

	Vec2T<int> vi(2, 4);
	Vec2 v4 = static_cast<Vec2>(vi);
	REQUIRE(v4.x == 2.0f);
	REQUIRE(v4.y == 4.0f);
}

TEST_CASE("Vec2 length and lengthSq", "[vec2]")
{
	Vec2 v1(3.0f, 4.0f);
	REQUIRE(v1.lengthSq() == Approx(25.0).epsilon(0.01));
	REQUIRE(v1.length() == Approx(5.0).epsilon(0.01));
}

TEST_CASE("Vec2 ratio", "[vec2]")
{
	Vec2 v1(4.0f, 2.0f);
	REQUIRE(v1.ratio() == Approx(2.0).epsilon(0.01));
}

TEST_CASE("Vec2 normalize", "[vec2]")
{
	Vec2 v0;
	v0.normalize();
	REQUIRE(v0.length() == Approx(0.0).epsilon(0.01));

	Vec2 v1(3.0f, 4.0f);
	v1.normalize();
	REQUIRE(v1.length() == Approx(1.0).epsilon(0.01));
}

TEST_CASE("Vec2 clampLength", "[vec2]")
{
	Vec2 v1(3.0f, 4.0f);
	v1.clampLength(2.5);
	REQUIRE(v1.length() == Approx(2.5).epsilon(0.01));
}

} // namespace MY
