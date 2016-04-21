
## 模块介绍

*   存放了与异常处理有关的一些工具.

## API Reference: errno_exception.h

### ErrnoException

```c++
struct ErrnoException;
```

*   与`std::exception`相比,该异常对象还保存了异常出现的函数名,一个整型错误码,还有一个错误描述字符串.
*   整型错误码取值最好是现有`errno`的值.

#### 构造析构

```c++
ErrnoException(const ErrnoException &);
ErrnoException(const std::string &fname,int errnum,const std::string &errmsg);
ErrnoException(const std::string &fname,int errnum,std::string &&errmsg);
ErrnoException(std::string &&fname,int errnum,std::string &&errmsg) noexcept;
ErrnoException(std::string &&fname,int errnum,const std::string &errmsg);
ErrnoException(ErrnoException &&other) noexcept;
```

*   呐..不需要过多说明吧.

#### operator=

```c++
ErrnoException& operator = (const ErrnoException &);
ErrnoException& operator = (ErrnoException &&other) noexcept;
```
*   呐..不需要过多说明吧.


### THROW

```c++
THROW(err_number);
THROW(err_number,err_msg);
THROW(err_number,err_msg_format,...);
```

*   该宏用于构建`ErrnoException`对象,并抛出该对象.

#### DEMO

```c++
int fd = open(path,O_RDONLY);
if (fd < 0)
    THROW(errno);
if (fd < 0)
    THROW(errno,"文件打开失败!");
if (fd < 0)
    THROW(errnom,"文件打开失败;fd: %d;path: %s",fd,path);
```

### PP_QQ_CHECK

*   考虑到`CHECK`宏名已经被用烂了,所以这里就只能强加个前缀了.

```c++
PP_QQ_CHECK(condition);
PP_QQ_CHECK(condition,errnum);
PP_QQ_CHECK(condition,errnum,format,...);
```

*   若`condition`为假,则构建并抛出一个`ErrnoException`异常;
*   若`condition`为真,则**确保不会**对`errnum`,`format`,`...`进行求值!
*   `PARAM:errnum`;用于指定构建的`ErrnoException`对象的错误码,默认为`EINVAL`.
*   `PARAM:format,...`;用于指定构建的`ErrnoException`对象的错误描述,默认为空.

### PP_QQ_DCHECK

```c++
#if defined(NDEBUG)
#define PP_QQ_DCHECK // 定义为空
#else
#define PP_QQ_DCHECK PP_QQ_CHECK
```


