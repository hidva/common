
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



