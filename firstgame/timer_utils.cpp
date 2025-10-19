#include "timer_utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace {
	struct TimerData {
		LARGE_INTEGER frequency;
		HANDLE timer;
	};
	thread_local TimerData timer_data = [] {
		TimerData data;
		QueryPerformanceFrequency(&data.frequency);
		data.timer = CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
		return data;
	}();
}

void nanosleep(long long ns) {
	LARGE_INTEGER start_time{}, current_time{};
	QueryPerformanceCounter(&start_time);
	auto intervals = ns / 500'000; // 0.5ms intervals
	intervals = max(intervals - 1, 0LL) * 5'000; // 100ns intervals
	if (intervals != 0) {
		LARGE_INTEGER dueTime;
		dueTime.QuadPart = -intervals;
		SetWaitableTimer(timer_data.timer, &dueTime, 0, nullptr, nullptr, FALSE);
		WaitForSingleObject(timer_data.timer, INFINITE);
	}
	// spin-wait
	while (true) {
		QueryPerformanceCounter(&current_time);
		auto elapsed_ns = (current_time.QuadPart - start_time.QuadPart) * (1'000'000'000 / timer_data.frequency.QuadPart);
		if (elapsed_ns >= ns) break;
		_mm_pause();

	}
}

unsigned long long get_time_ns() {
	LARGE_INTEGER current_time{};
	QueryPerformanceCounter(&current_time);
	return current_time.QuadPart * (1'000'000'000 / timer_data.frequency.QuadPart);
}