#include <ruby.h>
#include "dokan.h"

VALUE rb_cDokan = Qnil;


VALUE rb_dokan_init(VALUE self)
{
    rb_iv_set(self, "@mount_point", Qnil);
    rb_iv_set(self, "@network", Qfalse);
    rb_iv_set(self, "@removable", Qfalse);

    rb_iv_set(self, "@hooks", rb_hash_new());
    rb_iv_set(self, "@threads", INT2NUM(1));

    return Qnil;
}

VALUE rb_dokan_run(VALUE self)
{
    int res;
    PDOKAN_OPTIONS dk_opts;
    PDOKAN_OPERATIONS dk_ops;
    VALUE rb_mp;
    int rb_mp_len;
    LPWSTR wcs_mp;

    rb_mp = rb_iv_get(self, "@mount_point");

    Check_Type(rb_mp, T_STRING);
    rb_mp_len = RSTRING_LEN(rb_mp);

    wcs_mp = ALLOC_N(wchar_t, rb_mp_len+1);

    if (MultiByteToWideChar(CP_THREAD_ACP, 0, StringValueCStr(rb_mp), rb_mp_len, wcs_mp, rb_mp_len+1) != rb_mp_len) {
        xfree(wcs_mp);
        rb_raise(rb_eSystemCallError, "MultiByteToWideChar failed");
    }

    wcs_mp[rb_mp_len] = L'\0';

    dk_opts = ALLOC(DOKAN_OPTIONS);

    dk_opts->Version = DOKAN_VERSION;
    dk_opts->ThreadCount = 1;
    dk_opts->Options = 0;
    dk_opts->GlobalContext = 1000;
    dk_opts->MountPoint = wcs_mp;

    dk_ops = ALLOC(DOKAN_OPERATIONS);

    dk_ops->CreateFile           = NULL;
    dk_ops->OpenDirectory        = NULL;
    dk_ops->CreateDirectory      = NULL;
    dk_ops->Cleanup              = NULL;
    dk_ops->CloseFile            = NULL;
    dk_ops->ReadFile             = NULL;
    dk_ops->WriteFile            = NULL;
    dk_ops->FlushFileBuffers     = NULL;
    dk_ops->GetFileInformation   = NULL;
    dk_ops->FindFiles            = NULL;
    dk_ops->FindFilesWithPattern = NULL;
    dk_ops->SetFileAttributes    = NULL;
    dk_ops->SetFileTime          = NULL;
    dk_ops->DeleteFile           = NULL;
    dk_ops->DeleteDirectory      = NULL;
    dk_ops->MoveFile             = NULL;
    dk_ops->SetEndOfFile         = NULL;
    dk_ops->SetAllocationSize    = NULL;
    dk_ops->LockFile             = NULL;
    dk_ops->UnlockFile           = NULL;
    dk_ops->GetDiskFreeSpace     = NULL;
    dk_ops->GetVolumeInformation = NULL;
    dk_ops->Unmount              = NULL;
    dk_ops->GetFileSecurity      = NULL;
    dk_ops->SetFileSecurity      = NULL;

    res = DokanMain(dk_opts, dk_ops);

    if (res != DOKAN_SUCCESS) {
        xfree(dk_opts);
        xfree(dk_ops);
        xfree(wcs_mp);
        return INT2NUM(res);
    }

    return Qnil;
}

VALUE rb_dokan_hooks(VALUE self)
{
    return rb_iv_get(self, "@hooks");
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
    rb_define_method(rb_cDokan, "initialize", rb_dokan_init, 0);
    rb_define_method(rb_cDokan, "run", rb_dokan_run, 0);

    rb_define_method(rb_cDokan, "hooks", rb_dokan_hooks, 0);

    rb_define_method(rb_cDokan, "threads", rb_dokan_threads, 0);
    rb_define_method(rb_cDokan, "threads=", rb_dokan_threads_set, 1);

    rb_define_method(rb_cDokan, "network?", rb_dokan_is_network, 0);
    rb_define_method(rb_cDokan, "network=", rb_dokan_is_network_set, 1);

    rb_define_method(rb_cDokan, "removable?", rb_dokan_is_removable, 0);
    rb_define_method(rb_cDokan, "removable=", rb_dokan_is_removable_set, 1);

    rb_define_method(rb_cDokan, "mount_point", rb_dokan_mount_point, 0);
    rb_define_method(rb_cDokan, "mount_point=", rb_dokan_mount_point_set, 1);
}

