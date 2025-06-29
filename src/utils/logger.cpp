
#include "utils/logger.h"
#include <iostream>

Logger::Logger()
{
    std::cout << "Logger" << std::endl;
    InitLogging();
}

Logger::~Logger()
{
    std::cout << "~Logger" << std::endl;
    Flush();
    google::ShutdownGoogleLogging();
}

void Logger::Flush()
{
    google::FlushLogFiles(google::INFO);
}

//初始化日志
void Logger::InitLogging()
{
    // 设置日志目录
    std::string logDir = "./logs/";

    // 初始化日志
    _SetupLogging(logDir);

    // 初始化 Glog
    google::InitGoogleLogging(logDir.c_str());
    // google::InstallPrefixFormatter(_MyPrefixFormatter);
}

void Logger::_SetupLogging(const std::string& logDir)
{
    struct stat info;
    if (stat(logDir.c_str(), &info) != 0 || S_ISDIR(info.st_mode) == 0) {
        mkdir(logDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    // 设置日志输出目录
    FLAGS_log_dir = logDir;

    // 设置日志级别
    FLAGS_stderrthreshold = google::INFO;

    // 设置日志文件大小为10MB
    FLAGS_max_log_size = 10 * 1024;  // 10MB

    // 禁用日志文件名中的机器名后缀
    FLAGS_alsologtostderr = true;
    FLAGS_logtostderr = false;

    // 将所有 INFO 日志重定向到固定文件
    google::SetLogDestination(google::INFO, (logDir + "cpp_info.log").c_str());
    // 自动移除旧日志 day （apt旧版本没有）
    // google::EnableLogCleaner(3);
}

void Logger::_MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* data) {
    // s << google::GetLogSeverityName(m.severity())[0]
    //     << setw(4) << 1900 + m.time().year()
    //     << setw(2) << 1 + m.time().month()
    //     << setw(2) << m.time().day()
    //     << ' '
    //     << setw(2) << m.time().hour() << ':'
    //     << setw(2) << m.time().min()  << ':'
    //     << setw(2) << m.time().sec() << "."
    //     << setw(6) << m.time().usec()
    //     << ' '
    //     << setfill(' ') << setw(5)
    //     << m.thread_id() << setfill('0')
    //     << ' '
    //     << m.basename() << ':' << m.line() << "]";
}