#include "logs.h"
#include <zlog.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
static zlog_category_t* log= NULL;
int log_init()
{
	if(log)
		return 0;
	int flag=zlog_init("/data/zlog.conf");
	if(flag!=0)
		return -1;
	log= zlog_get_category("my_cat");
	if(!log)
	{
		zlog_fini();
		return -2;
	}
}
void log_msg(int priority,const char * msg,...)
{
	if(!log)
		log_init();
	va_list v;
	va_start(v,msg);
	vzlog(log, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__,priority, msg,v);
	va_end(v);
}

void log_trace(const char * file,int line,const  char * fun,const char * fmt,...)
{
	if(!log)
		log_init();
	char * headFmt="at line %d in file %s:%s ";
	char  newFmt[2048]={'\0'};
	int n=sprintf(newFmt,headFmt,line,file,fun);
	strcat(newFmt+n,fmt);
	va_list v;
	va_start(v,fmt);
	vzlog(log, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, ZLOG_LEVEL_DEBUG, newFmt,v);
	va_end(v);
}
int log_close()
{
	int returnCode=0;
	if(log!=NULL)
	{
		zlog_fini();
		log=NULL;
	}
	return returnCode;
}
