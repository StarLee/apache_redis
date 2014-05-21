apache_redis
============
过滤静态页面<br/>
需要用zlog进行日志依赖，没有使用apache http自带的日志<br/>
需要redis的支持<br/>
zlog日志的配置文件权限要求是777，否则会出错，调试了好久zlog才发现这个问题<br/>
