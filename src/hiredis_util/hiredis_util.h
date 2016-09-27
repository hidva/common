#ifndef ORG_PP_QQ_COMMON_HIREDIS_UTIL_HIREDIS_UTIL_H
#define ORG_PP_QQ_COMMON_HIREDIS_UTIL_HIREDIS_UTIL_H

#include <time.h>
#include <stdarg.h>

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include <hiredis/hiredis.h>
#include <hiredis/async.h>


/**
 * 与 redisConnect() 语义一致,只不过使用了 std::unique_ptr 来进行封装.
 *
 * 此时并不会进行出错处理,因此该函数可能会返回 nullptr,或者一个出错的 redisContext.
 */
inline std::unique_ptr<redisContext,void(*)(redisContext *)>
RedisConnect(const char *host, int port) noexcept
{
    // 这里之所以不对 void(*)(redisContext*) 作个 typedef,是因为没想起来好名字@_@
    return std::unique_ptr<redisContext,void(*)(redisContext*)>(redisConnect(host, port), redisFree);
}

inline std::unique_ptr<redisContext,void(*)(redisContext *)>
RedisConnect(const char *host, int port, const struct timeval &tv) noexcept
{
    return std::unique_ptr<redisContext,void(*)(redisContext*)>(redisConnectWithTimeout(host, port, tv), redisFree);
}


inline std::unique_ptr<redisReply, void(*)(void*)>
RedisCommand(redisContext *c, const char *format, ...) noexcept
{
    va_list ap;
    va_start(ap,format);
    void *ret = redisvCommand(c,format,ap);
    va_end(ap);
    return std::unique_ptr<redisReply, void(*)(void*)>{static_cast<redisReply*>(ret), freeReplyObject};
}

inline std::unique_ptr<redisReply, void(*)(void*)>
RedisCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen) noexcept {
    return std::unique_ptr<redisReply, void(*)(void*)>{
        static_cast<redisReply*>(redisCommandArgv(c, argc, argv, argvlen)),
        freeReplyObject
    };
}

std::unique_ptr<redisReply, void(*)(void*)>
RedisCommandArgv(redisContext *c, const std::vector<std::string> &cmd);

int RedisAsyncCommandArgv(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata,
                          const std::vector<std::string> &cmd);

/**
 * 参见其实现.
 */
bool IsStringArray(const redisReply *reply) noexcept;

inline std::unique_ptr<redisReply, void(*)(void*)>
RedisGetReply(redisContext *ctx) noexcept
{
    void *reply = nullptr;
    redisGetReply(ctx, &reply);
    return std::unique_ptr<redisReply, void(*)(void*)>{static_cast<redisReply*>(reply), freeReplyObject};
}

void RedisReplyToString(std::string &str, const redisReply *reply);

inline std::ostream& operator<<(std::ostream &out, const redisReply &reply) {
    std::string str;
    RedisReplyToString(str, &reply);
    out << str;
    return out;
}

#endif // ORG_PP_QQ_COMMON_HIREDIS_UTIL_HIREDIS_UTIL_H
