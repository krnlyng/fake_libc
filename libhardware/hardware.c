#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <assert.h>
#include <hardware/hardware.h>

#define LIBHARDWARE_PATH "/system/lib/libhardware.so"

#define SOFTFP __attribute__((pcs("aapcs")))

static void *libhardware_handle = NULL;

static int (*libhardware_hw_get_module)(const char *, const struct hw_module_t **) SOFTFP;
static int (*libhardware_hw_get_module_by_class)(const char *, const char *m, const struct hw_module_t **) SOFTFP;

#define LOAD_LIBHARDWARE() \
    if(!libhardware_handle) \
    { \
        int android_dl_namespace_id = 0; \
        int (*init_dl)(int); \
        void *libchandle = dlmopen(LM_ID_NEWLM, "libc.so", RTLD_NOW); \
        assert(libchandle != NULL); \
        dlinfo(libchandle, RTLD_DI_LMID, &android_dl_namespace_id); \
        init_dl = dlsym(libchandle, "init_dl"); \
        assert(init_dl != NULL); \
        init_dl(android_dl_namespace_id); \
        libhardware_handle = dlmopen(android_dl_namespace_id, LIBHARDWARE_PATH, RTLD_LAZY); \
        assert(libhardware_handle != NULL); \
    }

#define LOAD_LIBHARDWARE_FUNCTION(f) \
    if(!libhardware_ ## f) \
    { \
        libhardware_ ## f = dlsym(libhardware_handle, #f); \
    }

int hw_get_module(const char *n, const struct hw_module_t **m)
{
    LOAD_LIBHARDWARE();

    LOAD_LIBHARDWARE_FUNCTION(hw_get_module);

    return libhardware_hw_get_module(n, m);
}

int hw_get_module_by_class(const char *n, const char *m, const struct hw_module_t **o)
{
    LOAD_LIBHARDWARE();

    LOAD_LIBHARDWARE_FUNCTION(hw_get_module_by_class);

    return libhardware_hw_get_module_by_class(n, m, o);
}

