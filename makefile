apache_dev:mod_redis.c
	/usr/local/apache2/bin/apxs  -L /usr/local/lib/  -lhiredis  -I /usr/local/include/ -I /usr/local/apr/include/apr-1/ -I /usr/local/apache2/include/  -c -i  mod_redis.c
#mod_redis.o:mod_redis.c
#	/usr/local/apache2/bin/apxs -I /usr/local/include/ -I /usr/local/apr/include/apr-1/ -I /usr/local/apache2/include/  -c mod_redis.c
