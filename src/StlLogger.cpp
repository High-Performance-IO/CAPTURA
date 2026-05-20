#include "StlLogger.h"

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <climits>


StlLogger::StlLogger() { ensureFileOpen(); }


void StlLogger::rawWriteBytes(const char *buf, int len) {
    ensureFileOpen();
    logfile->write(buf, len);
    logfile->flush();
}

void StlLogger::rawWriteStr(const char *buf) {
    rawWriteBytes(buf, static_cast<int>(::strlen(buf)));
}

std::string StlLogger::getLogFileName() {
    return logFileName ? *logFileName : std::string{};
}


void StlLogger::ensureFileOpen() {
    if (logfile != nullptr && logfile->is_open()) { return; }

    std::string logDir;
    std::string prefix;

    if (const char *env = std::getenv("CAPTURA_LOG_DIR"); env != nullptr) {
        logDir = env;
    } else {
        logDir = CAPIO_DEFAULT_LOG_FOLDER;
    }

    if (const char *env = std::getenv("CAPTURA_LOG_PREFIX"); env != nullptr) {
        prefix = env;
    } else {
        prefix = CAPIO_SERVER_DEFAULT_LOG_FILE_PREFIX;
    }

    char hostname[HOST_NAME_MAX];
    ::gethostname(hostname, HOST_NAME_MAX);

    const std::filesystem::path outputFolder{logDir + "/stl/" + hostname};
    std::filesystem::create_directories(outputFolder);

    const std::filesystem::path path =
            outputFolder / (prefix + std::to_string(gettid()) + ".json");

    logfile = new std::ofstream(path, std::ofstream::app);
    logFileName = new std::string(path.string());
}
