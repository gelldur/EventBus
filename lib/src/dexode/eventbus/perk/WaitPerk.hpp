//
// Created by gelldur on 24.12.2019.
//
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "Perk.hpp"

namespace dexode::eventbus
{
class PostponeHelper;
}

namespace dexode::eventbus::perk
{

class WaitPerk : public Perk
{
public:
	bool wait();
	bool waitFor(std::chrono::milliseconds timeout);

	Flag onPostponeEvent(PostponeHelper& postponeCall);

private:
	std::condition_variable _eventWaiting;
	std::mutex _waitMutex;
	std::atomic<bool> _hasEvents = false;
};

} // namespace dexode::eventbus::perk
