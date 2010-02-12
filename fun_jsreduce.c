my_bool	jsreduce_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	status = 0;
	initid->ptr = 0;
	if(args->arg_count!=2)
	{
		strcpy(message,	"JSREDUCE takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"JSREDUCE : first parameter is not a string");
			status = 1;
		}
		if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"JSREDUCE : first parameter is not a string");
			status = 1;
		}
		if(status==0)
		{
			
		}
	}
	return status;
}
void	jsreduce_deinit(UDF_INIT *initid)
{
}
void    jsreduce_clear( UDF_INIT* initid, char* is_null, char *error )
{
}
void    jsreduce_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error )
{
}
char *  jsreduce(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	return result;
}
