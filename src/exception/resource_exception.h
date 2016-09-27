
#pragma once

#include <string>
#include <stdexcept>

struct ResourceException : public std::exception {
    ResourceException() = default;
    ResourceException(int errcode) :
        errcode_(errcode) {
    }

    ResourceException(int errcode, const std::string &err):
        errcode_(errcode),
        err_(err) {
    }

    ResourceException(int errcode, std::string &&err) noexcept :
        errcode_(errcode),
        err_(std::move(err)) {
    }

    ResourceException(const ResourceException &) = default;
    ResourceException(ResourceException &&other) noexcept :
        errcode_(other.errcode_),
        err_(std::move(other.err_)) {
    }

    ResourceException& operator=(const ResourceException &) = default;
    ResourceException& operator=(ResourceException &&other) noexcept {
        errcode_ = other.errcode_;
        err_ = std::move(other.err_);
        return *this;
    }

    const char* what() const noexcept override;

    int GetErrorCode() const noexcept {
        return errcode_;
    }

    const std::string& GetErrorMsg() const noexcept {
        return err_;
    }
private:
    int errcode_;
    std::string  err_;
};

