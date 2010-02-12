

typedef struct tagcloud
{
	struct tagcloud* children[27];
	char letter;
	int count;
} TAGCLOUD;

typedef struct tagcloudcontext
{
	int size;
	char * output;
	TAGCLOUD * root;
	char buffer[128];
} TAGCLOUDCONTEXT;


void tc_init(TAGCLOUD * root)
{
	int k;
	for(k = 0; k < 27; k++)
	{
		root->children[k] = 0;
	}
	root->count = 0;
}


void tcc_init(TAGCLOUDCONTEXT *C)
{
	C->size = 0;
	C->root = (TAGCLOUD *)malloc(sizeof(TAGCLOUD));
	tc_init(C->root);
	C->output = 0;
}

int letter2id(char x)
{
	if('a' <= x && x <= 'z') return x - 'a';
	return 26;
}

char letterOf(char x)
{
	if('a' <= x && x <= 'z') return x;
	return '_';
}

void tc_add(TAGCLOUD * root, char * tag, int count)
{
	char letter;
	int id;
	if(count > 0)
	{
		letter = letterOf(*tag);
		id = letter2id(letter);
		if(root->children[id]==0)
		{ 
			root->children[id] = (TAGCLOUD *)malloc(sizeof(TAGCLOUD));
			tc_init(root->children[id]);
			root->children[id]->letter = letter;
		}
		if(count > 1)
		{
			tc_add(root->children[id],tag + 1, count - 1);
		}
		else
		{
			root->children[id]->count++;
		}

	}
}

void tcc_add(TAGCLOUDCONTEXT * C, char * x, int len)
{
	tc_add(C->root,x,len);
}

void tc_destroy(TAGCLOUD * Root)
{
	int k;
	if(Root==0) return;
	for(k = 0; k < 27; k++)
	{
		if(Root->children[k] != 0)
		{
			tc_destroy(Root->children[k]);
			free(Root->children[k]);
		}
	}
}

void tcc_destroy(TAGCLOUDCONTEXT *C)
{
	tc_destroy(C->root);
	if(C->output != 0)
		free(C->output);
	free(C);
}

void tcc_clear(TAGCLOUDCONTEXT *C)
{
	tc_destroy(C->root);
	if(C->output != 0)
		free(C->output);
	C->output = 0;
	tcc_init(C);
}



void tcc_prepare(TAGCLOUDCONTEXT *C, TAGCLOUD *Root, char * buffer, int depth)
{
	int k;
	char counter[10];
	if(Root==0) return;
	if(depth >= 128) return;
	if(depth > 0)
	{
		buffer[depth - 1] = Root->letter;
		if(Root->count > 0)
		{
			buffer[depth] = 0;
			if(C->size > 0) C->size ++;
			C->size += strlen(buffer);
			C->size ++;
			if(Root->count >= 100000000) Root->count = 100000000;
			sprintf(counter,"%d",Root->count);
			C->size += strlen(counter);
		}
	}
	for(k = 0; k < 27; k++)
	{
		if(Root->children[k] != 0)
		{
			tcc_prepare(C,Root->children[k],buffer,depth + 1);
		}
	}
}


void tcc_write(TAGCLOUDCONTEXT *C, TAGCLOUD *Root, char * buffer, int depth)
{
	int k;
	char counter[10];
	char * next = C->output;
	if(Root==0) return;
	if(depth >= 128) return;
	if(depth > 0)
	{
		buffer[depth - 1] = Root->letter;
		if(Root->count > 0)
		{
			buffer[depth] = 0;
			if(C->size > 0)
			{
				next[0] = ',';
				C->size ++;
				next++;
			}
			strcpy(next,buffer);
			C->size += strlen(buffer);
			next += strlen(buffer);

			next[0] = ':';
			C->size ++;
			next++;

			if(Root->count >= 100000000) Root->count = 100000000;
			sprintf(counter,"%d",Root->count);

			strcpy(next,counter);
			C->size += strlen(counter);
			next += strlen(counter);
			C->output = next;

		}
	}
	for(k = 0; k < 27; k++)
	{
		if(Root->children[k] != 0)
		{
			tcc_write(C,Root->children[k],buffer,depth + 1);
		}
	}
}



void tcc_compute_output(TAGCLOUDCONTEXT * C)
{
	int a;
	char * orig;
	C->size = 0;
	tcc_prepare(C,C->root,C->buffer,0);
	a = C->size;
	C->output = (char *)malloc(sizeof(char) * (C->size + 1));
	orig = C->output;
	C->output[C->size] = 0;
	C->size = 0;
	tcc_write(C,C->root,C->buffer,0);
	C->output = orig;
}

my_bool	tagcloud_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	my_bool status;
	initid->ptr = 0;
	if(args->arg_count!=1)
	{
		strcpy(message,	"TAGCLOUD takes one string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"TAGCLOUD : first parameter is not a string");
			status = 1;
		}
		else
		{
			initid->max_length = 1024 * 1024;
			initid->ptr = malloc(sizeof(TAGCLOUDCONTEXT));
			if(initid->ptr==0)
			{
				strcpy(message,	"TAGCLOUD : could not allocate memory");
				status = 1;
			}
			else
			{
				tcc_init((TAGCLOUDCONTEXT *)initid->ptr);
				status = 0;
			}
		}
	}
	return status;
}
void	tagcloud_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=0)
	{
		free(initid->ptr);
		initid->ptr = 0;
	}
}
void    tagcloud_clear( UDF_INIT* initid, char* is_null, char *error )
{
	TAGCLOUDCONTEXT *C;
	C = (TAGCLOUDCONTEXT *)initid->ptr;
	if(C != 0)
	{
		tcc_clear(C);
	}
}
void    tagcloud_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error )
{
	TAGCLOUDCONTEXT *C;
	char* words=(char*)args->args[0];
	int len = args->lengths[0];
	char * V1 = words;
	char buffer[64];
	int avail = 0;
	int wlen = 0;
	C = (TAGCLOUDCONTEXT *)initid->ptr;
	if(C != 0)
	{
		avail = nextWord(&V1,buffer,&len,&wlen);
		while(len > 0)
		{
			if(avail==1) tcc_add(C,buffer,wlen);
			avail = nextWord(&V1,buffer,&len,&wlen);
		}
		if(avail==1) tcc_add(C,buffer,wlen);
	}
}
char *  tagcloud(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	TAGCLOUDCONTEXT *C;
	C = (TAGCLOUDCONTEXT *)initid->ptr;
	length[0] = 0;
	if(C != 0)
	{
		tcc_compute_output(C);
		strcpy(result,C->output);
		length[0] = C->size;
		free(C->output);
		C->output = 0;
	}
	return result;
}

// END OF DOCUMENT
