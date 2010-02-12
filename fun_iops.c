

typedef struct vectorregister
{
	struct vectorregister* children[11];
	char number;
	double value;
	int count;
} VECTORREGISTER;

typedef struct vectorregistercontext
{
	int size;
	char * output;
	VECTORREGISTER * root;
	char buffer[128];
	double len;
	int normalize;
} VECTORREGISTERCONTEXT;


void va_init(VECTORREGISTER * root)
{
	int k;
	for(k = 0; k < 11; k++)
	{
		root->children[k] = 0;
	}
	root->value = 0.0;
	root->count = 0;
}

void vac_init(VECTORREGISTERCONTEXT *C)
{
	C->size = 0;
	C->root = (VECTORREGISTER *)malloc(sizeof(VECTORREGISTER));
	va_init(C->root);
	C->output = 0;
	C->len = 0.0;
	C->normalize = 0;
}

int number2id(char x)
{
	if('0' <= x && x <= '9') return x - '0';
	return 10;
}

char numberOf(char x)
{
	if('0' <= x && x <= '9') return x;
	return '.';
}

void va_add(VECTORREGISTER * root, char * tag, int count, double value)
{
	char number;
	int id;
	if(count > 0)
	{
		number = numberOf(*tag);
		id = number2id(number);
		if(root->children[id]==0)
		{ 
			root->children[id] = (VECTORREGISTER *)malloc(sizeof(VECTORREGISTER));
			va_init(root->children[id]);
			root->children[id]->number = number;
		}
		if(count > 1)
		{
			va_add(root->children[id],tag + 1, count - 1,value);
		}
		else
		{
			root->children[id]->value += value;
			root->children[id]->count ++;
		}
	}
}

void vac_add(VECTORREGISTERCONTEXT * C, char * x, int len, double value)
{
	va_add(C->root,x,len,value);
}

void va_destroy(VECTORREGISTER * Root)
{
	int k;
	if(Root==0) return;
	for(k = 0; k < 11; k++)
	{
		if(Root->children[k] != 0)
		{
			va_destroy(Root->children[k]);
			free(Root->children[k]);
		}
	}
}

void vac_destroy(VECTORREGISTERCONTEXT *C)
{
	va_destroy(C->root);
	if(C->output != 0)
		free(C->output);
	free(C);
}

void vac_clear(VECTORREGISTERCONTEXT *C)
{
	va_destroy(C->root);
	if(C->output != 0)
		free(C->output);
	C->output = 0;
	vac_init(C);
}


void vac_prepare(VECTORREGISTERCONTEXT *C, VECTORREGISTER *Root, char * buffer, int depth)
{
	int k;
	char counter[128];
	if(Root==0) return;
	if(depth >= 128) return;
	if(depth > 0)
	{
		buffer[depth - 1] = Root->number;
		if(Root->count > 0)
		{
			buffer[depth] = 0;
			if(C->size > 0) C->size ++;
			C->size += strlen(buffer);
			C->size ++;
			C->len += Root->value * Root->value;
			sprintf(counter,"%f",Root->value);
			C->size += strlen(counter);
		}
	}
	for(k = 0; k < 11; k++)
	{
		if(Root->children[k] != 0)
		{
			vac_prepare(C,Root->children[k],buffer,depth + 1);
		}
	}
}


void vac_write(VECTORREGISTERCONTEXT *C, VECTORREGISTER *Root, char * buffer, int depth)
{
	int k;
	char counter[128];
	char * next = C->output;
	if(Root==0) return;
	if(depth >= 128) return;
	if(depth > 0)
	{
		buffer[depth - 1] = Root->number;
		if(Root->count > 0)
		{
			buffer[depth] = 0;
			if(C->size > 0)
			{
				next[0] = ';';
				C->size ++;
				next++;
			}
			strcpy(next,buffer);
			C->size += strlen(buffer);
			next += strlen(buffer);

			next[0] = ',';
			C->size ++;
			next++;

			if(C->normalize==1 && C->len > 0.000001 )
				Root->value /= C->len;
			sprintf(counter,"%f",Root->value);
			strcpy(next,counter);
			C->size += strlen(counter);
			next += strlen(counter);
			C->output = next;

		}
	}
	for(k = 0; k < 11; k++)
	{
		if(Root->children[k] != 0)
		{
			vac_write(C,Root->children[k],buffer,depth + 1);
		}
	}
}


void vac_compute_output(VECTORREGISTERCONTEXT * C)
{
	int a;
	char * orig;
	C->size = 0;
	vac_prepare(C,C->root,C->buffer,0);
	C->len = sqrt(C->len);
	a = C->size;
	C->output = (char *)malloc(sizeof(char) * (C->size + 1));
	orig = C->output;
	C->output[C->size] = 0;
	C->size = 0;
	vac_write(C,C->root,C->buffer,0);
	C->output = orig;
}











my_bool	isum_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	initid->ptr = 0;
	if(args->arg_count!=1)
	{
		strcpy(message,	"IADD takes one string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"IADD : first parameter is not a string");
			status = 1;
		}
		else
		{
			initid->max_length = 1024 * 1024;
			initid->ptr = malloc(sizeof(VECTORREGISTERCONTEXT));
			if(initid->ptr==0)
			{
				strcpy(message,	"IADD : could not allocate memory");
				status = 1;
			}
			else
			{
				vac_init((VECTORREGISTERCONTEXT *)initid->ptr);
				status = 0;
			}
		}
	}
	return status;
}

void isum_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=0)
	{
		free(initid->ptr);
		initid->ptr = 0;
	}
}

void isum_clear( UDF_INIT* initid, char* is_null, char *error )
{
	VECTORREGISTERCONTEXT *C;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	if(C != 0)
	{
		vac_clear(C);
	}
}
void isum_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error )
{
	VECTORREGISTERCONTEXT *C;
	char* words=(char*)args->args[0];
	int len = args->lengths[0];
	char * V1 = words;
	char buffer[64];
	char buffer2[128];
	int wlen = 0;
	double val = 0.0;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	if(C != 0)
	{
		nextWord(&V1,buffer,&len,&wlen);
		while(len > 0)
		{
			val = nextDouble(&V1,buffer2,&len);
			vac_add(C,buffer,wlen,val);

			nextWord(&V1,buffer,&len,&wlen);
		}
	}
}
char *  isum(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	VECTORREGISTERCONTEXT *C;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	length[0] = 0;
	if(C != 0)
	{
		vac_compute_output(C);
		strcpy(result,C->output);
		length[0] = C->size;
		free(C->output);
		C->output = 0;
	}
	return result;
}






my_bool	isumn_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	initid->ptr = 0;
	if(args->arg_count!=1)
	{
		strcpy(message,	"IADD takes one string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"IADD : first parameter is not a string");
			status = 1;
		}
		else
		{
			initid->max_length = 1024 * 1024;
			initid->ptr = malloc(sizeof(VECTORREGISTERCONTEXT));
			if(initid->ptr==0)
			{
				strcpy(message,	"IADD : could not allocate memory");
				status = 1;
			}
			else
			{
				vac_init((VECTORREGISTERCONTEXT *)initid->ptr);
				((VECTORREGISTERCONTEXT *)initid->ptr)->normalize = 1;
				status = 0;
			}
		}
	}
	return status;
}

void isumn_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=0)
	{
		free(initid->ptr);
		initid->ptr = 0;
	}
}

void isumn_clear( UDF_INIT* initid, char* is_null, char *error )
{
	VECTORREGISTERCONTEXT *C;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	if(C != 0)
	{
		vac_clear(C);
		C->normalize = 1;

	}
}
void isumn_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error )
{
	VECTORREGISTERCONTEXT *C;
	char* words=(char*)args->args[0];
	int len = args->lengths[0];
	char * V1 = words;
	char buffer[64];
	char buffer2[128];
	int wlen = 0;
	double val = 0.0;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	if(C != 0)
	{
		nextWord(&V1,buffer,&len,&wlen);
		while(len > 0)
		{
			val = nextDouble(&V1,buffer2,&len);
			vac_add(C,buffer,wlen,val);

			nextWord(&V1,buffer,&len,&wlen);
		}
	}
}
char *  isumn(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	VECTORREGISTERCONTEXT *C;
	C = (VECTORREGISTERCONTEXT *)initid->ptr;
	length[0] = 0;
	if(C != 0)
	{
		vac_compute_output(C);
		strcpy(result,C->output);
		length[0] = C->size;
		free(C->output);
		C->output = 0;
	}
	return result;
}


// END OF DOCUMENT

