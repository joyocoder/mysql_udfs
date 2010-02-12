my_bool	jsmap_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
	initid->ptr = 0;
	my_bool status;
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

typedef struct resultbuffer
{
	char 	*script_buffer;
	size_t 	script_sz;
	char 	*result_buffer;
	size_t   result_sz;
} RESULT_BUFFER;

void	jsmap_deinit(UDF_INIT *initid)
{
	if(initid->ptr != 0)
	{
		free( ((RESULT_BUFFER *)(initid->ptr))->script_buffer );
		free( ((RESULT_BUFFER *)(initid->ptr))->result_buffer );
		free( (RESULT_BUFFER *)(initid->ptr) );
	}
}


JSRuntime *rt = 0;
static JSClass global_class = {
 "global",0,
 JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
 JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


char *  jsmap(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	JSContext *cx;
	JSObject  *global;
	jsval rval;
	JSString *str;
	JSBool ok;
	char *buf;
	size_t sz;
	RESULT_BUFFER * gold;
	char *at;

	if(initid->ptr == 0)
	{
		gold = malloc(sizeof(RESULT_BUFFER));
		gold->script_buffer = malloc(sizeof(char)*1024);
		gold->script_sz = 1024;
		gold->result_buffer = malloc(sizeof(char)*256);
		gold->result_sz = 256;
		initid->ptr = (void*)gold;
	}
	else
	{
		gold = (RESULT_BUFFER*)(initid->ptr);
	}

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
	
	

	if(rt==0)
	{
		rt = JS_NewRuntime(0x100000);
	}
	cx = JS_NewContext(rt, 0x1000);
	global = JS_NewObject(cx, &global_class, NULL, NULL);
	JS_InitStandardClasses(cx, global);

	ok = JS_EvaluateScript(cx, global, gold->script_buffer, strlen(gold->script_buffer),"", 1, &rval);
	str = JS_ValueToString(cx, rval);
	buf = JS_GetStringBytes(str);

	*length = strlen((char *)buf);
	memcpy(gold->result_buffer,(char*)buf,*length+1);

	JS_DestroyContext(cx);
	return gold->result_buffer;
}

// END OF DOCUMENT
