#include "dokan_aux.h"


LPWSTR str2wcs(VALUE str)
{
    size_t str_len;
    LPWSTR res;

    Check_Type(str, T_STRING);

    str_len = RSTRING_LEN(str);

    res = (LPWSTR)ALLOC_N(wchar_t, str_len+1);

    if (res == NULL) {
        rb_raise(rb_eNoMemError, "Can't allocate memory by ALLOC_N");
    }

    if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(str), str_len, res, str_len) != (int)str_len) {
        xfree(res);
        rb_raise(rb_eSystemCallError, "MultiByteToWideChar is unexpectedly unsuccessful");
    }

    res[str_len] = '\0';

    return res;
}

void str2wcsbuf(LPWSTR buf, size_t buf_len, VALUE str)
{
    size_t str_len, len;

    Check_Type(str, T_STRING);

    str_len = RSTRING_LEN(str);
    len = min(buf_len, str_len);

    if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(str), str_len, buf, len) != (int)len) {
        rb_raise(rb_eSystemCallError, "MultiByteToWideChar is unexpectedly unsuccessful");
    }

    buf[len] = '\0';
}

void wcs_free(LPWSTR lpcws)
{
    xfree(lpcws);
}

VALUE wcs2str(LPCWSTR lpcws)
{
    int res;
    size_t lpcws_len;
    char* cs;

    lpcws_len = wcslen(lpcws);

    cs = ALLOC_N(char, lpcws_len);

    if (cs == NULL) {
        rb_raise(rb_eNoMemError, "Can't allocate memory by ALLOC_N");
    }

    if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, lpcws, lpcws_len, cs, lpcws_len, NULL, NULL)) != (int)lpcws_len) {
        xfree(cs);
        rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
    }

    return rb_str_new(cs, lpcws_len);
}

