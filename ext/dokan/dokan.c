#include <ruby.h>


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

