__asm__ (".symver dlopen, dlopen@LIBC");
__asm__ (".symver dladdr, dladdr@LIBC");
__asm__ (".symver dlsym, dlsym@LIBC");
__asm__ (".symver dlerror, dlerror@LIBC");
__asm__ (".symver dlclose, dlclose@LIBC");

__asm__ (".symver glibc_dlopen, dlopen@GLIBC_2.4");
__asm__ (".symver glibc_dladdr, dladdr@GLIBC_2.4");
__asm__ (".symver glibc_dlsym, dlsym@GLIBC_2.4");
__asm__ (".symver glibc_dlerror, dlerror@GLIBC_2.4");
__asm__ (".symver glibc_dlclose, dlclose@GLIBC_2.4");

void *dlopen(const char *filename, int flags)
{
    return glibc_dlopen(filename, flags);
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

