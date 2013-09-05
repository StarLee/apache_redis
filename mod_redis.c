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
/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static apr_status_t CaseFilter(ap_filter_t *f,apr_bucket_brigade *pbbIn);
static int fixHandle(request_rec *r);
static void filter_handler(request_rec *r);
static const char *filterName="CaseFilter";
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

		/* Hook the request handler */
		//ap_hook_insert_filter(example_handler, NULL, NULL, APR_HOOK_MIDDLE);
		//ap_register_output_filter_protocol(filterName,CaseFilter,NULL,AP_FTYPE_RESOURCE,AP_FILTER_PROTO_CHANGE);
		ap_hook_handler(fixHandle, NULL, NULL, APR_HOOK_MIDDLE);
}
static apr_status_t CaseFilter(ap_filter_t *f,apr_bucket_brigade *pbbIn)
{
		request_rec *r = f->r;
		conn_rec *c = r->connection;
		apr_bucket *pbktIn;
		apr_bucket_brigade *pbbOut;
		fixHandle(r);
		pbbOut=apr_brigade_create(r->pool, c->bucket_alloc);
		for (pbktIn = APR_BRIGADE_FIRST(pbbIn);pbktIn != APR_BRIGADE_SENTINEL(pbbIn);pbktIn = APR_BUCKET_NEXT(pbktIn))
		{
				const char *data;
				apr_size_t len;
				char *buf;
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
				for(n=0 ; n < len ; ++n)
						buf[n] = apr_toupper(data[n]);

				pbktOut = apr_bucket_heap_create(buf, len, apr_bucket_free,
								c->bucket_alloc);
				APR_BRIGADE_INSERT_TAIL(pbbOut,pbktOut);
		}

		return ap_pass_brigade(f->next,pbbOut);
}

static int fixHandle(request_rec *r)
{

	//if (!r->handler || strcmp(r->handler, "fixHandle")) return DECLINED;
			redisContext *rcxt;
			redisReply *reply;
			const char *hostname="127.0.0.1";
			int port=6379;
			struct timeval timeout={1,50000};
			rcxt=redisConnectWithTimeout(hostname,port,timeout);
			if(rcxt==NULL||rcxt->err)
			{
					if(rcxt)
					{
							printf("Connection error:%s\n",rcxt->errstr);
							redisFree(rcxt);
					}
					else
					{
							printf("Connection error:can't allocate redis context\n");
					}
					exit(1);
			}
			reply = redisCommand(rcxt,"GET foo");
			ap_set_content_type(r, "text/html");
			apr_table_set(r->headers_out,"ABC",reply->str);
			ap_rputs(apr_table_get(r->headers_in,"Host"),r);
			ap_rputs(reply->str, r);
			ap_rputs(r->uri,r);
			freeReplyObject(reply);
			FILE *f;
			f=fopen(r->filename,"r");
			if(f==NULL)
			{
				ap_rputs("the file doesn't exist",r);
				return OK;
			}
			char *a=NULL;
			a=(char *)malloc(1024);
			fread(a,1024 ,1,f);
			apr_finfo_t finfo=r->finfo;
			apr_file_t *fd=	NULL;	
			if(apr_file_open(&fd,r->filename, APR_FOPEN_READ, APR_OS_DEFAULT,r->pool)!=APR_SUCCESS)
			{
				ap_rputs(a,r);
				ap_rputs(finfo.fname, r);
				ap_rputs("dfd",r);
			}
			else
			{
				ap_rputs("the open success",r);
				apr_size_t siz;
				//apr_finfo_t *finfof=fd->finfo;
				char *length=apr_off_t_toa(r->pool,finfo.size);
				ap_rputs(length,r);	
				ap_send_fd(fd,r,0,finfo.size,&siz);
				ap_rputs(apr_itoa(r->pool, siz),r);	
			}
			apr_file_close(fd);
			return OK;
}

/* The handler function for our module.
 * This is where all the fun happens!
 */

static void filter_handler(request_rec *r)
{
		ap_add_output_filter(filterName,NULL,r,r->connection);
}
