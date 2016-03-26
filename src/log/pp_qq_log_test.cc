
#include "log/pp_qq_log.h"
#include "gtest/gtest.h"
#include "formatter/stdio_formatter.h"
#include <string>
#include <string.h>

TEST(PPQQLogTest,PrintTest)
{
    PP_QQ_LOG_V("Verbose");
    PP_QQ_LOG_V("Verbose: %d",33);
    PP_QQ_LOG_D("Debug");
    PP_QQ_LOG_D("Debug: %d",33);
    PP_QQ_LOG_I("Info");
    PP_QQ_LOG_I("Info: %d",33);
    PP_QQ_LOG_W("Warn");
    PP_QQ_LOG_W("Warn: %d",33);
    PP_QQ_LOG_E("Error");
    PP_QQ_LOG_E("Error: %d",33);
    PP_QQ_LOG_F("Fatal");
    PP_QQ_LOG_F("Fatal: %d",33);

    PP_QQ_LOG_V3("DebugTag","Verbose");
    PP_QQ_LOG_V3("DebugTag","Verbose: %d",33);
    PP_QQ_LOG_D3("DebugTag","Debug");
    PP_QQ_LOG_D3("DebugTag","Debug: %d",33);
    PP_QQ_LOG_I3("DebugTag","Info");
    PP_QQ_LOG_I3("DebugTag","Info: %d",33);
    PP_QQ_LOG_W3("DebugTag","Warn");
    PP_QQ_LOG_W3("DebugTag","Warn: %d",33);
    PP_QQ_LOG_E3("DebugTag","Error");
    PP_QQ_LOG_E3("DebugTag","Error: %d",33);
    PP_QQ_LOG_F3("DebugTag","Fatal");
    PP_QQ_LOG_F3("DebugTag","Fatal: %d",33);
}

namespace {

struct LogRecord {
    int level;
    const char *tag = nullptr;
    const char *file = nullptr;
    int line ;
    std::string log_msg;

    /* 测试当前 record 日志记录是否是源文件 filename 打印,若是则返回 true.
     */
    bool IsInFile(const char *filename) noexcept
    {
        return strstr(file,filename) != nullptr;
    }

};

LogRecord LogHandlerForTest(int level,const char *tag,const char *filename,int line,const char *func,const char *prettyFunc,const char *format,...) noexcept __attribute__((__format__(printf,7,8)));

LogRecord LogHandlerForTest(int level,const char *tag,const char *filename,int line,const char *func,const char *prettyFunc,const char *format,...) noexcept
{
    LogRecord record;
    record.level = level;
    record.tag = tag;
    record.file = filename;
    record.line = line;

    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT (end_ap) {
        va_end(ap);
    };

    VAppendStdioFormat(record.log_msg,format,ap);
    return record;
}

}

TEST(PPQQLogTest,HandlerTest)
{
    {
        LogRecord record1 = PP_QQ_LOG_D4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_DEBUG,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_D4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_DEBUG,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }

    {
        LogRecord record1 = PP_QQ_LOG_V4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_VERBOSE,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_V4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_VERBOSE,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }

    {
        LogRecord record1 = PP_QQ_LOG_I4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_INFO,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_I4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_INFO,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }

    {
        LogRecord record1 = PP_QQ_LOG_W4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_WARN,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_W4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_WARN,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }

    {
        LogRecord record1 = PP_QQ_LOG_E4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_ERROR,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_E4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_ERROR,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }

    {
        LogRecord record1 = PP_QQ_LOG_F4(LogHandlerForTest,"tag","hello");
        EXPECT_EQ(PP_QQ_LOG_LEVEL_FATAL,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello"},record1.log_msg);
    }
    {
        LogRecord record1 = PP_QQ_LOG_F4(LogHandlerForTest,"tag","hello: %d",33);
        EXPECT_EQ(PP_QQ_LOG_LEVEL_FATAL,record1.level);
        EXPECT_STREQ("tag",record1.tag);
        EXPECT_TRUE(record1.IsInFile(__FILE__));
        EXPECT_EQ(std::string{"hello: 33"},record1.log_msg);
    }
}
