//
// Created by gelldur on 26.11.2019.
//
#pragma once

#include <any>
#include <limits>
#include <map>
#include <memory>
#include <shared_mutex>

#include "dexode/eventbus/Bus.hpp"

namespace dexode
{

class EventBus : public dexode::eventbus::Bus
{
	template <typename>
	friend class dexode::eventbus::internal::ListenerAttorney;

public:
	std::size_t process() override
	{
		return processLimit(std::numeric_limits<std::size_t>::max());
	}

	std::size_t processLimit(std::size_t limit);

protected:
	eventbus::stream::EventStream* obtainStream(
		eventbus::internal::event_id_t eventID,
		eventbus::CreateStreamCallback createStreamCallback);

	bool postponeEvent(eventbus::PostponeHelper& postponeCall) override;
	eventbus::stream::EventStream* findStream(eventbus::internal::event_id_t eventID) const;

	void unlistenAll(std::uint32_t listenerID) override;
	eventbus::stream::EventStream* listen(
		std::uint32_t listenerID,
		eventbus::internal::event_id_t eventID,
		eventbus::CreateStreamCallback createStreamCallback) override;
	void unlisten(std::uint32_t listenerID, eventbus::internal::event_id_t eventID) override;

private:
	mutable std::shared_mutex _mutexStreams;
	std::shared_mutex _mutexProcess;
	std::vector<std::unique_ptr<eventbus::stream::EventStream>> _eventStreams;
	std::map<eventbus::internal::event_id_t, eventbus::stream::EventStream*> _eventToStream;

	eventbus::stream::EventStream* findStreamUnsafe(eventbus::internal::event_id_t eventID) const;
};

} // namespace dexode
