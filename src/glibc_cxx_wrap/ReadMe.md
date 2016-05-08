
## 关于模块

*   使用 C++ 异常机制对 glibc 接口进行封装,即不再使用返回值来判断异常.如:

    ```c++
    // glibc 中的声明.
    int open(const char *pathname, int flags);

    // 使用 C++ 异常之后的声明.
    int cxx_open(const char *pathname,int flags)
    {
        int fd = open(pathname,flags);
        if (fd == -1)
            THROW(errno,"pathname: %s;flags: %#x",pathname,flags);
        return fd;
    }
    ```

*   目录组织;

    -   若一个接口可以在[glibc文档][0]中存在,则其 C++ 封装版本所在头文件为:'章节号-章节名.h';
    -   若一个接口在[glibc文档][0]中不存在,则其 C++ 封装版本所在头文件为其原声明所在头文件相对
        于'glibc_cxx_wrap'中的相对路径下,并且文件名使用'cxx_'作为前缀,如:

    ```shell
    $ tree .
    glibc-cxx-wrap/
    ├── 12_IO_on_stream.h
    ├── 25_the_basic_program_system_interface.h
    ├── 26_processes.h
    ├── cxx_dlfcn.h
    ├── cxx_time.h
    ├── cxx_pthread.h
    └── sys
        ├── cxx_epoll.h
        ├── cxx_inotify.h
        ├── cxx_stat.h
        └── cxx_timerfd.h
    ```

*   自动生成计划;本意是通过读取 glibc 文档,解析文档,提取出函数声明以及函数出错类型,然后自动生成
    包装函数来着.但是简单看了一下 glibc 文档,好像没有发现太明显的规律;加上现在时间紧,所以就做个
    **TODO**吧.

## API Reference: macros.h

### GLIBC_CXX_WRAP_ERRNO

```c++
#define GLIBC_CXX_WRAP_ERRNO(fail_ret,glibc_call_statement);
#define GLIBC_CXX_WRAP_ERRNO(fail_ret,glibc_call_statement,format,...);
```

*   若一个 glibc 接口在出错时总是返回一个固定值`fail_ret`,并且错误码存放在`errno`中,则可以使
    用该宏封装;
*   这样若 glibc 接口执行成功,则无操作;
*   若执行失败,则会抛出`ErrnoException`类型异常.其中:

    -   `ErrnoException::errnum()`中记录着`errno`的值;
    -   `ErrnoException::err_msg()`中记录了`format`以及其后续参数格式化后的字符串;若未指定
        `format`及后续参数,则为空.


#### DEMO

```c++
struct stat file_stat_obj;
GLIBC_CXX_WRAP_ERRNO(-1,stat(path,&file_stat_obj));
// 若执行这里,则表明 stat() 执行成功;否则表明 stat() 出错,此时已经抛出了一个异常.
```

### GLIBC_CXX_WRAP_1_ERRNO

```c++
#define GLIBC_CXX_WRAP_1_ERRNO(statement);
#define GLIBC_CXX_WRAP_1_ERRNO(statement,format,...);
```

*   若 glibc 接口在出错时总是返回`-1`,并且错误码存放在`errno`中,则可以使用该宏来封装.
*   这样若 glibc 接口执行成功,则无操作;
*   若执行失败,则会抛出`ErrnoException`类型异常.其中:

    -   `ErrnoException::errnum()`中记录着`errno`的值;
    -   `ErrnoException::err_msg()`中记录了`format`以及其后续参数格式化后的字符串;若未指定
        `format`及后续参数,则为空.

### GLIBC_CXX_WRAP_NULL_ERRNO

```c++
#define GLIBC_CXX_WRAP_NULL_ERRNO(statement);
#define GLIBC_CXX_WRAP_NULL_ERRNO(statement,format,...);
```

*   若 glibc 接口在出错时返回`nullptr`,并且将错误码存放在`errno`中,则可以使用该宏来封装.
*   这样若 glibc 接口执行成功,则无操作;
*   若执行失败,则会抛出`ErrnoException`类型异常.其中:

    -   `ErrnoException::errnum()`中记录着`errno`的值;
    -   `ErrnoException::err_msg()`中记录了`format`以及其后续参数格式化后的字符串;若未指定
        `format`及后续参数,则为空.

### GLIBC_CXX_WARP_S0

```c++
#define GLIBC_CXX_WARP_S0(statement);
#define GLIBC_CXX_WARP_S0(statement,format,...);
```

*   若 glibc 接口在**成功**时返回`0`,失败时返回错误码,如 pthread 接口;则可以使用该宏来进行封装.
*   这样若 glibc 接口执行成功,则无操作;
*   若执行失败,则会抛出`ErrnoException`类型异常.其中:

    -   `ErrnoException::errnum()`中记录着`errno`的值;
    -   `ErrnoException::err_msg()`中记录了`format`以及其后续参数格式化后的字符串;若未指定
        `format`及后续参数,则为空.

## API Reference: 13_low_level_io.h

### IsNonblocking

```cpp
bool IsNonblocking(int fd);
```

### SetNonblocking

```cpp
void SetNonblocking(int fd);
```

### SetBlocking

```cpp
void SetBlocking(int fd);
```

## API Reference: 14_file_system_interface.h

### Readdir

```cpp
bool Readdir(DIR *dirp, struct dirent *entry);
```

*   读取目录项,存放到`entry`指向的空间中.
*   `RETURN:`;`true`,表明读取了一个目录项到`entry`指向的空间中;`false`,表明读取到底,即`EOF`.

### GetCWD

```cpp
template <typename StringType>
void GetCWD(StringType &str);
```

*   获取当前工作目录绝对路径,并将其追加到`str`之后.

```cpp
template <typename StringType>
StringType GetCWD();
```

*   根据当前工作目录绝对路径构建一个`StringType`对象.

[0]: <https://www.gnu.org/software/libc/manual/html_mono/libc.html>

