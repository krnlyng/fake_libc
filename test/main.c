#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char *argv[])
{
    void (*test_function)(void);

    void *handle = dlopen("/root/fake_libc/test.so", RTLD_NOW);
    if(handle == NULL)
    {
        printf("loading failed: %s\n", dlerror());
        return -1;
    }

    test_function = dlsym(handle, "test_function");

    test_function();

    dlclose(handle);
}

