
my_bool dotf_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	if(args->arg_count!=2)
	{
		strcpy(message,	"DOTV takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"DOTV : first parameter is not a string");
			status = 1;
		} else if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"DOTV : second parameter is not a string");
			status = 1;
		}
		else
		{
			status = 0;
		}
	}
	return status;
}

void dotf_deinit(UDF_INIT *initid)
{
}



my_bool doti_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	if(args->arg_count!=2)
	{
		strcpy(message,	"DOTI takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"DOTI : first parameter is not a string");
			status = 1;
		} else if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"DOTI: second parameter is not a string");
			status = 1;
		}
		else
		{
			status = 0;
		}
	}
	return status;
}

void doti_deinit(UDF_INIT *initid)
{
}



double dotf(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	char* vec1=(char*)args->args[0];
	char* vec2=(char*)args->args[1];
	int len1 = args->lengths[0];
	int len2 = args->lengths[1];
	char * V1 = vec1;
	char * V2 = vec2;
	double dp = 0.0;
	char buffer[64];
	while(len1 > 0 && len2 > 0)
	{
		dp += nextDouble(&V1,buffer,&len1) * nextDouble(&V2,buffer,&len2);
	}
	return dp;
}



double doti(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	char* vec1=(char *)args->args[0];
	char* vec2=(char *)args->args[1];
	int len1 = args->lengths[0];
	int len2 = args->lengths[1];
	char * V1 = vec1;
	char * V2 = vec2;
	double dp = 0.0;
	char buffer[64];
	int idx1 = nextInt(&V1,buffer,&len1);
	int idx2 = nextInt(&V2,buffer,&len2);
	while(len1 > 0 && len2 > 0)
	{
		if(idx1==idx2)
		{
			dp += nextDouble(&V1,buffer,&len1) * nextDouble(&V2,buffer,&len2);
			idx1 = nextInt(&V1,buffer,&len1);
			idx2 = nextInt(&V2,buffer,&len2);
		}
		else if(idx1 < idx2)
		{
			// progress idx1
			nextDouble(&V1,buffer,&len1);
			idx1 = nextInt(&V1,buffer,&len1);
		}
		else
		{
			// progress idx2
			nextDouble(&V2,buffer,&len2);
			idx2 = nextInt(&V2,buffer,&len2);
		}
	}
	return dp;
}

// END OF DOCUMENT
