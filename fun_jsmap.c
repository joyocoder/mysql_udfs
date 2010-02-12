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
void	jsmap_deinit(UDF_INIT *initid)
{
	if(initid->ptr != 0) free(initid->ptr);
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
	char *script;

	script = malloc(256 + args->lengths[0] + args->lengths[1]);
	sprintf(script,"var Input=%s;%s",args->args[0],args->args[1]);
	
	if(rt==0)
	{
		rt = JS_NewRuntime(0x100000);
	}
	cx = JS_NewContext(rt, 0x1000);
	global = JS_NewObject(cx, &global_class, NULL, NULL);
	JS_InitStandardClasses(cx, global);

	ok = JS_EvaluateScript(cx, global, script, strlen(script),"", 1, &rval);
	free(script);
	str = JS_ValueToString(cx, rval);
	initid->ptr = JS_GetStringBytes(str);
	*length = strlen((char *)initid->ptr);
	buf = malloc(*length + 1);
	memcpy(buf, initid->ptr, *length+1);
	initid->ptr = buf;
	
	JS_DestroyContext(cx);
	return initid->ptr;
}

// END OF DOCUMENT
