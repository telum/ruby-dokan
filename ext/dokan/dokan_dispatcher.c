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
        puts("OpenDirectory");
        break;
    case DF_CREATEDIRECTORY:
        puts("CreateDirectory");
        break;
    case DF_CLEANUP:
        puts("Cleanup");
        break;
    case DF_CLOSEFILE:
        puts("CloseFile");
        break;
    case DF_READFILE:{
        int res;
        const wchar_t* wcsFileName = (LPCWSTR)drs.argv[0];
        size_t fileNameLen = wcslen(wcsFileName);
        char* csFileName = ALLOC_N(char, fileNameLen+1);

        if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsFileName, fileNameLen, csFileName, fileNameLen, NULL, NULL)) != (int)fileNameLen) {
            xfree(csFileName);
            rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        }
        csFileName[fileNameLen] = '\0';

        if (rb_respond_to(rb_cDokan, rb_intern("read_file"))) {
            VALUE rb_res = rb_funcall(
                rb_cDokan,
                rb_intern("read_file"),
                3,
	              rb_str_new2(csFileName),//LPCWSTR          FileName,
	              //LPVOID           Buffer,
	              INT2NUM((DWORD)drs.argv[2]),//DWORD            NumberOfBytesToRead,
	              //LPDWORD          NumberOfBytesRead,
	              LL2NUM(*((LONGLONG*)drs.argv[4]))//LONGLONG         Offset,
	              //PDOKAN_FILE_INFO FileInfo
            );

            if (rb_res == Qnil || TYPE(rb_res) == T_STRING) {
                memcpy(drs.argv[1], RSTRING_PTR(rb_res), RSTRING_LEN(rb_res));
                *((LPDWORD)drs.argv[3]) = RSTRING_LEN(rb_res);
                drs.res = 0;
            } else {
                drs.res = 1;
            }
        }

        xfree(csFileName);

        }break;
    case DF_WRITEFILE:
        puts("WriteFile");
        break;
    case DF_FLUSHFILEBUFFERS:
        puts("FlushFileBuffers");
        break;
    case DF_GETFILEINFORMATION:{
        int res;

        const wchar_t* wcsFileName = (LPCWSTR)drs.argv[0];
        size_t fileNameLen = wcslen(wcsFileName);
        char* csFileName = ALLOC_N(char, fileNameLen+1);

        if ((res = WideCharToMultiByte(CP_THREAD_ACP, 0, wcsFileName, fileNameLen, csFileName, fileNameLen, NULL, NULL)) != (int)fileNameLen) {
            xfree(csFileName);
            rb_raise(rb_eSystemCallError, "WideCharToMultiByte failed (%d)", res);
        }
        csFileName[fileNameLen] = '\0';

        if (rb_respond_to(rb_cDokan, rb_intern("getfileinfo"))) {
            VALUE rb_fileinfo = rb_hash_new();

            VALUE rb_res = rb_funcall(
                rb_cDokan,
                rb_intern("getfileinfo"),
                2,
	              rb_str_new2(csFileName),//LPCWSTR FileName,
	              rb_fileinfo//LPBY_HANDLE_FILE_INFORMATION HandleFileInfo,
	              //PDOKAN_FILE_INFO FileInfo
            );

            if (rb_res == Qnil || TYPE(rb_res) == T_FIXNUM) {
                LONGLONG size, atime, ctime, mtime;
                LPBY_HANDLE_FILE_INFORMATION hdlFileInfo;
                VALUE rb_fileSize, rb_atime, rb_ctime, rb_mtime;

                rb_fileSize = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("size")));
                rb_atime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("atime")));
                rb_ctime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("ctime")));
                rb_mtime = rb_hash_aref(rb_fileinfo, ID2SYM(rb_intern("mtime")));

                if (TYPE(rb_fileSize) != T_FIXNUM && TYPE(rb_fileSize) != T_BIGNUM) {
                    rb_raise(rb_eTypeError, "size of file is not Fixnum");
                }

                if (TYPE(rb_atime) != T_FIXNUM && TYPE(rb_atime) != T_BIGNUM) {
                    rb_raise(rb_eTypeError, "atime of file is not Fixnum (%d)", TYPE(rb_atime));
                }

                if (TYPE(rb_ctime) != T_FIXNUM && TYPE(rb_ctime) != T_BIGNUM) {
                    rb_raise(rb_eTypeError, "ctime of file is not Fixnum (%d)", TYPE(rb_ctime));
                }

                if (TYPE(rb_mtime) != T_FIXNUM && TYPE(rb_mtime) != T_BIGNUM) {
                    rb_raise(rb_eTypeError, "mtime of file is not Fixnum (%d)", TYPE(rb_mtime));
                }

                size = NUM2LL(rb_fileSize);
                atime = NUM2LL(rb_atime);
                ctime = NUM2LL(rb_ctime);
                mtime = NUM2LL(rb_mtime);
                hdlFileInfo = (LPBY_HANDLE_FILE_INFORMATION)drs.argv[1];

                hdlFileInfo->nFileSizeLow = (DWORD)size;
                hdlFileInfo->nFileSizeHigh = (DWORD)(size>>32);
                hdlFileInfo->ftCreationTime.dwLowDateTime = (DWORD)ctime;
                hdlFileInfo->ftCreationTime.dwHighDateTime = (DWORD)(ctime>>32);
                hdlFileInfo->ftLastAccessTime.dwLowDateTime = (DWORD)atime;
                hdlFileInfo->ftLastAccessTime.dwHighDateTime = (DWORD)(atime>>32);
                hdlFileInfo->ftLastWriteTime.dwLowDateTime = (DWORD)mtime;
                hdlFileInfo->ftLastWriteTime.dwHighDateTime = (DWORD)(mtime>>32);
            } else {
                rb_raise(rb_eTypeError, "Error return type for callback function");
            }
        }
        }break;
    case DF_FINDFILES:
        puts("FindFiles");
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
                        VALUE rb_file = rb_ary_entry(rb_ary, i);
                        VALUE rb_fileName = rb_funcall(rb_file, rb_intern("basename"), 0);
                        VALUE rb_fileSize = rb_funcall(rb_file, rb_intern("size"), 0);

                        VALUE rb_atime = rb_funcall(rb_file, rb_intern("atime"), 0);
                        VALUE rb_mtime = rb_funcall(rb_file, rb_intern("mtime"), 0);
                        VALUE rb_ctime = rb_funcall(rb_file, rb_intern("ctime"), 0);

                        size_t fileName_len = RSTRING_LEN(rb_fileName);

                        LONGLONG fileSize = NUM2LL(rb_fileSize);
                        LONGLONG atime = NUM2LL(rb_atime);
                        LONGLONG mtime = NUM2LL(rb_mtime);
                        LONGLONG ctime = NUM2LL(rb_ctime);

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

                        if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(rb_fileName), fileName_len, wfd.cFileName, MAX_PATH) != (int)fileName_len) {
                            rb_raise(rb_eSystemCallError, "MultiByteToWideChar failed");
                        }
                        wfd.cFileName[min(fileName_len, MAX_PATH-1)] = '\0';


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

