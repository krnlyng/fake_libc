#define _GNU_SOURCE
#include "helpers.h"
#include <link.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <linux/posix_types.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <string.h>

#define LM_ID_BASE 0
#define LM_ID_NEW -1

/* Base address to check for Android specifics */                               
#define ANDROID_TOP_ADDR_VALUE_MUTEX  0xFFFF                                    
#define ANDROID_TOP_ADDR_VALUE_COND   0xFFFF                                    
#define ANDROID_TOP_ADDR_VALUE_RWLOCK 0xFFFF                                    
                                                                                
#define ANDROID_MUTEX_SHARED_MASK      0x2000                                   
#define ANDROID_COND_SHARED_MASK       0x0001                                   
#define ANDROID_RWLOCKATTR_SHARED_MASK 0x0010                                   
                                                                                
/* For the static initializer types */                                          
#define ANDROID_PTHREAD_MUTEX_INITIALIZER            0                          
#define ANDROID_PTHREAD_RECURSIVE_MUTEX_INITIALIZER  0x4000                     
#define ANDROID_PTHREAD_ERRORCHECK_MUTEX_INITIALIZER 0x8000                     
#define ANDROID_PTHREAD_COND_INITIALIZER             0                          
#define ANDROID_PTHREAD_RWLOCK_INITIALIZER           0

enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
//#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
//#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
};

typedef off_t bionic_fpos_t;

struct __sbuf {
	unsigned char *_base;
	int	_size;
};

typedef struct __sFILE {
    unsigned char *_p;  /* current position in (some) buffer */
    int _r;     /* read space left for getc() */
    int _w;     /* write space left for putc() */
    short   _flags;     /* flags, below; this FILE is free if 0 */
    short   _file;      /* fileno, if Unix descriptor, else -1 */
    struct  __sbuf _bf; /* the buffer (at least 1 byte, if !NULL) */
    int _lbfsize;   /* 0 or -_bf._size, for inline putc */

    /* operations */
    void    *_cookie;   /* cookie passed to io functions */
    int (*_close)(void *);
    int (*_read)(void *, char *, int);
    bionic_fpos_t  (*_seek)(void *, bionic_fpos_t, int);
    int (*_write)(void *, const char *, int);

    /* extension data, to avoid further ABI breakage */
    struct  __sbuf _ext;
    /* data for long sequences of ungetc() */
    unsigned char *_up; /* saved _p when _p is doing ungetc data */
    int _ur;        /* saved _r when _r is counting ungetc data */

    /* tricks to meet minimum requirements even when malloc() fails */
    unsigned char _ubuf[3]; /* guarantee an ungetc() buffer */
    unsigned char _nbuf[1]; /* guarantee a getc() buffer */

    /* separate buffer for fgetln() when line crosses buffer boundary */
    struct  __sbuf _lb; /* buffer for fgetln() */

    /* Unix stdio files get aligned to block boundaries on fseek() */
    int _blksize;   /* stat.st_blksize (may be != _bf._size) */
    bionic_fpos_t  _offset;    /* current lseek offset */
} BIONIC_FILE;

typedef long unsigned int* _Unwind_Ptr;
typedef __WCHAR_TYPE__ wchar_t;

typedef struct {
  uint8_t __seq[4];
#ifdef __LP64__
  char __reserved[4];
#endif
} mbstate_t;

typedef long wctype_t;

void *glibc_handle = NULL;
void *rt_handle = NULL;
void *gcc_handle = NULL;
void *pthread_handle = NULL;

void *setjmp_handle = NULL;

// glibc protoypes
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

int *__h_errno_location(void);

int clock_gettime(int clk_id, void *res);
void abort(void) __attribute__((noreturn));
char* __strncpy_chk(char *a, const char* b, size_t c, size_t d);

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

// dynamically loaded glibc/rt/pthread/setjmp functions
static void (*glibc___cxa_finalize)(void *dso_handle) = NULL;

static int (*glibc_getc)(void *stream) = NULL;
static int (*glibc_ungetc)(int c, void *stream) = NULL;
static __WINT_TYPE__ (*glibc_getwc)(void *) = NULL;
static __WINT_TYPE__ (*glibc_putwc)(wchar_t, void *) = NULL;

static double (*glibc_wcstod)(const wchar_t*, wchar_t**) = NULL;
static float (*glibc_wcstof)(const wchar_t*, wchar_t**) = NULL;
static long double (*glibc_wcstold)(const wchar_t*, wchar_t**) = NULL;
static double (*glibc_atof)(const char*a) = NULL;
static double (*glibc_strtod)(const char*, char**) = NULL;
static float (*glibc_strtof)(const char*, char**) = NULL;
static long double (*glibc_strtold)(const char*, char**) = NULL;
static long double (*glibc_strtold_l)(const char *, char **, void*) = NULL;
static long double (*glibc_wcstold_l)(const wchar_t *a, wchar_t **b, void* c) = NULL;
static double (*glibc_difftime)(time_t, time_t) = NULL;

static ssize_t (*glibc_getline)(char **lineptr, size_t *n, void *stream) = NULL;
static int (*glibc_vswprintf)(wchar_t*, size_t, const wchar_t*, va_list) = NULL;
static int (*glibc_vswscanf)(const wchar_t*, const wchar_t*, va_list) = NULL;
static int (*glibc_vwprintf)(const wchar_t*, va_list) = NULL;
static int (*glibc_vwscanf)(const wchar_t*, va_list) = NULL;
static int (*glibc_vscanf)(const char *format, va_list ap) = NULL;
static int (*glibc_vsscanf)(const char *str, const char *format, va_list ap) = NULL;
static int (*glibc_sscanf)(const char *str, const char *format, ...) = NULL;
static int (*glibc___vsprintf_chk)(char * str, int flag, size_t strlen, const char * format, va_list ap) = NULL;
static int (*glibc___vsnprintf_chk)(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...) = NULL;
static int (*glibc_vasprintf)(char **strp, const char *fmt, va_list ap) = NULL;
static int (*glibc_vfprintf)(void *fp, const char *fmt, va_list ap) = NULL;
static int (*glibc_fprintf)(void *stream, const char *format, ...) = NULL;
static double (*glibc_ldexp)(double, int) = NULL;
static int (*glibc_fputs)(const char *s, void *stream) = NULL;
static int (*glibc_fputc)(int c, void *stream) = NULL;
int (*glibc_pthread_mutex_lock)(void *__mutex) = NULL;
int (*glibc_pthread_mutex_unlock)(void *__mutex) = NULL;
static int (*glibc_pthread_cond_timedwait)(void *restrict cond, void *restrict mutex, void *restrict abstime) = NULL;
static int (*glibc_pthread_cond_wait)(void *restrict cond, void *restrict mutex) = NULL;
static int (*glibc_pthread_cond_broadcast)(void *cond) = NULL;
static int (*glibc_pthread_cond_signal)(void *cond) = NULL;
static int (*glibc_pthread_cond_destroy)(void *cond) = NULL;
static int (*glibc_pthread_cond_init)(void *restrict cond, const void *restrict attr) = NULL;
static int (*glibc_fgetc)(void *stream) = NULL;
static char *(*glibc_fgets)(char *s, int size, void *stream) = NULL;
static size_t (*glibc_fwrite)(const void *ptr, size_t size, size_t nmemb, void *stream) = NULL;
static size_t (*glibc_fread)(void *ptr, size_t size, size_t nmemb, void *stream) = NULL;
static int (*glibc_fflush)(void *stream) = NULL;
static int (*glibc_vfscanf)(void *stream, const char *format, va_list ap) = NULL;
static __WINT_TYPE__ (*glibc_fgetwc)(void *) = NULL;
static wchar_t *(*glibc_fgetws)(wchar_t *, int, void *) = NULL;
static __WINT_TYPE__ (*glibc_fputwc)(wchar_t, void *) = NULL;
static int (*glibc_fputws)(const wchar_t *, void *) = NULL;
static int (*glibc_fwide)(void *, int) = NULL;
static int (*glibc_vfwprintf)(void*, const wchar_t*, va_list) = NULL;
static int (*glibc_vfwscanf)(void*, const wchar_t*, va_list) = NULL;

static double (*glibc_drand48)(void) = NULL;
static double (*glibc_erand48)(unsigned short a[3]) = NULL;

static int (*glibc_pthread_mutex_destroy)(void *mutex) = NULL;
static int (*glibc_pthread_mutex_init)(void *restrict mutex, const void *restrict attr) = NULL;
static int (*glibc_pthread_mutex_trylock)(void *mutex) = NULL;

//static int (*glibc_printf)(const char *format, ...) = NULL;
//static int (*glibc___sprintf_chk)(char * str, int flag, size_t sl, const char * format, ...) = NULL;
//static int (*glibc_dprintf)(int fd, const char *format, ...) = NULL;
//static int (*glibc_sprintf)(char *str, const char *format, ...) = NULL;
//static int (*glibc_snprintf)(char *str, size_t size, const char *format, ...) = NULL;
static int (*glibc_vprintf)(const char *format, va_list ap) = NULL;
static int (*glibc_vdprintf)(int fd, const char *format, va_list ap) = NULL;
static int (*glibc_vsprintf)(char *str, const char *format, va_list ap) = NULL;
static int (*glibc_vsnprintf)(char *str, size_t size, const char *format, va_list ap) = NULL;

static long (*glibc_sysconf)(int name) = NULL;

static void *(*glibc_fdopen)(int fd, const char *mode) = NULL;
static void *(*glibc_freopen)(const char *path, const char *mode, void *stream) = NULL;
static void *(*glibc_fopen)(const char *path, const char *mode) = NULL;
static int (*glibc_fclose)(void *stream) = NULL;
static int (*glibc_feof)(void *) = NULL;

static int (*rt_pthread_create)(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
long (*glibc_syscall)(long number, ...) = NULL;
static int (*glibc_pthread_attr_init)(pthread_attr_t *__attr) = NULL;
static int (*glibc_pthread_attr_destroy)(pthread_attr_t *__attr) = NULL;
static int (*glibc_pthread_attr_setdetachstate)(pthread_attr_t *__attr, int state) = NULL;
static int (*glibc_pthread_attr_getdetachstate)(pthread_attr_t const *__attr, int *state) = NULL;
static int (*glibc_pthread_attr_setschedpolicy)(pthread_attr_t *__attr, int policy) = NULL;
static int (*glibc_pthread_attr_getschedpolicy)(pthread_attr_t const *__attr, int *policy) = NULL;
static int (*glibc_pthread_attr_setschedparam)(pthread_attr_t *__attr, void const *param) = NULL;
static int (*glibc_pthread_attr_getschedparam)(pthread_attr_t const *__attr, void *param) = NULL;
static int (*glibc_pthread_attr_setstacksize)(pthread_attr_t *__attr, size_t stack_size) = NULL;
static int (*glibc_pthread_attr_getstacksize)(pthread_attr_t const *__attr, size_t *stack_size) = NULL;
static int (*glibc_pthread_attr_setstackaddr)(pthread_attr_t *__attr, void *stack_addr) = NULL;
static int (*glibc_pthread_attr_getstackaddr)(pthread_attr_t const *__attr, void **stack_addr) = NULL;
static int (*glibc_pthread_attr_setstack)(pthread_attr_t *__attr, void *stack_base, size_t stack_size) = NULL;
static int (*glibc_pthread_attr_getstack)(pthread_attr_t const *__attr, void **stack_base, size_t *stack_size) = NULL;
static int (*glibc_pthread_attr_setguardsize)(pthread_attr_t *__attr, size_t guard_size) = NULL;
static int (*glibc_pthread_attr_getguardsize)(pthread_attr_t const *__attr, size_t *guard_size) = NULL;
static int (*glibc_pthread_attr_setscope)(pthread_attr_t *__attr, int scope) = NULL;
static int (*glibc_pthread_attr_getscope)(pthread_attr_t const *__attr, int *scope) = NULL;
static int (*glibc_pthread_cond_timedwait_relative_np)(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime) = NULL;
static int (*glibc_pthread_condattr_init)(pthread_condattr_t *attr) = NULL;
int (*glibc_readdir_r)(void *dir, void *entry, void **result) = NULL;
void *(*glibc_readdir)(void *dir) = NULL;
static int (*glibc_ferror)(void *stream) = NULL;

static int (*glibc_finite)(double x) = NULL;
static int (*glibc_finitef)(float x) = NULL;
static int (*glibc_finitel)(long double x) = NULL;

static int (*glibc_isinf)(double x) = NULL;
static int (*glibc_isinff)(float x) = NULL;
static int (*glibc_isinfl)(long double x) = NULL;

static int (*glibc_isnan)(double x) = NULL;
static int (*glibc_isnanf)(float x) = NULL;
static int (*glibc_isnanl)(long double x) = NULL;
int (*glibc_getaddrinfo)(const char *hostname, const char *servname,
    const void *hints, void **res) = NULL;
int (*glibc_freeaddrinfo)(void *__ai) = NULL;

static int (*glibc_fseek)(void *stream, long offset, int whence) = NULL;
static int (*glibc_fseeko)(void *stream, off_t offset, int whence) = NULL;
static long (*glibc_ftell)(void *stream) = NULL;
static int (*glibc_fileno)(void *stream) = NULL;
static void (*glibc_rewind)(BIONIC_FILE *stream) = NULL;
static off_t (*glibc_ftello)(void *stream) = NULL;
static void *(*glibc_funopen)(const void *cookie, int (*readfn)(void *, char *, int), int (*writefn)(void *, const char *, int), off_t (*seekfn)(void *, off_t, int), int (*closefn)(void *)) = NULL;
static int (*glibc_fsetpos)(void *stream, const void *pos) = NULL;
static int (*glibc_fgetpos)(void *stream, void *pos) = NULL;

static int *(*glibc___h_errno_location)(void) = NULL;

static void **glibc_stdin = NULL;
static void **glibc_stdout = NULL;
static void **glibc_stderr = NULL;

void *(*glibc_dlmopen)(int lmid, const char *filename, int flags) = NULL;
void *(*glibc_dlsym)(void *handle, const char *symbol) = NULL;

int android_dl_namespace_id = 0;

#define LOAD_GLIBC() \
    if(glibc_handle == NULL) \
    { \
        glibc_handle = glibc_dlmopen(LM_ID_BASE, "/lib/libc.so.6", RTLD_LAZY); \
    }

#define LOAD_GLIBC_SYMBOL(f) \
    if(glibc_ ## f == NULL) \
    { \
        LOAD_GLIBC(); \
        glibc_ ## f = glibc_dlsym(glibc_handle, #f); \
    }/* \
    if(!glibc_fprintf) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(!glibc_stderr) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p called from %s\n", #f, glibc_ ## f, __FUNCTION__);*/

#define LOAD_PTHREAD() \
    if(pthread_handle == NULL) \
    { \
        pthread_handle = glibc_dlmopen(LM_ID_BASE, "/lib/libpthread.so.0", RTLD_LAZY); \
    }

#define LOAD_RT() \
    if(rt_handle == NULL) \
    { \
        rt_handle = glibc_dlmopen(LM_ID_BASE, "/lib/librt.so.1", RTLD_LAZY); \
    }

#define LOAD_RT_SYMBOL(f) \
    if(rt_ ## f == NULL) \
    { \
        LOAD_RT(); \
        rt_ ## f = glibc_dlsym(rt_handle, #f); \
    }/* \
    if(!glibc_fprintf) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(!glibc_stderr) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p called from %s\n", #f, rt_ ## f, __FUNCTION__);*/

#define LOAD_GCC() \
    if(gcc_handle == NULL) \
    { \
        gcc_handle = glibc_dlmopen(LM_ID_BASE, "/usr/lib/libgcc_s.so", RTLD_LAZY); \
    }

#define LOAD_GCC_SYMBOL(f) \
    if(gcc_ ## f == NULL) \
    { \
        LOAD_GCC(); \
        gcc_ ## f = glibc_dlsym(gcc_handle, #f); \
    }/* \
    if(!glibc_fprintf) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(!glibc_stderr) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p called from %s\n", #f, gcc_ ## f, __FUNCTION__);*/

#define LOAD_SETJMP() \
    if(setjmp_handle == NULL) \
    { \
        setjmp_handle = glibc_dlmopen(android_dl_namespace_id, "libsetjmp.so", RTLD_LAZY); \
    }
#define LOAD_SETJMP_SYMBOL(f) \
    if(setjmp_ ## f == NULL) \
    { \
        LOAD_SETJMP(); \
        setjmp_ ## f = glibc_dlsym(setjmp_handle, STRINGIFY(my_ ##f); \
    }/* \
    if(!glibc_fprintf) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(!glibc_stderr) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p called from %s\n", #f, setjmp_ ## f, __FUNCTION__);*/

static void (*init_dlfunctions)(int a_dl_ns_id, void *op, void *sym, void *addr, void *err, void *clo, void *it);

void init_setjmp()
{
    LOAD_SETJMP();
}

// defined below in android symbol section
extern BIONIC_FILE __sF[3];
extern char *__progname;

// provide dlopen and friends for fake libdl.so
void init_dl(int a_dl_ns_id, void *op, void *sym, void *addr, void *err, void *clo, void *it, char *programname)
{
    glibc_dlmopen = op;
    glibc_dlsym = sym;

    LOAD_GLIBC();
    LOAD_RT();
    LOAD_GCC();
    LOAD_PTHREAD();

    android_dl_namespace_id = a_dl_ns_id;

    void *libdl = glibc_dlmopen(android_dl_namespace_id, "libdl.so", RTLD_LAZY);
    init_dlfunctions = glibc_dlsym(libdl, "init_dlfunctions");

    init_dlfunctions(android_dl_namespace_id, op, sym, addr, err, clo, it);

    LOAD_GLIBC_SYMBOL(syscall);
    __progname = programname;
}

static void *_get_actual_fp(BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(stdin);
    LOAD_GLIBC_SYMBOL(stdout);
    LOAD_GLIBC_SYMBOL(stderr);

    if (fp == &__sF[0])
        return *glibc_stdin;
    else if (fp == &__sF[1])
        return *glibc_stdout;
    else if (fp == &__sF[2])
        return *glibc_stderr;

    return fp;
}



// symbols for android
char *__progname = NULL;
uintptr_t __stack_chk_guard = 0;

BIONIC_FILE __sF[3];

BIONIC_FILE *stdin = &__sF[0];
BIONIC_FILE *stdout = &__sF[1];
BIONIC_FILE *stderr = &__sF[2];

void *_get_actual_fp(BIONIC_FILE *fp);

int __sprintf_chk(char * str, int flag, size_t sl, const char * format, ...)
{
    LOAD_GLIBC_SYMBOL(__vsprintf_chk);

    int ret = 0;
    va_list ap;
    va_start(ap, format);
    ret = glibc___vsprintf_chk(str, flag, sl, format, ap);
    va_end(ap);

    return ret;
}

int fprintf(BIONIC_FILE *fp, const char *fmt, ...) SOFTFP;

int vfprintf(BIONIC_FILE *fp, const char *fmt, va_list ap) SOFTFP;

int vfprintf(BIONIC_FILE *fp, const char *fmt, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vfprintf);

    return glibc_vfprintf(_get_actual_fp(fp), fmt, ap);
}

int fprintf(BIONIC_FILE *fp, const char *fmt, ...)
{
    LOAD_GLIBC_SYMBOL(vfprintf);

    int ret = 0;

    va_list args;
    va_start(args, fmt);
    ret = glibc_vfprintf(_get_actual_fp(fp), fmt, args);
    va_end(args);

    return ret;
}

double ldexp(double, int) SOFTFP;

double  ldexp(double a, int b)
{
    LOAD_GLIBC_SYMBOL(ldexp);

    return glibc_ldexp(a, b);
}

int fputs(const char *s, void *stream) SOFTFP;

int fputs(const char *s, void *stream)
{
    LOAD_GLIBC_SYMBOL(fputs);

    return glibc_fputs(s, _get_actual_fp(stream));
}

int fputc(int c, void *stream) SOFTFP;

int fputc(int c, void *stream)
{
    LOAD_GLIBC_SYMBOL(fputc);

    return glibc_fputc(c, _get_actual_fp(stream));
}

int vasprintf(char **strp, const char *fmt, va_list ap) SOFTFP;

int vasprintf(char **strp, const char *fmt, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vasprintf);

    return glibc_vasprintf(strp, fmt, ap);
}

int asprintf(char **strp, const char *fmt, ...) SOFTFP;

int asprintf(char **strp, const char *fmt, ...)
{
    LOAD_GLIBC_SYMBOL(vasprintf);

    int ret = 0;

    va_list ap;
    va_start(ap, fmt);
    ret = glibc_vasprintf(strp, fmt, ap);
    va_end(ap);

    return ret;
}

void android_set_abort_message(const char* msg) SOFTFP;

void android_set_abort_message(const char* msg)
{
    LOAD_GLIBC_SYMBOL(fprintf);

    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "stub: android_set_abort_message(%s)\n", msg);
}

void openlog(const char* log_tag, int options, int facility) SOFTFP;

void openlog(const char* log_tag, int options, int facility)
{
    LOAD_GLIBC_SYMBOL(fprintf);

    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "stub: openlog(%s %d %d)\n", log_tag, options, facility);
}

void syslog(int priority, const char* fmt, ...) SOFTFP;

void syslog(int priority, const char* fmt, ...)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(vfprintf);

    va_list args;
    va_start(args, fmt);
    glibc_vfprintf(*glibc_stderr, fmt, args);
    va_end(args);
}

void closelog(void) SOFTFP;

void closelog()
{
    LOAD_GLIBC_SYMBOL(fprintf);

    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "stub: closelog()\n");
}

int __snprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...) SOFTFP;

int __snprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...)
{
    LOAD_GLIBC_SYMBOL(__vsnprintf_chk);

    int ret = 0;
    va_list va;
    va_start(va, format);
    ret = glibc___vsnprintf_chk(dest, supplied_size, flags, dest_len_from_compiler, format, va);
    va_end(va);

    return ret;
}

int __vsnprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, va_list va) SOFTFP;

int __vsnprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, va_list va)
{
    LOAD_GLIBC_SYMBOL(__vsnprintf_chk);

    return glibc___vsnprintf_chk(dest, supplied_size, flags, dest_len_from_compiler, format, va);
}

static void hybris_set_mutex_attr(unsigned int android_value, void *attr)
{
    /* Init already sets as PTHREAD_MUTEX_NORMAL */
    pthread_mutexattr_init(attr);

    if (android_value & ANDROID_PTHREAD_RECURSIVE_MUTEX_INITIALIZER) {
        pthread_mutexattr_settype(attr, PTHREAD_MUTEX_RECURSIVE);
    } else if (android_value & ANDROID_PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
        pthread_mutexattr_settype(attr, PTHREAD_MUTEX_ERRORCHECK);
    }
}

static void* hybris_alloc_init_mutex(unsigned int android_mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_init);
    void *realmutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutexattr_t attr;
    hybris_set_mutex_attr(android_mutex, &attr);
    glibc_pthread_mutex_init(realmutex, &attr);
    return realmutex;
}

int pthread_attr_init(pthread_attr_t *__attr)
{
    LOAD_GLIBC_SYMBOL(pthread_attr_init);
    pthread_attr_t *realattr;

    realattr = malloc(sizeof(pthread_attr_t)); 
    *((unsigned int *)__attr) = (unsigned int) realattr;

    return glibc_pthread_attr_init(realattr);
}

int pthread_attr_destroy(pthread_attr_t *__attr)
{
    int ret;
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;

    LOAD_GLIBC_SYMBOL(pthread_attr_destroy);

    ret = glibc_pthread_attr_destroy(realattr);
    /* We need to release the memory allocated at pthread_attr_init
     * Possible side effects if destroy is called without our init */
    free(realattr);

    return ret;
}

int pthread_attr_setdetachstate(pthread_attr_t *__attr, int state)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setdetachstate);
    return glibc_pthread_attr_setdetachstate(realattr, state);
}

int pthread_attr_getdetachstate(pthread_attr_t const *__attr, int *state)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getdetachstate);
    return glibc_pthread_attr_getdetachstate(realattr, state);
}

int pthread_attr_setschedpolicy(pthread_attr_t *__attr, int policy)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setschedpolicy);
    return glibc_pthread_attr_setschedpolicy(realattr, policy);
}

int pthread_attr_getschedpolicy(pthread_attr_t const *__attr, int *policy)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getschedpolicy);
    return glibc_pthread_attr_getschedpolicy(realattr, policy);
}

int pthread_attr_setschedparam(pthread_attr_t *__attr, void const *param)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setschedparam);
    return glibc_pthread_attr_setschedparam(realattr, param);
}

int pthread_attr_getschedparam(pthread_attr_t const *__attr, void *param)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getschedparam);
    return glibc_pthread_attr_getschedparam(realattr, param);
}

int pthread_attr_setstacksize(pthread_attr_t *__attr, size_t stack_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setstacksize);
    return glibc_pthread_attr_setstacksize(realattr, stack_size);
}

int pthread_attr_getstacksize(pthread_attr_t const *__attr, size_t *stack_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getstacksize);
    return glibc_pthread_attr_getstacksize(realattr, stack_size);
}

int pthread_attr_setstackaddr(pthread_attr_t *__attr, void *stack_addr)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setstackaddr);
    return glibc_pthread_attr_setstackaddr(realattr, stack_addr);
}

int pthread_attr_getstackaddr(pthread_attr_t const *__attr, void **stack_addr)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getstackaddr);
    return glibc_pthread_attr_getstackaddr(realattr, stack_addr);
}

int pthread_attr_setstack(pthread_attr_t *__attr, void *stack_base, size_t stack_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setstack);
    return glibc_pthread_attr_setstack(realattr, stack_base, stack_size);
}

int pthread_attr_getstack(pthread_attr_t const *__attr, void **stack_base, size_t *stack_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getstack);
    return glibc_pthread_attr_getstack(realattr, stack_base, stack_size);
}

int pthread_attr_setguardsize(pthread_attr_t *__attr, size_t guard_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setguardsize);
    return glibc_pthread_attr_setguardsize(realattr, guard_size);
}

int pthread_attr_getguardsize(pthread_attr_t const *__attr, size_t *guard_size)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_getguardsize);
    return glibc_pthread_attr_getguardsize(realattr, guard_size);
}

int pthread_attr_setscope(pthread_attr_t *__attr, int scope)
{
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;
    LOAD_GLIBC_SYMBOL(pthread_attr_setscope);
    return glibc_pthread_attr_setscope(realattr, scope);
}

int pthread_attr_getscope(pthread_attr_t const *__attr)
{
    int scope;
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;

    LOAD_GLIBC_SYMBOL(pthread_attr_getscope);
    /* Android doesn't have the scope attribute because it always
     * returns PTHREAD_SCOPE_SYSTEM */
    glibc_pthread_attr_getscope(realattr, &scope);

    return scope;
}

// PTHREAD TODO!
int pthread_mutex_lock(void *__mutex) SOFTFP;

int pthread_mutex_lock(void *__mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_lock);

    unsigned int value = (*(unsigned int*)__mutex);

    if(value <= ANDROID_TOP_ADDR_VALUE_MUTEX)
    {
        *(unsigned int*)__mutex = (unsigned int)hybris_alloc_init_mutex(value);
        value = *(unsigned int*)__mutex;
    }

    int ret = glibc_pthread_mutex_lock((void*)value);
    return ret;
}

int pthread_mutex_unlock(void *__mutex) SOFTFP;

int pthread_mutex_unlock(void *__mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_unlock);

    int ret = glibc_pthread_mutex_unlock((void*)*(unsigned int*)__mutex);
    return ret;
}

long __set_errno_internal(int e)
{
    errno = e;
    return -1;
}

void *my_start_routine(void *con)
{
    void **container = con;
    void *(*start_routine)(void *) = container[0];

    pthread_t thread = pthread_self();
    pid_t pid = gettid();

    my_settid_to_thread(thread, pid);

    start_routine(container[1]);

    my_remtid(thread);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) SOFTFP;
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
    LOAD_RT_SYMBOL(pthread_create);
    LOAD_GLIBC_SYMBOL(pthread_mutex_lock);
    LOAD_GLIBC_SYMBOL(pthread_mutex_unlock);

    pthread_attr_t *realattr = NULL;

    if (attr != NULL)
            realattr = (pthread_attr_t *) *(unsigned int *) attr;

    void *container[2];
    container[0] = start_routine;
    container[1] = arg;

    int ret = rt_pthread_create(thread, realattr, my_start_routine, &container);
    return ret;
}

static pthread_cond_t* hybris_alloc_init_cond(void)
{
    pthread_cond_t *realcond = malloc(sizeof(pthread_cond_t));

    LOAD_GLIBC_SYMBOL(pthread_condattr_init);
    LOAD_GLIBC_SYMBOL(pthread_cond_init);

    pthread_condattr_t attr;
    glibc_pthread_condattr_init(&attr);
    glibc_pthread_cond_init(realcond, &attr);
    return realcond;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    /* Both cond and mutex can be statically initialized, check for both */
    unsigned int cvalue = (*(unsigned int *) cond);
    unsigned int mvalue = (*(unsigned int *) mutex);

    LOAD_GLIBC_SYMBOL(pthread_cond_wait);

    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;

    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;

    if (mvalue <= ANDROID_TOP_ADDR_VALUE_MUTEX) {
        realmutex = hybris_alloc_init_mutex(mvalue);
        *((unsigned int *) mutex) = (unsigned int) realmutex;
    }

    return glibc_pthread_cond_wait(realcond, realmutex);
}

int pthread_cond_timedwait(pthread_cond_t *cond,
                pthread_mutex_t *mutex, const struct timespec *abstime)
{
    /* Both cond and mutex can be statically initialized, check for both */
    unsigned int cvalue = (*(unsigned int *) cond);
    unsigned int mvalue = (*(unsigned int *) mutex);

    LOAD_GLIBC_SYMBOL(pthread_cond_timedwait);

    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;

    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;

    if (mvalue <= ANDROID_TOP_ADDR_VALUE_MUTEX) {
        realmutex = hybris_alloc_init_mutex(mvalue);
        *((unsigned int *) mutex) = (unsigned int) realmutex;
    }

    return glibc_pthread_cond_timedwait(realcond, realmutex, (void*)abstime);
}

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond,
                pthread_mutex_t *mutex, const struct timespec *reltime)
{
    /* Both cond and mutex can be statically initialized, check for both */
    unsigned int cvalue = (*(unsigned int *) cond);
    unsigned int mvalue = (*(unsigned int *) mutex);

    LOAD_GLIBC_SYMBOL(pthread_cond_timedwait_relative_np);

    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;

    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;

    if (mvalue <= ANDROID_TOP_ADDR_VALUE_MUTEX) {
        realmutex = hybris_alloc_init_mutex(mvalue);
        *((unsigned int *) mutex) = (unsigned int) realmutex;
    }

#define CLOCK_REALTIME 0
    struct timespec tv;
    // rt_
    clock_gettime(CLOCK_REALTIME, &tv);
    tv.tv_sec += reltime->tv_sec;
    tv.tv_nsec += reltime->tv_nsec;
    if (tv.tv_nsec >= 1000000000) {
      tv.tv_sec++;
      tv.tv_nsec -= 1000000000;
    }
    return glibc_pthread_cond_timedwait(realcond, realmutex, &tv);
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    int ret;
    pthread_cond_t *realcond = (pthread_cond_t *) *(unsigned int *) cond;

    LOAD_GLIBC_SYMBOL(pthread_cond_destroy);

    if (!realcond) {
      return EINVAL;
    }

    glibc_pthread_cond_destroy(realcond);

    *((unsigned int *)cond) = 0;

    return ret;
}

int pthread_cond_init(pthread_cond_t *cond,
                                const pthread_condattr_t *attr)
{
    pthread_cond_t *realcond = NULL;

    LOAD_GLIBC_SYMBOL(pthread_cond_init);

    realcond = malloc(sizeof(pthread_cond_t));

    *((unsigned int *) cond) = (unsigned int) realcond;

    return glibc_pthread_cond_init(realcond, attr);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    unsigned int value = (*(unsigned int *) cond);

    LOAD_GLIBC_SYMBOL(pthread_cond_broadcast);

    pthread_cond_t *realcond = (pthread_cond_t *) value;

    if (value <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    return glibc_pthread_cond_broadcast(realcond);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    unsigned int value = (*(unsigned int *) cond);
    LOAD_GLIBC_SYMBOL(pthread_cond_signal);

    pthread_cond_t *realcond = (pthread_cond_t *) value;

    if (value <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    return glibc_pthread_cond_signal(realcond);
}

int fgetc(void *stream) SOFTFP;

int fgetc(void *stream)
{
    LOAD_GLIBC_SYMBOL(fgetc);

    return glibc_fgetc(_get_actual_fp(stream));
}

char *fgets(char *s, int size, void *stream) SOFTFP;

char *fgets(char *s, int size, void *stream)
{
    LOAD_GLIBC_SYMBOL(fgets);

    return glibc_fgets(s, size, _get_actual_fp(stream));
}

int getc(void *stream) SOFTFP;

int getc(void *stream)
{
    LOAD_GLIBC_SYMBOL(getc);

    return glibc_getc(_get_actual_fp(stream));
}

int ungetc(int c, void *stream) SOFTFP;

int ungetc(int c, void *stream)
{
    LOAD_GLIBC_SYMBOL(ungetc);

    return glibc_ungetc(c, _get_actual_fp(stream));
}

size_t fread(void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream) SOFTFP;

size_t fread(void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(fread);

    return glibc_fread(ptr, size, nmemb, _get_actual_fp(stream));
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream) SOFTFP;

size_t fwrite(const void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(fwrite);

    return glibc_fwrite(ptr, size, nmemb, _get_actual_fp(stream));
}

int fflush(BIONIC_FILE *stream) SOFTFP;

int fflush(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(fflush);

    return glibc_fflush(_get_actual_fp(stream));
}

int scanf(const char *format, ...) SOFTFP;

int scanf(const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(vscanf);

    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vscanf(format, args);
    va_end(args);

    return ret;
}

int fscanf(BIONIC_FILE *stream, const char *format, ...) SOFTFP;

int fscanf(BIONIC_FILE *stream, const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(vfscanf);

    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vfscanf(_get_actual_fp(stream), format, args);
    va_end(args);

    return ret;
}

int sscanf(const char *str, const char *format, ...) SOFTFP;

int sscanf(const char *str, const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(sscanf);

    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vsscanf(str, format, args);
    va_end(args);

    return ret;
}

int vscanf(const char *format, va_list ap) SOFTFP;

int vscanf(const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vscanf);

    return glibc_vscanf(format, ap);
}

int vsscanf(const char *str, const char *format, va_list ap) SOFTFP;

int vsscanf(const char *str, const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vsscanf);

    return glibc_vsscanf(str, format, ap);
}

int vfscanf(BIONIC_FILE *stream, const char *format, va_list ap) SOFTFP;

int vfscanf(BIONIC_FILE *stream, const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vfscanf);

    return glibc_vfscanf(_get_actual_fp(stream), format, ap);
}

int fwprintf(BIONIC_FILE *fp, const wchar_t *format, ...) SOFTFP;
int fwprintf(BIONIC_FILE *fp, const wchar_t *format, ...)
{
    LOAD_GLIBC_SYMBOL(vfwprintf);

    int ret = 0;

    va_list ap;
    va_start(ap, format);
    ret = glibc_vfwprintf(_get_actual_fp(fp), format, ap);
    va_end(ap);

    return ret;
}


int fwscanf(BIONIC_FILE *fp, const wchar_t *format, ...) SOFTFP;
int fwscanf(BIONIC_FILE *fp, const wchar_t *format, ...)
{
    LOAD_GLIBC_SYMBOL(vfwscanf);

    int ret = 0;

    va_list ap;
    va_start(ap, format);
    ret = glibc_vfwscanf(_get_actual_fp(fp), format, ap);
    va_end(ap);

    return ret;
}

__WINT_TYPE__ fgetwc(BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ fgetwc(BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(fgetwc);

    return glibc_fgetwc(_get_actual_fp(fp));
}


wchar_t *fgetws(wchar_t *a, int b, BIONIC_FILE *fp) SOFTFP;
wchar_t *fgetws(wchar_t *a, int b, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(fgetws);

    return glibc_fgetws(a, b, _get_actual_fp(fp));
}


__WINT_TYPE__ fputwc(wchar_t a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ fputwc(wchar_t a, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(fputwc);

    return glibc_fputwc(a, _get_actual_fp(fp));
}


int fputws(const wchar_t *a, BIONIC_FILE *fp) SOFTFP;
int fputws(const wchar_t *a, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(fputws);

    return glibc_fputws(a, _get_actual_fp(fp));
}


int fwide(BIONIC_FILE *fp, int a) SOFTFP;
int fwide(BIONIC_FILE *fp, int a)
{
    LOAD_GLIBC_SYMBOL(fwide);

    return glibc_fwide(_get_actual_fp(fp), a);
}

__WINT_TYPE__ getwc(BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ getwc(BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(getwc);

    return glibc_getwc(_get_actual_fp(fp));
}

__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(putwc);

    return glibc_putwc(a, _get_actual_fp(fp));
}

int swprintf(wchar_t * a, size_t b, const wchar_t *c, ...) SOFTFP;
int swprintf(wchar_t * a, size_t b, const wchar_t *c, ...)
{
    LOAD_GLIBC_SYMBOL(vswprintf);

    int ret = 0;
    va_list ap;
    va_start(ap, c);
    ret = glibc_vswprintf(a, b, c, ap);
    va_end(ap);
    return ret;
}


int swscanf(const wchar_t *a, const wchar_t *b, ...) SOFTFP;
int swscanf(const wchar_t *a, const wchar_t *b, ...)
{
    LOAD_GLIBC_SYMBOL(vswscanf);

    int ret = 0;

    va_list ap;
    va_start(ap, b);
    ret = glibc_vswscanf(a, b, ap);
    va_end(ap);
    return ret;
}

int vfwprintf(BIONIC_FILE*a, const wchar_t*b, va_list ap) SOFTFP;
int vfwprintf(BIONIC_FILE*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vfwprintf);

    return glibc_vfwprintf(a, b, ap);
}


int vfwscanf(BIONIC_FILE*a, const wchar_t*b, va_list ap) SOFTFP;
int vfwscanf(BIONIC_FILE*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vfwscanf);

    return glibc_vfwscanf(a, b, ap);
}


int vswprintf(wchar_t* a, size_t b, const wchar_t* c, va_list ap) SOFTFP;
int vswprintf(wchar_t* a, size_t b, const wchar_t* c, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vswprintf);

    return glibc_vswprintf(a, b, c, ap);
}


int vswscanf(const wchar_t*a, const wchar_t*b, va_list ap) SOFTFP;
int vswscanf(const wchar_t*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vswscanf);

    return glibc_vswscanf(a, b, ap);
}


int vwprintf(const wchar_t*a, va_list ap) SOFTFP;
int vwprintf(const wchar_t*a, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vwprintf);

    return glibc_vwprintf(a, ap);
}


int vwscanf(const wchar_t*a, va_list ap) SOFTFP;
int vwscanf(const wchar_t*a, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vwscanf);

    return glibc_vwscanf(a, ap);
}


double wcstod(const wchar_t* a,  wchar_t** b) SOFTFP;
double wcstod(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC_SYMBOL(wcstod);

    return glibc_wcstod(a, b);
}


float wcstof(const wchar_t* a,  wchar_t** b) SOFTFP;
float wcstof(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC_SYMBOL(wcstof);

    return glibc_wcstof(a, b);
}

int wprintf(const wchar_t * a,  ...) SOFTFP;
int wprintf(const wchar_t * a,  ...)
{
    LOAD_GLIBC_SYMBOL(vwprintf);

    int ret = 0;
    va_list ap;
    va_start(ap, a);
    ret = glibc_vwprintf(a, ap);
    va_end(ap);
    return ret;
}


int wscanf(const wchar_t * a,  ...) SOFTFP;
int wscanf(const wchar_t * a,  ...)
{
    LOAD_GLIBC_SYMBOL(vwscanf);

    int ret = 0;
    va_list ap;
    va_start(ap, a);
    ret = glibc_vwscanf(a, ap);
    va_end(ap);

    return ret;
}

long double wcstold(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC_SYMBOL(wcstold);

    return glibc_wcstold(a, b);
}

long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c) SOFTFP;
long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c)
{
    LOAD_GLIBC_SYMBOL(wcstold_l);

    return glibc_wcstold_l(a, b, c);
}

double atof(const char*a) SOFTFP;
double atof(const char*a)
{
    LOAD_GLIBC_SYMBOL(atof);

    return glibc_atof(a);
}

double strtod(const char*a, char**b) SOFTFP;

double strtod(const char*a, char**b)
{
    LOAD_GLIBC_SYMBOL(strtod);

    return glibc_strtod(a, b);
}

long double strtold(const char*a, char**b) SOFTFP;

long double strtold(const char*a, char**b)
{
    LOAD_GLIBC_SYMBOL(strtold);

    return glibc_strtold(a, b);
}


long double strtold_l(const char *a, char **b, void*c) SOFTFP;

long double strtold_l(const char *a, char **b, void*c)
{
    LOAD_GLIBC_SYMBOL(strtold_l);

    return glibc_strtold_l(a, b, c);
}

double drand48(void) SOFTFP;

double drand48(void)
{
    LOAD_GLIBC_SYMBOL(drand48);

    return glibc_drand48();
}

double erand48(unsigned short a[3]) SOFTFP;

double erand48(unsigned short a[3])
{
    LOAD_GLIBC_SYMBOL(erand48);

    return glibc_erand48(a);
}

int pthread_mutex_destroy(void *mutex) SOFTFP;
int pthread_mutex_init(void *restrict mutex, const void *restrict attr) SOFTFP;
int pthread_mutex_trylock(void *mutex) SOFTFP;

int pthread_mutex_destroy(void *mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_destroy);

    pthread_mutex_t *realmutex = (pthread_mutex_t *) *(unsigned int *) mutex;

    if(!realmutex) return EINVAL;
    int ret = glibc_pthread_mutex_destroy(realmutex);
    return ret;
}

int pthread_mutex_init(void *restrict mutex, const void *restrict attr)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_init);

    *(unsigned int *) mutex = (unsigned int)malloc(sizeof(pthread_mutex_t));

    return glibc_pthread_mutex_init((void*)*(unsigned int*)mutex, attr);
}

int pthread_mutex_trylock(void *mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_trylock);

    void *realmutex = (void*)*(unsigned int*)mutex;

    return glibc_pthread_mutex_trylock(realmutex);
}

float strtof(const char*a, char**b) SOFTFP;
float strtof(const char*a, char**b)
{
    LOAD_GLIBC_SYMBOL(strtof);

    return glibc_strtof(a, b);
}

int printf(const char *format, ...) SOFTFP;
int dprintf(int fd, const char *format, ...) SOFTFP;
int sprintf(char *str, const char *format, ...) SOFTFP;
int snprintf(char *str, size_t size, const char *format, ...) SOFTFP;
int vprintf(const char *format, va_list ap) SOFTFP;
int vdprintf(int fd, const char *format, va_list ap) SOFTFP;
int vsprintf(char *str, const char *format, va_list ap) SOFTFP;
int vsnprintf(char *str, size_t size, const char *format, va_list ap) SOFTFP;

int printf(const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(vfprintf);

    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vprintf(format, args);
    va_end(args);

    return ret;
}

int dprintf(int fd, const char *fmt, ...)
{
    LOAD_GLIBC_SYMBOL(vdprintf);

    int ret = 0;

    va_list args;
    va_start(args, fmt);
    ret = glibc_vdprintf(fd, fmt, args);
    va_end(args);

    return ret;
}

int sprintf(char *str, const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(vsprintf);

    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vsprintf(str, format, args);
    va_end(args);

    return ret;
}

int snprintf(char *str, size_t size, const char *format, ...)
{
    LOAD_GLIBC_SYMBOL(vsnprintf);
    int ret = 0;

    va_list args;
    va_start(args, format);
    ret = glibc_vsnprintf(str, size, format, args);
    va_end(args);

    return ret;
}

int vprintf(const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vprintf);

    return glibc_vprintf(format, ap);
}

int vdprintf(int fd, const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vdprintf);

    return glibc_vdprintf(fd, format, ap);
}

int vsprintf(char *str, const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vsprintf);

    return glibc_vsprintf(str, format, ap);
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    LOAD_GLIBC_SYMBOL(vsnprintf);

    return glibc_vsnprintf(str, size, format, ap);
}

/*
 * bionic/libc/include/sys/sysconf.h processed with s/define _\(\w*\)\(.*\)/define \1\2\r#ifdef _\1\rMAP_TO_UNISTD(\1),\r#endif/g
 */
#define MAP_TO_UNISTD(a) [a]=_##a
static int sysconf_map[]= {
#define SC_ARG_MAX             0x0000
#ifdef _SC_ARG_MAX
MAP_TO_UNISTD(SC_ARG_MAX),
#endif
#define SC_BC_BASE_MAX         0x0001
#ifdef _SC_BC_BASE_MAX
MAP_TO_UNISTD(SC_BC_BASE_MAX),
#endif
#define SC_BC_DIM_MAX          0x0002
#ifdef _SC_BC_DIM_MAX
MAP_TO_UNISTD(SC_BC_DIM_MAX),
#endif
#define SC_BC_SCALE_MAX        0x0003
#ifdef _SC_BC_SCALE_MAX
MAP_TO_UNISTD(SC_BC_SCALE_MAX),
#endif
#define SC_BC_STRING_MAX       0x0004
#ifdef _SC_BC_STRING_MAX
MAP_TO_UNISTD(SC_BC_STRING_MAX),
#endif
#define SC_CHILD_MAX           0x0005
#ifdef _SC_CHILD_MAX
MAP_TO_UNISTD(SC_CHILD_MAX),
#endif
#define SC_CLK_TCK             0x0006
#ifdef _SC_CLK_TCK
MAP_TO_UNISTD(SC_CLK_TCK),
#endif
#define SC_COLL_WEIGHTS_MAX    0x0007
#ifdef _SC_COLL_WEIGHTS_MAX
MAP_TO_UNISTD(SC_COLL_WEIGHTS_MAX),
#endif
#define SC_EXPR_NEST_MAX       0x0008
#ifdef _SC_EXPR_NEST_MAX
MAP_TO_UNISTD(SC_EXPR_NEST_MAX),
#endif
#define SC_LINE_MAX            0x0009
#ifdef _SC_LINE_MAX
MAP_TO_UNISTD(SC_LINE_MAX),
#endif
#define SC_NGROUPS_MAX         0x000a
#ifdef _SC_NGROUPS_MAX
MAP_TO_UNISTD(SC_NGROUPS_MAX),
#endif
#define SC_OPEN_MAX            0x000b
#ifdef _SC_OPEN_MAX
MAP_TO_UNISTD(SC_OPEN_MAX),
#endif
#define SC_PASS_MAX            0x000c
#ifdef _SC_PASS_MAX
MAP_TO_UNISTD(SC_PASS_MAX),
#endif
#define SC_2_C_BIND            0x000d
#ifdef _SC_2_C_BIND
MAP_TO_UNISTD(SC_2_C_BIND),
#endif
#define SC_2_C_DEV             0x000e
#ifdef _SC_2_C_DEV
MAP_TO_UNISTD(SC_2_C_DEV),
#endif
#define SC_2_C_VERSION         0x000f
#ifdef _SC_2_C_VERSION
MAP_TO_UNISTD(SC_2_C_VERSION),
#endif
#define SC_2_CHAR_TERM         0x0010
#ifdef _SC_2_CHAR_TERM
MAP_TO_UNISTD(SC_2_CHAR_TERM),
#endif
#define SC_2_FORT_DEV          0x0011
#ifdef _SC_2_FORT_DEV
MAP_TO_UNISTD(SC_2_FORT_DEV),
#endif
#define SC_2_FORT_RUN          0x0012
#ifdef _SC_2_FORT_RUN
MAP_TO_UNISTD(SC_2_FORT_RUN),
#endif
#define SC_2_LOCALEDEF         0x0013
#ifdef _SC_2_LOCALEDEF
MAP_TO_UNISTD(SC_2_LOCALEDEF),
#endif
#define SC_2_SW_DEV            0x0014
#ifdef _SC_2_SW_DEV
MAP_TO_UNISTD(SC_2_SW_DEV),
#endif
#define SC_2_UPE               0x0015
#ifdef _SC_2_UPE
MAP_TO_UNISTD(SC_2_UPE),
#endif
#define SC_2_VERSION           0x0016
#ifdef _SC_2_VERSION
MAP_TO_UNISTD(SC_2_VERSION),
#endif
#define SC_JOB_CONTROL         0x0017
#ifdef _SC_JOB_CONTROL
MAP_TO_UNISTD(SC_JOB_CONTROL),
#endif
#define SC_SAVED_IDS           0x0018
#ifdef _SC_SAVED_IDS
MAP_TO_UNISTD(SC_SAVED_IDS),
#endif
#define SC_VERSION             0x0019
#ifdef _SC_VERSION
MAP_TO_UNISTD(SC_VERSION),
#endif
#define SC_RE_DUP_MAX          0x001a
#ifdef _SC_RE_DUP_MAX
MAP_TO_UNISTD(SC_RE_DUP_MAX),
#endif
#define SC_STREAM_MAX          0x001b
#ifdef _SC_STREAM_MAX
MAP_TO_UNISTD(SC_STREAM_MAX),
#endif
#define SC_TZNAME_MAX          0x001c
#ifdef _SC_TZNAME_MAX
MAP_TO_UNISTD(SC_TZNAME_MAX),
#endif
#define SC_XOPEN_CRYPT         0x001d
#ifdef _SC_XOPEN_CRYPT
MAP_TO_UNISTD(SC_XOPEN_CRYPT),
#endif
#define SC_XOPEN_ENH_I18N      0x001e
#ifdef _SC_XOPEN_ENH_I18N
MAP_TO_UNISTD(SC_XOPEN_ENH_I18N),
#endif
#define SC_XOPEN_SHM           0x001f
#ifdef _SC_XOPEN_SHM
MAP_TO_UNISTD(SC_XOPEN_SHM),
#endif
#define SC_XOPEN_VERSION       0x0020
#ifdef _SC_XOPEN_VERSION
MAP_TO_UNISTD(SC_XOPEN_VERSION),
#endif
#define SC_XOPEN_XCU_VERSION   0x0021
#ifdef _SC_XOPEN_XCU_VERSION
MAP_TO_UNISTD(SC_XOPEN_XCU_VERSION),
#endif
#define SC_XOPEN_REALTIME      0x0022
#ifdef _SC_XOPEN_REALTIME
MAP_TO_UNISTD(SC_XOPEN_REALTIME),
#endif
#define SC_XOPEN_REALTIME_THREADS  0x0023
#ifdef _SC_XOPEN_REALTIME_THREADS
MAP_TO_UNISTD(SC_XOPEN_REALTIME_THREADS),
#endif
#define SC_XOPEN_LEGACY        0x0024
#ifdef _SC_XOPEN_LEGACY
MAP_TO_UNISTD(SC_XOPEN_LEGACY),
#endif
#define SC_ATEXIT_MAX          0x0025
#ifdef _SC_ATEXIT_MAX
MAP_TO_UNISTD(SC_ATEXIT_MAX),
#endif
#define SC_IOV_MAX             0x0026
#ifdef _SC_IOV_MAX
MAP_TO_UNISTD(SC_IOV_MAX),
#endif
#define SC_PAGESIZE            0x0027
#ifdef _SC_PAGESIZE
MAP_TO_UNISTD(SC_PAGESIZE),
#endif
#define SC_PAGE_SIZE           0x0028
#ifdef _SC_PAGE_SIZE
MAP_TO_UNISTD(SC_PAGE_SIZE),
#endif
#define SC_XOPEN_UNIX          0x0029
#ifdef _SC_XOPEN_UNIX
MAP_TO_UNISTD(SC_XOPEN_UNIX),
#endif
#define SC_XBS5_ILP32_OFF32    0x002a
#ifdef _SC_XBS5_ILP32_OFF32
MAP_TO_UNISTD(SC_XBS5_ILP32_OFF32),
#endif
#define SC_XBS5_ILP32_OFFBIG   0x002b
#ifdef _SC_XBS5_ILP32_OFFBIG
MAP_TO_UNISTD(SC_XBS5_ILP32_OFFBIG),
#endif
#define SC_XBS5_LP64_OFF64     0x002c
#ifdef _SC_XBS5_LP64_OFF64
MAP_TO_UNISTD(SC_XBS5_LP64_OFF64),
#endif
#define SC_XBS5_LPBIG_OFFBIG   0x002d
#ifdef _SC_XBS5_LPBIG_OFFBIG
MAP_TO_UNISTD(SC_XBS5_LPBIG_OFFBIG),
#endif
#define SC_AIO_LISTIO_MAX      0x002e
#ifdef _SC_AIO_LISTIO_MAX
MAP_TO_UNISTD(SC_AIO_LISTIO_MAX),
#endif
#define SC_AIO_MAX             0x002f
#ifdef _SC_AIO_MAX
MAP_TO_UNISTD(SC_AIO_MAX),
#endif
#define SC_AIO_PRIO_DELTA_MAX  0x0030
#ifdef _SC_AIO_PRIO_DELTA_MAX
MAP_TO_UNISTD(SC_AIO_PRIO_DELTA_MAX),
#endif
#define SC_DELAYTIMER_MAX      0x0031
#ifdef _SC_DELAYTIMER_MAX
MAP_TO_UNISTD(SC_DELAYTIMER_MAX),
#endif
#define SC_MQ_OPEN_MAX         0x0032
#ifdef _SC_MQ_OPEN_MAX
MAP_TO_UNISTD(SC_MQ_OPEN_MAX),
#endif
#define SC_MQ_PRIO_MAX         0x0033
#ifdef _SC_MQ_PRIO_MAX
MAP_TO_UNISTD(SC_MQ_PRIO_MAX),
#endif
#define SC_RTSIG_MAX           0x0034
#ifdef _SC_RTSIG_MAX
MAP_TO_UNISTD(SC_RTSIG_MAX),
#endif
#define SC_SEM_NSEMS_MAX       0x0035
#ifdef _SC_SEM_NSEMS_MAX
MAP_TO_UNISTD(SC_SEM_NSEMS_MAX),
#endif
#define SC_SEM_VALUE_MAX       0x0036
#ifdef _SC_SEM_VALUE_MAX
MAP_TO_UNISTD(SC_SEM_VALUE_MAX),
#endif
#define SC_SIGQUEUE_MAX        0x0037
#ifdef _SC_SIGQUEUE_MAX
MAP_TO_UNISTD(SC_SIGQUEUE_MAX),
#endif
#define SC_TIMER_MAX           0x0038
#ifdef _SC_TIMER_MAX
MAP_TO_UNISTD(SC_TIMER_MAX),
#endif
#define SC_ASYNCHRONOUS_IO     0x0039
#ifdef _SC_ASYNCHRONOUS_IO
MAP_TO_UNISTD(SC_ASYNCHRONOUS_IO),
#endif
#define SC_FSYNC               0x003a
#ifdef _SC_FSYNC
MAP_TO_UNISTD(SC_FSYNC),
#endif
#define SC_MAPPED_FILES        0x003b
#ifdef _SC_MAPPED_FILES
MAP_TO_UNISTD(SC_MAPPED_FILES),
#endif
#define SC_MEMLOCK             0x003c
#ifdef _SC_MEMLOCK
MAP_TO_UNISTD(SC_MEMLOCK),
#endif
#define SC_MEMLOCK_RANGE       0x003d
#ifdef _SC_MEMLOCK_RANGE
MAP_TO_UNISTD(SC_MEMLOCK_RANGE),
#endif
#define SC_MEMORY_PROTECTION   0x003e
#ifdef _SC_MEMORY_PROTECTION
MAP_TO_UNISTD(SC_MEMORY_PROTECTION),
#endif
#define SC_MESSAGE_PASSING     0x003f
#ifdef _SC_MESSAGE_PASSING
MAP_TO_UNISTD(SC_MESSAGE_PASSING),
#endif
#define SC_PRIORITIZED_IO      0x0040
#ifdef _SC_PRIORITIZED_IO
MAP_TO_UNISTD(SC_PRIORITIZED_IO),
#endif
#define SC_PRIORITY_SCHEDULING 0x0041
#ifdef _SC_PRIORITY_SCHEDULING
MAP_TO_UNISTD(SC_PRIORITY_SCHEDULING),
#endif
#define SC_REALTIME_SIGNALS    0x0042
#ifdef _SC_REALTIME_SIGNALS
MAP_TO_UNISTD(SC_REALTIME_SIGNALS),
#endif
#define SC_SEMAPHORES          0x0043
#ifdef _SC_SEMAPHORES
MAP_TO_UNISTD(SC_SEMAPHORES),
#endif
#define SC_SHARED_MEMORY_OBJECTS  0x0044
#ifdef _SC_SHARED_MEMORY_OBJECTS
MAP_TO_UNISTD(SC_SHARED_MEMORY_OBJECTS),
#endif
#define SC_SYNCHRONIZED_IO     0x0045
#ifdef _SC_SYNCHRONIZED_IO
MAP_TO_UNISTD(SC_SYNCHRONIZED_IO),
#endif
#define SC_TIMERS              0x0046
#ifdef _SC_TIMERS
MAP_TO_UNISTD(SC_TIMERS),
#endif
#define SC_GETGR_R_SIZE_MAX    0x0047
#ifdef _SC_GETGR_R_SIZE_MAX
MAP_TO_UNISTD(SC_GETGR_R_SIZE_MAX),
#endif
#define SC_GETPW_R_SIZE_MAX    0x0048
#ifdef _SC_GETPW_R_SIZE_MAX
MAP_TO_UNISTD(SC_GETPW_R_SIZE_MAX),
#endif
#define SC_LOGIN_NAME_MAX      0x0049
#ifdef _SC_LOGIN_NAME_MAX
MAP_TO_UNISTD(SC_LOGIN_NAME_MAX),
#endif
#define SC_THREAD_DESTRUCTOR_ITERATIONS  0x004a
#ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
MAP_TO_UNISTD(SC_THREAD_DESTRUCTOR_ITERATIONS),
#endif
#define SC_THREAD_KEYS_MAX     0x004b
#ifdef _SC_THREAD_KEYS_MAX
MAP_TO_UNISTD(SC_THREAD_KEYS_MAX),
#endif
#define SC_THREAD_STACK_MIN    0x004c
#ifdef _SC_THREAD_STACK_MIN
MAP_TO_UNISTD(SC_THREAD_STACK_MIN),
#endif
#define SC_THREAD_THREADS_MAX  0x004d
#ifdef _SC_THREAD_THREADS_MAX
MAP_TO_UNISTD(SC_THREAD_THREADS_MAX),
#endif
#define SC_TTY_NAME_MAX        0x004e
#ifdef _SC_TTY_NAME_MAX
MAP_TO_UNISTD(SC_TTY_NAME_MAX),
#endif

#define SC_THREADS                     0x004f
#ifdef _SC_THREADS
MAP_TO_UNISTD(SC_THREADS),
#endif
#define SC_THREAD_ATTR_STACKADDR       0x0050
#ifdef _SC_THREAD_ATTR_STACKADDR
MAP_TO_UNISTD(SC_THREAD_ATTR_STACKADDR),
#endif
#define SC_THREAD_ATTR_STACKSIZE       0x0051
#ifdef _SC_THREAD_ATTR_STACKSIZE
MAP_TO_UNISTD(SC_THREAD_ATTR_STACKSIZE),
#endif
#define SC_THREAD_PRIORITY_SCHEDULING  0x0052
#ifdef _SC_THREAD_PRIORITY_SCHEDULING
MAP_TO_UNISTD(SC_THREAD_PRIORITY_SCHEDULING),
#endif
#define SC_THREAD_PRIO_INHERIT         0x0053
#ifdef _SC_THREAD_PRIO_INHERIT
MAP_TO_UNISTD(SC_THREAD_PRIO_INHERIT),
#endif
#define SC_THREAD_PRIO_PROTECT         0x0054
#ifdef _SC_THREAD_PRIO_PROTECT
MAP_TO_UNISTD(SC_THREAD_PRIO_PROTECT),
#endif
#define SC_THREAD_SAFE_FUNCTIONS       0x0055
#ifdef _SC_THREAD_SAFE_FUNCTIONS
MAP_TO_UNISTD(SC_THREAD_SAFE_FUNCTIONS),
#endif

#define SC_NPROCESSORS_CONF            0x0060
#ifdef _SC_NPROCESSORS_CONF
MAP_TO_UNISTD(SC_NPROCESSORS_CONF),
#endif
#define SC_NPROCESSORS_ONLN            0x0061
#ifdef _SC_NPROCESSORS_ONLN
MAP_TO_UNISTD(SC_NPROCESSORS_ONLN),
#endif
#define SC_PHYS_PAGES                  0x0062
#ifdef _SC_PHYS_PAGES
MAP_TO_UNISTD(SC_PHYS_PAGES),
#endif
#define SC_AVPHYS_PAGES                0x0063
#ifdef _SC_AVPHYS_PAGES
MAP_TO_UNISTD(SC_AVPHYS_PAGES),
#endif
};
#undef MAP_TO_UNISTD

long sysconf(int name) SOFTFP;

long sysconf(int name)
{
    LOAD_GLIBC_SYMBOL(sysconf);

	return glibc_sysconf(sysconf_map[name]);
}

double difftime(time_t, time_t) SOFTFP;

double difftime(time_t a, time_t b)
{
    LOAD_GLIBC_SYMBOL(difftime);

    return glibc_difftime(a, b);
}

extern size_t strlcpy(char* restrict dest, const char* restrict src, size_t size);

extern size_t strlcat(char* restrict dest, const char* restrict src, size_t size);

size_t __strlcat_chk(char *dest, char *src, size_t size, size_t c) SOFTFP;
size_t __strlcat_chk(char *dest, char *src, size_t size, size_t c)
{
    return strlcat(dest, src, size); // TODO
}

size_t __strlcpy_chk(char *dest, char *src, size_t size, size_t c) SOFTFP;
size_t __strlcpy_chk(char *dest, char *src, size_t size, size_t c)
{
    return strlcpy(dest, src, size); // TODO
}

int fclose(BIONIC_FILE *stream) SOFTFP;
int fclose(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(fclose);

    return glibc_fclose(_get_actual_fp(stream));
}

BIONIC_FILE *fopen(const char *path, const char *mode) SOFTFP;
BIONIC_FILE *fopen(const char *path, const char *mode)
{
    LOAD_GLIBC_SYMBOL(fopen);

    return glibc_fopen(path, mode);
}

ssize_t getline(char **lineptr, size_t *n, BIONIC_FILE *stream) SOFTFP;
ssize_t getline(char **lineptr, size_t *n, BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(getline);

    return glibc_getline(lineptr, n, _get_actual_fp(stream));
}

char last_property[4096] = {0};
char last_value[4096] = {0};

// TODO
int __system_property_get(const char *name, char *value) SOFTFP;
int __system_property_get(const char *name, char *value)
{
    if(strcmp(name, last_property) == 0)
    {
        strcpy(value, last_value);
        return strlen(value);
    }

    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(fopen);
    LOAD_GLIBC_SYMBOL(fclose);
    LOAD_GLIBC_SYMBOL(getline);
    LOAD_GLIBC_SYMBOL(feof);
    
    glibc_fprintf(*glibc_stderr, "__system_property_get(%s)\n", name);

    void *fp = glibc_fopen("/system/build.prop", "r");

    char *line = NULL;
    while(fp && !glibc_feof(fp))
    {
        size_t len = 4096;
        int read;
        while((read = glibc_getline(&line, &len, fp)) != -1)
        {
            line[read-1] = 0;
            char *pos = strstr(line, name);
            if(pos != NULL)
            {
                //glibc_fprintf(*glibc_stderr, "found: \"%s\"\n", line);
                strcpy(value, pos+strlen(name)+1);
                strcpy(last_value, value);
                strcpy(last_property, name);
                //glibc_fprintf(*glibc_stderr, "new value: \"%s\"\n", value);
                glibc_fclose(fp);

                free(line);
                return strlen(value);
            }
        }
        free(line);
    }

    strcpy(last_value, "");
    strcpy(last_property, name);
    //glibc_fprintf(*glibc_stderr, "%s not found\n", name);
    glibc_fclose(fp);
    return 0;
}

int __system_property_set(const char *name, char *value) SOFTFP;
int __system_property_set(const char *name, char *value)
{
    //fprintf(stderr, "__system_property_set(%s, %s)\n", name, value);
    return 0;
}

int __system_property_foreach(void (*propfn)(const void *pi, void *cookie), void *cookie) SOFTFP;
int __system_property_foreach(void (*propfn)(const void *pi, void *cookie), void *cookie)
{
     //fprintf(stderr, "__system_property_foreach\n");

     return 0;
}

unsigned int __system_property_serial(void *pi) SOFTFP;
unsigned int __system_property_serial(void *pi)
{
    //fprintf(stderr, "__system_property_serial(%p)\n", pi);

    return 0;
}

void *__system_property_find_compat(const char *name) SOFTFP; 
void *__system_property_find_compat(const char *name)
{
    //fprintf(stderr, "__system_property_find_compat(%s)\n", name);

    return NULL;
}

const void *__system_property_find(const char *name) SOFTFP;
const void *__system_property_find(const char *name)
{
    //fprintf(stderr, "__system_property_find(%s)\n", name);

    return NULL;
}

unsigned int __system_property_area_serial(void) SOFTFP;

unsigned int __system_property_area_serial(void)
{
    //fprintf(stderr, "__system_property_area_serial\n");

    return 0;
}

int __system_property_read(const void *pi, char *name, char *value)
{
    //fprintf(stderr, "__system_property_read(%p, %s, %s)\n", pi, name, value);

    return 0;
}

BIONIC_FILE *fdopen(int fd, const char *mode)
{
    LOAD_GLIBC_SYMBOL(fdopen);

    return glibc_fdopen(fd, mode);
}

BIONIC_FILE *freopen(const char *path, const char *mode, BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(freopen);

    return glibc_freopen(path, mode, _get_actual_fp(stream));
}

extern int my_readdir_r(void *dir, void *entry, void **result);
extern void *my_readdir(void *dir);

int readdir_r(void *dir, void *entry,
        void **result) SOFTFP;
void *readdir(void *dir) SOFTFP;

int readdir_r(void *dir, void *entry,
        void **result)
{
    LOAD_GLIBC_SYMBOL(readdir_r);

    return my_readdir_r(dir, entry, result);
}

void *readdir(void *dir)
{
    LOAD_GLIBC_SYMBOL(readdir_r);

    return my_readdir(dir);
}

void __assert2(const char* file, int line, const char* function, const char* failed_expression) __attribute__((noreturn));
void __assert2(const char* file, int line, const char* function, const char* failed_expression)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(stderr);

    glibc_fprintf(*glibc_stderr, "assertion failed: %s @ %s:%d(%s)\n", failed_expression, file, line, function);

    abort();
}

pid_t pthread_gettid_np(pthread_t t) SOFTFP;
pid_t pthread_gettid_np(pthread_t t)
{
    return my_gettid_from_thread(t);
}

pid_t __pthread_gettid(pthread_t t) SOFTFP;
pid_t __pthread_gettid(pthread_t t)
{
    return pthread_gettid_np(t);
}

int ferror(BIONIC_FILE *stream) SOFTFP;

int ferror(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(ferror);

    return glibc_ferror(_get_actual_fp(stream));
}

int *__get_h_errno(void) SOFTFP;
int *__get_h_errno(void)
{
    LOAD_GLIBC_SYMBOL(__h_errno_location);

    return glibc___h_errno_location();
}

int finite(double x) SOFTFP;
int finite(double x)
{
    LOAD_GLIBC_SYMBOL(finite);
    return glibc_finite(x);
}

int finitef(float x) SOFTFP;
int finitef(float x)
{
    LOAD_GLIBC_SYMBOL(finitef);
    return glibc_finitef(x);
}

int finitel(long double x) SOFTFP;
int finitel(long double x)
{
    LOAD_GLIBC_SYMBOL(finitel);
    return glibc_finitel(x);
}

int isinf(double x) SOFTFP;
int isinf(double x)
{
    LOAD_GLIBC_SYMBOL(isinf);
    return glibc_isinf(x);
}

int isinff(float x) SOFTFP;
int isinff(float x)
{
    LOAD_GLIBC_SYMBOL(isinff);
    return glibc_isinff(x);
}

int isinfl(long double x) SOFTFP;
int isinfl(long double x)
{
    LOAD_GLIBC_SYMBOL(isinfl);
    return glibc_isinfl(x);
}

int isnan(double x) SOFTFP;
int isnan(double x)
{
    LOAD_GLIBC_SYMBOL(isnan);
    return glibc_isnan(x);
}

int isnanf(float x) SOFTFP;
int isnanf(float x)
{
    LOAD_GLIBC_SYMBOL(isnanf);
    return glibc_isnanf(x);
}

int isnanl(long double x) SOFTFP;
int isnanl(long double x)
{
    LOAD_GLIBC_SYMBOL(isnanl);
    return glibc_isnanl(x);
}

#include <sys/select.h>
#include <sys/types.h>
// TODO?
void __FD_SET_chk(int a, fd_set *b, size_t c) SOFTFP;
void __FD_SET_chk(int a, fd_set *b, size_t c)
{
    FD_SET(a, b);
}

void __FD_CLR_chk(int a, fd_set *b) SOFTFP;
void __FD_CLR_chk(int a, fd_set *b)
{
    FD_CLR(a, b);
}

int __FD_ISSET_chk(int a, fd_set *b) SOFTFP;
int __FD_ISSET_chk(int a, fd_set *b)
{
    return FD_ISSET(a, b);
}


static __thread void *tls_hooks[16];

void *__get_tls_hooks(void)
{
    return tls_hooks;
}

extern void my_freeaddrinfo(void*);
extern int my_getaddrinfo(const char *, const char *, const void*, void**);

int getaddrinfo(const char *hostname, const char *servname,
    const void *hints, void **res) SOFTFP;
int getaddrinfo(const char *hostname, const char *servname,
    const void *hints, void **res)
{
    LOAD_GLIBC_SYMBOL(getaddrinfo);

    return my_getaddrinfo(hostname, servname, hints, res);
}

void freeaddrinfo(void *__ai) SOFTFP;
void freeaddrinfo(void *__ai)
{
    LOAD_GLIBC_SYMBOL(freeaddrinfo);
    my_freeaddrinfo(__ai);
}

int fseek(BIONIC_FILE *stream, long offset, int whence) SOFTFP;
int fseek(BIONIC_FILE *stream, long offset, int whence)
{
    LOAD_GLIBC_SYMBOL(fseek);

    return glibc_fseek(_get_actual_fp(stream), offset, whence);
}

int fseeko(BIONIC_FILE *stream, off_t offset, int whence) SOFTFP;
int fseeko(BIONIC_FILE *stream, off_t offset, int whence)
{
    LOAD_GLIBC_SYMBOL(fseeko);

    return glibc_fseeko(_get_actual_fp(stream), offset, whence);
}

long ftell(BIONIC_FILE *stream) SOFTFP;
long ftell(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(ftell);

    return glibc_ftell(_get_actual_fp(stream));
}

int fileno(BIONIC_FILE *stream) SOFTFP;
int fileno(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(fileno);

    return glibc_fileno(_get_actual_fp(stream));
}

void rewind(BIONIC_FILE *stream) SOFTFP;
void rewind(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(rewind);
    
    return glibc_rewind(stream);
}

off_t ftello(BIONIC_FILE *stream) SOFTFP;
off_t ftello(BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(ftello);

    return glibc_ftello(stream);
}

char* __strncpy_chk2(char *a, const char* b, size_t c, size_t d, size_t e) SOFTFP;
char* __strncpy_chk2(char *a, const char* b, size_t c, size_t d, size_t e)
{
    return __strncpy_chk(a, b, c, d);
}

// this is not so beautiful.......
void __cxa_finalize(void* dso_handle) SOFTFP;
void __cxa_finalize(void* dso_handle)
{
    LOAD_GLIBC_SYMBOL(__cxa_finalize);

    glibc___cxa_finalize(dso_handle);
}

const char* getprogname(void)
{
    return __progname;
}

void setprogname(const char *a)
{
    __progname = a;
}

#include <math.h>

#define ANDROID_FP_INFINITE     0x01                                                        
#define ANDROID_FP_NAN          0x02                                                        
#define ANDROID_FP_NORMAL       0x04                                                        
#define ANDROID_FP_SUBNORMAL    0x08                                                    
#define ANDROID_FP_ZERO         0x10

int __isfinite(double d) SOFTFP;
int __isnormal(double d) SOFTFP;
int __isnan(double d) SOFTFP;
int __isinf(double d) SOFTFP;

int __isfinite(double d)
{
    return isfinite(d);
}

int __isnormal(double d)
{
    return isnormal(d);
}

int __isnan(double f)
{
    return isnan(f);
}

int __isinf(double d)
{
    return isinf(d);
}

int __isfinitef(float d) SOFTFP;
int __isnormalf(float d) SOFTFP;
int __isnanf(float d) SOFTFP;
int __isinff(float d) SOFTFP;

int __isfinitef(float d)
{
    return isfinite(d);
}

int __isnormalf(float d)
{
    return isnormal(d);
}

int __isnanf(float f)
{
    return isnan(f);
}

int __isinff(float f)
{
    return isinf(f);
}

BIONIC_FILE *funopen(const void *cookie, int (*readfn)(void *, char *, int), int (*writefn)(void *, const char *, int), off_t (*seekfn)(void *, off_t, int), int (*closefn)(void *)) SOFTFP;
BIONIC_FILE *funopen(const void *cookie, int (*readfn)(void *, char *, int), int (*writefn)(void *, const char *, int), off_t (*seekfn)(void *, off_t, int), int (*closefn)(void *))
{
    LOAD_GLIBC_SYMBOL(funopen);

    return funopen(cookie, readfn, writefn, seekfn, closefn);
}

int fgetpos(BIONIC_FILE *stream, void *pos) SOFTFP;
int fsetpos(BIONIC_FILE *stream, const void *pos) SOFTFP;

int fgetpos(BIONIC_FILE *stream, void *pos)
{
    LOAD_GLIBC_SYMBOL(fgetpos);

    return fgetpos(_get_actual_fp(stream), pos);
}

int fsetpos(BIONIC_FILE *stream, const void *pos)
{
    LOAD_GLIBC_SYMBOL(fsetpos);

    return fsetpos(_get_actual_fp(stream), pos);
}

