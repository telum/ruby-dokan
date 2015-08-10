#include <ruby.h>
#include "libdokan.h"
#include "dokan.h"
#include "dokan_ops.h"


VALUE rb_cDokan = Qnil;


VALUE rb_dokan_run(VALUE self)
{
    int res;
    PDOKAN_OPTIONS dk_opts;
    PDOKAN_OPERATIONS dk_ops;
    ULONG options;
    //ULONG64 context;
    LPWSTR wcs_mp;
    VALUE rb_mp;
    int rb_mp_len;

    rb_mp = rb_iv_get(self, "@mount_point");

    if (TYPE(rb_mp) != T_STRING) {
        rb_raise(rb_eArgError, "mount_point shall be initialized");
    }

    rb_mp_len = RSTRING_LEN(rb_mp);

    wcs_mp = ALLOC_N(wchar_t, rb_mp_len+1);

    if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(rb_mp), rb_mp_len, wcs_mp, rb_mp_len+1) != rb_mp_len) {
        xfree(wcs_mp);
        rb_raise(rb_eSystemCallError, "MultiByteToWideChar failed");
    }

    wcs_mp[rb_mp_len] = L'\0';

    dk_opts = ALLOC(DOKAN_OPTIONS);

    options = 0;
    if (rb_iv_get(self, "@network") == Qtrue) options |= DOKAN_OPTION_NETWORK;
    if (rb_iv_get(self, "@removable") == Qtrue) options |= DOKAN_OPTION_REMOVABLE;

    //context = GetCurrentProcessId();
    //context <<= 32;
    //context |= GetCurrentThreadId();

    dk_opts->Version = DOKAN_VERSION;
    dk_opts->ThreadCount = FIX2INT(rb_iv_get(self, "@threads"));
    dk_opts->Options = options;
    //dk_opts->GlobalContext = context;// GlobalContext is not used by Dokan driver
    dk_opts->MountPoint = wcs_mp;

    dk_ops = ALLOC(DOKAN_OPERATIONS);

    dk_ops->CreateFile           = RubyDokan_CreateFile;
    dk_ops->OpenDirectory        = RubyDokan_OpenDirectory;
    dk_ops->CreateDirectory      = RubyDokan_CreateDirectory;
    dk_ops->Cleanup              = RubyDokan_Cleanup;
    dk_ops->CloseFile            = RubyDokan_CloseFile;
    dk_ops->ReadFile             = RubyDokan_ReadFile;
    dk_ops->WriteFile            = RubyDokan_WriteFile;
    dk_ops->FlushFileBuffers     = RubyDokan_FlushFileBuffers;
    dk_ops->GetFileInformation   = RubyDokan_GetFileInformation;
    dk_ops->FindFiles            = RubyDokan_FindFiles;
    dk_ops->FindFilesWithPattern = RubyDokan_FindFilesWithPattern;
    dk_ops->SetFileAttributes    = RubyDokan_SetFileAttributes;
    dk_ops->SetFileTime          = RubyDokan_SetFileTime;
    dk_ops->DeleteFile           = RubyDokan_DeleteFile;
    dk_ops->DeleteDirectory      = RubyDokan_DeleteDirectory;
    dk_ops->MoveFile             = RubyDokan_MoveFile;
    dk_ops->SetEndOfFile         = RubyDokan_SetEndOfFile;
    dk_ops->SetAllocationSize    = RubyDokan_SetAllocationSize;
    dk_ops->LockFile             = RubyDokan_LockFile;
    dk_ops->UnlockFile           = RubyDokan_UnlockFile;
    dk_ops->GetDiskFreeSpace     = RubyDokan_GetDiskFreeSpace;
    dk_ops->GetVolumeInformation = RubyDokan_GetVolumeInformation;
    dk_ops->Unmount              = RubyDokan_Unmount;
    dk_ops->GetFileSecurity      = RubyDokan_GetFileSecurity;
    dk_ops->SetFileSecurity      = RubyDokan_SetFileSecurity;

    res = DokanMain(dk_opts, dk_ops);

    if (res != DOKAN_SUCCESS) {
        xfree(dk_opts);
        xfree(dk_ops);
        xfree(wcs_mp);

        return INT2NUM(res);
    }

    return Qnil;
}

VALUE rb_dokan_mount_point(VALUE self)
{
    return rb_iv_get(self, "@mount_point");
}

VALUE rb_dokan_mount_point_set(VALUE self, VALUE mp)
{
    Check_Type(mp, T_STRING);

    rb_iv_set(self, "@mount_point", mp);

    return Qnil;
}

VALUE rb_dokan_threads(VALUE self)
{
    return rb_iv_get(self, "@threads");
}

VALUE rb_dokan_threads_set(VALUE self, VALUE n)
{
    Check_Type(n, T_FIXNUM);

    rb_iv_set(self, "@threads", n);

    return Qnil;
}

VALUE rb_dokan_is_network(VALUE self)
{
    return rb_iv_get(self, "@network");
}

VALUE rb_dokan_is_network_set(VALUE self, VALUE b)
{
    if (b != Qfalse && b != Qtrue) {
        rb_raise(rb_eArgError, "network shall be false or true");
    }

    rb_iv_set(self, "@network", b);

    return Qnil;
}

VALUE rb_dokan_is_removable(VALUE self)
{
    return rb_iv_get(self, "@removable");
}

VALUE rb_dokan_is_removable_set(VALUE self, VALUE b)
{
    if (b != Qfalse && b != Qtrue) {
        rb_raise(rb_eArgError, "removable shall be false or true");
    }

    rb_iv_set(self, "@removable", b);

    return Qnil;
}

void Init_dokan(void)
{
    rb_cDokan = rb_define_class("Dokan", rb_cObject);

    rb_define_singleton_method(rb_cDokan, "run", rb_dokan_run, 0);

    rb_define_singleton_method(rb_cDokan, "threads", rb_dokan_threads, 0);
    rb_define_singleton_method(rb_cDokan, "threads=", rb_dokan_threads_set, 1);

    rb_define_singleton_method(rb_cDokan, "network?", rb_dokan_is_network, 0);
    rb_define_singleton_method(rb_cDokan, "network=", rb_dokan_is_network_set, 1);

    rb_define_singleton_method(rb_cDokan, "removable?", rb_dokan_is_removable, 0);
    rb_define_singleton_method(rb_cDokan, "removable=", rb_dokan_is_removable_set, 1);

    rb_define_singleton_method(rb_cDokan, "mount_point", rb_dokan_mount_point, 0);
    rb_define_singleton_method(rb_cDokan, "mount_point=", rb_dokan_mount_point_set, 1);

    rb_iv_set(rb_cDokan, "@mount_point", Qnil);
    rb_iv_set(rb_cDokan, "@network", Qfalse);
    rb_iv_set(rb_cDokan, "@removable", Qfalse);
    rb_iv_set(rb_cDokan, "@threads", INT2NUM(1));
}

