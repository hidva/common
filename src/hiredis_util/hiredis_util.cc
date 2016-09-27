#include "hiredis_util/hiredis_util.h"
#include "exception/errno_exception.h"
#include "formatter/stdio_formatter.h"

bool IsStringArray(const redisReply *reply) noexcept
{
    if (reply->type != REDIS_REPLY_ARRAY)
        return false;

    for (size_t i = 0; i < reply->elements; ++i) {
        auto ele_type = reply->element[i]->type;
        if (ele_type != REDIS_REPLY_ERROR && ele_type != REDIS_REPLY_STATUS && ele_type != REDIS_REPLY_STRING)
            return false;
    }

    return true;
}


void RedisReplyToString(std::string &str, const redisReply *reply) {

    if (reply == nullptr)
        str.append("reply **IS** nullptr");

    switch (reply->type) {
    case REDIS_REPLY_STATUS:
        str.append("type: REDIS_REPLY_STATUS,val: ").append(reply->str, reply->len);
        break ;
    case REDIS_REPLY_ERROR:
        str.append("type: REDIS_REPLY_ERROR,val: ").append(reply->str, reply->len);
        break ;
    case REDIS_REPLY_STRING:
        str.append("type: REDIS_REPLY_STRING,val: ").append(reply->str, reply->len);
        break ;
    case REDIS_REPLY_NIL:
        str.append("type: REDIS_REPLY_NIL");
        break ;
    case REDIS_REPLY_INTEGER:
        AppendStdioFormat(str, "type: REDIS_REPLY_INTEGER,val: %lld", reply->integer);
        break ;
    case REDIS_REPLY_ARRAY:
        str.append("type: REDIS_REPLY_ARRAY");
        break ;
    default:
        AppendStdioFormat(str, "type: %d(Unknown)", reply->type);
        break ;
    }

    return ;
}
std::unique_ptr<redisReply, void(*)(void*)>
RedisCommandArgv(redisContext *c, const std::vector<std::string> &cmd) {
    std::vector<const char *> cmd_argv;
    std::vector<size_t> cmd_argv_size;
    cmd_argv.reserve(cmd.size());
    cmd_argv_size.reserve(cmd.size());

    for (const std::string &cmd_com : cmd) {
        cmd_argv.push_back(cmd_com.data());
        cmd_argv_size.push_back(cmd_com.size());
    }

    return RedisCommandArgv(c, cmd_argv.size(), cmd_argv.data(), cmd_argv_size.data());
}

int RedisAsyncCommandArgv(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata,
                          const std::vector<std::string> &cmd) {
    std::vector<const char *> cmd_argv;
    std::vector<size_t> cmd_argv_size;
    cmd_argv.reserve(cmd.size());
    cmd_argv_size.reserve(cmd.size());

    for (const std::string &cmd_com : cmd) {
        cmd_argv.push_back(cmd_com.data());
        cmd_argv_size.push_back(cmd_com.size());
    }

    return redisAsyncCommandArgv(ac, fn, privdata, cmd_argv.size(), cmd_argv.data(), cmd_argv_size.data());
}

