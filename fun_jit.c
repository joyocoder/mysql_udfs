
my_bool jit_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	if(args->arg_count!=2)
	{
		strcpy(message,	"JIT takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"JIT : first parameter is not a string");
			status = 1;
		} else if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"JIT : second parameter is not a string");
			status = 1;
		}
		else
		{
			status = 0;
		}
	}
	return status;
}

void jit_deinit(UDF_INIT *initid)
{
}


longlong jit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	char* vec1=(char*)args->args[0];
	char* vec2=(char*)args->args[1];
	int len1 = args->lengths[0];
	int len2 = args->lengths[1];
	char * V1 = vec1;
	char * V2 = vec2;
	char buffer[64];
	int idx1 = nextInt(&V1,buffer,&len1);
	int idx2 = nextInt(&V2,buffer,&len2);
	if(idx1==idx2)
	{
		return idx1;
	}
	while(len1 > 0 && len2 > 0)
	{
		if(idx1==idx2)
		{
			return idx1;
		}
		else if(idx1 < idx2)
		{
			idx1 = nextInt(&V1,buffer,&len1);
		}
		else
		{
			idx2 = nextInt(&V2,buffer,&len2);
		}
	}
	while(len1 > 0 && idx1 < idx2)
	{
		idx1 = nextInt(&V1,buffer,&len1);
	}
	while(len2 > 0 && idx1 > idx2)
	{
		idx2 = nextInt(&V2,buffer,&len2);
	}
	if(idx1==idx2)
	{
		return idx1;
	}

	return 0;
}
// END OF DOCUMENT
