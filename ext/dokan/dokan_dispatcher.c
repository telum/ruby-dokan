#include "dokan_ops.h"
#include "dokan.h"
#include <ruby.h>


static void dokan_dispatcher_loop(void);


void dokan_dispatcher_init(void)
{
}

void dokan_dispatcher_thread(void)
{
    while(1) {
        WaitForSingleObject(drs.dispatchEvent, INFINITE);
        dokan_dispatcher_loop();
        SetEvent(drs.dispatchedEvent);
    }
}

static void dokan_dispatcher_loop(void)
{
    switch (drs.func) {
    case DF_CREATEFILE:{
        int res;
        const wchar_t* wcsFileName = (LPCWSTR)drs.argv[0];
        size_t fileNameLen = wcslen(wcsFileName);
        char* csFileName = ALLOC_N(char, fileNameLen+1);

        if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsFileName, fileNameLen, csFileName, fileNameLen, NULL, NULL)) != (int)fileNameLen) {
            xfree(csFileName);
            rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        }

        csFileName[fileNameLen] = '\0';

        if (rb_respond_to(rb_cDokan, rb_intern("create_file"))) {
            VALUE rb_res = rb_funcall(
                rb_cDokan,
                rb_intern("create_file"),
                5,
                rb_str_new2(csFileName),//LPCWSTR FileName
                INT2NUM((DWORD)drs.argv[1]),//DWORD DesiredAccess
                INT2NUM((DWORD)drs.argv[2]),//DWORD ShareMode
                INT2NUM((DWORD)drs.argv[3]),//DWORD CreationDisposition
                INT2NUM((DWORD)drs.argv[4])//,//DWORD FlagsAndAttributes
                //fileInfo//PDOKAN_FILE_INFO FileInfo
            );

            if (TYPE(rb_res) == T_FIXNUM || TYPE(rb_res) == T_NIL) {
                drs.res = TYPE(rb_res) == T_NIL ? 0 : NUM2INT(rb_res);
            } else {
                rb_raise(rb_eTypeError, "Error return type for callback function");
            }
        }

        xfree(csFileName);
        }break;
    case DF_OPENDIRECTORY:
        break;
    case DF_CREATEDIRECTORY:
        break;
    case DF_CLEANUP:
        break;
    case DF_CLOSEFILE:
        break;
    case DF_READFILE:
        break;
    case DF_WRITEFILE:
        break;
    case DF_FLUSHFILEBUFFERS:
        break;
    case DF_GETFILEINFORMATION:
        break;
    case DF_FINDFILES:
        break;
    case DF_FINDFILESWITHPATTERN:{
        int res;

        const wchar_t* wcsPathName = (LPCWSTR)drs.argv[0];
        size_t pathNameLen = wcslen(wcsPathName);
        char* csPathName = ALLOC_N(char, pathNameLen+1);

        const wchar_t* wcsSearchPattern = (LPCWSTR)drs.argv[1];
        size_t searchPatternLen = wcslen(wcsSearchPattern);
        char* csSearchPattern = ALLOC_N(char, searchPatternLen+1);

        if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsPathName, pathNameLen, csPathName, pathNameLen, NULL, NULL)) != (int)pathNameLen) {
            xfree(csPathName);
            rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        }
        csPathName[pathNameLen] = '\0';

        if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsSearchPattern, searchPatternLen, csSearchPattern, searchPatternLen, NULL, NULL)) != (int)searchPatternLen) {
            xfree(csSearchPattern);
            rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        }
        csSearchPattern[searchPatternLen] = '\0';

        if (rb_respond_to(rb_cDokan, rb_intern("find_files_with_pattern"))) {
            VALUE rb_ary = rb_ary_new();

            VALUE rb_res = rb_funcall(
                rb_cDokan,
                rb_intern("find_files_with_pattern"),
                3,
                rb_str_new2(csPathName),//LPCWSTR PathName
                rb_str_new2(csSearchPattern),//LPCWSTR SearchPattern
                rb_ary//PFillFindData    FFData
                //PDOKAN_FILE_INFO FileInfo
            );

            if (rb_res == Qnil || TYPE(rb_res) == T_FIXNUM) {
                if (rb_res == Qnil || NUM2INT(rb_res) == 0) {
                    size_t i; 
                    size_t count = RARRAY_LEN(rb_ary);
                    PFillFindData FFData = (PFillFindData)drs.argv[2];

                    for (i = 0; i < count; i++) {
                        WIN32_FIND_DATAW wfd;
                        VALUE rb_fileName = rb_ary_entry(rb_ary, i);
                        size_t rb_fileName_len = RSTRING_LEN(rb_fileName);

                        wfd.dwFileAttributes = 0;
                        wfd.ftCreationTime.dwLowDateTime = 0;
                        wfd.ftCreationTime.dwHighDateTime = 0;
                        wfd.ftLastAccessTime.dwLowDateTime = 0;
                        wfd.ftLastAccessTime.dwHighDateTime = 0;
                        wfd.ftLastWriteTime.dwLowDateTime = 0;
                        wfd.ftLastWriteTime.dwHighDateTime = 0;
                        wfd.nFileSizeHigh = 0;
                        wfd.nFileSizeLow = 0;
                        wfd.dwReserved0 = 0;
                        wfd.dwReserved1 = 0;
                        wfd.cAlternateFileName[0] = '\0';

                        if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(rb_fileName), rb_fileName_len, wfd.cFileName, MAX_PATH) != (int)rb_fileName_len) {
                            rb_raise(rb_eSystemCallError, "MultiByteToWideChar failed");
                        }
                        wfd.cFileName[min(rb_fileName_len, MAX_PATH-1)] = '\0';


                        FFData(&wfd, (PDOKAN_FILE_INFO)drs.argv[3]);
                    }
                } else {
                    drs.res = NUM2INT(rb_res);
                }
            } else {
                rb_raise(rb_eTypeError, "Error return type for callback function");
            }
        }

        xfree(csPathName);
        xfree(csSearchPattern);
        }break;
    case DF_SETFILEATTRIBUTES:
        break;
    case DF_SETFILETIME:
        break;
    case DF_DELETEFILE:
        break;
    case DF_DELETEDIRECTORY:
        break;
    case DF_MOVEFILE:
        break;
    case DF_SETENDOFFILE:
        break;
    case DF_SETALLOCATIONSIZE:
        break;
    case DF_LOCKFILE:
        break;
    case DF_UNLOCKFILE:
        break;
    case DF_GETDISKFREESPACE:
        break;
    case DF_GETVOLUMEINFORMATION:
        break;
    case DF_UNMOUNT:
        break;
    case DF_GETFILESECURITY:
        break;
    case DF_SETFILESECURITY:
        break;
    default:;
    }
}

