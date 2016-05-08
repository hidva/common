#include "log/pp_qq_log.h"
#include "formatter/stdio_formatter.h"

#include <string>
#include <pthread.h>
#include <unistd.h>

namespace {

const char LOG_LEVEL_CHAR[] {'U','U','V','D','I','W','E','F'};

inline char LogLevel2Char(int level)
{
    return LOG_LEVEL_CHAR[level];
}


}

void StdoutHandler(int level,const char *tag,const char */* file */,int /* line */,const char */* func */,const char */* prettyFunc */,const char *format,...) noexcept
{
    std::string log_msg;
    AppendStdioFormat(log_msg,"%c/%s;pid: %d;thread: %#lx;",LogLevel2Char(level),tag,getpid(),pthread_self());

    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT(va_end_ap) {
        va_end(ap);
    };
    VAppendStdioFormat(log_msg,format,ap);

    log_msg.append(1,'\n');
    fputs(log_msg.data(),stdout);
    return ;
}

#if defined(PP_QQ_COMMON_ON_ANDROID)

namespace {

const int kLogcatLevelTable[] {
        ANDROID_LOG_VERBOSE,
        ANDROID_LOG_VERBOSE,
        ANDROID_LOG_VERBOSE,
        ANDROID_LOG_DEBUG,
        ANDROID_LOG_INFO,
        ANDROID_LOG_WARN,
        ANDROID_LOG_ERROR,
        ANDROID_LOG_FATAL,
};

inline int Level2LogcatLevel(int common_level)
{
    return kLogcatLevelTable[common_level];
}

}

void LogcatHandler(int level,const char *tag,const char * /* file */,int /* line */,const char * /* func */,const char * /* prettyFunc */,const char *format,...) noexcept
{
    std::string log_msg;
    AppendStdioFormat(log_msg,"pid: %d;threadid %#lx;",getpid(),pthread_self());

    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT(va_end_ap) {
        va_end(ap);
    };
    VAppendStdioFormat(log_msg,format,ap);

    __android_log_write(Level2LogcatLevel(level),tag,log_msg.data());
    return ;
}


#endif
