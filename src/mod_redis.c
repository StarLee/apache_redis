/* Include the required headers from httpd */
#include <httpd.h>
#include <http_core.h>
#include <http_protocol.h>
//#include <http_log.h>
#include <http_request.h>
#include <hiredis/hiredis.h>
#include <apr_buckets.h>
#include <apr_general.h>
#include <util_filter.h>
#include <apr_file_io.h>
#include <apr_lib.h>
#include <ctype.h>
#include <stdio.h>
#include <apr_strings.h>
#include <stdbool.h>
#include "con_redis.h" 
#include "logs.h"
/* Define prototypes of our functions in this module */
static const char * setRotationFilterEnable(cmd_parms * params,void *sconf,int flag); 
static const char * setErrorPage(cmd_parms *params,void * sconf,const char *flag);
static const char * setType(cmd_parms *params,void * sconf,const char *flag);
static const command_rec redisCmds[]={
	AP_INIT_FLAG("NotationFilter",setRotationFilterEnable,NULL,RSRC_CONF,"filter the $$xx$$ notatioin"),
	AP_INIT_TAKE1("ErrorPage",setErrorPage,NULL,RSRC_CONF|OR_AUTHCFG,"set the error page"),
	AP_INIT_TAKE1("Type",setType,NULL,RSRC_CONF|OR_AUTHCFG,"set the error page"),
	{NULL}
};

static void *  redisCreateServerConfig(apr_pool_t * p,server_rec *s);
static void register_hooks(apr_pool_t *pool); 

module AP_MODULE_DECLARE_DATA   redis_module =
{
		STANDARD20_MODULE_STUFF,
		NULL,            // Per-directory configuration handler
		NULL,            // Merge handler for per-directory configurations
		redisCreateServerConfig,            // Per-server configuration handler
		NULL,            // Merge handler for per-server configurations
		redisCmds,            // Any directives we may have for httpd
		register_hooks   // Our hook registering wfunction
};

typedef struct
{
	int notationEnabled;
	char * url;
	char * type;
}redisConfig;
/**
 * 过滤掉特殊符号，不要进入输出
 */
static apr_status_t notationFilterOutFilter(ap_filter_t *f,
                                        apr_bucket_brigade *pbbIn)
    {
    request_rec *r = f->r;
    conn_rec *c = r->connection;
    apr_bucket *pbktIn;
    apr_bucket_brigade *pbbOut;

    pbbOut=apr_brigade_create(r->pool, c->bucket_alloc);
    for (pbktIn = APR_BRIGADE_FIRST(pbbIn);
         pbktIn != APR_BRIGADE_SENTINEL(pbbIn);
         pbktIn = APR_BUCKET_NEXT(pbktIn))
    {
		const char *data;
		apr_size_t len;
		char *buf;
		char *newdata;
		apr_size_t n;
		apr_bucket *pbktOut;

		if(APR_BUCKET_IS_EOS(pbktIn))
		{
			apr_bucket *pbktEOS=apr_bucket_eos_create(c->bucket_alloc);
			APR_BRIGADE_INSERT_TAIL(pbbOut,pbktEOS);
			continue;
		}

		/* read */
		apr_bucket_read(pbktIn,&data,&len,APR_BLOCK_READ);

		/* write */
		buf = apr_bucket_alloc(len, c->bucket_alloc);
		newdata=apr_bucket_alloc(len,c->bucket_alloc);
		bool start_flag=false;	
		bool end_flag=false;	
		int start=0;//equals end
		int begin=0;
		int content=0;//record the real content
		for(n=0 ; n < len; ++n)
		{
			if(data[n]=='$'&&data[n+1]=='$'&&!start_flag)
			{
				n++;
				start_flag=true;
				begin=n;
				continue;
			}
			if(start_flag&&!end_flag)
			{
				if(data[n]=='$'&&data[n+1]=='$')
				{
					n++;
					end_flag=true;
					continue;
				}
				if(data[n]!=' ')
				{
					buf[start]=data[n];
					start++;
					continue;
				}
			}

			else
			{
				newdata[content]=data[n];
				content++;
			}
			//buf[n] = apr_toupper(data[n]);
			//buf[n] ='a';
		}
		/*if(start!=0&&end_flag)
		{
			apr_bucket *pbetout;
			//char *other=apr_bucket_alloc(start, c->bucket_alloc);
			//apr_cpystrn(other,buf,start);
			buf[start]='\0';
			session_v  v={NULL,0};
			getSession(buf,&v);
			pbetout = apr_bucket_heap_create(v.value,v.len,NULL,
					c->bucket_alloc);
			APR_BRIGADE_INSERT_TAIL(pbbOut,pbetout);
		}*/

		pbktOut = apr_bucket_heap_create(newdata, content, apr_bucket_free,
				c->bucket_alloc);
		APR_BRIGADE_INSERT_TAIL(pbbOut,pbktOut);
	}
    apr_brigade_cleanup(pbbIn);
    return ap_pass_brigade(f->next,pbbOut);
    }
static void notationFilterInsertFilter(request_rec *r)
{
	redisConfig *pConfig=ap_get_module_config(r->server->module_config,
			&redis_module);

	if(!pConfig->notationEnabled)
		return;
	ap_add_output_filter("NOTATION",NULL,r,r->connection);
}

static int sessionHandle(request_rec *r)
{
	/*if (!r->handler || strcmp(r->handler, "redis_module"))
	   	return DECLINED;*/

	redisConfig * config=ap_get_module_config(r->server->module_config,&redis_module);
	if(config->type==NULL||config->url==NULL)
	{
		return DECLINED;
	}
	char *filename=r->filename;
	char *type=	strrchr(filename,'.');
	if(type)
	{
		type+=1;
		if(strcmp(type,config->type))
		{
			return DECLINED;
		}
	}
	ap_set_content_type(r, "text/html");
	//apr_table_set(r->headers_out,"ABC",reply->str);
	apr_finfo_t finfo=r->finfo;
	apr_file_t *fd=	NULL;	
	if(apr_file_open(&fd,r->filename, APR_FOPEN_READ, APR_OS_DEFAULT,r->pool)==APR_SUCCESS)
	{
		bool start_flag=false;
		bool end_flag=false;
		char mark[1024]={'\0'};
		int n=0;
		session_v  v={NULL,0};
		while(apr_file_eof(fd)==APR_SUCCESS)
		{
			char flag;
			apr_file_getc(&flag,fd);
			if(flag=='$')
			{
				if(apr_file_eof(fd)!=APR_EOF)
				{
					apr_file_getc(&flag,fd);
					if(flag=='$')
					{
						if(!start_flag)
						{
							start_flag=true;
							continue;
						}
						else
						{
							end_flag=true;
							continue;
						}
					}
				}
			}
			if(start_flag&&!end_flag)
			{
				mark[n]=flag;
				n++;
			}
		}
		if(end_flag)
		{
			getSession(mark,&v);
		}
		if(v.value==NULL)
		{
			return DECLINED;
		}
		apr_size_t siz;
		//apr_finfo_t *finfof=fd->finfo;
		if(strcmp(v.value,"123")==0)
		{
			ap_send_fd(fd,r,0,finfo.size,&siz);
		}
		else
		{
			apr_file_t *fdd=	NULL;
			LOG_TRACE("未登陆");
			if(apr_file_open(&fdd,config->url,APR_FOPEN_READ, APR_OS_DEFAULT,r->pool)==APR_SUCCESS)
			{
				apr_finfo_t finfo_l;
				apr_file_info_get(&finfo_l,APR_FINFO_SIZE,fdd);
				ap_send_fd(fdd,r,0,finfo_l.size,&siz);
				apr_file_close(fdd);
			}

		}
		apr_file_close(fd);
	}
	else
	{
		ap_rputs("the open error",r);
	}
	return OK;
}
static void *  redisCreateServerConfig(apr_pool_t * p,server_rec *s)
{
	redisConfig * sConfig=apr_palloc(p,sizeof(redisConfig));
	sConfig->notationEnabled=0;
	sConfig->url=NULL;
	sConfig->type=NULL;
   	return sConfig;
}
static const char * setRotationFilterEnable(cmd_parms * params,void *sconf,int flag) 
{
	redisConfig * sConfig=ap_get_module_config(params->server->module_config,&redis_module);
	sConfig->notationEnabled=flag;
	return NULL; 
}
static const char * setErrorPage(cmd_parms *params,void * sconf,const char *flag)
{
	redisConfig * sConfig=ap_get_module_config(params->server->module_config,&redis_module);
	sConfig->url=flag;
	return NULL;
}

static const char * setType(cmd_parms *params,void * sconf,const char *flag)
{
	redisConfig * sConfig=ap_get_module_config(params->server->module_config,&redis_module);
	sConfig->type=flag;
	return NULL;

}
/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
	ap_hook_handler(sessionHandle, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_insert_filter(notationFilterInsertFilter,NULL,NULL,APR_HOOK_MIDDLE);//need
	ap_register_output_filter("NOTATION",notationFilterOutFilter,NULL,
			AP_FTYPE_RESOURCE);
}



