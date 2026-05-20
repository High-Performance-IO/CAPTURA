#ifndef CAPTURA_BASELOGGER_H
#define CAPTURA_BASELOGGER_H

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cxxabi.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <type_traits>
#include <unistd.h>

#include "constants.h"

// -------------------------------------------------------------------------
//  Utilities
// -------------------------------------------------------------------------

template <typename T> std::string demangled_name(const T &obj) {
    int status;
    const char *mangled = typeid(obj).name();
    std::unique_ptr<char, void (*)(void *)> demangled(
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status), std::free);
    return status == 0 ? demangled.get() : mangled;
}

// Defined in BaseLogger.cpp
extern bool continue_on_error;

void raise_termination(bool raise_exception, const std::string &message);

long long current_time_in_millis();

#ifdef __CAPTURA_POSIX
// POSIX interceptor: logging starts disabled and is enabled explicitly
// after setup to prevent re-entrance into the interceptor itself.
inline thread_local bool enable_logger = false;
#else
// Server / non-POSIX: logging is always active from the first call.
inline thread_local bool enable_logger = true;
#endif

class SyscallLoggingSuspender {
  public:
    SyscallLoggingSuspender() { enable_logger = false; }
    ~SyscallLoggingSuspender() { enable_logger = true; }
};

template <typename Adapter> class TemplateLogger {
    static thread_local int current_log_level;

    char invoker[256]{0};
    char file[256]{0};
    unsigned int line{0};
    long int tid{0};

    Adapter adapter;

  public:
    TemplateLogger(const char invoker[], const char file[], unsigned int line, long int tid,
                   const char *message, ...) {
        this->tid  = tid;
        this->line = line;
        strncpy(this->invoker, invoker, sizeof(this->invoker) - 1);
        strncpy(this->file, file, sizeof(this->file) - 1);

        if (!enable_logger) {
            return;
        }

        current_log_level++;

        va_list argp;
        va_start(argp, message);
        if (current_log_level == 1) {
            adapter.writeOpening(current_time_in_millis(), this->invoker, this->file, this->line,
                                 message, argp);
        } else {
            adapter.printFormatted(current_time_in_millis(), this->invoker, this->file, this->line,
                                   CAPIO_LOG_PRE_MSG, message, argp);
        }
        va_end(argp);
    }

    TemplateLogger(const TemplateLogger &) = delete;

    TemplateLogger &operator=(const TemplateLogger &) = delete;

    ~TemplateLogger() {
        if (!enable_logger) {
            return;
        }

        if (current_log_level == 1) {
            adapter.writeEpilogue(static_cast<unsigned long>(current_time_in_millis()));
        }
        current_log_level--;
    }

    void log(const char *message, ...) {
        if (!enable_logger) {
            return;
        }

        va_list argp;
        va_start(argp, message);
        adapter.printFormatted(current_time_in_millis(), this->invoker, this->file, this->line,
                               CAPIO_LOG_PRE_MSG, message, argp);
        va_end(argp);
    }

    std::string getLogFileName() { return adapter.getLogFileName(); }

    static void reset_log_level() { current_log_level = 0; }
};

template <typename T> inline thread_local int TemplateLogger<T>::current_log_level = 0;

#ifdef CAPTURA_LOG

#define ERR_EXIT_EXCEPT_CHOICE(raise_exception, message, ...)                                      \
    log.log(message, ##__VA_ARGS__);                                                               \
    if (!continue_on_error) {                                                                      \
        char err_msg[CAPIO_LOG_MAX_MSG_LEN];                                                       \
        snprintf(err_msg, CAPIO_LOG_MAX_MSG_LEN, message, ##__VA_ARGS__);                          \
        raise_termination(raise_exception, err_msg);                                               \
    }
#define ERR_EXIT(message, ...) ERR_EXIT_EXCEPT_CHOICE(true, message, ##__VA_ARGS__)

#else // CAPTURA_LOG not defined — zero-cost stubs

#define ERR_EXIT_EXCEPT_CHOICE(raise_exception, message, ...)                                      \
    if (!continue_on_error) {                                                                      \
        char err_msg[CAPIO_LOG_MAX_MSG_LEN];                                                       \
        snprintf(err_msg, sizeof(err_msg), message, ##__VA_ARGS__);                                \
        raise_termination(raise_exception, err_msg);                                               \
    }
#define ERR_EXIT(message, ...) ERR_EXIT_EXCEPT_CHOICE(true, message, ##__VA_ARGS__)

#endif

#endif // CAPTURA_BASELOGGER_H
