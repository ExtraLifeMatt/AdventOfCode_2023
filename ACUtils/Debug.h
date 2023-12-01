#pragma once

#include <assert.h>
#include <cstdint>
#include <chrono>


namespace Debug
{
	enum class ACStopWatchUnits : uint8_t
	{
		Seconds = 0,
		Milliseconds,
		Nanoseconds
	};

	class ACStopWatch
	{
	public:
		ACStopWatch(ACStopWatchUnits units = ACStopWatchUnits::Milliseconds)
			: m_units(units)
		{
		}

		void Start()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		double Peek()
		{
			const std::chrono::steady_clock::duration dur = (std::chrono::high_resolution_clock::now() - m_start);
			return FormatToUnits(dur);
		}

		double Stop()
		{
			m_end = std::chrono::high_resolution_clock::now();
			return Duration();
		}

		double Duration() const
		{
			return FormatToUnits(m_end - m_start);
		}
	private:
		double FormatToUnits(const std::chrono::steady_clock::duration& dur) const
		{
			uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
			switch (m_units)
			{
			case ACStopWatchUnits::Seconds:
			{
				return (double)(ns / 1000000ULL) / 1000.0;
			}
			break;
			case ACStopWatchUnits::Milliseconds:
			{
				return (double)(ns / 1000000ULL);
			}
			default:
			case ACStopWatchUnits::Nanoseconds:
			{
				return (double)(ns);
			}
			break;
			}
		}

		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
		ACStopWatchUnits m_units;
	};

	// Debug
	void DebuggerPause();
	void CheckNoEntry();
}
