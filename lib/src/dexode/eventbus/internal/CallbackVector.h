#pragma once

namespace dexode::eventbus::internal
{

struct CallbackVector
{
	virtual ~CallbackVector() = default;

	virtual void remove(const int token) = 0;
};

} // namespace dexode::eventbus::internal
