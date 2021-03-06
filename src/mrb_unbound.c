
/* mrb_unbound.c - Unbound Class
 *
 *
 */

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/class.h"
#include "mruby/value.h"
#include "mruby/array.h"

#include "mrb_unbound.h"
#include "mrb_result.h"

#include <unbound.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>



#define ADDR_LEN 37
typedef struct {
    struct ub_ctx *ctx;
}mrb_unbound_data;

static void mrb_unbound_data_free(mrb_state *mrb, void *p)
{
    mrb_unbound_data *data = (mrb_unbound_data *)p;
    ub_ctx_delete(data->ctx);
}

static const struct mrb_data_type mrb_unbound_data_type = {
    "mrb_unbound_data", mrb_unbound_data_free
};

static mrb_value mrb_unbound_init(mrb_state *mrb, mrb_value self)
{
    struct ub_ctx *ctx;

    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    if(data)
    {
        mrb_free(mrb, data);
    }
    mrb_data_init(self, NULL, &mrb_unbound_data_type);
    data = (mrb_unbound_data *)mrb_malloc(mrb, sizeof(mrb_unbound_data) );

    ctx  = ub_ctx_create();
    if(!ctx)
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "libunbound error: %S", mrb_str_new_cstr(mrb, "could not create unbound context\n") );
    }

    data->ctx = ctx;

    mrb_data_init(self, data, &mrb_unbound_data_type);
    return self;
}

// power user option. low priority
static mrb_value mrb_ub_set_option(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);
    ctx = data->ctx;
    int i;
    char *opt,*v;

    mrb_get_args(mrb,"zz",&opt, &v);
    i = ub_ctx_set_option(ctx, opt, v);

    return mrb_str_new(mrb,v,sizeof(v));
}

//power user potion. low priority
static mrb_value mrb_ub_get_option(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);
    ctx = data->ctx;
    char *opt, *v;
    int i;
    mrb_get_args(mrb,"z",&opt);

    i = ub_ctx_get_option(ctx,opt,&v);

    return mrb_str_new_cstr(mrb,v);
}

// power user option. low priority
static mrb_value mrb_ub_ctx_config(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);
    int i;
    char *file;
    ctx = data->ctx;

    mrb_get_args(mrb,"z",&file);

    i= ub_ctx_config(ctx, file);

    
    return mrb_fixnum_value(i);
}

// fowrder setting
static mrb_value mrb_ub_ctx_set_fwd(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data=(mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx;
    int i;
    char *arg;
    ctx = data->ctx;

    mrb_get_args(mrb,"z",&arg);

    i = ub_ctx_set_fwd(ctx, arg);
    return mrb_fixnum_value(i);
}

//config
static mrb_value mrb_ub_ctx_resolvconf(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int i;
    char *fname =NULL;
    mrb_get_args(mrb,"S",&fname);

    i = ub_ctx_resolvconf(ctx, fname);
    return mrb_fixnum_value(i);
}

static mrb_value mrb_ub_ctx_hosts(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    char *fname=NULL;
    mrb_int i;

    mrb_get_args(mrb,"z",&fname);

    i = ub_ctx_hosts(ctx,fname);

    return mrb_fixnum_value(i);
}


static mrb_value mrb_ub_ctx_add_ta(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int i;
    char *s;

    mrb_get_args(mrb,"z",&s);

    i = ub_ctx_add_ta(ctx, s);

    return mrb_fixnum_value(i);
}

static mrb_value mrb_ub_ctx_add_ta_autr(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    char *p;
    mrb_int i;

    mrb_get_args(mrb,"z", &p);

    i = ub_ctx_add_ta_autr(ctx ,p);
    return mrb_fixnum_value(i);
}

static mrb_value mrb_ub_ctx_add_ta_file(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    char *p=NULL;
    mrb_int i;

    mrb_get_args(mrb,"z", &p);

    i = ub_ctx_add_ta_file(ctx, p);
    return mrb_fixnum_value(i);
}

// low priorty
static mrb_value mrb_ub_ctx_trustedkeys(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);
    ctx = data->ctx;
    char *fname = NULL;
    int i=0;

    mrb_get_args(mrb,"S",&fname);

    i = ub_ctx_trustedkeys(ctx,fname);

    return mrb_true_value();
}
/*
 * it maybe needs mruby-file
static mrb_value mrb_ub_ctx_debugout(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);

    ctx = data->ctx;
    return mrb_nil_value();
}
*/

// developer api
static mrb_value mrb_ub_ctx_debuglevel(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int level, res;

    mrb_get_args(mrb,"i", &level);

    res = ub_ctx_debuglevel(ctx,level);
    return mrb_fixnum_value(res);
}

/*
static mrb_value mrb_ub_ctx_async(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    ub_ctx *ctx;
    data = (mrb_unbound_data *)DATA_PTR(self);
    ctx = data->ctx;
    return mrb_fixnum_value(0);
}
*/

// absolutely need
static mrb_value mrb_ub_poll(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int poll;


    poll = ub_poll(ctx);
    return mrb_fixnum_value(poll);
}

// absolutely need
static mrb_value mrb_ub_fd(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data= (mrb_unbound_data *)DATA_PTR(self);
    mrb_int fd;

    fd = ub_fd(data->ctx);
    return mrb_fixnum_value(fd);
}

// absolutely need
static mrb_value mrb_ub_process(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data =(mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx =data->ctx;
    mrb_int num;

    num = ub_process(ctx);
    
    return mrb_fixnum_value(num);
}


// absolutely need
static mrb_value mrb_ub_resolve(mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data *)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    struct ub_result *result;
    struct in_addr *addr;
    mrb_int retval;
    mrb_int  rrtype=1, rrclass=1;
    mrb_value resobj;
    char *name;
    struct RClass *result_class, *unbound_class;

    mrb_get_args(mrb,"z|ii",&name,&rrtype,&rrclass);
    

    retval = ub_resolve(ctx, name, rrtype, rrclass, &result);
    if(retval != 0 || !result->havedata)
    {
        return mrb_nil_value();
    }

    unbound_class = mrb_class_get(mrb, "Unbound");
    result_class = mrb_class_get_under(mrb, unbound_class, "Result");

    resobj = mrb_obj_new(mrb, result_class, 0, NULL);

    return mrb_result_iv_set(mrb, resobj, result);
}
/*

// absolutely need
static mrb_value mrb_resolv_async (mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data;
    struct ub_ctx *ctx;
    struct ub_result *result;
    //void *call_back(void *mydata, int err, struct ub_ctx* result);

    int retval;
    mrb_int rrtype;
    mrb_value str;

    data = (mrb_unbound_data *)DATA_PTR(self);
    ctx = data->ctx;

    mrb_get_args(mrb,"Si",&str,&rrtype);

    ub_resolve_async(ctx, str, rrtype, 1, MYDATA, , NULL);


    
    return mrb_str_new_cstr("");
}
*/

static mrb_value mrb_ub_cancel (mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data*)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int i, number=0;
    mrb_get_args(mrb,"i",&number);

    i = ub_cancel(ctx,number);
    return mrb_fixnum_value(i);
}

//low priority
static mrb_value mrb_ub_ctx_print_local_zones (mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data = (mrb_unbound_data*)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int i;

    i = ub_ctx_print_local_zones(ctx);
    return mrb_fixnum_value(i);
}

//low priority
static mrb_value mrb_ub_ctx_zone_add (mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data  = (mrb_unbound_data*)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    char* type, *zname;
    mrb_int i;
    mrb_get_args(mrb,"zz",&zname,&type);
    

    i = ub_ctx_zone_add(ctx, zname, type);

    return mrb_fixnum_value(i);
}

static mrb_value mrb_ub_ctx_zone_remove (mrb_state *mrb, mrb_value self)
{
    mrb_unbound_data *data  = (mrb_unbound_data*)DATA_PTR(self);
    struct ub_ctx *ctx = data->ctx;
    mrb_int i;
    char *p;

    mrb_get_args(mrb,"z",&p);
    i = ub_ctx_zone_remove(ctx, p);
    return mrb_fixnum_value(i);
}


void mrb_mruby_unbound_gem_init(mrb_state *mrb)
{
    struct RClass *unbound, *type, *class ;
    unbound = mrb_define_class(mrb, "Unbound",  mrb->object_class);
    MRB_SET_INSTANCE_TT(unbound, MRB_TT_DATA);

    type = mrb_define_class_under(mrb, unbound, "Type", mrb->object_class);
    MRB_SET_INSTANCE_TT(type, MRB_TT_DATA);

    class = mrb_define_class_under(mrb, unbound, "Class", mrb->object_class);
    MRB_SET_INSTANCE_TT(class, MRB_TT_DATA);

    mrb_define_method(mrb,  unbound,    "initialize",   mrb_unbound_init,       MRB_ARGS_NONE()     );
    mrb_define_method(mrb,  unbound,    "resolve",      mrb_ub_resolve,         MRB_ARGS_ARG(1,2)   );
    mrb_define_method(mrb,  unbound,    "resolvconf",   mrb_ub_ctx_resolvconf,  MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "set_option",   mrb_ub_set_option,      MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "get_option",   mrb_ub_get_option,      MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "hosts",        mrb_ub_ctx_hosts,       MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "config",       mrb_ub_ctx_config,      MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "set_fwd",      mrb_ub_ctx_set_fwd,     MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "add_ta",       mrb_ub_ctx_add_ta,      MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "add_ta_autr",  mrb_ub_ctx_add_ta_autr, MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "add_ta_file",  mrb_ub_ctx_add_ta_file, MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "trustedkeys",  mrb_ub_ctx_trustedkeys, MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "debuglevel",   mrb_ub_ctx_debuglevel,  MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "fd",           mrb_ub_fd,              MRB_ARGS_NONE()     );
    mrb_define_method(mrb,  unbound,    "poll",         mrb_ub_poll,            MRB_ARGS_NONE()     );
    mrb_define_method(mrb,  unbound,    "process",      mrb_ub_process,         MRB_ARGS_NONE()     );
    mrb_define_method(mrb,  unbound,    "cancel",       mrb_ub_cancel,         MRB_ARGS_REQ(1)     );
    mrb_define_method(mrb,  unbound,    "print_local_zones",      mrb_ub_ctx_print_local_zones,         MRB_ARGS_NONE()     );
    mrb_define_method(mrb,  unbound,    "zone_add",       mrb_ub_ctx_zone_add,         MRB_ARGS_REQ(2)     );
    mrb_define_method(mrb,  unbound,    "zone_remove",       mrb_ub_ctx_zone_remove,         MRB_ARGS_REQ(1)     );

    /*  
     *  mrb_define_method(mrb,  unbound,    "async",        mrb_ub_ctx_async,       MRB_ARGS_ARG(1,3)   );
     */


    mrb_define_const(mrb, type, "A", mrb_fixnum_value(1));
    mrb_define_const(mrb, type, "NS", mrb_fixnum_value(2));
    mrb_define_const(mrb, type, "CNAME", mrb_fixnum_value(5));
    mrb_define_const(mrb, type, "SOA", mrb_fixnum_value(6));
    mrb_define_const(mrb, type, "PTR", mrb_fixnum_value(12));
    mrb_define_const(mrb, type, "HINFO", mrb_fixnum_value(13));
    mrb_define_const(mrb, type, "MX", mrb_fixnum_value(15));
    mrb_define_const(mrb, type, "TXT", mrb_fixnum_value(16));
    mrb_define_const(mrb, type, "AAAA", mrb_fixnum_value(28));

    mrb_define_const(mrb, class, "IN", mrb_fixnum_value(1));

    mrb_define_unbound_result(mrb);
 }

void mrb_mruby_unbound_gem_final(mrb_state *mrb)
{
}


