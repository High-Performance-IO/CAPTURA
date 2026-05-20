#ifndef CAPTURA_SYSCALLLOGGER_H
#define CAPTURA_SYSCALLLOGGER_H

#include <climits>
#include <cstdlib>
#include <cstring>
#include <sys/syscall.h>
#include <unistd.h>

#include "BaseLogger.h"
#include "JsonBaseLogger.h"

struct SyscallLogger : JsonLogBase<SyscallLogger> {

    static thread_local int fileFD;
    static thread_local char filePath[PATH_MAX];

    using SyscallFn = long (*)(long, ...);
    static SyscallFn syscallFn;

    static void setSyscallFn(SyscallFn fn) { syscallFn = fn; }
    explicit SyscallLogger();

    static void rawWriteBytes(const char *buf, int len);
    static void rawWriteStr(const char *buf);

    [[nodiscard]] static std::string getLogFileName();

  private:
    static void ensureFileOpen();

    static const char *getHostname();
    static const char *getLogDir();
    static const char *getLogPrefix();
    static const char *getSyscallLogDir();
    static const char *getHostLogDir();
};

inline thread_local int SyscallLogger::fileFD              = -1;
inline thread_local char SyscallLogger::filePath[PATH_MAX] = {'\0'};

extern SyscallLogger::SyscallFn _captura_syscall_fn_storage;
inline SyscallLogger::SyscallFn SyscallLogger::syscallFn = ::syscall;

using Logger = TemplateLogger<SyscallLogger>;

#ifdef CAPTURA_LOG

#define LOG(message, ...) log.log(message, ##__VA_ARGS__)

#define START_LOG(tid, message, ...)                                                               \
    Logger log(__func__, __FILE__, __LINE__, tid, message, ##__VA_ARGS__)

#define ENABLE_LOGGER() enable_logger = true
#define DISABLE_LOGGER()                                                                           \
    SyscallLoggingSuspender sls {}

#define DBG(tid, lambda)                                                                           \
    {                                                                                              \
        START_LOG(tid, "[  DBG  ]~~~ START ~~~[  DBG  ]");                                         \
        lambda;                                                                                    \
        LOG("[  DBG  ]~~~ END   ~~~[  DBG  ]");                                                    \
    }

#else // zero-cost stubs

#define LOG(message, ...)
#define START_LOG(tid, message, ...)
#define DBG(tid, lambda)
#define ENABLE_LOGGER()
#define DISABLE_LOGGER()

#endif // CAPTURA_LOG

#endif // CAPTURA_SYSCALLLOGGER_H