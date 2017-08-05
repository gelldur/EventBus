#pragma once

#include <functional>
#include <ostream>

namespace Dexode
{

template<typename ... Args>
class Notification2
{
public:
	using Callback = std::function<void(Args...)>;

	constexpr explicit Notification2(const std::string& name)
			: _key{std::hash<std::string>{}(name + typeid(Callback).name())}
			, _name{name}
	{
	}

	constexpr Notification2(const Notification2& other)
			: _key{other._key}
			, _name{other._name}
	{
	}

	Notification2(Notification2&& other)
			: _key{other._key}
			, _name{other._name}
	{
	}

	Notification2& operator=(Notification2&&) = delete;

	Notification2& operator=(const Notification2&) = delete;

	const size_t getKey() const
	{
		return _key;
	}

	const std::string& getName() const
	{
		return _name;
	}

	bool operator==(const Notification2& rhs) const
	{
		return _key == rhs._key &&
			   _name == rhs._name;
	}

	bool operator!=(const Notification2& rhs) const
	{
		return !(rhs == *this);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Notification2& notification)
	{
		stream << "Notification{name: " << notification._name << " key: " << notification._key;
		return stream;
	}

private:
	const std::size_t _key;
	const std::string _name;
};

template<typename NotificationType>
struct notification_traits;

template<typename ... Args>
struct notification_traits<Notification2<Args...>>
{
	using callback_type = typename std::function<void(Args...)>;
};

}
