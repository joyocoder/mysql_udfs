JSRuntime *rt = 0;

typedef struct resultbuffer
{
	char 	*script_buffer;
	size_t 	script_sz;
	char 	*result_buffer;
	size_t   result_sz;
	JSContext *cx;
	JSObject  *global;
} RESULT_BUFFER;

static JSClass global_class = {
 "global",0,
 JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
 JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};



my_bool	jsmap_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	RESULT_BUFFER * gold;
	my_bool status;
	if(rt==0)
	{
		rt = JS_NewRuntime(0x100000);
	}
	gold = malloc(sizeof(RESULT_BUFFER));
	gold->cx = JS_NewContext(rt, 0x1000);
	gold->global = JS_NewObject(gold->cx, &global_class, NULL, NULL);
	JS_InitStandardClasses(gold->cx, gold->global);
	gold->script_buffer = malloc(sizeof(char)*1024);
	gold->script_sz = 1024;
	gold->result_buffer = malloc(sizeof(char)*256);
	gold->result_sz = 256;
	initid->ptr = (void*)gold;
	if(args->arg_count!=2)
	{
		strcpy(message,	"JSMAP takes two string parameters!");
		status = 1;
	}
	else
	{
		if(args->arg_type[0] != STRING_RESULT)
		{
			strcpy(message,	"JSMAP : first parameter is not a string");
			status = 1;
		}
		else
		{
			if(args->arg_type[1] != STRING_RESULT)
			{
				strcpy(message,	"TAGCLOUD : second parameter is not a string");
				status = 1;
			}
			else
			{
			}
		}
	}
	return status;
}

void	jsmap_deinit(UDF_INIT *initid)
{
	RESULT_BUFFER * gold;
	if(initid->ptr != 0)
	{
		gold = (RESULT_BUFFER*)(initid->ptr);
		JS_DestroyContext(gold->cx);
		free( gold->script_buffer );
		free( gold->result_buffer );
		free( (RESULT_BUFFER *)(initid->ptr) );
	}
}



char *  jsmap(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	jsval rval;
	JSString *str;
	JSBool ok;
	char *buf;
	size_t sz;
	RESULT_BUFFER * gold;
	char *at;

	gold = (RESULT_BUFFER*)(initid->ptr);
	sz = 16 + args->lengths[0] + args->lengths[1];
	if(sz >= gold->script_sz)
	{
		sz = sz * 2 + 256; // grow the buffer
		gold->script_buffer = (char*) realloc(gold->script_buffer,sz);
	}

	// manual sprintf (fixes the bug with args->args[0] being not null terminated....
	memset(gold->script_buffer,0,gold->script_sz);
	memcpy(gold->script_buffer,"var Input=",10);
	at = gold->script_buffer + 10;
	memcpy(at,(char *)(args->args[0]),args->lengths[0]);
	at += args->lengths[0];
	at[0] = ';';
	at++;
	memcpy(at,(char *)(args->args[1]),args->lengths[1]);
	at += args->lengths[1];
	at[0] = 0;
	
	
	ok = JS_EvaluateScript(gold->cx, gold->global, gold->script_buffer, strlen(gold->script_buffer),"", 1, &rval);
	str = JS_ValueToString(gold->cx, rval);
	buf = JS_GetStringBytes(str);

	*length = strlen((char *)buf);
	if(gold->result_sz <= *length)
	{
		gold->result_buffer = realloc(gold->result_buffer,*length * 2 + 256);
		gold->result_sz = *length * 2 + 256;
	}
	memcpy(gold->result_buffer,(char*)buf,*length+1);

	return gold->result_buffer;
}


