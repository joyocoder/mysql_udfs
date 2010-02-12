/* 
*/

use mysql;

drop function dotf;
drop function doti;
drop function dist2f;
drop function dist2i;
drop function jit;
drop function tagcloud;
drop function isum;
drop function isumn;
drop function jsmap;
drop function jsreduce;

create function dotf                 returns REAL   soname 'libjoyo.so';
create function doti                 returns REAL   soname 'libjoyo.so';
create function dist2f               returns REAL   soname 'libjoyo.so';
create function dist2i               returns REAL   soname 'libjoyo.so';
create function jit                  returns INT    soname 'libjoyo.so';
create function jsmap                returns STRING soname 'libjoyo.so';
create aggregate function jsreduce   returns STRING soname 'libjoyo.so';
create aggregate function tagcloud   returns STRING soname 'libjoyo.so';
create aggregate function isum       returns STRING soname 'libjoyo.so';
create aggregate function isumn      returns STRING soname 'libjoyo.so';
