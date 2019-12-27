#include <set>

#include <catch2/catch.hpp>

#include "dexode/eventbus/internal/event_id.hpp"

using namespace dexode;

namespace
{
struct Anonymous
{};
} // namespace

struct TestA
{
	int a;
};

namespace Test
{
struct TestA
{
	bool b;
};

namespace TestN
{
struct TestA
{
	long long c;
};

} // namespace TestN

} // namespace Test

namespace dexode::eventbus::test
{

TEST_CASE("Should return unique id for each event When using event_id<Event>", "[EventID]")
{
	std::set<eventbus::internal::event_id_t> unique;

	REQUIRE(unique.insert(internal::event_id<Anonymous>()).second);
	REQUIRE_FALSE(unique.insert(internal::event_id<Anonymous>()).second); // already there

	struct TestA // "name collision" but not quite collision
	{};

	REQUIRE(unique.insert(internal::event_id<TestA>()).second);
	REQUIRE(unique.insert(internal::event_id<::TestA>()).second);
	REQUIRE(unique.insert(internal::event_id<Test::TestA>()).second);
	REQUIRE(unique.insert(internal::event_id<Test::TestN::TestA>()).second);
}

} // namespace dexode::eventbus::test
