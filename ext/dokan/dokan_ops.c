#include "dokan_ops.h"
#include "dokan.h"
#include "ruby.h"
#include <stdio.h>


DokanRubySandbox drs;


BOOL RubyDokan_Init(void)
{
    drs.dispatchEvent = CreateEvent(NULL, FALSE, FALSE, "DokanSB_DispatchEvent");

    if (drs.dispatchEvent == NULL) {
        return FALSE;
    }

    drs.dispatchedEvent = CreateEvent(NULL, FALSE, FALSE, "DokanSB_DispatchedEvent");

    if (drs.dispatchedEvent == NULL) {
        return FALSE;
    }

    return TRUE;
}


static void RubyDokan_DispatchAndWait(void)
{
    SetEvent(drs.dispatchEvent);
    WaitForSingleObject(drs.dispatchedEvent, INFINITE);
}

HANDLE hBarrier;

void win32_callback_barrier_init(void)
{
    hBarrier = CreateMutex(NULL, FALSE, "Win32_Callback_Barrier");
}

void win32_callback_barrier_start(void)
{
    WaitForSingleObject(hBarrier, INFINITE);
}

void win32_callback_barrier_stop(void)
{
    ReleaseMutex(hBarrier);
}

int DOKAN_CALLBACK RubyDokan_CreateFile (
	LPCWSTR          FileName,
	DWORD            DesiredAccess,
	DWORD            ShareMode,
	DWORD            CreationDisposition,
	DWORD            FlagsAndAttributes,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_CREATEFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)DesiredAccess;
    drs.argv[2] = (void*)ShareMode;
    drs.argv[3] = (void*)CreationDisposition;
    drs.argv[4] = (void*)FlagsAndAttributes;
    drs.argv[5] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_OpenDirectory (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_OPENDIRECTORY;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_CreateDirectory (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_CREATEDIRECTORY;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

// When FileInfo->DeleteOnClose is true, you must delete the file in Cleanup.
int DOKAN_CALLBACK RubyDokan_Cleanup (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_CLEANUP;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_CloseFile (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_CLOSEFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_ReadFile (
	LPCWSTR          FileName,
	LPVOID           Buffer,
	DWORD            NumberOfBytesToRead,
	LPDWORD          NumberOfBytesRead,
	LONGLONG         Offset,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_READFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)Buffer;
    drs.argv[2] = (void*)NumberOfBytesToRead;
    drs.argv[3] = (void*)NumberOfBytesRead;
    drs.argv[4] = (void*)&Offset;
    drs.argv[5] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_WriteFile (
	LPCWSTR          FileName,
	LPCVOID          Buffer,
	DWORD            NumberOfBytesToWrite,
	LPDWORD          NumberOfBytesWritten,
	LONGLONG         Offset,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_WRITEFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)Buffer;
    drs.argv[2] = (void*)NumberOfBytesToWrite;
    drs.argv[3] = (void*)NumberOfBytesWritten;
    drs.argv[4] = (void*)&Offset;
    drs.argv[5] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_FlushFileBuffers (
	LPCWSTR          FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_FLUSHFILEBUFFERS;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_GetFileInformation (
	LPCWSTR                      FileName,
	LPBY_HANDLE_FILE_INFORMATION HandleFileInfo,
	PDOKAN_FILE_INFO             FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_GETFILEINFORMATION;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)HandleFileInfo;
    drs.argv[2] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_FindFiles (
	LPCWSTR          PathName,
	PFillFindData    FFData,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_FINDFILES;
    drs.argv[0] = (void*)PathName;
    drs.argv[1] = (void*)FFData;
    drs.argv[2] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


// You should implement either FindFiles or FindFilesWithPattern
int DOKAN_CALLBACK RubyDokan_FindFilesWithPattern (
	LPCWSTR          PathName,
	LPCWSTR 			   SearchPattern,
	PFillFindData    FFData,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    dokan_thread_started();
    win32_callback_barrier_start();

    drs.func = DF_FINDFILESWITHPATTERN;
    drs.argv[0] = (void*)PathName;
    drs.argv[1] = (void*)SearchPattern;
    drs.argv[2] = (void*)FFData;
    drs.argv[3] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_SetFileAttributes (
	LPCWSTR          FileName,
	DWORD            FileAttributes,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_SETFILEATTRIBUTES;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileAttributes;
    drs.argv[2] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_SetFileTime (
	LPCWSTR		       FileName,
	CONST FILETIME*  CreationTime,
	CONST FILETIME*  LastAccessTime,
	CONST FILETIME*  LastWriteTime,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_SETFILETIME;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)CreationTime;
    drs.argv[2] = (void*)LastAccessTime;
    drs.argv[3] = (void*)LastWriteTime;
    drs.argv[4] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


// You should not delete file on DeleteFile or DeleteDirectory.
// When DeleteFile or DeleteDirectory, you must check whether
// you can delete the file or not, and return 0 (when you can delete it)
// or appropriate error codes such as -ERROR_DIR_NOT_EMPTY,
// -ERROR_SHARING_VIOLATION.
// When you return 0 (ERROR_SUCCESS), you get a Cleanup call with
// FileInfo->DeleteOnClose set to TRUE and you have to delete the
// file being closed.
int DOKAN_CALLBACK RubyDokan_DeleteFile (
	LPCWSTR          FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_DELETEFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_DeleteDirectory ( 
	LPCWSTR          FileName,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_DELETEDIRECTORY;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_MoveFile (
	LPCWSTR          ExistingFileName,
	LPCWSTR          NewFileName,
	BOOL             ReplaceExisting,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_MOVEFILE;
    drs.argv[0] = (void*)ExistingFileName;
    drs.argv[1] = (void*)NewFileName;
    drs.argv[2] = (void*)ReplaceExisting;
    drs.argv[3] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_SetEndOfFile (
	LPCWSTR          FileName,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_SETENDOFFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)&Length;
    drs.argv[2] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_SetAllocationSize (
	LPCWSTR          FileName,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_SETALLOCATIONSIZE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)&Length;
    drs.argv[2] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_LockFile (
	LPCWSTR          FileName,
	LONGLONG         BytesOffset,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_LOCKFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)&BytesOffset;
    drs.argv[2] = (void*)&Length;
    drs.argv[3] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_UnlockFile (
	LPCWSTR          FileName,
	LONGLONG         BytesOffset,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_UNLOCKFILE;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)&BytesOffset;
    drs.argv[2] = (void*)&Length;
    drs.argv[3] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


// Neither GetDiskFreeSpace nor GetVolumeInformation
// save the DokanFileContext->Context.
// Before these methods are called, CreateFile may not be called.
// (ditto CloseFile and Cleanup)

// see Win32 API GetDiskFreeSpaceEx
int DOKAN_CALLBACK RubyDokan_GetDiskFreeSpace (
	PULONGLONG       FreeBytesAvailable,
	PULONGLONG       TotalNumberOfBytes,
	PULONGLONG       TotalNumberOfFreeBytes,
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_GETDISKFREESPACE;
    drs.argv[0] = (void*)FreeBytesAvailable;
    drs.argv[1] = (void*)TotalNumberOfBytes;
    drs.argv[2] = (void*)TotalNumberOfFreeBytes;
    drs.argv[3] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


// see Win32 API GetVolumeInformation
int DOKAN_CALLBACK RubyDokan_GetVolumeInformation (
	LPWSTR           VolumeNameBuffer,
	DWORD 	         VolumeNameSize, // in num of chars
	LPDWORD          VolumeSerialNumber,
	LPDWORD          MaximumComponentLength, // in num of chars
	LPDWORD          FileSystemFlags,
	LPWSTR 	         FileSystemNameBuffer,
	DWORD 	         FileSystemNameSize, // in num of chars
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_GETVOLUMEINFORMATION;
    drs.argv[0] = (void*)VolumeNameBuffer;
    drs.argv[1] = (void*)VolumeNameSize;
    drs.argv[2] = (void*)VolumeSerialNumber;
    drs.argv[3] = (void*)MaximumComponentLength;
    drs.argv[4] = (void*)FileSystemFlags;
    drs.argv[5] = (void*)FileSystemNameBuffer;
    drs.argv[6] = (void*)FileSystemNameSize;
    drs.argv[7] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


int DOKAN_CALLBACK RubyDokan_Unmount (
	PDOKAN_FILE_INFO FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_UNMOUNT;
    drs.argv[0] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}


// Suported since 0.6.0. You must specify the version at DOKAN_OPTIONS.Version.
int DOKAN_CALLBACK RubyDokan_GetFileSecurity (
	LPCWSTR               FileName,
	PSECURITY_INFORMATION SecInfo, // A pointer to SECURITY_INFORMATION value being requested
	PSECURITY_DESCRIPTOR  SecDesc, // A pointer to SECURITY_DESCRIPTOR buffer to be filled
	ULONG                 SecDescBufLen, // length of Security descriptor buffer
	PULONG                LengthNeeded,
	PDOKAN_FILE_INFO      FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_GETFILESECURITY;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)SecInfo;
    drs.argv[2] = (void*)SecDesc;
    drs.argv[3] = (void*)SecDescBufLen;
    drs.argv[4] = (void*)LengthNeeded;
    drs.argv[5] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

int DOKAN_CALLBACK RubyDokan_SetFileSecurity (
	LPCWSTR               FileName,
	PSECURITY_INFORMATION SecInfo,
	PSECURITY_DESCRIPTOR  SecDesc, // SecurityDescriptor
	ULONG                 SecDescLen, // SecurityDescriptor length
	PDOKAN_FILE_INFO      FileInfo)
{
    int res;

    win32_callback_barrier_start();
    dokan_thread_started();

    drs.func = DF_SETFILESECURITY;
    drs.argv[0] = (void*)FileName;
    drs.argv[1] = (void*)SecInfo;
    drs.argv[2] = (void*)SecDesc;
    drs.argv[3] = (void*)SecDescLen;
    drs.argv[4] = (void*)FileInfo;

    drs.res = 0;

    RubyDokan_DispatchAndWait();

    res = drs.res;

    win32_callback_barrier_stop();

    return res;
}

