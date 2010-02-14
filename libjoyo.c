/* 
*/


#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport) 
#else
#define DLLEXP
#endif

#ifdef STANDARD
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#endif
#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>
#include <stdlib.h>

#define XP_UNIX
#include "js/jsapi.h"


#include <ctype.h>

#ifdef HAVE_DLOPEN

#define LIBVERSION "mysql_udfs_joyo 0.5"

#ifdef	__cplusplus
extern "C" {
#endif

DLLEXP my_bool	dotf_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		dotf_deinit(UDF_INIT *initid);
DLLEXP double	dotf(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

DLLEXP my_bool	dist2f_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		dist2f_deinit(UDF_INIT *initid);
DLLEXP double	dist2f(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

DLLEXP my_bool	doti_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		doti_deinit(UDF_INIT *initid);
DLLEXP double	doti(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

DLLEXP my_bool	dist2i_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		dist2i_deinit(UDF_INIT *initid);
DLLEXP double	dist2i(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

DLLEXP my_bool	jit_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		jit_deinit(UDF_INIT *initid);
DLLEXP longlong	jit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

DLLEXP my_bool	jsmap_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void	jsmap_deinit(UDF_INIT *initid);
DLLEXP char *	jsmap(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


DLLEXP my_bool	jsreduce_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		jsreduce_deinit(UDF_INIT *initid);
DLLEXP void     jsreduce_clear( UDF_INIT* initid, char* is_null, char *error );
DLLEXP void     jsreduce_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error );
DLLEXP char *   jsreduce(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);



DLLEXP my_bool	tagcloud_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		tagcloud_deinit(UDF_INIT *initid);
DLLEXP void     tagcloud_clear( UDF_INIT* initid, char* is_null, char *error );
DLLEXP void     tagcloud_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error );
DLLEXP char *   tagcloud(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


DLLEXP my_bool	isum_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		isum_deinit(UDF_INIT *initid);
DLLEXP void     isum_clear( UDF_INIT* initid, char* is_null, char *error );
DLLEXP void     isum_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error );
DLLEXP char *   isum(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

DLLEXP my_bool	isumn_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message);
DLLEXP void		isumn_deinit(UDF_INIT *initid);
DLLEXP void     isumn_clear( UDF_INIT* initid, char* is_null, char *error );
DLLEXP void     isumn_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error );
DLLEXP char *   isumn(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


#ifdef	__cplusplus
}
#endif


double nextDouble(char ** input, char * buffer, int *len)
{
	char *x = input[0];
	int i,bound;

	x = input[0];
	bound= *len < 63 ? *len : 63; 
	if(bound==0) { return 0.0; }

	for(i = 0; i < bound; i++)
	{
		if(x[i]==0||x[i]==';'||x[i]==',')
		{
			buffer[i] = 0;
			input[0] = x + i+1;
			len[0] -= i + 1;
			return atof(buffer);
		}
		else
		{
			buffer[i] = x[i];
		}
	}
	buffer[bound] = 0;
	len[0] = 0;
	input[0] += len[0];
	return atof(buffer);
}


int nextInt(char ** input, char * buffer, int *len)
{
	char *x = input[0];
	int i,bound;

	x = input[0];
	bound= *len < 63 ? *len : 63; 
	if(bound==0) { return 0; }

	for(i = 0; i < bound; i++)
	{
		if(x[i]==0||x[i]==';'||x[i]==',')
		{
			buffer[i] = 0;
			input[0] = x + i+1;
			len[0] -= i + 1;
			return atoi(buffer);
		}
		else
		{
			buffer[i] = x[i];
		}
	}
	buffer[bound] = 0;
	len[0] = 0;
	input[0] += len[0];
	return atoi(buffer);
}


int nextWord(char ** input, char * buffer, int *len, int *wlen)
{
	char *x = input[0];
	int i,bound;

	buffer[0] = 0;
	x = input[0];
	bound= *len < 63 ? *len : 63; 
	if(bound==0) { return 0; }
	for(i = 0; i < bound; i++)
	{
		if(x[i]==0||x[i]==';'||x[i]==',')
		{
			buffer[i] = 0;
			input[0] = x + i+1;
			len[0] -= i + 1;
			wlen[0] = i;
			return 1;
		}
		else
		{
			buffer[i] = x[i];
		}
	}
	buffer[i] = 0;
	len[0] = 0;
	wlen[0] = i;
	input[0] += i;
	return 1;
}

#include "fun_dot.c"
#include "fun_dist2.c"
#include "fun_jit.c"
#include "fun_tagcloud.c"
#include "fun_iops.c"
#include "core_js.c"
#include "fun_jsmap.c"
#include "fun_jsreduce.c"

#endif /* HAVE_DLOPEN */

