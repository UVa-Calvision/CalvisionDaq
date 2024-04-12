#pragma once

#include <chrono>

template <typename time_unit>
class Stopwatch {
public:
    using clock = std::chrono::high_resolution_clock;
    using time = std::chrono::time_point<clock>;

    Stopwatch()
        : time_(clock::now())
    {}

    auto operator()() {
        time old_time = time_;
        time_ = clock::now();
        return std::chrono::duration_cast<time_unit>(time_ - old_time);
    }

private:
    time time_;
};
