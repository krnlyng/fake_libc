#include <dlfcn.h>
#include <stdarg.h>
#include <assert.h>

#include "helpers.h"

#define NULL ((void*) 0)
#define SOFTFP __attribute__((pcs("aapcs")))

typedef long unsigned int* _Unwind_Ptr;

static void *glibc_handle = NULL;
static int (*glibc_vfprintf)(void *fp, const char *fmt, ...);
static _Unwind_Ptr (*glibc___gnu_Unwind_Find_exidx)(_Unwind_Ptr pc, int *pcount);
static void (*glibc___cxa_finalize)(void *dso_handle);
static int (*glibc___cxa_atexit)(void (*)(void *), void *, void *);
static int (*glibc___register_atfork)(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso);
static double (*glibc_ldexp)(double, int);
static void *(*glibc_memset)(void *s, int c, size_t n);
static int (*glibc_isxdigit)(int c);
static void (*glibc_abort)(void);
static void *(*glibc_memcpy)(void *, void *, size_t);
static int (*glibc_fputs)(const char *s, void *stream);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define APPEND_FUNCTION(x, f) x ## f

#define LOAD_GLIBC() \
    if(glibc_handle == NULL) \
    { \
        glibc_handle = dlopen("/lib/libc.so.6", RTLD_NOW); \
        assert(glibc_handle != NULL); \
    }

#define LOAD_GLIBC_FUNCTION(f) \
    if(glibc_handle != NULL) \
    { \
        glibc_ ## f = dlsym(glibc_handle, #f); \
        assert(glibc_ ## f != NULL); \
    }

BIONIC_FILE __sF[3];

BIONIC_FILE *stdin = &__sF[0];
BIONIC_FILE *stdout = &__sF[1];
BIONIC_FILE *stderr = &__sF[2];

extern void *_get_actual_fp(BIONIC_FILE *fp);

int fprintf(BIONIC_FILE *fp, const char *fmt, ...) SOFTFP;

int fprintf(BIONIC_FILE *fp, const char *fmt, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfprintf);

    int ret = 0;

    va_list args;
    va_start(args,fmt);
    ret = glibc_vfprintf(_get_actual_fp(fp), fmt, args);
    va_end(args);

    return ret;
}

_Unwind_Ptr __gnu_Unwind_Find_exidx(_Unwind_Ptr pc, int *pcount) SOFTFP;

_Unwind_Ptr __gnu_Unwind_Find_exidx(_Unwind_Ptr pc, int *pcount)
{
    LOAD_GLIBC();
    return glibc___gnu_Unwind_Find_exidx(pc, pcount);
}

void __cxa_finalize(void* dso_handle) SOFTFP;

void __cxa_finalize(void* dso_handle)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__cxa_finalize);

    glibc___cxa_finalize(dso_handle);
}

int __cxa_atexit(void (*p1)(void *), void *p2, void *p3) SOFTFP;

int __cxa_atexit(void (*p1)(void *), void *p2, void *p3)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__cxa_atexit);

    return glibc___cxa_atexit(p1, p2, p3);
}

int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso) SOFTFP;

int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__register_atfork);

    return glibc___register_atfork(prepare, parent, child, dso);
}

double ldexp(double, int) SOFTFP;

double  ldexp(double a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ldexp);

    return glibc_ldexp(a, b);
}

void *memset(void *s, int c, size_t n) SOFTFP;

void *memset(void *s, int c, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(memset);

    return glibc_memset(s, c, n);
}

int isxdigit(int c) SOFTFP;

int isxdigit(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isxdigit);

    return glibc_isxdigit(c);
}

void abort(void) SOFTFP;

void abort(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(abort);
    glibc_abort();

    for(;;); // noreturn
}

void *memcpy(void *a, void *b, size_t s) SOFTFP;

void *memcpy(void *a, void *b, size_t s)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(memcpy);

    return glibc_memcpy(a, b, s);
}

int fputs(const char *s, void *stream) SOFTFP;

int fputs(const char *s, void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fputs);

    return glibc_fputs(s, _get_actual_fp(stream));
}

