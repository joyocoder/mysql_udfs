linux: \
 libjoyo.so

libjoyo.so: \
 
	gcc -Wall -fPIC -Imysql_include -I/usr/include -I. -shared libjoyo.c libjs.a -o libjoyo.so
