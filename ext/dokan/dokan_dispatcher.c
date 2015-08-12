#include "dokan_ops.h"
#include "dokan.h"
#include <ruby.h>


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
    switch (drs.func) {
    case DF_CREATEFILE:{
        }break;
    case DF_OPENDIRECTORY:
        break;
    case DF_CREATEDIRECTORY:
        break;
    case DF_CLEANUP:
        break;
    case DF_CLOSEFILE:
        break;
    case DF_READFILE:{
        const char* rb_cmd = "binread";
        VALUE rb_res;
        int res;

        if (!rb_respond_to(rb_cFile, rb_intern(rb_cmd))) {
            rb_raise(rb_eArgError, "File object shall respond to '%s'", rb_cmd);
        }

        {
            LPCWSTR FileName          = (LPCWSTR)drs.argv[0];
            LPVOID Buffer             = (LPVOID)drs.argv[1];
            DWORD NumberOfBytesToRead = (DWORD)drs.argv[2];
            LPDWORD NumberOfBytesRead = (LPDWORD)drs.argv[3];
            LONGLONG Offset           = *((PLONGLONG)drs.argv[4]);
            PDOKAN_FILE_INFO FileInfo = (PDOKAN_FILE_INFO)drs.argv[5];

            rb_res = rb_funcall(
                rb_cFile,
                rb_intern(rb_cmd),
                3,
                wcs2str(FileName),
                INT2NUM(NumberOfBytesToRead),
                LL2NUM(Offset)
            );

            if (rb_res == Qnil || TYPE(rb_res) == T_STRING) {
                memcpy(Buffer, RSTRING_PTR(rb_res), RSTRING_LEN(rb_res));
                *NumberOfBytesRead = RSTRING_LEN(rb_res);
                drs.res = ERROR_SUCCESS;
            } else {
                drs.res = ERROR_INVALID_PARAMETER;
            }
        }

        }break;
    case DF_WRITEFILE:
        break;
    case DF_FLUSHFILEBUFFERS:
        break;
    case DF_GETFILEINFORMATION:{
        //int res;

        //const wchar_t* wcsFileName = (LPCWSTR)drs.argv[0];
        //size_t fileNameLen = wcslen(wcsFileName);
        //char* csFileName = ALLOC_N(char, fileNameLen+1);

        //if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsFileName, fileNameLen, csFileName, fileNameLen, NULL, NULL)) != (int)fileNameLen) {
        //    xfree(csFileName);
        //    rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        //}
        //csFileName[fileNameLen] = '\0';

        //if (rb_respond_to(rb_cDokan, rb_intern("getfileinfo"))) {
        //    VALUE rb_fileinfo = rb_hash_new();

        //    VALUE rb_res = rb_funcall(
        //        rb_cDokan,
        //        rb_intern("getfileinfo"),
        //        2,
	      //        rb_str_new2(csFileName),//LPCWSTR FileName,
	      //        rb_fileinfo//LPBY_HANDLE_FILE_INFORMATION HandleFileInfo,
	      //        //PDOKAN_FILE_INFO FileInfo
        //    );

        //    if (rb_res == Qnil || TYPE(rb_res) == T_FIXNUM) {
        //        LONGLONG size, atime, ctime, mtime;
        //        LPBY_HANDLE_FILE_INFORMATION hdlFileInfo;
        //        VALUE rb_fileSize, rb_atime, rb_ctime, rb_mtime;

        //        rb_fileSize = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("size")));
        //        rb_atime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("atime")));
        //        rb_ctime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("ctime")));
        //        rb_mtime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("mtime")));

        //        if (TYPE(rb_fileSize) != T_FIXNUM && TYPE(rb_fileSize) != T_BIGNUM) {
        //            rb_raise(rb_eTypeError, "size of file is not Fixnum");
        //        }

        //        if (TYPE(rb_atime) != T_FIXNUM && TYPE(rb_atime) != T_BIGNUM) {
        //            rb_raise(rb_eTypeError, "atime of file is not Fixnum (%d)", TYPE(rb_atime));
        //        }

        //        if (TYPE(rb_ctime) != T_FIXNUM && TYPE(rb_ctime) != T_BIGNUM) {
        //            rb_raise(rb_eTypeError, "ctime of file is not Fixnum (%d)", TYPE(rb_ctime));
        //        }

        //        if (TYPE(rb_mtime) != T_FIXNUM && TYPE(rb_mtime) != T_BIGNUM) {
        //            rb_raise(rb_eTypeError, "mtime of file is not Fixnum (%d)", TYPE(rb_mtime));
        //        }

        //        size = NUM2LL(rb_fileSize);
        //        atime = NUM2LL(rb_atime);
        //        ctime = NUM2LL(rb_ctime);
        //        mtime = NUM2LL(rb_mtime);
        //        hdlFileInfo = (LPBY_HANDLE_FILE_INFORMATION)drs.argv[1];

        //        hdlFileInfo->nFileSizeLow = (DWORD)size;
        //        hdlFileInfo->nFileSizeHigh = (DWORD)(size>>32);
        //        hdlFileInfo->ftCreationTime.dwLowDateTime = (DWORD)ctime;
        //        hdlFileInfo->ftCreationTime.dwHighDateTime = (DWORD)(ctime>>32);
        //        hdlFileInfo->ftLastAccessTime.dwLowDateTime = (DWORD)atime;
        //        hdlFileInfo->ftLastAccessTime.dwHighDateTime = (DWORD)(atime>>32);
        //        hdlFileInfo->ftLastWriteTime.dwLowDateTime = (DWORD)mtime;
        //        hdlFileInfo->ftLastWriteTime.dwHighDateTime = (DWORD)(mtime>>32);
        //    } else {
        //        rb_raise(rb_eTypeError, "Error return type for callback function");
        //    }
        //}
        }break;
    case DF_FINDFILES:
        break;
    case DF_FINDFILESWITHPATTERN:{
        int res;
        const char* rb_cmd = "entries";

        if (!rb_respond_to(rb_cDir, rb_intern(rb_cmd))) {
            rb_raise(rb_eArgError, "Dir object shall respond to '%s'", rb_cmd);
        }

        {
            LPCWSTR PathName          = (LPCWSTR)drs.argv[0];
            LPCWSTR SearchPattern     = (LPCWSTR)drs.argv[1];
            PFillFindData FFData      = (PFillFindData)drs.argv[2];
            PDOKAN_FILE_INFO FileInfo = (PDOKAN_FILE_INFO)drs.argv[3];

            VALUE rb_res = rb_funcall(
                rb_cDir,
                rb_intern(rb_cmd),
                3,
                wcs2str(PathName), // TODO: Path = PathName+SearchPattern
                wcs2str(SearchPattern)
            );

            if (TYPE(rb_res) != T_ARRAY) {
                rb_raise(rb_eArgError, "Dir.entries shall return array");
            }

            size_t i; 
            size_t count = RARRAY_LEN(rb_res);

            for (i = 0; i < count; i++) {
                WIN32_FIND_DATAW wfd;
                VALUE fileName = rb_ary_entry(rb_ary, i);
                LONGLONG ctime = 0, atime = 0, mtime = 0, fileSize = 0;

                wfd.dwFileAttributes = 0;
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
                str2wcsbuf(wfd.cFileName, fileName);

                FFData(&wfd, FileInfo);
            }
        }

        }break;
    case DF_SETFILEATTRIBUTES:
        puts("SetFileAttributes");
        break;
    case DF_SETFILETIME:
        puts("SetFileTime");
        break;
    case DF_DELETEFILE:
        puts("DeleteFile");
        break;
    case DF_DELETEDIRECTORY:
        puts("DeleteDirectory");
        break;
    case DF_MOVEFILE:
        puts("MoveFile");
        break;
    case DF_SETENDOFFILE:
        puts("SetEndOfFile");
        break;
    case DF_SETALLOCATIONSIZE:
        puts("SetAllocationSize");
        break;
    case DF_LOCKFILE:
        puts("LockFile");
        break;
    case DF_UNLOCKFILE:
        puts("UnlockFile");
        break;
    case DF_GETDISKFREESPACE:
        puts("GetDiskFreeSpace");
        break;
    case DF_GETVOLUMEINFORMATION:
        puts("GetVolumeInformation");
        break;
    case DF_UNMOUNT:
        puts("Unmount");
        break;
    case DF_GETFILESECURITY:
        puts("GetFileSecurity");
        break;
    case DF_SETFILESECURITY:
        puts("SetFileSecurity");
        break;
    default:;
    }
}

