#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/*Module config*/
/*模块配置信息结构,根据Nginx模块开发规则
  这个结构的命名规则为 ngx_http_[module-name]_[main|srv|loc]_conf_t
  其中main,srv和loc分别用于表示同一模块在三层block中的配置信息*/
typedef struct {

	ngx_str_t text;

}ngx_http_xxxx_loc_conf_t;


static char* ngx_http_xxxx_set(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);
static void* ngx_http_xxxx_create_loc_conf(ngx_conf_t* cf);
static char* ngx_http_xxxx_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child);


/*Directives*/
/*指令数组的命名规则为 ngx_http_[module-name]_commands
  注意数组最后一个元素要是ngx_null_command结束*/
static ngx_command_t ngx_http_xxxx_commands[] = {

	{
		ngx_string("xxxx"),
		NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
		ngx_http_xxxx_set,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_xxxx_loc_conf_t,text),
		NULL
	},

	ngx_null_command
};

/*http context of the module*/
/*定义一个ngx_http_module_t类型的结构体变量
  命名规则为 ngx_http_[module-name]_module_ctx*/
static ngx_http_module_t ngx_http_xxxx_module_ctx = {

	NULL,                                  /* preconfiguration */
	NULL,                                  /* postconfiguration */

	NULL,                                  /* create main configuration */
	NULL,                                  /* init main configuration */

	NULL,                                  /* create server configuration */
	NULL,                                  /* merge server configuration */

	ngx_http_xxxx_create_loc_conf,         /* create location configration */
	ngx_http_xxxx_merge_loc_conf           /* merge location configration */

		/*一共有8个Hook注入点,分别会在不同时刻被Nginx调用
	  这两个函数会被Nginx自动调用
	  命名规则 ngx_http_[module-name]_[create|merge]_[main|srv|loc]_conf*/
};

/*Module*/
ngx_module_t ngx_http_xxxx_module = {
	NGX_MODULE_V1,                        
	&ngx_http_xxxx_module_ctx,             /* module context */
	ngx_http_xxxx_commands,                /* module directives */
	NGX_HTTP_MODULE,                       /* module type */
	NULL,                                  /* init master */
	NULL,                                  /* init module */
	NULL,                                  /* init process */
	NULL,                                  /* init thread */
	NULL,                                  /* exit thread */
	NULL,                                  /* exit process */
	NULL,                                  /* exit master */
	NGX_MODULE_V1_PADDING
};

/*handler function*/
/*编写Handler:它主要有以下四项职责
	读入模块配置
	处理功能业务
	产生HTTP header
	产生HTTP body */
static ngx_int_t
ngx_http_xxxx_handler(ngx_http_request_t* r)
{
	ngx_int_t rc;
	ngx_buf_t* b;
	ngx_chain_t out;

	ngx_http_xxxx_loc_conf_t* xlcf;
	xlcf = ngx_http_get_module_loc_conf(r, ngx_http_xxxx_module);

	if(!(r->method & (NGX_HTTP_HEAD|NGX_HTTP_GET|NGX_HTTP_POST)))
		return NGX_HTTP_NOT_ALLOWED;

	r->headers_out.content_type.len = sizeof("text/html") - 1;
	r->headers_out.content_type.data = (u_char*)"text/html";

	r->headers_out.status = NGX_HTTP_OK;
	r->headers_out.content_length_n = xlcf->text.len;

	if(r->method == NGX_HTTP_HEAD){

		rc = ngx_http_send_header(r);
		if(rc != NGX_OK)
			return rc;
	}

	b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
	if(b == NULL){
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	out.buf = b;
	out.next = NULL;

	b->pos = xlcf->text.data;
	b->last = xlcf->text.data + (xlcf->text.len);
	b->memory = 1;
	b->last_buf = 1;

	rc = ngx_http_send_header(r);
	if(rc != NGX_OK)
		return rc;

	return ngx_http_output_filter(r, &out);
}

static char* 
ngx_http_xxxx_set(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
	ngx_http_core_loc_conf_t* clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_xxxx_handler;
	ngx_conf_set_str_slot(cf, cmd, conf);

	return NGX_CONF_OK;
}

static void*
ngx_http_xxxx_create_loc_conf(ngx_conf_t* cf)
{
	ngx_http_xxxx_loc_conf_t* conf;

	conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_xxxx_loc_conf_t));
	if(conf == NULL){
		return NGX_CONF_ERROR;
	}

	conf->text.len = 0;
	conf->text.data = NULL;

	return conf;
}

static char*
ngx_http_xxxx_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child)
{
	ngx_http_xxxx_loc_conf_t* prev = parent;
	ngx_http_xxxx_loc_conf_t* conf = child;

	ngx_conf_merge_str_value(conf->text, prev->text, "");

	return NGX_CONF_OK;
}