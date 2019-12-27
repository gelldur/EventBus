//
// Created by gelldur on 24.12.2019.
//
#include "WaitPerk.hpp"

namespace dexode::eventbus::perk
{

bool WaitPerk::wait()
{
	using namespace std::chrono_literals;
	std::unique_lock<std::mutex> lock(_waitMutex);
	_eventWaiting.wait(lock);

	return true;
}

bool WaitPerk::waitFor(const std::chrono::milliseconds timeout)
{
	using namespace std::chrono_literals;
	std::unique_lock<std::mutex> lock(_waitMutex);
	if(_eventWaiting.wait_for(lock, timeout) == std::cv_status::timeout)
	{
		return false;
	}
	return true;
}

Flag WaitPerk::onPostponeEvent(PostponeHelper&)
{
	_eventWaiting.notify_one();
	return Flag::postpone_continue;
}

} // namespace dexode::eventbus::perk
