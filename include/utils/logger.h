
#ifndef LOGGER_H
#define LOGGER_H
#include <glog/logging.h>
#include <sys/stat.h>

class Logger {
public:
    Logger();
    ~Logger();
    void InitLogging();
    void Flush();
private:
    void _SetupLogging(const std::string& logDir);
    void _MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* data);
};

#endif //LOGGER_H