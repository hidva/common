#ifndef ORG_PP_QQ_COMMON_LOG_PP_QQ_LOG_H
#define ORG_PP_QQ_COMMON_LOG_PP_QQ_LOG_H

#include "common/config.h"
#if defined(PP_QQ_COMMON_ON_ANDROID)
#include <android/log.h>
#endif

/* 日志级别定义.
 * * ALL,SLIENT 仅会在定义 CURRENT_LOG_LEVEL 时会使用到,不要显式使用这 2 个级别.
 *
 * 2;为什么不将这些定义为枚举类型?
 * * 因为我的想法就是当日志不需要输出时,完全不会影响原程序的性能;因此"日志处理流程"第一条是使用 #if
 *   来实现的;如果把以下宏定义换成枚举,则无法使用 #if;不得不使用 if 在代码运行时判断,多多少少会
 *   影响原程序的性能.
 *
 * 3;不要修改这里宏定义的值;因为有以下在使用着这里的值:
 * * level2Char(int) 的实现.
 * * LOG_LEVEL_CHAR;
 */
#define PP_QQ_LOG_LEVEL_ALL      1
#define PP_QQ_LOG_LEVEL_VERBOSE  2
#define PP_QQ_LOG_LEVEL_DEBUG    3
#define PP_QQ_LOG_LEVEL_INFO     4
#define PP_QQ_LOG_LEVEL_WARN     5
#define PP_QQ_LOG_LEVEL_ERROR    6
#define PP_QQ_LOG_LEVEL_FATAL    7
#define PP_QQ_LOG_LEVEL_SLIENT   8

// 将 format 将参数格式化为一条日志,追加'\n'后;将日志信息送往 stdout 中.
void StdoutHandler(int level,const char *tag,const char *file,int line,const char *func,const char *prettyFunc,const char *format,...) noexcept __attribute__((__format__(printf,7,8)));

#if defined(PP_QQ_COMMON_ON_ANDROID)
void LogcatHandler(int level,const char *tag,const char *file,int line,const char *func,const char *prettyFunc,const char *format,...) noexcept __attribute__((__format__(printf,7,8)));
#endif

#if !defined(PP_QQ_CURRENT_LOG_LEVEL)
#	define PP_QQ_CURRENT_LOG_LEVEL PP_QQ_LOG_LEVEL_ALL
#endif

#if !defined(PP_QQ_DEFAULT_HANDLER)

#if defined(PP_QQ_COMMON_ON_ANDROID)
#define PP_QQ_DEFAULT_HANDLER LogcatHandler
#else
#define PP_QQ_DEFAULT_HANDLER StdoutHandler
#endif

#endif

#if !defined(PP_QQ_DEFAULT_LOG_TAG)
#	define PP_QQ_DEFAULT_LOG_TAG "org.pp_qq"
#endif

/* 该宏完成上述"日志处理流程"中后2,3步.
 *
 * 1;handler 的调用形式:
 * void handler(int level,const char *tag,const char *filename,int line,const char *func,const char *prettyFunc,const char *format,...) noexcept;
 */
#define __PP_QQ_DO_LOG(Handler,tag,level,format,args...)					 \
    Handler(level,tag,__FILE__,__LINE__,__FUNCTION__,__PRETTY_FUNCTION__,format,##args)

#if PP_QQ_LOG_LEVEL_VERBOSE >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_V4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_VERBOSE,format,##args)
#else
#	define PP_QQ_LOG_V4(handler,tag,format,args...)
#endif

#if PP_QQ_LOG_LEVEL_DEBUG >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_D4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_DEBUG,format,##args)
#else
#	define PP_QQ_LOG_D4(handler,tag,format,args...)
#endif

#if PP_QQ_LOG_LEVEL_INFO >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_I4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_INFO,format,##args)
#else
#	define PP_QQ_LOG_I4(handler,tag,format,args...)
#endif

#if PP_QQ_LOG_LEVEL_WARN >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_W4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_WARN,format,##args)
#else
#	define PP_QQ_LOG_W4(handler,tag,format,args...)
#endif

#if PP_QQ_LOG_LEVEL_ERROR >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_E4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_ERROR,format,##args)
#else
#	define PP_QQ_LOG_E4(handler,tag,format,args...)
#endif

#if PP_QQ_LOG_LEVEL_FATAL >= PP_QQ_CURRENT_LOG_LEVEL
#	define PP_QQ_LOG_F4(handler,tag,format,args...)	__PP_QQ_DO_LOG(handler,tag,PP_QQ_LOG_LEVEL_FATAL,format,##args)
#else
#	define PP_QQ_LOG_F4(handler,tag,format,args...)
#endif

#define PP_QQ_LOG_V3(tag,format,args...)	PP_QQ_LOG_V4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)
#define PP_QQ_LOG_D3(tag,format,args...)	PP_QQ_LOG_D4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)
#define PP_QQ_LOG_I3(tag,format,args...)	PP_QQ_LOG_I4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)
#define PP_QQ_LOG_W3(tag,format,args...)	PP_QQ_LOG_W4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)
#define PP_QQ_LOG_E3(tag,format,args...)	PP_QQ_LOG_E4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)
#define PP_QQ_LOG_F3(tag,format,args...)	PP_QQ_LOG_F4(PP_QQ_DEFAULT_HANDLER,tag,format,##args)

#define PP_QQ_LOG_V(format,args...)    PP_QQ_LOG_V3(PP_QQ_DEFAULT_LOG_TAG,format,##args)
#define PP_QQ_LOG_D(format,args...)    PP_QQ_LOG_D3(PP_QQ_DEFAULT_LOG_TAG,format,##args)
#define PP_QQ_LOG_I(format,args...)    PP_QQ_LOG_I3(PP_QQ_DEFAULT_LOG_TAG,format,##args)
#define PP_QQ_LOG_W(format,args...)    PP_QQ_LOG_W3(PP_QQ_DEFAULT_LOG_TAG,format,##args)
#define PP_QQ_LOG_E(format,args...)    PP_QQ_LOG_E3(PP_QQ_DEFAULT_LOG_TAG,format,##args)
#define PP_QQ_LOG_F(format,args...)    PP_QQ_LOG_F3(PP_QQ_DEFAULT_LOG_TAG,format,##args)

#endif // ORG_PP_QQ_COMMON_LOG_PP_QQ_LOG_H
