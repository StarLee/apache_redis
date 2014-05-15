#ifndef REDIS_ABOUT_H
#define REDIS_ABOUT_H
#include <hiredis/hiredis.h>
redisContext * modred_connect(const char * ip,const int port);
void modred_disconnect(redisContext * rcxt);
redisReply * modred_command(const char * command,redisContext *rctx);
void modred_releaseReply(redisReply * reply);

#endif //REDIS_ABOUT_H
