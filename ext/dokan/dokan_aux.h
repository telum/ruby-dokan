#ifndef DOKAN_AUX_H
#define DOKAN_AUX_H

LPWSTR str2wcs(VALUE str);
void wcs_free(LPWSTR lpwcs);

VALUE wcs2str(LPCWSTR lpwcs);

#endif

