


my_bool dist2f_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	if(args->arg_count!=2)
	{
		strcpy(message,	"DIST2F takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"DIST2F : first parameter is not a string");
			status = 1;
		} else if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"DIST2F : second parameter is not a string");
			status = 1;
		}
		else
		{
			status = 0;
		}
	}
	return status;
}

void dist2f_deinit(UDF_INIT *initid)
{
}


my_bool dist2i_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	if(args->arg_count!=2)
	{
		strcpy(message,	"DIST2I takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"DIST2I : first parameter is not a string");
			status = 1;
		} else if(args->arg_type[1] != STRING_RESULT)
		{
			strcpy(message,	"DIST2I: second parameter is not a string");
			status = 1;
		}
		else
		{
			status = 0;
		}
	}
	return status;
}

void dist2i_deinit(UDF_INIT *initid)
{
}

double dist2f(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	char* vec1=(char*)args->args[0];
	char* vec2=(char*)args->args[1];
	int len1 = args->lengths[0];
	int len2 = args->lengths[1];
	char * V1 = vec1;
	char * V2 = vec2;
	double dp = 0.0;
	double dt = 0;
	char buffer[64];
	while(len1 > 0 && len2 > 0)
	{
		dt = nextDouble(&V1,buffer,&len1) - nextDouble(&V2,buffer,&len2);
		dp += dt*dt;
	}
	while(len1 > 0)
	{
		dt = nextDouble(&V1,buffer,&len1);
		dp += dt*dt;
	}
	while(len2 > 0)
	{
		dt = nextDouble(&V2,buffer,&len2);
		dp += dt*dt;
	}
	return dp;
}


double dist2i(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	char* vec1=(char*)args->args[0];
	char* vec2=(char*)args->args[1];
	int len1 = args->lengths[0];
	int len2 = args->lengths[1];
	char * V1 = vec1;
	char * V2 = vec2;
	double dp = 0.0;
	char buffer[64];
	int idx1 = nextInt(&V1,buffer,&len1);
	int idx2 = nextInt(&V2,buffer,&len2);
	double dt;
	while(len1 > 0 && len2 > 0)
	{
		if(idx1==idx2)
		{
			dt = nextDouble(&V1,buffer,&len1) - nextDouble(&V2,buffer,&len2);
			dp += dt * dt;
			idx1 = nextInt(&V1,buffer,&len1);
			idx2 = nextInt(&V2,buffer,&len2);
		}
		else if(idx1 < idx2)
		{
			// progress idx1
			dt = nextDouble(&V1,buffer,&len1);
			dp += dt * dt;
			idx1 = nextInt(&V1,buffer,&len1);
		}
		else
		{
			// progress idx2
			dt = nextDouble(&V2,buffer,&len2);
			dp += dt * dt;
			idx2 = nextInt(&V2,buffer,&len2);
		}
	}
	while(len1 > 0)
	{
		dt = nextDouble(&V1,buffer,&len1);
		dp += dt*dt;
		idx1 = nextInt(&V1,buffer,&len1);
	}
	while(len2 > 0)
	{
		dt = nextDouble(&V2,buffer,&len2);
		dp += dt * dt;
		idx2 = nextInt(&V2,buffer,&len2);
	}
	return dp;
}
// END OF DOCUMENT
