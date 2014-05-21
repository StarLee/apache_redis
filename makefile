VPATH=./include:./src:./bin
OBJ=./bin/
CC=gcc -g -I ./include/ -I /usr/local/include/ -I /usr/local/include/log4c/ -L /usr/local/lib/  -lzlog -lhiredis
#apache_dev:$(OBJ)mod_redis.o
#	/usr/local/apache2/bin/apxs  -L /usr/local/lib/  -lhiredis  -I /usr/local/include/ -I /usr/local/apr/include/apr-1/ -I /usr/local/apache2/include/  -c -i $< 
#$(OBJ)mod_case_filter.o:mod_case_filter.c
#	gcc -I /usr/local/include/ -I /usr/local/apr/include/apr-1/ -I/usr/local/apache2/include/  -c  -o $@ $<
all:$(OBJ)con_redis.o $(OBJ)logs.o $(OBJ)redis_about.o
	$(CC) -shared -fPIC -o libxxsession.so $(OBJ)con_redis.o $(OBJ)logs.o $(OBJ)redis_about.o
$(OBJ)con_redis.o:con_redis.c con_redis.h 
	$(CC) -fPIC -c -o $@ $<
$(OBJ)logs.o:logs.c logs.h
	$(CC) -fPIC -c -o $@ $< 
$(OBJ)redis_about.o:redis_about.c redis_about.h
	$(CC) -fPIC -c -o $@ $<
$(OBJ)test_log.o:test_log.c
	$(CC) -c -o $@ $<
test:$(OBJ)test_log.o 
	$(CC) -lxxsession -o $@ $< 
install:
	cp /home/starlee/dev/apache_redis/libxxsession.so /usr/local/lib/
	/usr/local/apache2/bin/apxs -I ./include -c -L/usr/local/lib -lxxsession -i -Wc,-O0 ./src/mod_case_filter.c
clean:
	rm ./bin/*.o
install_fix:
	/usr/local/apache2/bin/apxs -I ./include -c -L/usr/local/lib -lhiredis -lxxsession -i -Wc,-O0 ./src/mod_redis.c
.PHONY:all install clean install_fix 
