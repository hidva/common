
*   存放着 common 项目所有模块共享的文件.

## API Reference: preprocess.h

### PP_QQ_CONCAT_DIRECT

```c++
PP_QQ_CONCAT_DIRECT(x,y)
```
*   将`x`,`y`拼接在一起,若`x`,`y`是宏,则**不会**预先展开!.

### PP_QQ_CONCAT_WITH_EXPAND

```c++
PP_QQ_CONCAT_WITH_EXPAND(x,y)
```
*   将`x`,`y`拼接在一起,若`x`,`y`是宏,则**会**预先展开!.


### PP_QQ_ANONYMOUS_NAME

```c++
PP_QQ_ANONYMOUS_NAME(object_name)
```
*   根据`object_name`以及`PP_QQ_ANONYMOUS_NAME`在文本文件中的行号拼接成一个无意义的标识符.

#### DEMO
```c++
auto PP_QQ_ANONYMOUS_NAME(on_exception_obj) = MakeOnException(f);
```


## API Reference: inline_utils.h

*   该文件**只定义**`inline`工具函数! 存放着一些全局通用的工具函数.
*   由于该文件只定义了`inline`函数,而且`inline`函数只会在用到时才会展开,所以将大量`inline`
    函数汇聚在一起除了不那么直观之外,并没有什么副作用了

### IsEven

```c++
bool IsEven(unsigned long number) noexcept;
```

*   若`number`为偶数,则返回`true`;否则返回`false`.

### IsOdd

```c++
bool IsOdd(unsigned long number) noexcept;
```

*   若`number`为奇数,则返回`true`;否则返回`false`.


### IsOverlap()

```c++
inline bool 
IsOverlap(const void *left,size_t left_size,const void *right,size_t right_size) noexcept;
```

*   若`left,left_size`确定的缓冲区与`right,right_size`确定的缓冲区重叠,则返回真.
*   缓冲区重叠意味着`left,left_size`确定的缓冲区与`right,right_size`确定的缓冲区的交集不
    为空;所以很显然若`left`,`right`一方缓冲区中为空,则重叠总是为空,所以总是不重叠的.


