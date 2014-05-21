#ifndef LOGS_H
#define	LOGS_H
extern void log_msg(int priority,const  char * msg,...);
extern void log_trace(const  char * file,int line,const  char * fun,const char *fmt,...);
extern int log_init();
extern int log_close();
#define LOG_ERROR(fmt,args...) log_msg(ZLOG_LEVEL_ERROR,fmt,##args)
#define LOG_WARN(fmt,args...) log_msg(ZLOG_LEVEL_WARN,fmt,##args)
#define LOG_INFO(fmt,args...) log_msg(ZLOG_LEVEL_INFO,fmt,##args)
#define LOG_DEBUG(fmt,args...) log_msg(ZLOG_LEVEL_DEBUG,fmt,##args)
#define LOG_TRACE(fmt,args...) log_trace(__FILE__,__LINE__,__FUNCTION__,fmt,##args)
#endif ///LOGS_H

