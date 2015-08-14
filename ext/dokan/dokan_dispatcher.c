#include "dokan.h"
#include "dokan_ops.h"
#include "dokan_aux.h"
#include <ruby.h>


#define WIN32_UNIX_EPOH_DIFF 11644473600UL

const char* funcNames[] = {
  "DF_NOP",
  "DF_CREATEFILE",
  "DF_OPENDIRECTORY",
  "DF_CREATEDIRECTORY",
  "DF_CLEANUP",
  "DF_CLOSEFILE",
  "DF_READFILE",
  "DF_WRITEFILE",
  "DF_FLUSHFILEBUFFERS",
  "DF_GETFILEINFORMATION",
  "DF_FINDFILES",
  "DF_FINDFILESWITHPATTERN",
  "DF_SETFILEATTRIBUTES",
  "DF_SETFILETIME",
  "DF_DELETEFILE",
  "DF_DELETEDIRECTORY",
  "DF_MOVEFILE",
  "DF_SETENDOFFILE",
  "DF_SETALLOCATIONSIZE",
  "DF_LOCKFILE",
  "DF_UNLOCKFILE",
  "DF_GETDISKFREESPACE",
  "DF_GETVOLUMEINFORMATION",
  "DF_UNMOUNT",
  "DF_GETFILESECURITY",
  "DF_SETFILESECURITY"
};

static void dokan_dispatcher_loop(void);


void dokan_dispatcher_run(void)
{
    while(1) {
        WaitForSingleObject(drs.dispatchEvent, INFINITE);
        dokan_dispatcher_loop();
        SetEvent(drs.dispatchedEvent);
    }
}

static void dokan_dispatcher_loop(void)
{
    //printf("%s{", funcNames[drs.func]);

    switch (drs.func) {
    case DF_CREATEFILE:{
	      LPCWSTR          FileName            = (LPCWSTR)          drs.argv[0];
	      //DWORD            DesiredAccess       = (DWORD)            drs.argv[1];
	      //DWORD            ShareMode           = (DWORD)            drs.argv[2];
	      DWORD            CreationDisposition = (DWORD)            drs.argv[3];
	      //DWORD            FlagsAndAttributes  = (DWORD)            drs.argv[4];
	      //PDOKAN_FILE_INFO FileInfo            = (PDOKAN_FILE_INFO) drs.argv[5];

        switch (CreationDisposition) {
        case CREATE_ALWAYS:{
            VALUE rb_res;
            VALUE filePath = wcs2str(FileName);
            
            rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

            if (rb_res == Qtrue) {
                drs.res = ERROR_ALREADY_EXISTS;
                rb_funcall(rb_cFileObj, rb_intern("truncate"), 2, filePath, INT2NUM(0));
            } else {
                rb_funcall(rb_cFileObj, rb_intern("binwrite"), 2, filePath, rb_str_new2(""));
            }
            }break;
        case CREATE_NEW:{
            VALUE rb_res;
            VALUE filePath = wcs2str(FileName);
            
            rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

            if (rb_res == Qtrue) {
                drs.res = ERROR_FILE_EXISTS;
            } else {
                rb_funcall(rb_cFileObj, rb_intern("binwrite"), 2, filePath, rb_str_new2(""));
            }
            }break;
        case OPEN_ALWAYS:{
            VALUE rb_res;
            VALUE filePath = wcs2str(FileName);
            
            rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

            if (rb_res == Qtrue) {
                drs.res = ERROR_ALREADY_EXISTS;
            }
            }break;
        case OPEN_EXISTING:{
            VALUE rb_res;
            VALUE filePath = wcs2str(FileName);
            
            rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

            if (rb_res == Qfalse) {
                drs.res = ERROR_FILE_NOT_FOUND;
            }
            }break;
        case TRUNCATE_EXISTING:{
            VALUE rb_res;
            VALUE filePath = wcs2str(FileName);
            
            rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

            if (rb_res == Qfalse) {
                drs.res = ERROR_FILE_NOT_FOUND;
            } else {
                rb_funcall(rb_cFileObj, rb_intern("truncate"), 2, filePath, INT2NUM(0));
            }
            }break;
        }
        }break;
    case DF_OPENDIRECTORY:{
        }break;
    case DF_CREATEDIRECTORY:
        break;
    case DF_CLEANUP:
        break;
    case DF_CLOSEFILE:
        break;
    case DF_READFILE:{
        const char* rb_cmd = "binread";
        VALUE rb_res;
        LPCWSTR FileName          = (LPCWSTR)drs.argv[0];
        LPVOID Buffer             = (LPVOID)drs.argv[1];
        DWORD NumberOfBytesToRead = (DWORD)drs.argv[2];
        LPDWORD NumberOfBytesRead = (LPDWORD)drs.argv[3];
        LONGLONG Offset           = *((PLONGLONG)drs.argv[4]);
        //PDOKAN_FILE_INFO FileInfo = (PDOKAN_FILE_INFO)drs.argv[5];
        VALUE rb_sPathName = wcs2str(FileName);

        rb_funcall(rb_sPathName, rb_intern("gsub!"), 2, rb_str_new2("\\"), rb_str_new2("/"));

        rb_res = rb_funcall(
            rb_cFileObj,
            rb_intern("readable?"),
            1,
           rb_sPathName 
        );

        if (rb_res != Qtrue) {
            drs.res = ERROR_FILE_NOT_FOUND;
        } else {
            rb_res = rb_funcall(
                rb_cFileObj,
                rb_intern(rb_cmd),
                3,
                rb_sPathName,
                INT2NUM(NumberOfBytesToRead),
                LL2NUM(Offset)
            );

            if (rb_res == Qnil || TYPE(rb_res) == T_STRING) {
                if (TYPE(rb_res) == T_STRING) {
                    memcpy(Buffer, RSTRING_PTR(rb_res), RSTRING_LEN(rb_res));
                    *NumberOfBytesRead = RSTRING_LEN(rb_res);
                }
                drs.res = ERROR_SUCCESS;
            } else {
                drs.res = ERROR_INVALID_PARAMETER;
            }
        }

        }break;
    case DF_WRITEFILE:{
        const char* rb_cmd = "binwrite";
        VALUE rb_res;

        if (!rb_respond_to(rb_cFileObj, rb_intern(rb_cmd))) {
            rb_raise(rb_eArgError, "File object shall respond to '%s'", rb_cmd);
        }

        {
	          LPCWSTR          FileName             = (LPCWSTR)drs.argv[0];
	          LPCVOID          Buffer               = (LPCVOID)drs.argv[1];
	          DWORD            NumberOfBytesToWrite = (DWORD)drs.argv[2];
	          LPDWORD          NumberOfBytesWritten = (LPDWORD)drs.argv[3];
	          PLONGLONG        pOffset              = (PLONGLONG)drs.argv[4];
	          //PDOKAN_FILE_INFO FileInfo             = (PDOKAN_FILE_INFO)drs.argv[5];

            rb_res = rb_funcall(
                rb_cFileObj,
                rb_intern("writable?"),
                1,
                wcs2str(FileName)
            );

            if (rb_res != Qtrue) {
                drs.res = ERROR_FILE_NOT_FOUND;
            } else {
                rb_res = rb_funcall(
                    rb_cFileObj,
                    rb_intern(rb_cmd),
                    3,
                    wcs2str(FileName),
                    rb_str_new(Buffer, NumberOfBytesToWrite),
                    LL2NUM(*pOffset)
                );

                *NumberOfBytesWritten = NUM2LL(rb_res);
            }
        }

        }break;
    case DF_FLUSHFILEBUFFERS:
        break;
    case DF_GETFILEINFORMATION:{
        LPCWSTR                      FileName = (LPCWSTR)drs.argv[0];
        LPBY_HANDLE_FILE_INFORMATION HandleFileInfo = (LPBY_HANDLE_FILE_INFORMATION)drs.argv[1];
        //PDOKAN_FILE_INFO             FileInfo = (PDOKAN_FILE_INFO)drs.argv[2];
        VALUE rb_sPathName = wcs2str(FileName);
        VALUE rb_res;

        rb_funcall(rb_sPathName, rb_intern("gsub!"), 2, rb_str_new2("\\"), rb_str_new2("/"));

        rb_res = rb_funcall(
            rb_cFileObj,
            rb_intern("readable?"),
            1,
            rb_sPathName
        );

        if (rb_res != Qtrue) {
            drs.res = ERROR_FILE_NOT_FOUND;
        } else {
            LONGLONG size, ctime, atime, mtime;

            size = NUM2LL(rb_funcall(rb_cFileObj, rb_intern("size"), 1, rb_sPathName));
            ctime = NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("ctime"), 1, rb_sPathName), rb_intern("to_i"), 0));
            atime = NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("atime"), 1, rb_sPathName), rb_intern("to_i"), 0));
            mtime = NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("mtime"), 1, rb_sPathName), rb_intern("to_i"), 0));

            HandleFileInfo->nFileSizeLow = (DWORD)size;
            HandleFileInfo->nFileSizeHigh = (DWORD)(size>>32);
            HandleFileInfo->ftCreationTime.dwLowDateTime = (DWORD)ctime;
            HandleFileInfo->ftCreationTime.dwHighDateTime = (DWORD)(ctime>>32);
            HandleFileInfo->ftLastAccessTime.dwLowDateTime = (DWORD)atime;
            HandleFileInfo->ftLastAccessTime.dwHighDateTime = (DWORD)(atime>>32);
            HandleFileInfo->ftLastWriteTime.dwLowDateTime = (DWORD)mtime;
            HandleFileInfo->ftLastWriteTime.dwHighDateTime = (DWORD)(mtime>>32);
        }

        }break;
    case DF_FINDFILES:
        break;
    case DF_FINDFILESWITHPATTERN:{
        VALUE rb_res;
        size_t i;
        size_t count;
        LPCWSTR PathName          = (LPCWSTR)drs.argv[0];
        LPCWSTR SearchPattern     = (LPCWSTR)drs.argv[1];
        PFillFindData FFData      = (PFillFindData)drs.argv[2];
        PDOKAN_FILE_INFO FileInfo = (PDOKAN_FILE_INFO)drs.argv[3];
        VALUE rb_cModule          = rb_const_get(rb_cObject, rb_intern("Module"));
        VALUE rb_cPathname        = rb_const_get(rb_cModule, rb_intern("Pathname"));
        VALUE rb_pPathName        = rb_funcall(rb_cPathname, rb_intern("new"), 1, wcs2str(PathName));
        VALUE rb_sPathName;

        rb_pPathName = rb_funcall(rb_pPathName, rb_intern("+"), 1, wcs2str(SearchPattern));
        rb_sPathName = rb_funcall(rb_pPathName, rb_intern("to_s"), 0);

        rb_funcall(rb_sPathName, rb_intern("gsub!"), 2, rb_str_new2("\\"), rb_str_new2("/"));

        rb_res = rb_funcall(
            rb_cDirObj,
            rb_intern("glob"),
            1,
            rb_sPathName
        );

        if (TYPE(rb_res) != T_ARRAY) {
            rb_raise(rb_eArgError, "Dir.glob shall return array");
        }

        count = RARRAY_LEN(rb_res);

        for (i = 0; i < count; i++) {
            WIN32_FIND_DATAW wfd;
            VALUE pFileName = rb_funcall(rb_cPathname, rb_intern("new"), 1, rb_ary_entry(rb_res, i));
            VALUE fileName = rb_funcall(rb_funcall(pFileName, rb_intern("basename"), 0), rb_intern("to_s"), 0);
            LONGLONG ctime = 0, atime = 0, mtime = 0, fileSize = 0;
            VALUE sFileName = rb_funcall(pFileName, rb_intern("to_s"), 0);

            wfd.dwFileAttributes = 0;

            if (rb_funcall(rb_cFileObj, rb_intern("directory?"), 1, rb_funcall(pFileName, rb_intern("to_s"), 0))) {
                wfd.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }

            fileSize = NUM2LL(rb_funcall(rb_cFileObj, rb_intern("size"), 1, sFileName));
            ctime = (WIN32_UNIX_EPOH_DIFF + NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("ctime"), 1, sFileName), rb_intern("to_i"), 0)))*10000000UL;
            atime = (WIN32_UNIX_EPOH_DIFF + NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("atime"), 1, sFileName), rb_intern("to_i"), 0)))*10000000UL;
            mtime = (WIN32_UNIX_EPOH_DIFF + NUM2LL(rb_funcall(rb_funcall(rb_cFileObj, rb_intern("mtime"), 1, sFileName), rb_intern("to_i"), 0)))*10000000UL;

            wfd.ftCreationTime.dwLowDateTime = (DWORD)ctime;
            wfd.ftCreationTime.dwHighDateTime = (DWORD)(ctime>>32);
            wfd.ftLastAccessTime.dwLowDateTime = (DWORD)atime;
            wfd.ftLastAccessTime.dwHighDateTime = (DWORD)(atime>>32);
            wfd.ftLastWriteTime.dwLowDateTime = (DWORD)mtime;
            wfd.ftLastWriteTime.dwHighDateTime = (DWORD)(mtime>>32);
            wfd.nFileSizeHigh = (DWORD)(fileSize>>32);
            wfd.nFileSizeLow = (DWORD)fileSize;
            wfd.dwReserved0 = 0;
            wfd.dwReserved1 = 0;
            wfd.cAlternateFileName[0] = '\0';
            str2wcsbuf(wfd.cFileName, MAX_PATH, fileName);

            FFData(&wfd, FileInfo);
        }

        }break;
    case DF_SETFILEATTRIBUTES:
        break;
    case DF_SETFILETIME:
        break;
    case DF_DELETEFILE:{
	      LPCWSTR          FileName = (LPCWSTR)          drs.argv[0];
	      //PDOKAN_FILE_INFO FileInfo = (PDOKAN_FILE_INFO) drs.argv[1];
        VALUE rb_res;
        VALUE filePath;

        filePath = wcs2str(FileName);

        rb_res = rb_funcall(rb_cFileObj, rb_intern("exist?"), 1, filePath);

        if (rb_res = Qtrue) {
            rb_funcall(rb_cFileObj, rb_intern("delete"), 1, filePath);
        } else {
            drs.res = ERROR_FILE_NOT_FOUND;
        }
        }break;
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
    //printf("}\n");
}

