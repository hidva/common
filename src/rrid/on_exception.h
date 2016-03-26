#ifndef ORG_PP_QQ_COMMON_RRID_ON_EXCEPTION_H
#define ORG_PP_QQ_COMMON_RRID_ON_EXCEPTION_H

#include <exception>
#include "rrid/scope_exit.h"
#include "common/preprocess.h"

namespace pp_qq {

namespace rrid_detail {


#if defined(__GNUG__) || defined(__CLANG__)
#define PP_QQ_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS
namespace __cxxabiv1 {
// forward declaration (originally defined in unwind-cxx.h from from libstdc++)
struct __cxa_eh_globals;
// declared in cxxabi.h from libstdc++-v3
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
}
#elif defined(_MSC_VER) && (_MSC_VER >= 1400) && (_MSC_VER < 1900) // MSVC++ 8.0 or greater
#define PP_QQ_EXCEPTION_COUNT_USE_GETPTD
// forward declaration (originally defined in mtdll.h from MSVCRT)
struct _tiddata;
extern "C" _tiddata* _getptd(); // declared in mtdll.h from MSVCRT
#elif defined(_MSC_VER) && (_MSC_VER >= 1900) // MSVC++ 2015
#define PP_QQ_EXCEPTION_COUNT_USE_STD
#else
#error "Unsupported platform, don't include this header."
#endif

/* 获取当前线程中 live exception object 的数目;
 * live exception object;也就是自抛出(throw,rethrow)之后,尚未进入到匹配的 catch 子句
 * 的对象.
 * @note 参考 unwind-cxx.h(src/lib/cppsup/unwind-cxx.h).
 * @note 参考 facebook/folly/detail/UncaughtExceptionCounter.h.
 */
inline int GetUncaughtExceptionsNum() noexcept
{
#if defined(PP_QQ_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS)
  // __cxa_get_globals returns a __cxa_eh_globals* (defined in unwind-cxx.h).
  // The offset below returns __cxa_eh_globals::uncaughtExceptions.
  return *(reinterpret_cast<unsigned int*>(static_cast<char*>(
      static_cast<void*>(__cxxabiv1::__cxa_get_globals())) + sizeof(void*)));
#elif defined(PP_QQ_EXCEPTION_COUNT_USE_GETPTD)
  // _getptd() returns a _tiddata* (defined in mtdll.h).
  // The offset below returns _tiddata::_ProcessingThrow.
  return *(reinterpret_cast<int*>(static_cast<char*>(
      static_cast<void*>(_getptd())) + sizeof(void*) * 28 + 0x4 * 8));
#elif defined(PP_QQ_EXCEPTION_COUNT_USE_STD)
  return std::uncaught_exceptions();
#else
#error "Unsupported platform, don't include this header."
#endif

}


template <typename FunctionType>
struct OnExceptionHelper : private rrid_detail::ScopeGuardImplBase {

    OnExceptionHelper(FunctionType &function) noexcept(std::is_nothrow_copy_constructible<FunctionType>::value):
        OnExceptionHelper(static_cast<const FunctionType&>(function),MakeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},&function))
    {
    }

    OnExceptionHelper(const FunctionType &function) noexcept(std::is_nothrow_copy_constructible<FunctionType>::value):
        OnExceptionHelper(function,MakeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},&function))
    {
    }

    OnExceptionHelper(FunctionType &&function) noexcept(std::is_nothrow_move_constructible<FunctionType>::value):
        OnExceptionHelper(std::move(function),MakeFailsafe(std::is_nothrow_move_constructible<FunctionType>{},&function))
    {
    }

    OnExceptionHelper(OnExceptionHelper &&other) noexcept(std::is_nothrow_move_constructible<FunctionType>::value ):
        ScopeGuardImplBase{},
        function_ (std::move(other.function_)),
        uncaught_exception_num_ (other.uncaught_exception_num_)
    {
        other.Dismiss();
        return ;
    }

    ~OnExceptionHelper() noexcept
    {
        if (!dismissed_ && rrid_detail::GetUncaughtExceptionsNum() > uncaught_exception_num_)
            function_();
        return ;
    }

private:
    FunctionType function_;
    int uncaught_exception_num_;

private:
    static rrid_detail::ScopeGuardImplBase MakeFailsafe(std::true_type, const void*) noexcept
    {
        return rrid_detail::ScopeGuardImplBase{};
    }

    template <typename Fn>
    static auto MakeFailsafe(std::false_type, Fn* fn) noexcept -> ScopeGuardImpl<decltype(std::ref(*fn))>
    {
        return ScopeGuardImpl<decltype(std::ref(*fn))>{std::ref(*fn)};
    }

    template <typename Fn>
    OnExceptionHelper(Fn &&fn,rrid_detail::ScopeGuardImplBase &&failsafe_obj):
        ScopeGuardImplBase{},
        function_(std::forward<Fn>(fn)),
        uncaught_exception_num_(rrid_detail::GetUncaughtExceptionsNum())
    {
        failsafe_obj.Dismiss();
        return ;
    }
private:
    OnExceptionHelper() = delete;
    OnExceptionHelper(const OnExceptionHelper &) = delete;
    OnExceptionHelper& operator = (const OnExceptionHelper &) = delete;
    OnExceptionHelper& operator = (OnExceptionHelper &&) = delete;
};

} //namespace rrid_detail

template <typename FunctionType>
inline rrid_detail::OnExceptionHelper<typename std::decay<FunctionType>::type>
MakeOnException(FunctionType&& fn)
{
    return rrid_detail::OnExceptionHelper<typename std::decay<FunctionType>::type>{std::forward<FunctionType>(fn)};
}


namespace rrid_detail {

enum class OnExceptionHelperEnum {};

template <typename FunctionType>
inline OnExceptionHelper<typename std::decay<FunctionType>::type>
operator * (OnExceptionHelperEnum ,FunctionType &&fn)
{
    return MakeOnException(std::forward<FunctionType>(fn));
}


} // namespace rrid_detail

#define PP_QQ_ON_EXCEPTION auto PP_QQ_ANONYMOUS_NAME(on_exception_object_xxx) = ::pp_qq::rrid_detail::OnExceptionHelperEnum{} * [&] () -> void
#define ON_EXCEPTIN PP_QQ_ON_EXCEPTION

} // namespace pp_qq

#endif // ORG_PP_QQ_COMMON_RRID_ON_EXCEPTION_H
