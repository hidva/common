
## 关于模块

*   获取程序的执行时间.比如函数的执行时间.

## API Reference: execute_time_get.h

### ExecuteTimeGet

```c++
template <typename F>
uint_fast64_t ExecuteTimeGet(size_t iter,size_t times,F &&func);
```

*   获取`func()`执行时间.

*   `PARAM:iter,times`;
    -   首先了解:`ExecuteTimeGet()`测试出来的时间实际上`func()`执行时间 func_exec_time 与
        各种噪音 noise_time 之和;即`ExecuteTimeGet()`的返回值 fact_time = func_exec_time + noise_time;
        所以`ExecuteTimeGet()`内部进行了多次测试,得到多个 fact_time,然后选取最小的一个,这样确保 noise_time 最小,
        从而使 fact_time 最接近 noise_time.

    -   所以函数里对`iter`,`times`的使用逻辑:
    
        ```c++
        for (i = 0; i < iter; ++i) {
            begin = get_clock_time();
            for (j = 0; j < times; ++j)
                func();
            end = get_clock_time();
            time_use = end - begin;
        }
        ```
        
*   `PARAM:func`;其调用形式:

    ```c++
    void func() noexcept;
    ```

*   `RETURN`;`func()`循环运行 times 次所耗费的时间.


