#include "BaseLogger.h"

bool continue_on_error = false;



void raise_termination(const bool raise_exception, const std::string &message) {
    if (raise_exception) {
        throw std::runtime_error(message);
    }
    ::write(STDERR_FILENO, message.c_str(), message.size());
    exit(EXIT_FAILURE);
}

long long current_time_in_millis() {
    timespec ts{};
    static long long start_time = -1;
    if (start_time == -1) {
        clock_gettime(CLOCK_REALTIME, &ts);
        start_time = static_cast<long long>(ts.tv_sec) * 1000
                     + ts.tv_nsec / 1000000;
    }
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<long long>(ts.tv_sec) * 1000
           + ts.tv_nsec / 1000000
           - start_time;
}