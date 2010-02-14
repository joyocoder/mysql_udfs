JSRuntime *rt = 0;

typedef struct resultbuffer
{
        char    *script_buffer;
        size_t  script_sz;
        char    *result_buffer;
        size_t   result_sz;
        JSContext *cx;
        JSObject  *global;
} RESULT_BUFFER;

static JSClass global_class = {
 "global",0,
 JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
 JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};
