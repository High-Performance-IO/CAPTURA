#ifndef CAPTURA_CONSTANTS_H
#define CAPTURA_CONSTANTS_H

// Maximum length of a single log message (in bytes).
#ifndef CAPIO_LOG_MAX_MSG_LEN
#define CAPIO_LOG_MAX_MSG_LEN 4096
#endif

// Default log output directory.
#ifndef CAPIO_DEFAULT_LOG_FOLDER
#define CAPIO_DEFAULT_LOG_FOLDER "./captura_logs"
#endif

// Default filename prefix for POSIX/syscall log files.
#ifndef CAPIO_LOG_POSIX_DEFAULT_LOG_FILE_PREFIX
#define CAPIO_LOG_POSIX_DEFAULT_LOG_FILE_PREFIX "posix_"
#endif

// Default filename prefix for STL/server log files.
#ifndef CAPIO_SERVER_DEFAULT_LOG_FILE_PREFIX
#define CAPIO_SERVER_DEFAULT_LOG_FILE_PREFIX "server_"
#endif

// Pre-message format string passed to printFormatted for LOG() entries.
#ifndef CAPIO_LOG_PRE_MSG
#define CAPIO_LOG_PRE_MSG "%s"
#endif

#endif // CAPTURA_CONSTANTS_H