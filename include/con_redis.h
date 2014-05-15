#ifndef CON_REDIS_H
#define CON_REDIS_H
typedef struct
{
	char * value;
	int len;
}session_v,* p_session_v;
void getSession(const char * key,p_session_v);
#endif
