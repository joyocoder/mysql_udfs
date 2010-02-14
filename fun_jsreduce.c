
my_bool	jsreduce_init(UDF_INIT *initid,	UDF_ARGS *args,	char *message)
{
        RESULT_BUFFER * gold;
        my_bool status;
	size_t sz;
	jsval rval;

        if(rt==0)
        {
                rt = JS_NewRuntime(0x100000);
        }
	status = 0;
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
		        sz = 16 + args->lengths[0];
		        if(sz >= gold->script_sz)
		        {
                		sz = sz * 2 + 256; // grow the buffer
		                gold->script_buffer = (char*) realloc(gold->script_buffer,sz);
		        }
		        memset(gold->script_buffer,0,gold->script_sz);
		        memcpy(gold->script_buffer,(char *)(args->args[0]),args->lengths[0]);
			gold->script_buffer[args->lengths[0]] = 0;
			JS_EvaluateScript(gold->cx, gold->global, gold->script_buffer, strlen(gold->script_buffer),"", 1, &rval);
		}
	}
	return status;
}
void	jsreduce_deinit(UDF_INIT *initid)
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
void    jsreduce_clear( UDF_INIT* initid, char* is_null, char *error )
{
        RESULT_BUFFER * gold;
	jsval rval;
        gold = (RESULT_BUFFER*)(initid->ptr);
	JS_EvaluateScript(gold->cx,gold->global,"Clear();",8,"JsReduce",1,&rval);
}
void    jsreduce_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error )
{
        jsval rval;
        size_t sz;
        RESULT_BUFFER * gold;
        char *at;

        gold = (RESULT_BUFFER*)(initid->ptr);
        sz = 16 + args->lengths[1];
        if(sz >= gold->script_sz)
        {
                sz = sz * 2 + 256; // grow the buffer
                gold->script_buffer = (char*) realloc(gold->script_buffer,sz);
		gold->script_sz = sz;
        }
        memset(gold->script_buffer,0,gold->script_sz);
        memcpy(gold->script_buffer,"Add(",4);
        at = gold->script_buffer + 4;
        memcpy(at,(char *)(args->args[1]),args->lengths[1]);
        at += args->lengths[1];
        at[0] = ')';at++;
        at[0] = ';';at++;
        at[0] = 0;
	JS_EvaluateScript(gold->cx, gold->global, gold->script_buffer, strlen(gold->script_buffer),"", 1, &rval);
}
char *  jsreduce(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
        jsval rval;
        JSString *str;
        JSBool ok;
        char *buf;
        RESULT_BUFFER * gold;

	gold = (RESULT_BUFFER*)(initid->ptr);

	ok = JS_EvaluateScript(gold->cx, gold->global, "Reduce();", 9,"jsreduce()", 1, &rval);
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

	return result;
}
