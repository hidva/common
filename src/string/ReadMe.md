
## 关于模块

*   实现了一种新的字符串类型(毕竟这可是 C++ 程序员的日常呐);没啥,只是感觉到 std::string 对
    COW 机制不是很友好,比如:
    
    ```c++
    std::string str1{"HelloWorld"};
    std::string str2{str1};
    
    char ch = str2[0];
    // 这里会导致 str2 fork内存,并且此时也会把 str2 设置为不可共享.
    // 大概是怕用户通过 operator[] 返回的引用来直接修改 str2 的内部内存吧.
    // 但是实际上,这里并没有必要 fork 内存,然后再设置为不可共享之类的.
    ```

    ```c++
    std::string str1 {"HelloWorld"};
    std::string str2 {str1};
    
    char *str2_ptr = const_cast<char*>(str2.data());
    str2_ptr[0] = 'c';
    // 此时会修改 str1 的值,正确的做法应该是:
    //  str2.reserve(str2.capacity());
    //  char *str2_ptr = const_cast<char*>(str2.data());
    // 或者由 str2 提供一个接口:char* data();
    ```

*   所以准备实现一个对 COW 友好的 string,在标准 std::string 提供的接口之上新增若干接口,在
    每一个会返回引用(或地址)的

