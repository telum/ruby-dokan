#ifndef DOKAN_AUX_H
#define DOKAN_AUX_H


#include <ruby.h>
#include <libdokan.h>


LPWSTR str2wcs(VALUE str);
void str2wcsbuf(LPWSTR buf, size_t buf_len, VALUE str);
void wcs_free(LPWSTR lpwcs);

VALUE wcs2str(LPCWSTR lpwcs);


#endif

