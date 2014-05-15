/* Include the required headers from httpd */
#include <httpd.h>
#include <http_core.h>
#include <http_protocol.h>
#include <http_log.h>
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
/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static apr_status_t CaseFilter(ap_filter_t *f,apr_bucket_brigade *pbbIn);
static int fixHandle(request_rec *r);
static void filter_handler(request_rec *r);
/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA   redis_module =
{
		STANDARD20_MODULE_STUFF,
		NULL,            // Per-directory configuration handler
		NULL,            // Merge handler for per-directory configurations
		NULL,            // Per-server configuration handler
		NULL,            // Merge handler for per-server configurations
		NULL,            // Any directives we may have for httpd
		register_hooks   // Our hook registering wfunction
};


/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
		ap_hook_handler(fixHandle, NULL, NULL, APR_HOOK_MIDDLE);
}

static void getKey()
{

}

static int fixHandle(request_rec *r)
{
	if (!r->handler || strcmp(r->handler, "redis_module"))
	   	return DECLINED;
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
		apr_size_t siz;
		//apr_finfo_t *finfof=fd->finfo;
		if(strcmp(v.value,"123")==0)
		{
			ap_send_fd(fd,r,0,finfo.size,&siz);
		}
		else
		{
			apr_file_t *fdd=	NULL;
			if(apr_file_open(&fdd,"/data/test/group.html", APR_FOPEN_READ, APR_OS_DEFAULT,r->pool)==APR_SUCCESS)
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
		ap_rputs("the open success error",r);
	}
	return OK;
}

