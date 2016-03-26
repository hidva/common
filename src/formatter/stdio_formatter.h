#ifndef ORG_PP_QQ_COMMON_FORMATTER_STDIO_FORMATTER_H
#define ORG_PP_QQ_COMMON_FORMATTER_STDIO_FORMATTER_H

#include <stdarg.h>
#include <stdexcept>
#include "rrid/scope_exit.h"
#include "rrid/on_exception.h"

template <typename StringType>
StringType& VAppendStdioFormat(StringType &str,const char *f,va_list ap) __attribute__((__format__(printf,2,0)));

template <typename StringType>
StringType& AppendStdioFormat(StringType &str,const char *format,...) __attribute__((__format__(printf,2,3)));

template <typename StringType>
StringType& VAppendStdioFormat(StringType &str,const char *f,va_list ap)
{
    if (f != nullptr && *f != '\0') {
        va_list ap1;
        va_copy(ap1,ap);
        ON_SCOPE_EXIT(end_ap1) {
            va_end(ap1);
        };

        constexpr size_t kBufSize = 256;
        char buf[kBufSize];
        int ret = vsnprintf(buf,kBufSize,f,ap);
        if (ret < 0)
            throw std::runtime_error(std::string{"VAppendStdioFormat Error!FormatArg: "} + f);

        if (static_cast<size_t>(ret) < kBufSize) {
            str.append(buf,ret);
        } else { // 表明 buf 空间不足.
            const size_t kOldSize = str.size();
            str.resize(kOldSize + ret + 1);
            ON_EXCEPTIN {
                str.resize(kOldSize);
            };

            /* const_cast 主要是因为 std::string::data() 返回值是 const char *!.所以
             * 需要去除 const 限定.
             *
             * Q1: 这里会不会由于 std::string 采用 COW 机制而破坏了 std::string 的内容.
             * A1: 不会,因为在之前 resize() 中因为进行了 detach 操作.
             */
            int ret2 = vsnprintf(const_cast<char*>(str.data()) + kOldSize,ret + 1,f,ap1);
            if (ret2 < 0 || ret2 != ret)
                throw std::runtime_error(std::string{"VAppendStdioFormat Error!FormatArg: "} + f);
            str.resize(kOldSize + ret);
        }
    }
    return str;
}

template <typename StringType>
StringType& AppendStdioFormat(StringType &str,const char *format,...)
{
    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT(end_ap) {
        va_end(ap);
    };

    return VAppendStdioFormat(str,format,ap);
}



#endif // ORG_PP_QQ_COMMON_FORMATTER_STDIO_FORMATTER_H
