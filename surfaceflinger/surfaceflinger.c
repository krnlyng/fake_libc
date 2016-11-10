
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <dlfcn.h>

extern void *android_dlopen(const char *name, int flags);

static void *surfaceflinger_handle = NULL;

extern int setenv (const char *, const char *, int);

void start_surfaceflinger()
{
    int (*main_surfaceflinger)(int argc, char *argv[]) = NULL;

    surfaceflinger_handle = android_dlopen("surfaceflinger.so", RTLD_LAZY);

    assert(surfaceflinger_handle != NULL);

    main_surfaceflinger = dlsym(surfaceflinger_handle, "main");

    assert(main_surfaceflinger != NULL);

    // tmp hack
    setenv("OPENSSL_armcap", "0", 1);
    char *arg = "/system/bin/surfaceflinger";
    char **argv = malloc(sizeof(char*));
    *argv = arg;
    main_surfaceflinger(1, argv);
}

int main(int argc, char *argv[])
{
    start_surfaceflinger();

    return 0;
}

