#pragma once

#include <type_traits>

namespace Dexode
{
namespace Internal
{

template <typename>
void type_id() // Helper for getting "type id"
{
}

using type_id_t = void (*)(); // Function pointer

template <class Event>
constexpr bool validateEvent()
{
	static_assert(std::is_const<Event>::value == false, "Struct must be without const");
	static_assert(std::is_volatile<Event>::value == false, "Struct must be without volatile");
	static_assert(std::is_reference<Event>::value == false, "Struct must be without reference");
	static_assert(std::is_pointer<Event>::value == false, "Struct must be without pointer");
	return true;
}

} // namespace Internal
} // namespace Dexode
