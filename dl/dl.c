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

static char cur_filename[4096];

static char symbols[4096][4096];;
static int symbol_count = 0;

// THIS IS VERY SILLY BECAUSE dlsym crashes often when a symbol does not exist
// and the library was loaded with dlmopen... TODO
void *dlopen(const char *filename, int flags)
{
    if(flags == 0) flags = 1;

    strcpy(cur_filename, filename);

    // dlmopen crashes when file does not exist?
    if(filename[0] == '/' && access(filename, F_OK))
    {
        symbol_count = 0;
        return NULL;
    }

    char command[4096];
    snprintf(command, 4096, "LD_DEBUG= /usr/bin/readelf -Ws %s | /bin/grep GLOBAL |/bin/awk '{print $8}'", cur_filename);
    fprintf(stderr, command);
    FILE *fp = popen(command, "r");
    if(fp != NULL)
    {
        int i = 0;
        int n = 4096;
        int read = 0;
        char *test = NULL;
        while((read = getline(&test, &n, fp)) != -1)
        {
            if(i >= 4096)
            {
                exit(-1);
            }
            if(!n) { continue;}
            test[read-1] = 0;
            strcpy(symbols[i], test);
            i++; 
            symbol_count = i;
        }
        free(test);
        pclose(fp);
    }
    else symbol_count = 0;

    void *ret= glibc_dlmopen(android_dl_namespace_id, filename, flags);
    return ret;
}

// TODO unstoopidize
//
void *dlsym(void *handle, const char *symbol)
{

    for(unsigned int i=0;i<symbol_count;i++)
    {
        if(strcmp(symbols[i], symbol) == 0)
        {
            return glibc_dlsym(handle, symbol);
        }
    }

    return NULL;
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

// TODO
int dl_iterate_phdr(int (*callback) (void *info, size_t size, void *data), void *data)
{
    return glibc_dl_iterate_phdr(callback, data);
}

