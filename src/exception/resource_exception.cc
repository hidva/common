
#include "exception/resource_exception.h"

const char* ResourceException::what() const noexcept {
    return err_.c_str();
}




