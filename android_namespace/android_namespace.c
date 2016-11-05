#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>

int android_dl_namespace_id = -1;

void *android_dlopen(const char *name, int flags)
{
    if(android_dl_namespace_id == -1)
    {
    	int (*init_dl)(int);
	    void *libchandle = dlmopen(LM_ID_NEWLM, "libc.so", RTLD_LAZY);
    	assert(libchandle != NULL);
	    dlinfo(libchandle, RTLD_DI_LMID, &android_dl_namespace_id);
    	init_dl = dlsym(libchandle, "init_dl");
	    assert(init_dl != NULL);
    	init_dl(android_dl_namespace_id);
    }

    return dlmopen(android_dl_namespace_id, name, flags);
}

