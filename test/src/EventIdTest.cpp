#include <catch2/catch.hpp>

#include <set>

#include <eventbus/internal/common.h>

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
	std::set<Dexode::Internal::event_id_t> unique;

	REQUIRE(unique.insert(Dexode::Internal::event_id<Anonymous>()).second);
	REQUIRE_FALSE(unique.insert(Dexode::Internal::event_id<Anonymous>()).second); //already there

	struct TestA
	{};

	REQUIRE(unique.insert(Dexode::Internal::event_id<TestA>()).second);
	REQUIRE(unique.insert(Dexode::Internal::event_id<::TestA>()).second);
	REQUIRE(unique.insert(Dexode::Internal::event_id<Test::TestA>()).second);
	REQUIRE(unique.insert(Dexode::Internal::event_id<Test::TestN::TestA>()).second);
}
