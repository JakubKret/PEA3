#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    double getElapsedSeconds() const {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - startTime;
        return elapsed.count();
    }
};

#endif // TIMER_H