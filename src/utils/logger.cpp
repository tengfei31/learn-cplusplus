
#include "utils/logger.h"

static void SetupLogging(const std::string& logDir)
{
    // 设置日志输出目录
    FLAGS_log_dir = logDir;
    // google::SetLogDestination(google::INFO, "./logs/my_log_");

    // 设置日志级别
    FLAGS_stderrthreshold = google::INFO;

    // 设置日志文件大小为10MB
    FLAGS_max_log_size = 10 * 1024;  // 10MB

    // 禁用日志文件名中的机器名后缀
    FLAGS_alsologtostderr = true;
    FLAGS_logtostderr = true;

    // 自动移除旧日志 day （apt旧版本没有）
    // google::EnableLogCleaner(3);
}

void MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* data) {
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

//初始化日志
void InitLogging()
{
    // 设置日志目录
    std::string logDir = "logs/";

    // 初始化日志
    SetupLogging(logDir);

    // 初始化 Glog
    google::InitGoogleLogging("logs/");
    // google::InstallPrefixFormatter(MyPrefixFormatter);
}