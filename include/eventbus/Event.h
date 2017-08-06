#pragma once

#include <functional>
#include <ostream>

namespace Dexode
{

template<typename ... Args>
class Event
{
public:
	using Callback = std::function<void(Args...)>;

	constexpr explicit Event(const std::string& name)
			: _key{std::hash<std::string>{}(name + typeid(Callback).name())}
			, _name{name}
	{
	}

	constexpr Event(const Event& other)
			: _key{other._key}
			, _name{other._name}
	{
	}

	Event(Event&& other)
			: _key{other._key}
			, _name{other._name}
	{
	}

	Event& operator=(Event&&) = delete;

	Event& operator=(const Event&) = delete;

	const size_t getKey() const
	{
		return _key;
	}

	const std::string& getName() const
	{
		return _name;
	}

	bool operator==(const Event& rhs) const
	{
		return _key == rhs._key &&
			   _name == rhs._name;
	}

	bool operator!=(const Event& rhs) const
	{
		return !(rhs == *this);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Event& notification)
	{
		stream << "Event{name: " << notification._name << " key: " << notification._key;
		return stream;
	}

private:
	const std::size_t _key;
	const std::string _name;
};

template<typename EventType>
struct event_traits;

template<typename ... Args>
struct event_traits<Event<Args...>>
{
	using callback_type = typename std::function<void(Args...)>;
};

}
