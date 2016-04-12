
#ifndef ORG_PP_QQ_COMMON_COMMON_EXECUTE_TIME_GET_H_
#define ORG_PP_QQ_COMMON_COMMON_EXECUTE_TIME_GET_H_

template <typename F>
class ExecuteTimeGet {
public:
    explicit ExecuteTimeGet(const F &f);
    explicit ExecuteTimeGet(F &&f);

    ExecuteTimeGet(ExecuteTimeGet &&other):
        f_(std::move(other.f_))
    {
    }

    ~ExecuteTimeGet() noexcept = default;

    uint64_t execute(size_t iter,size_t times);

private:
    F f_;

private:
    ExecuteTimeGet(const ExecuteTimeGet &) = delete;
    ExecuteTimeGet& operator = (ExecuteTimeGet &&) = delete;
    ExecuteTimeGet& operator = (const ExecuteTimeGet &) = delete;
};

template <typename F>
inline ExecuteTimeGet<F>::ExecuteTimeGet(const F &f):
    f_(f)
{
}

template <typename F>
inline ExecuteTimeGet<F>::ExecuteTimeGet(F &&f):
    f_(std::move(f))
{
}


template <typename F>
inline ExecuteTimeGet<typename std::decay<F>::type>
MakeBenchmarkTest(F &&fn)
{
    return ExecuteTimeGet<typename std::decay<F>::type>(std::forward<F>(fn));
}

template <typename F>
uint64_t ExecuteTimeGet<F>::execute(size_t iter,size_t times)
{
    uint64_t time_use_min = 0xFFFFFFFFFFFFFFFFU;

    for (size_t i = 0; i < iter; ++i) {
        struct timespec begin_time,end_time;
        cxx_clock_gettime(CLOCK_REALTIME,&begin_time);
        for (size_t i = 0; i < times; ++i)
            f_();
        cxx_clock_gettime(CLOCK_REALTIME,&end_time);
        
        uint64_t current_time_use = GetTimespecDiff(&end_time,&begin_time);
        if (current_time_use < time_use_min)
            time_use_min = current_time_use;
    }

    return time_use_min;
}

#endif /* ORG_PP_QQ_COMMON_COMMON_EXECUTE_TIME_GET_H_ */
