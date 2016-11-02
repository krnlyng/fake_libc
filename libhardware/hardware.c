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
        int android_namespace_id = 0; \
		libhardware_handle = dlmopen(LM_ID_NEWLM, LIBHARDWARE_PATH, RTLD_LAZY); \
        dlinfo(libhardware_handle, RTLD_DI_LMID, &android_namespace_id); \
        void *libdlhandle = dlmopen(android_namespace_id, "libdl.so", RTLD_LAZY); \
        *(unsigned int*)dlsym(libdlhandle, "android_namespace_id") = android_namespace_id; \
		assert(libhardware_handle != NULL); \
	}
        //dlmopen(LM_ID_BASE, LIBHARDWARE_PATH, RTLD_LAZY);

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

