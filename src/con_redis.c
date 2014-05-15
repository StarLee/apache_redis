#include "con_redis.h"
#include "logs.h"
#include <hiredis/hiredis.h>
#include "redis_about.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void getSession(const char * key,p_session_v v)
{
	//log_init();//初始日志
	redisContext *rctx=modred_connect("127.0.0.1",6379);
	if(rctx->err==0)
	{
		char * comm=(char *)malloc(5+strlen(key));
		sprintf(comm,"GET %s",key);
		redisReply *reply=modred_command(comm,rctx);
		free(comm);
		if(reply!=NULL)
		{
			//printf("the result is%s\n",reply->str);
			v->value=(char *)malloc(reply->len);
			sprintf(v->value,"%s",reply->str);
			v->len=reply->len;
			modred_releaseReply(reply);
		}
		else
		{
			//LOG_TRACE("%s命令错误:%s\n",comm,reply->str);
		}
		modred_disconnect(rctx);
	}
	else
	{
		printf("redis conn error %s\n",rctx->errstr);
	}
	//LOG_TRACE("test");
	//log_close();//关闭日志
}
