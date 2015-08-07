#include "dokan_context.h"


static VALUE rb_cDokanContexts;


void dokan_context_init(void)
{
    rb_cDokanContexts = rb_hash_new();
}

void dokan_context_register(ULONG64 context, VALUE obj)
{
    rb_hash_aset(rb_cDokanContexts, ULL2NUM(context), obj);
}

void dokan_context_unregister(ULONG64 context)
{
    rb_hash_delete(rb_cDokanContexts, ULL2NUM(context));
}

VALUE dokan_context_get(ULONG64 context)
{
    return rb_hash_aref(rb_cDokanContexts, ULL2NUM(context));
}

