#ifndef DOKAN_OPS_H
#define DOKAN_OPS_H


#include "libdokan.h"


#define SB_ARGS_MAX_COUNT 8


typedef enum {
  DF_NOP,
  DF_CREATEFILE,
  DF_OPENDIRECTORY,
  DF_CREATEDIRECTORY,
  DF_CLEANUP,
  DF_CLOSEFILE,
  DF_READFILE,
  DF_WRITEFILE,
  DF_FLUSHFILEBUFFERS,
  DF_GETFILEINFORMATION,
  DF_FINDFILES,
  DF_FINDFILESWITHPATTERN,
  DF_SETFILEATTRIBUTES,
  DF_SETFILETIME,
  DF_DELETEFILE,
  DF_DELETEDIRECTORY,
  DF_MOVEFILE,
  DF_SETENDOFFILE,
  DF_SETALLOCATIONSIZE,
  DF_LOCKFILE,
  DF_UNLOCKFILE,
  DF_GETDISKFREESPACE,
  DF_GETVOLUMEINFORMATION,
  DF_UNMOUNT,
  DF_GETFILESECURITY,
  DF_SETFILESECURITY
} DokanFunc;


typedef struct
{
  DokanFunc func;
  void* argv[SB_ARGS_MAX_COUNT];
  int res;
  HANDLE dispatchEvent;
  HANDLE dispatchedEvent;
} DokanRubySandbox;

extern DokanRubySandbox drs;


BOOL RubyDokan_Init(void);

int DOKAN_CALLBACK RubyDokan_CreateFile (
	LPCWSTR          FileName,
	DWORD            DesiredAccess,
	DWORD            ShareMode,
	DWORD            CreationDisposition,
	DWORD            FlagsAndAttributes,
	PDOKAN_FILE_INFO FileInfo);

int DOKAN_CALLBACK RubyDokan_OpenDirectory (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo);

int DOKAN_CALLBACK RubyDokan_CreateDirectory (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo);

// When FileInfo->DeleteOnClose is true, you must delete the file in Cleanup.
int DOKAN_CALLBACK RubyDokan_Cleanup (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo);

int DOKAN_CALLBACK RubyDokan_CloseFile (
	LPCWSTR FileName,
	PDOKAN_FILE_INFO FileInfo);

int DOKAN_CALLBACK RubyDokan_ReadFile (
	LPCWSTR          FileName,
	LPVOID           Buffer,
	DWORD            NumberOfBytesToRead,
	LPDWORD          NumberOfBytesRead,
	LONGLONG         Offset,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_WriteFile (
	LPCWSTR          FileName,
	LPCVOID          Buffer,
	DWORD            NumberOfBytesToWrite,
	LPDWORD          NumberOfBytesWritten,
	LONGLONG         Offset,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_FlushFileBuffers (
	LPCWSTR          FileName,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_GetFileInformation (
	LPCWSTR                      FileName,
	LPBY_HANDLE_FILE_INFORMATION Buffer,
	PDOKAN_FILE_INFO             FileInfo);


int DOKAN_CALLBACK RubyDokan_FindFiles (
	LPCWSTR          PathName,
	PFillFindData    FFData,
	PDOKAN_FILE_INFO FileInfo);


// You should implement either FindFiles or FindFilesWithPattern
int DOKAN_CALLBACK RubyDokan_FindFilesWithPattern (
	LPCWSTR          PathName,
	LPCWSTR 			   SearchPattern,
	PFillFindData    FFData,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_SetFileAttributes (
	LPCWSTR          FileName,
	DWORD            FileAttributes,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_SetFileTime (
	LPCWSTR		       FileName,
	CONST FILETIME*  CreationTime,
	CONST FILETIME*  LastAccessTime,
	CONST FILETIME*  LastWriteTime,
	PDOKAN_FILE_INFO FileInfo);


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
	PDOKAN_FILE_INFO FileInfo);

int DOKAN_CALLBACK RubyDokan_DeleteDirectory ( 
	LPCWSTR          FileName,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_MoveFile (
	LPCWSTR          ExistingFileName,
	LPCWSTR          NewFileName,
	BOOL             ReplaceExisiting,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_SetEndOfFile (
	LPCWSTR          FileName,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_SetAllocationSize (
	LPCWSTR          FileName,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_LockFile (
	LPCWSTR          FileName,
	LONGLONG         ByteOffset,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_UnlockFile (
	LPCWSTR          FileName,
	LONGLONG         ByteOffset,
	LONGLONG         Length,
	PDOKAN_FILE_INFO FileInfo);


// Neither GetDiskFreeSpace nor GetVolumeInformation
// save the DokanFileContext->Context.
// Before these methods are called, CreateFile may not be called.
// (ditto CloseFile and Cleanup)

// see Win32 API GetDiskFreeSpaceEx
int DOKAN_CALLBACK RubyDokan_GetDiskFreeSpace (
	PULONGLONG       FreeBytesAvailable,
	PULONGLONG       TotalNumberOfBytes,
	PULONGLONG       TotalNumberOfFreeBytes,
	PDOKAN_FILE_INFO FileInfo);


// see Win32 API GetVolumeInformation
int DOKAN_CALLBACK RubyDokan_GetVolumeInformation (
	LPWSTR           VolumeNameBuffer,
	DWORD 	         VolumeNameSize, // in num of chars
	LPDWORD          VolumeSerialNumber,
	LPDWORD          MaximumComponentLength, // in num of chars
	LPDWORD          FileSystemFlags,
	LPWSTR 	         FileSystemNameBuffer,
	DWORD 	         FileSystemNameSize, // in num of chars
	PDOKAN_FILE_INFO FileInfo);


int DOKAN_CALLBACK RubyDokan_Unmount (
	PDOKAN_FILE_INFO FileInfo);


// Suported since 0.6.0. You must specify the version at DOKAN_OPTIONS.Version.
int DOKAN_CALLBACK RubyDokan_GetFileSecurity (
	LPCWSTR               FileName,
	PSECURITY_INFORMATION SecInfo, // A pointer to SECURITY_INFORMATION value being requested
	PSECURITY_DESCRIPTOR  SecDesc, // A pointer to SECURITY_DESCRIPTOR buffer to be filled
	ULONG                 SecDescBufLen, // length of Security descriptor buffer
	PULONG                LengthNeeded,
	PDOKAN_FILE_INFO      FileInfo);

int DOKAN_CALLBACK RubyDokan_SetFileSecurity (
	LPCWSTR               FileName,
	PSECURITY_INFORMATION SecInfo,
	PSECURITY_DESCRIPTOR  SecDesc, // SecurityDescriptor
	ULONG                 SecDescLen, // SecurityDescriptor length
	PDOKAN_FILE_INFO      FileInfo);

#endif

