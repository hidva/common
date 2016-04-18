
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

## API Reference:


basic_fbstring& operator=(basic_fbstring&& goner) noexcept {
if (FBSTRING_UNLIKELY(&goner == this)) {
    // Compatibility with std::basic_string<>,
    // C++11 21.4.2 [string.cons] / 23 requires self-move-assignment support.
    return *this;
}
// No need of this anymore
this->~basic_fbstring();
// Move the goner into this
// 这里 fbstring_core<E> 不应该出现!
new(&store_) fbstring_core<E>(std::move(goner.store_));
return *this;
}

basic_fbstring& assign(const value_type* s, const size_type n) {
Invariant checker(*this);

// s can alias this, we need to use pod_move.
if (size() >= n) {
    fbstring_detail::pod_move(s, s + n, store_.mutable_data());
    resize(n);
    assert(size() == n);
} else {
    const value_type *const s2 = s + size();
    // pod_copy() 即可
    fbstring_detail::pod_move(s, s2, store_.mutable_data());
    append(s2, n - size());
    assert(size() == n);
}
assert(size() == n);
return *this;
}

  iterator insert(const_iterator p, const value_type c) {
    const size_type pos = p - begin();
    insert(p, 1, c);
    return begin() + pos;
  }

  
**注意** 延迟加'\0'的问题.
**注意** expand_noinit() 不再会追加0.
**注意** raw_data(),const_raw_data();data(),
  
    1. 在调用 data() 时,根据情况转化为 cdata(),raw_data();
    2. 在调用 expand_noinit() 时需要注意末尾不再保证 0!
    
FBString 的实现对延迟追加'\0'不是很友好,如下:
template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(const value_type* s, size_type n) -> FBStringBasic&
{
    if (FBSTRING_UNLIKELY(!n))
        return *this;

    auto const old_size = size();
    auto const old_data = data(); // 这里调用 data(),而 data() 保证了以'\0'结尾,万一底层存储延迟'\0',此时可能会分配内存,这样当 [s,s + n) 在 FBString 内部时就会出问题.
    
    /* 
     */
    std::less_equal<const value_type*> le;
    if (FBSTRING_UNLIKELY(le(old_data, s) && !le(old_data + old_size, s))) {
        const size_type offset = s - old_data;
        store_.reserve(old_size + n);
        s = const_raw_data() + offset;
    }

    fbstring_detail::pod_copy(s, s + n, store_.expand_noinit(n,  true));
    return *this;
}

TODO 在 FBStringBasic<E,S,T,A>::replaceAliased(iterator i1, iterator i2,FwdIterator s1, FwdIterator s2, std::true_type) 实现中使用了不必要的原子操作,测试去掉该原子操作之后的时间比.
  
[0]: <https://github.com/facebook/folly>
[1]: <http://pp-qq.github.io/2016/03/28/FBString.html>

