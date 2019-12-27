#pragma once

#include <any>
#include <cstdint>

namespace dexode::eventbus::stream
{

class EventStream
{
public:
	virtual ~EventStream() = default;

	virtual void postpone(std::any event) = 0;
	virtual std::size_t process(std::size_t limit) = 0;

	virtual bool addListener(std::uint32_t listenerID, std::any callback) = 0;
	virtual bool removeListener(std::uint32_t listenerID) = 0;
};

class NoopEventStream : public EventStream
{
public:
	void postpone(std::any event) override
	{
		throw std::runtime_error{"Noop"};
	}
	size_t process(std::size_t limit) override
	{
		throw std::runtime_error{"Noop"};
	}
	bool addListener(std::uint32_t listenerID, std::any callback) override
	{
		throw std::runtime_error{"Noop"};
	}
	bool removeListener(std::uint32_t listenerID) override
	{
		throw std::runtime_error{"Noop"};
	}
};

} // namespace dexode::eventbus::stream
