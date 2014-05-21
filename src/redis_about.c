#include "redis_about.h"
#include <hiredis/hiredis.h>
#include "logs.h"
redisContext * modred_connect(const char * hostname,int port)
{
	redisContext *rcxt=NULL;
	struct timeval timeout={1,50000};
	rcxt=redisConnectWithTimeout(hostname,port,timeout);
	if(rcxt==NULL||rcxt->err)
	{
		if(rcxt)
		{
			LOG_TRACE("Connection error:%s\n",rcxt->errstr);
			redisFree(rcxt);
		}
		else
		{
			LOG_TRACE("Connection error:can't allocate redis context\n");
		}
	}
	return rcxt;
}

void modred_disconnect(redisContext * rcxt)
{
	redisFree(rcxt);
}
redisReply * modred_command(const char *command,redisContext *context)
{
	redisReply * reply=redisCommand(context,command);
	return reply;
}
void modred_releaseReply(redisReply * reply)
{
	freeReplyObject(reply);
}
