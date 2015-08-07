#ifndef DOKAN_CONTEXT_H
#define DOKAN_CONTEXT_H

#include <ruby.h>

void dokan_context_init(void);
void dokan_context_register(ULONG64 context, VALUE obj);
void dokan_context_unregister(ULONG64 context);
VALUE dokan_context_get(ULONG64 context);

#endif

