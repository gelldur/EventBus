//
// Created by gelldur on 14.06.19.
//
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include <benchmark/benchmark.h>

#include <eventbus/AsyncEventBus.h>
#include <eventbus/TokenHolder.h>

namespace
{
struct SimpleEvent
{
	std::int64_t value = 0;
};

Dexode::AsyncEventBus bus;

} // namespace

void checkFor(benchmark::State& state)
{
	if(state.thread_index == 0)
	{
		Dexode::TokenHolder<Dexode::AsyncEventBus> listener {&bus};
		std::uint64_t consumed = 0;
		listener.listen<SimpleEvent>(
			[&consumed](const auto& event) { benchmark::DoNotOptimize(consumed += 1); });

		for(auto _ : state)
		{
			//if(bus.wait())
			{
				bus.consume();
			}
		}
		state.counters["consumed"] = consumed;
	}
	else
	{
		for(auto _ : state)
		{
			bus.schedule(SimpleEvent {std::chrono::steady_clock::now().time_since_epoch().count()});
		}
	}
}

BENCHMARK(checkFor)->Threads(2)->MinTime(1)->MeasureProcessCPUTime();
BENCHMARK(checkFor)->Threads(5)->MinTime(1)->MeasureProcessCPUTime();
BENCHMARK(checkFor)->Threads(10)->MinTime(1)->MeasureProcessCPUTime();
