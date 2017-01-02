#include <string.h>

#include <iostream>
#include <string>
#include <initializer_list>

struct StringRef {
    StringRef() = default;

    StringRef(const char *data, size_t size) :
        data_(data),
        size_(size) {
    }

    StringRef(const char *data) :
        StringRef(data, strlen(data)) {
    }

    StringRef(const std::string &str):
        StringRef(str.data(), str.size()) {
    }

    const char* data() const noexcept {
        return data_;
    }

    size_t size() const noexcept {
        return size_;
    }


private:
    const char *data_ = nullptr;
    size_t size_ = 0;
};

std::ostream& operator<<(std::ostream &out, const StringRef &str) {
    out << "{data: " << str.data() << ", size: " << str.size() << "}";
    return out;
}

void F(std::initializer_list<StringRef> strs) {
    for (StringRef str : strs) {
        std::cout << "str: " << str << std::endl;
    }
    return ;
}


int main() {
    F({"a", "b", "c"});
    std::string a("a");
    std::string b("b");
    std::string c("c");
    F({a, b, c});
    return 0;
}

