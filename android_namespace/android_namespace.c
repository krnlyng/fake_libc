#define _GNU_SOURCE
#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
#include <link.h>

int android_dl_namespace_id = -1;

void *android_dlopen(const char *name, int flags)
{
    if(android_dl_namespace_id == -1)
    {
        int (*init_dl)(int id, void *op, void *sym, void *addr, void *clo, void *it, char *pron);
        void *libchandle = dlmopen(LM_ID_NEWLM, "libc.so", RTLD_LAZY);
        assert(libchandle != NULL);
        dlinfo(libchandle, RTLD_DI_LMID, &android_dl_namespace_id);
        init_dl = dlsym(libchandle, "init_dl");
        assert(init_dl != NULL);
        init_dl(android_dl_namespace_id, dlmopen, dlsym, dladdr, dlclose, dl_iterate_phdr, program_invocation_name);
    }

    return dlmopen(android_dl_namespace_id, name, flags);
}

