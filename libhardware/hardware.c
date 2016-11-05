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

extern void *android_dlopen(const char *name, int flags);

#define LOAD_LIBHARDWARE() \
    if(!libhardware_handle) \
    { \
        libhardware_handle = android_dlopen(LIBHARDWARE_PATH, RTLD_LAZY); \
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

