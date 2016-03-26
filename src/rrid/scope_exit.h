#ifndef ORG_PP_QQ_COMMON_RRID_SCOPE_EXIT_H
#define ORG_PP_QQ_COMMON_RRID_SCOPE_EXIT_H

#include "common/config.h"
#include <functional>
#include <type_traits>

namespace pp_qq {

namespace rrid_detail {

struct ScopeGuardImplBase {
    ScopeGuardImplBase() noexcept(true) = default;
    inline void Dismiss() noexcept(true);

#if defined(ENABLE_PP_QQ_COMMON_TEST)
    bool dismissed() noexcept(true)
    {
        return dismissed_;
    }
#endif

protected:
    /* 若为真,则表明不执行回调;
     */
    bool dismissed_ = false;
};

inline void ScopeGuardImplBase::Dismiss() noexcept(true)
{
    dismissed_ = true;
    return ;
}



/* 编写异常安全的代码可真是麻烦呐... */

template <typename FunctionType>
struct ScopeGuardImpl : public rrid_detail::ScopeGuardImplBase {
    explicit ScopeGuardImpl(FunctionType& fn) noexcept(std::is_nothrow_copy_constructible<FunctionType>::value):
        ScopeGuardImpl(static_cast<const FunctionType&>(fn),MakeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},&fn))
    {
    }

    explicit ScopeGuardImpl(const FunctionType& fn) noexcept(std::is_nothrow_copy_constructible<FunctionType>::value):
        ScopeGuardImpl(fn,MakeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},&fn))
    {
    }

    explicit ScopeGuardImpl(FunctionType&& fn) noexcept(std::is_nothrow_move_constructible<FunctionType>::value):
        ScopeGuardImpl(std::forward<FunctionType>(fn),MakeFailsafe(std::is_nothrow_move_constructible<FunctionType>{},&fn))
    {
    }

    ScopeGuardImpl(ScopeGuardImpl&& other) noexcept(std::is_nothrow_move_constructible<FunctionType>::value):
        function_(std::move(other.function_))
    {
        dismissed_ = other.dismissed_;
        other.dismissed_ = true;
        return ;
    }

    ~ScopeGuardImpl() noexcept
    {
        if (!dismissed_)
            function_();
        return ;
    }

private:
    FunctionType function_;

private:
    static ScopeGuardImplBase MakeFailsafe(std::true_type, const void*) noexcept
    {
        return ScopeGuardImplBase{};
    }

    template <typename Fn>
    static auto MakeFailsafe(std::false_type, Fn* fn) noexcept -> ScopeGuardImpl<decltype(std::ref(*fn))>
    {
        return ScopeGuardImpl<decltype(std::ref(*fn))>{std::ref(*fn)};
    }

    template <typename Fn>
    ScopeGuardImpl(Fn&& fn, ScopeGuardImplBase&& failsafe):
        ScopeGuardImplBase{},
        function_(std::forward<Fn>(fn))
    {
        failsafe.Dismiss();
    }

private:
    ScopeGuardImpl() = delete;
    ScopeGuardImpl(const ScopeGuardImpl &) = delete;
    ScopeGuardImpl& operator =(const ScopeGuardImpl &) = delete;
    ScopeGuardImpl& operator =(ScopeGuardImpl &&) = delete;
};

} // namespace rrid_detail

template <typename FunctionType>
inline rrid_detail::ScopeGuardImpl<typename std::decay<FunctionType>::type>
MakeScopeGuard(FunctionType&& fn)
{
    return rrid_detail::ScopeGuardImpl<typename std::decay<FunctionType>::type>(std::forward<FunctionType>(fn));
}

namespace rrid_detail {

enum class ScopeGuardOnExit {};

template <typename FunctionType>
inline ScopeGuardImpl<typename std::decay<FunctionType>::type>
operator +(ScopeGuardOnExit, FunctionType&& fn)
{
    return ScopeGuardImpl<typename std::decay<FunctionType>::type>(std::forward<FunctionType>(fn));
}



} // namespace rrid_detail

#define PP_QQ_ON_SCOPE_EXIT(object_name)  auto object_name = ::pp_qq::rrid_detail::ScopeGuardOnExit {} + [&] () noexcept -> void
#define ON_SCOPE_EXIT(object_name) PP_QQ_ON_SCOPE_EXIT(object_name)


} // namespace pp_qq

#endif // ORG_PP_QQ_COMMON_RRID_SCOPE_EXIT_H
