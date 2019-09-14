#include <set>

#include <catch2/catch.hpp>
#include <dexode/eventbus/internal/common.h>

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

TEST_CASE("Should return unique id for each event When using Internal::event_id<Event>",
		  "[EventId]")
{
	std::set<eventbus::internal::event_id_t> unique;

	REQUIRE(unique.insert(eventbus::internal::event_id<Anonymous>()).second);
	REQUIRE_FALSE(unique.insert(eventbus::internal::event_id<Anonymous>()).second); // already there

	struct TestA
	{};

	REQUIRE(unique.insert(eventbus::internal::event_id<TestA>()).second);
	REQUIRE(unique.insert(eventbus::internal::event_id<::TestA>()).second);
	REQUIRE(unique.insert(eventbus::internal::event_id<Test::TestA>()).second);
	REQUIRE(unique.insert(eventbus::internal::event_id<Test::TestN::TestA>()).second);
}
