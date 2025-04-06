#pragma once
#include <deque>
#include <mutex>

inline const int MAX_SAMPLES = 10;

inline std::mutex delayMutex;
inline std::deque<int> delaySamples;

inline void UpdateDelaySample(int rtt_ms) {
	std::lock_guard<std::mutex> lock(delayMutex);
	delaySamples.push_back(rtt_ms);
	if (delaySamples.size() > MAX_SAMPLES)
		delaySamples.pop_front();
}

inline int GetAverageDelay() {
	std::lock_guard<std::mutex> lock(delayMutex);
	if (delaySamples.empty()) return 0;
	int sum = 0;
	for (auto d : delaySamples) sum += d;
	return sum / delaySamples.size();
}
