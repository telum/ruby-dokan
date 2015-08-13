#include <ruby.h>
#include "libdokan.h"
#include "dokan.h"
#include "dokan_ops.h"
#include "dokan_aux.h"
#include "dokan_dispatcher.h"


VALUE rb_cDokan = Qnil;


struct _Dokan
{
    HANDLE thread;
    HANDLE thStartEvent;
    DOKAN_OPTIONS opts;
    DOKAN_OPERATIONS ops;
} dokan;

void dokan_clean(void);

DWORD WINAPI th_dokan_mainloop(LPVOID lp)
{
    int res;

    win32_callback_barrier_init();

    res = DokanMain(&dokan.opts, &dokan.ops);

    if (res != DOKAN_SUCCESS) {
        dokan_clean();

        return INT2NUM(res);
    }

    return ERROR_SUCCESS;
}


void dokan_thread_started(void)
{
    if (WaitForSingleObject(dokan.thStartEvent, 0) != WAIT_OBJECT_0) {
        SetEvent(dokan.thStartEvent);
    }
}


VALUE rb_dokan_mount(VALUE self, VALUE mount_point, VALUE dirObj, VALUE fileObj)
{
    ULONG options;
    PDOKAN_OPTIONS opts;
    PDOKAN_OPERATIONS ops;

    rb_require("pathname");

    opts = &dokan.opts;
    ops = &dokan.ops;

    options = 0;  // TODO: parametrize useful options such as DOKAN_OPTION_NETWORK and DOKAN_OPTION_REMOVABLE

    opts->Version = DOKAN_VERSION;
    opts->ThreadCount = 0;  // TODO: parametrize this
    opts->Options = options;
    opts->GlobalContext = 0;  // TODO: fix context work in Dokan driver
    opts->MountPoint = str2wcs(mount_point);


    ops->CreateFile           = RubyDokan_CreateFile;
    ops->OpenDirectory        = RubyDokan_OpenDirectory;
    ops->CreateDirectory      = RubyDokan_CreateDirectory;
    ops->Cleanup              = RubyDokan_Cleanup;
    ops->CloseFile            = RubyDokan_CloseFile;
    ops->ReadFile             = RubyDokan_ReadFile;
    ops->WriteFile            = RubyDokan_WriteFile;
    ops->FlushFileBuffers     = RubyDokan_FlushFileBuffers;
    ops->GetFileInformation   = RubyDokan_GetFileInformation;
    ops->FindFiles            = RubyDokan_FindFiles;
    ops->FindFilesWithPattern = RubyDokan_FindFilesWithPattern;
    ops->SetFileAttributes    = RubyDokan_SetFileAttributes;
    ops->SetFileTime          = RubyDokan_SetFileTime;
    ops->DeleteFile           = RubyDokan_DeleteFile;
    ops->DeleteDirectory      = RubyDokan_DeleteDirectory;
    ops->MoveFile             = RubyDokan_MoveFile;
    ops->SetEndOfFile         = RubyDokan_SetEndOfFile;
    ops->SetAllocationSize    = RubyDokan_SetAllocationSize;
    ops->LockFile             = RubyDokan_LockFile;
    ops->UnlockFile           = RubyDokan_UnlockFile;
    ops->GetDiskFreeSpace     = RubyDokan_GetDiskFreeSpace;
    ops->GetVolumeInformation = RubyDokan_GetVolumeInformation;
    ops->Unmount              = RubyDokan_Unmount;
    ops->GetFileSecurity      = RubyDokan_GetFileSecurity;
    ops->SetFileSecurity      = RubyDokan_SetFileSecurity;


    dokan.thStartEvent = CreateEvent(NULL, FALSE, FALSE, "DokanThreadStartEvent");

    dokan.thread = CreateThread (
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)th_dokan_mainloop,
        NULL,
        0,
        NULL
    );

    WaitForSingleObject(dokan.thStartEvent, INFINITE);

    dokan_dispatcher_run();

    return Qnil;
}

void dokan_clean(void)
{
    if (dokan.opts.MountPoint)
        wcs_free((LPWSTR)dokan.opts.MountPoint);
}

void Init_dokan(void)
{
    memset(&dokan, 0, sizeof(struct _Dokan));

    rb_cDokan = rb_define_class("Dokan", rb_cObject);

    rb_define_singleton_method(rb_cDokan, "mount", rb_dokan_mount, 3);

    RubyDokan_Init();
}

