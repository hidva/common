
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

*   在实现上**大量参考**了[facebook/folly][0]中`FBString`;对`FBString`的实现分析在[这里][1].

*   无论是`std::string`还是`folly::fbstring`,她们都默认末尾'\0'总是在的,若一个底层存储类
    是延迟追加'\0',则现有 API 只是告诉了这个底层存储类何时追加'\0',并没有说过我现在不需要'\0',
    所以新增了如下 API:
    
    ```c++
    /* 返回一个地址,指向着内部存储,不能确保该存储以'\0'结尾.
     */
    value_type* raw_data();
    const value_type* raw_data() const;
    const value_type* const_raw_data() const;    
    ```

## API Reference: extended_std_string.h

*   对`std::string`的扩充,增加了上述所说的接口.

## API Reference: string_piece.h

### StringPiece

*   StringPiece 是一个类`std::string`对象,只不过其内存是由用户来提供,内部不会尝试分配内存.
    除此之外,其与`std::string`的行为完全相似.



TODO 重构所有会使用到字符串的模块,如 stdioformat.  
  
 
[0]: <https://github.com/facebook/folly>
[1]: <http://pp-qq.github.io/2016/03/28/FBString.html>

