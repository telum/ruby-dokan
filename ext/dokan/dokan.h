#ifndef RUBY_DOKAN_H
#define RUBY_DOKAN_H

#include "ruby.h"

extern VALUE rb_cDokan;
extern VALUE rb_cDirObj, rb_cFileObj;

void dokan_thread_started(void);

#endif

