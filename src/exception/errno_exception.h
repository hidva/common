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


    ErrnoException(const std::string &fname,int errnum,const char *format,...) __attribute__((__format__(printf,4,5)));
    ErrnoException(std::string &&fname,int errnum,const char *format,...) __attribute__((__format__(printf,4,5)));

    ErrnoException(const std::string &fname,int errnum):
        ErrnoException(fname,errnum,std::string{})
    {
    }

    ErrnoException(std::string &&fname,int errnum):
        ErrnoException(std::move(fname),errnum,std::string{})
    {
    }

    ErrnoException(const std::string &fname):
        ErrnoException(fname,EINVAL)
    {
    }

    ErrnoException(std::string &&fname):
        ErrnoException(std::move(fname),EINVAL)
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

#define PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(args.../* errnum,format */) ::pp_qq::ErrnoException (__PRETTY_FUNCTION__,##args)
#define THROW(args.../* errnum,format,... */) throw PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(args);

#define PP_QQ_CHECK(cond,args...)   \
    if (!(cond))    \
        THROW(args)

#if defined(NDEBUG)
#define PP_QQ_DCHECK(cond,args...)
#else
#define PP_QQ_DCHECK(cond,args...) PP_QQ_CHECK((cond),##args)
#endif

#endif // ORG_PP_QQ_COMMON_EXCEPTION_ERRNO_EXCEPTION_H
