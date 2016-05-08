#ifndef PP_QQ_COMMON_COMMON_CONFIG_H
#define PP_QQ_COMMON_COMMON_CONFIG_H

#if defined(ANDROID) || defined(__ANDROID__)
#include <android/api-level.h>
#define PP_QQ_COMMON_ON_ANDROID __ANDROID_API__
#else
#undef PP_QQ_COMMON_ON_ANDROID
#endif



#endif // PP_QQ_COMMON_COMMON_CONFIG_H

