#include "errno_exception.h"
#include "formatter/stdio_formatter.h"

namespace pp_qq {

const char* ErrnoException::what() const noexcept
{
    buf_for_what_.resize(0);
    AppendStdioFormat(buf_for_what_,"func: %s;errnum: %d",function_name_.data(),err_number_);
    if (!err_msg_.empty())
        AppendStdioFormat(buf_for_what_,";err: %s",err_msg_.data());
    return buf_for_what_.data();
}

ErrnoException::ErrnoException(const std::string &fname,int errnum,const char *format,...):
    function_name_(fname),
    err_number_(errnum)
{
    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT(va_end_ap) {
        va_end(ap);
    };

    VAppendStdioFormat(err_msg_,format,ap);
    return ;
}

ErrnoException::ErrnoException(std::string &&fname,int errnum,const char *format,...):
    function_name_(std::move(fname)),
    err_number_(errnum)
{
    va_list ap;
    va_start(ap,format);
    ON_SCOPE_EXIT(va_end_ap) {
        va_end(ap);
    };

    VAppendStdioFormat(err_msg_,format,ap);
    return ;
}


}
