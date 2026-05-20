#ifndef CAPTURA_SYSCALLLOGGER_H
#define CAPTURA_SYSCALLLOGGER_H

#include <climits>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include "JsonBaseLogger.h"
#include "BaseLogger.h"

/**
 * Adapter that writes structured JSON log output using raw POSIX syscalls.
 * Intended for the syscall-interceptor build (__CAPTURA_POSIX) where STL
 * I/O cannot be used safely inside an intercepted context.
 *
 * All path-resolution and file-open logic lives in SyscallLogger.cpp so
 * that the syscall wrappers (capio_syscall) are compiled in only one TU.
 */
struct SyscallLogger : JsonLogBase<SyscallLogger> {
    static thread_local int fileFD;
    static thread_local char filePath[PATH_MAX];


    explicit SyscallLogger();


    static void rawWriteBytes(const char *buf, int len);

    static void rawWriteStr(const char *buf);


    std::string getLogFileName() const;

private:
    static void ensureFileOpen();


    static const char *getHostname();

    static const char *getLogDir();

    static const char *getLogPrefix();

    static const char *getSyscallLogDir();

    static const char *getHostLogDir();
};

inline thread_local int SyscallLogger::fileFD = -1;
inline thread_local char SyscallLogger::filePath[PATH_MAX] = {'\0'};

using Logger = TemplateLogger<SyscallLogger>;

#ifdef CAPTURA_LOG

#define LOG(message, ...)    log.log(message, ##__VA_ARGS__)

#define START_LOG(tid, message, ...)                                           \
    Logger log(__func__, __FILE__, __LINE__, tid, message, ##__VA_ARGS__)

#define ENABLE_LOGGER()  enable_logger = true
#define DISABLE_LOGGER() SyscallLoggingSuspender sls{}

#define DBG(tid, lambda)                                                       \
    {                                                                          \
        START_LOG(tid, "[  DBG  ]~~~ START ~~~[  DBG  ]");                     \
        lambda;                                                                \
        LOG("[  DBG  ]~~~ END   ~~~[  DBG  ]");                                \
    }

#else

#define LOG(message, ...)
#define START_LOG(tid, message, ...)
#define DBG(tid, lambda)
#define ENABLE_LOGGER()
#define DISABLE_LOGGER()

#endif

#endif // CAPTURA_SYSCALLLOGGER_H
