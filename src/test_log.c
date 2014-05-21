#include "logs.h"
#include <hiredis/hiredis.h>
#include "redis_about.h"
#include <stdio.h>
int main()
{
	log_init();//初始日志
	redisContext *rctx=modred_connect("127.0.0.1",6379);
	if(rctx->err==0)
	{
		char * comm="GET test";
		redisReply *reply=modred_command(comm,rctx);
		if(reply!=NULL)
		{
			printf("the result is%s\n",reply->str);
			modred_releaseReply(reply);

		}
		else
		{
			LOG_TRACE("%s命令错误:%s\n",comm,reply->str);
		}
		modred_disconnect(rctx);
	}
	else
	{
		printf("redis conn error %s\n",rctx->errstr);
	}
	LOG_TRACE("test");
	log_close();//关闭日志
	log_init();
	LOG_TRACE("test");
	log_close();
	return 0;
}
