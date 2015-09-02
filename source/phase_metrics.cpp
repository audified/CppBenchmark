/*!
    \file phase_metrics.cpp
    \brief Benchmark phase metrics implementation
    \author Ivan Shynkarenka
    \date 03.07.2015
    \copyright MIT License
*/

#include "phase_metrics.h"
#include "system.h"

namespace CppBenchmark {

int64_t PhaseMetrics::avg_time() const noexcept
{
    return (_total_iterations > 0) ? (_total_time / _total_iterations) : 0;
}

int64_t PhaseMetrics::min_time() const noexcept
{
    return (_total_iterations > 0) ? (_min_time / _total_iterations) : 0;
}

int64_t PhaseMetrics::max_time() const noexcept
{
    return (_total_iterations > 0) ? (_max_time / _total_iterations) : 0;
}

int64_t PhaseMetrics::iterations_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    return (_total_iterations * 1000000000) / _total_time;
}

int64_t PhaseMetrics::items_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    return (_total_items * 1000000000) / _total_time;
}

int64_t PhaseMetrics::bytes_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    return (_total_bytes * 1000000000) / _total_time;
}

void PhaseMetrics::StartCollecting() noexcept
{
    _timestamp = System::Timestamp();
}

void PhaseMetrics::StopCollecting() noexcept
{
    // Get iteration duration
    int64_t duration = System::Timestamp() - _timestamp;

    // Limitation of the Unix environment
    if (duration > 143838832099148626)
        duration = 143838832099148626;

    // Update time counters
    if (duration < _min_time)
        _min_time = duration;
    if (duration > _max_time)
        _max_time = duration;
    _total_time += duration;
}

void PhaseMetrics::MergeMetrics(const PhaseMetrics& metrics)
{
    // Choose best min time
    if (metrics._min_time < _min_time)
        _min_time = metrics._min_time;

    // Choose best max time
    if (metrics._max_time > _max_time)
        _max_time = metrics._max_time;

    // Limitation of the Unix environment
    if (metrics._max_time > 143838832099148626)
        _max_time = 143838832099148626;

    // Merge custom hash tables
    _custom_int.insert(metrics._custom_int.begin(), metrics._custom_int.end());
    _custom_str.insert(metrics._custom_str.begin(), metrics._custom_str.end());

    // Choose best total time with iterations, items and bytes
    if (metrics._total_time < _total_time) {
        _total_time = metrics._total_time;
        _total_iterations = metrics._total_iterations;
        _total_items = metrics._total_items;
        _total_bytes = metrics._total_bytes;
        // Overwrite metrics custom hash tables
        for (auto& it : metrics._custom_int)
            _custom_int[it.first] = it.second;
        for (auto& it : metrics._custom_str)
            _custom_str[it.first] = it.second;
    }
}

} // namespace CppBenchmark
