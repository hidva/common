#ifndef ORG_PP_QQ_COMMON_JNI_UTILS_JNI_UTILS_H
#define ORG_PP_QQ_COMMON_JNI_UTILS_JNI_UTILS_H

#include <jni.h>
#include "exception/errno_exception.h"

/**
 * 将 jstr 指向着的 java string 转化为 UTF-8 之后追加到 str 之后.
 */
template <typename StringType>
void AppendJavaString(StringType &bytes,JNIEnv *env,jstring jstr)
{
    PP_QQ_CHECK(jstr != nullptr && env != nullptr);

    const char *utf8_bytes_ptr = env->GetStringUTFChars(jstr,nullptr);
    PP_QQ_CHECK(utf8_bytes_ptr != nullptr);
    ON_SCOPE_EXIT (release_string_utf_chars) {
        env->ReleaseStringUTFChars(jstr,utf8_bytes_ptr);
    };

    jsize utf8_bytes_size = env->GetStringUTFLength(jstr);
    PP_QQ_CHECK(utf8_bytes_size >= 0);

    bytes.append(utf8_bytes_ptr,utf8_bytes_size);
    return ;
}

/**
 * 根据 jstr 指向着的 Java String 的 UTF-8 编码字节数组创建一个 StringType 对象.
 */
template <typename StringType>
StringType CreateStringFromJavaString(JNIEnv *env,jstring jstr)
{
    StringType str;
    AppendJavaString(str,env,jstr);
    return str;
}


#endif // ORG_PP_QQ_COMMON_JNI_UTILS_JNI_UTILS_H
