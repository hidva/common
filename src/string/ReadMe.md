
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
    每一个会返回引用(或地址)的接口都提供如下三个版本:
    
    ```c++
    // 很显然 cfront() 与 front()[非 const 版本] 最大的区别在其返回的是一份拷贝,这时候对于
    // 底层 string 来说,是不需要 fork 字符串内存的.
    char& front();
    char front() const;
    char cfront() const;
    ```
    
    ```c++
    char& back();
    char back() const;
    char cback() const;
    ```

*   在实现上"参考"了[facebook/folly][0]中`FBString`;对`FBString`的实现分析在[这里][1].



[0]: <https://github.com/facebook/folly>
[1]: <http://pp-qq.github.io/2016/03/28/FBString.html>

