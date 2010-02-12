linux: 
 libjoyo.so

libjoyo.so: \
 gcc -Wall -fPIC -Ijs -Imysql_include -Ijs -I. -shared libjoyo.c libjs.a -o libjoyo.so
