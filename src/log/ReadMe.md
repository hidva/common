
## 模块介绍

*   log;用于日志记录.

## log 实现架构

*   参考了 java.util.logging 包对日志架构;对日志处理流程如下:

    1.  若请求的日志级别小于当前日志级别,则不输出日志;否则:
    2.  收到能收集到的元信息;当前收集以下信息:
        -  __func__;
        -  __PRETTY_FUNCTION__;
        -  __FILE__;
        -  __LINE__;
        -  logMsg;
    3.  调用指定的 handler 来进行处理;

*   可以自定义 handler 来自定义日志的处理.参见宏`__PP_QQ_DO_LOG`中对 Handler 的调用.

## API Reference: pp_qq_log.h

*   参见 pp_qq_log.h 文件即可.



