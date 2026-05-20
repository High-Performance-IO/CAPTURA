#ifndef CAPTURA_STLLOGGER_H
#define CAPTURA_STLLOGGER_H

#include <filesystem>
#include <fstream>
#include <string>

#include "BaseLogger.h"
#include "JsonBaseLogger.h"

struct StlLogger : JsonLogBase<StlLogger> {
    static thread_local std::ofstream *logfile;
    static thread_local std::string *logFileName;

    explicit StlLogger();

    static std::string getLogFileName();

    static void rawWriteBytes(const char *buf, int len);

    static void rawWriteStr(const char *buf);

  private:
    static void ensureFileOpen();
};

inline thread_local std::ofstream *StlLogger::logfile   = nullptr;
inline thread_local std::string *StlLogger::logFileName = nullptr;

using Logger = TemplateLogger<StlLogger>;

#ifdef CAPTURA_LOG

#define LOG(message, ...) log.log(message, ##__VA_ARGS__)

#define START_LOG(tid, message, ...)                                                               \
    Logger::reset_log_level();                                                                     \
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

#else

#define LOG(message, ...)
#define START_LOG(tid, message, ...)
#define DBG(tid, lambda)
#define ENABLE_LOGGER()
#define DISABLE_LOGGER()

#endif

#endif
