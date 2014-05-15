#include "logs.h"
#include <log4c.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
static log4c_category_t* log= NULL;
int log_init()
{
	int flag=log4c_init();
	if(flag!=0)
		return -1;
	log= log4c_category_get("six13log.log.app.application1");
}
void log_msg(int priority,const char * msg,...)
{

	va_list v;
	va_start(v,msg);
	log4c_category_vlog(log,priority,msg,v);	      
	va_end(v);

}

void log_trace(const char * file,int line,const  char * fun,const char * fmt,...)
{
	char * headFmt="at line %d in file %s : %s";
	char  newFmt[2048]={'\0'};
	int n=sprintf(newFmt,headFmt,line,file,fun);
	strcat(newFmt+n,fmt);
	va_list v;
	va_start(v,fmt);
	log4c_category_vlog(log,LOG4C_PRIORITY_TRACE,newFmt,v);
	va_end(v);

}
int log_close()
{
	return (log4c_fini());
}
