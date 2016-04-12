
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

*   自动生成


## API Reference: macros.h



[0]: <https://www.gnu.org/software/libc/manual/html_mono/libc.html>

