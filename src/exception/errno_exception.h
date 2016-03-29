#ifndef ORG_PP_QQ_COMMON_EXCEPTION_ERRNO_EXCEPTION_H
#define ORG_PP_QQ_COMMON_EXCEPTION_ERRNO_EXCEPTION_H

#include <string>
#include <stdexcept>

#include "formatter/stdio_formatter.h"

namespace pp_qq {

struct ErrnoException : public std::exception {
    ErrnoException(const ErrnoException &) = default;
    ErrnoException(const std::string &fname,int errnum,const std::string &errmsg):
        function_name_(fname),
        err_msg_(errmsg),
        err_number_(errnum)
    {
    }

    ErrnoException(const std::string &fname,int errnum,std::string &&errmsg):
        function_name_(fname),
        err_msg_(std::move(errmsg)),
        err_number_(errnum)
    {
    }

    ErrnoException(std::string &&fname,int errnum,std::string &&errmsg) noexcept:
        function_name_(std::move(fname)),
        err_msg_(std::move(errmsg)),
        err_number_(errnum)
    {
    }

    ErrnoException(std::string &&fname,int errnum,const std::string &errmsg):
        function_name_(std::move(fname)),
        err_msg_(errmsg),
        err_number_(errnum)
    {
    }

    ErrnoException(ErrnoException &&other) noexcept:
        function_name_(std::move(other.function_name_)),
        err_msg_(std::move(other.err_msg_)),
        err_number_(other.err_number_)
    {
    }

    ErrnoException& operator = (const ErrnoException &) = default;
    ErrnoException& operator = (ErrnoException &&other) noexcept
    {
        function_name_ = std::move(other.function_name_);
        err_msg_ = std::move(other.err_msg_);
        err_number_ = other.err_number_;
        return *this;
    }

    const char* what() const noexcept override;

private:
    mutable std::string buf_for_what_;
    std::string function_name_;
    std::string err_msg_;
    int err_number_;

private:
    ErrnoException() = delete;
};


} // namespace pp_qq

#define PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(errnum,args...) ::pp_qq::ErrnoException (__PRETTY_FUNCTION__,errnum,CreateStringFromStdioFormat<std::string>(args))
#define THROW(errnum,args...) throw PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(errnum,##args);

#endif // ORG_PP_QQ_COMMON_EXCEPTION_ERRNO_EXCEPTION_H
