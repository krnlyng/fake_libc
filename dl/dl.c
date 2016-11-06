#include <stdio.h>
#include <unistd.h>
#include <link.h>
#include <string.h>

static void *(*glibc_dlmopen)(int id, const char *filename, int flags);
static void *(*glibc_dlsym)(void *handle, const char *symbol);
//static void *(*glibc_dlvsym)(void *handle, const char *symbol, const char *version);
static int *(*glibc_dladdr)(void *addr, void *info);
static char *(*glibc_dlerror)(void);
static int (*glibc_dlclose)(void *handle);
static int (*glibc_dl_iterate_phdr)(int (*callback) (void *info, size_t size, void *data), void *data);

// initialized from main program which knows which id the android libraries have.
static int android_dl_namespace_id;

void init_dlfunctions(int a_dl_ns_id, void *op, void *sym, void *addr, void *err, void *clo, void *it)
{
    android_dl_namespace_id = a_dl_ns_id;
    glibc_dlmopen = op;
    glibc_dlsym = sym;
    glibc_dladdr = addr;
    glibc_dlerror = err;
    glibc_dlclose = clo;
    glibc_dl_iterate_phdr = it;
}

__asm__ (".symver dlopen, dlopen@LIBC");
__asm__ (".symver dladdr, dladdr@LIBC");
__asm__ (".symver dlsym, dlsym@LIBC");
__asm__ (".symver dlerror, dlerror@LIBC");
__asm__ (".symver dlclose, dlclose@LIBC");

void *dlopen(const char *filename, int flags)
{
    if(flags == 0) flags = 1;
    return glibc_dlmopen(android_dl_namespace_id, filename, flags);
}

void *dlsym(void *handle, const char *symbol)
{
    return glibc_dlsym(handle, symbol);
}

int dladdr(void *addr, void *info)
{
    return glibc_dladdr(addr, info);
}

char *dlerror(void)
{
    return glibc_dlerror();
}

int dlclose(void *handle)
{
    return glibc_dlclose(handle);
}

int dl_iterate_phdr(int (*callback) (void *info, size_t size, void *data), void *data)
{
    return glibc_dl_iterate_phdr(callback, data);
}

