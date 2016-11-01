#define _GNU_SOURCE
#include "helpers.h"
#include <link.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <linux/posix_types.h>
#include <sys/uio.h>

#define NULL ((void*) 0)

// be careful with printfs. or any other calls. those will propably call other
// functions in here.

typedef long unsigned int* _Unwind_Ptr;
typedef __WCHAR_TYPE__ wchar_t;

typedef struct {
  uint8_t __seq[4];
#ifdef __LP64__
  char __reserved[4];
#endif
} mbstate_t;

typedef long wctype_t;

static void *glibc_handle = NULL;
static void *rt_handle = NULL;
static int (*glibc_vfprintf)(void *fp, const char *fmt, va_list ap) = NULL;
static int (*glibc_fprintf)(void *stream, const char *format, ...) = NULL;
static _Unwind_Ptr (*glibc___gnu_Unwind_Find_exidx)(_Unwind_Ptr pc, int *pcount) = NULL;
static void (*glibc___cxa_finalize)(void *dso_handle) = NULL;
static int (*glibc___cxa_atexit)(void (*)(void *), void *, void *) = NULL;
static int (*glibc___register_atfork)(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso) = NULL;
static double (*glibc_ldexp)(double, int) = NULL;
static void *(*glibc___aeabi_memset)(void *s, int c, size_t n) = NULL;
int (*glibc___aeabi_atexit)(void *object, void (*destructor) (void *), void *dso_handle) = NULL;
static int (*glibc_isxdigit)(int c) = NULL;
static void (*glibc_abort)(void) = NULL;
static int (*glibc_fputs)(const char *s, void *stream) = NULL;
static int (*glibc_fputc)(int c, void *stream) = NULL;
static int (*glibc_vasprintf)(char **strp, const char *fmt, va_list ap) = NULL;
//static int (*glibc_asprintf)(char **strp, const char *fmt, ...) = NULL;
static void *(*glibc_malloc)(size_t size) = NULL;
static void (*glibc_free)(void *ptr) = NULL;
static void *(*glibc_realloc)(void *ptr, size_t size) = NULL;
static void *(*glibc_memcpy)(void *a, void *b, size_t s) = NULL;
static void (*glibc___aeabi_memcpy)(void *dest, const void *src, size_t n) = NULL;
static void (*glibc___stack_chk_fail)(void) = NULL;
static void *(*glibc___aeabi_memmove)(void *dest, const void *src, size_t n) = NULL;
static int (*glibc_isalnum)(int) = NULL;
static int (*glibc_isalpha)(int) = NULL;
static int (*glibc_iscntrl)(int) = NULL;
static int (*glibc_isdigit)(int) = NULL;
static int (*glibc_isgraph)(int) = NULL;
static int (*glibc_islower)(int) = NULL;
static int (*glibc_isprint)(int) = NULL;
static int (*glibc_ispunct)(int) = NULL;
static int (*glibc_isspace)(int) = NULL;
static int (*glibc_isupper)(int) = NULL;
static int (*glibc_isascii)(int) = NULL;
static int (*glibc_isblank)(int) = NULL;
static int (*glibc_isalnum_l)(int, void*) = NULL;
static int (*glibc_isalpha_l)(int, void*) = NULL;
static int (*glibc_isblank_l)(int, void*) = NULL;
static int (*glibc_iscntrl_l)(int, void*) = NULL;
static int (*glibc_isdigit_l)(int, void*) = NULL;
static int (*glibc_isgraph_l)(int, void*) = NULL;
static int (*glibc_isprint_l)(int, void*) = NULL;
static int (*glibc_ispunct_l)(int, void*) = NULL;
static int (*glibc_isspace_l)(int, void*) = NULL;
static int (*glibc_islower_l)(int c, void* locale) = NULL;
static int (*glibc_isupper_l)(int, void*) = NULL;
static int (*glibc_isxdigit_l)(int, void*) = NULL;
static int (*glibc_isascii_l)(int, void*) = NULL;
static int (*glibc___vsnprintf_chk)(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...) = NULL;
static void *(*glibc_calloc)(size_t nmemb, size_t size) = NULL;
static int (*glibc_pthread_setspecific)(unsigned int key, const void *value) = NULL;
static int (*rt_pthread_once)(int *once_control, void (*init_routine)(void)) = NULL;
static void *(*glibc_pthread_getspecific)(unsigned int key) = NULL;
static int (*glibc_pthread_key_create)(unsigned int *key, void (*destructor)(void*)) = NULL;
static int (*glibc_pthread_mutex_lock)(void *__mutex) = NULL;
static int (*glibc_pthread_mutex_unlock)(void *__mutex) = NULL;
static int (*glibc_pthread_cond_timedwait)(void *restrict cond, void *restrict mutex, void *restrict abstime) = NULL;
static int (*glibc_pthread_cond_wait)(void *restrict cond, void *restrict mutex) = NULL;
static int (*glibc_pthread_cond_broadcast)(void *cond) = NULL;
static int (*glibc_pthread_cond_signal)(void *cond) = NULL;
static int (*glibc___cxa_thread_atexit_impl)(void (*fn)(void*), void* arg, void* dso_handle) = NULL;
static int (*glibc_gettimeofday)(void *restrict tp, void *restrict tzp) = NULL;
static int (*glibc_clock_getres)(int clk_id, void *res) = NULL;
static int (*glibc_clock_gettime)(int clk_id, void *tp) = NULL;
static int (*glibc_clock_settime)(int clk_id, const void *tp) = NULL;
static int (*glibc_pthread_cond_destroy)(void *cond) = NULL;
static int (*glibc_pthread_cond_init)(void *restrict cond, const void *restrict attr) = NULL;
static int (*glibc_putc)(int c, void *stream) = NULL;
static int (*glibc_putchar)(int c) = NULL;
static int (*glibc_puts)(const char *s) = NULL;
static wchar_t *(*glibc_wmemcpy)(wchar_t *dest, const wchar_t *src, size_t n) = NULL;
static int (*glibc_fgetc)(void *stream) = NULL;
static char *(*glibc_fgets)(char *s, int size, void *stream) = NULL;
static int (*glibc_getc)(void *stream) = NULL;
static int (*glibc_getchar)(void) = NULL;
static int (*glibc_ungetc)(int c, void *stream) = NULL;
static size_t (*glibc_fwrite)(const void *ptr, size_t size, size_t nmemb, void *stream) = NULL;
static size_t (*glibc_fread)(void *ptr, size_t size, size_t nmemb, void *stream) = NULL;
static int (*glibc_fflush)(void *stream) = NULL;
static void *(*glibc_uselocale)(void *newloc) = NULL;
//static int (*glibc_scanf)(const char *format, ...) = NULL;
//static int (*glibc_fscanf)(void *stream, const char *format, ...) = NULL;
static int (*glibc_sscanf)(const char *str, const char *format, ...) = NULL;
static int (*glibc_vscanf)(const char *format, va_list ap) = NULL;
static int (*glibc_vsscanf)(const char *str, const char *format, va_list ap) = NULL;
static int (*glibc_vfscanf)(void *stream, const char *format, va_list ap) = NULL;
static void (*glibc_freelocale)(void *locobj) = NULL;
static void *(*glibc_newlocale)(int category_mask, const char *locale, void *base) = NULL;
static size_t (*glibc_strftime)(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr) = NULL;
static __WINT_TYPE__ (*glibc_btowc)(int) = NULL;
//static int (*glibc_fwprintf)(void *, const wchar_t *, ...) = NULL;
//static int (*glibc_fwscanf)(void *, const wchar_t *, ...) = NULL;
static int (*glibc_iswalnum)(__WINT_TYPE__) = NULL;
static int (*glibc_iswalpha)(__WINT_TYPE__) = NULL;
static int (*glibc_iswblank)(__WINT_TYPE__) = NULL;
static int (*glibc_iswcntrl)(__WINT_TYPE__) = NULL;
static int (*glibc_iswdigit)(__WINT_TYPE__) = NULL;
static int (*glibc_iswgraph)(__WINT_TYPE__) = NULL;
static int (*glibc_iswlower)(__WINT_TYPE__) = NULL;
static int (*glibc_iswprint)(__WINT_TYPE__) = NULL;
static int (*glibc_iswpunct)(__WINT_TYPE__) = NULL;
static int (*glibc_iswspace)(__WINT_TYPE__) = NULL;
static int (*glibc_iswupper)(__WINT_TYPE__) = NULL;
static int (*glibc_iswxdigit)(__WINT_TYPE__) = NULL;
static int (*glibc_iswctype)(__WINT_TYPE__, wctype_t) = NULL;
static __WINT_TYPE__ (*glibc_fgetwc)(void *) = NULL;
static wchar_t *(*glibc_fgetws)(wchar_t *, int, void *) = NULL;
static __WINT_TYPE__ (*glibc_fputwc)(wchar_t, void *) = NULL;
static int (*glibc_fputws)(const wchar_t *, void *) = NULL;
static int (*glibc_fwide)(void *, int) = NULL;
static __WINT_TYPE__ (*glibc_getwc)(void *) = NULL;
static __WINT_TYPE__ (*glibc_getwchar)(void) = NULL;
static int (*glibc_mbsinit)(const mbstate_t *) = NULL;
static size_t (*glibc_mbrlen)(const char *, size_t, mbstate_t *) = NULL;
static size_t (*glibc_mbrtowc)(wchar_t *, const char *, size_t, mbstate_t *) = NULL;
static size_t (*glibc_mbsrtowcs)(wchar_t*, const char**, size_t, mbstate_t*) = NULL;
static size_t (*glibc_mbsnrtowcs)(wchar_t*, const char**, size_t, size_t, mbstate_t*) = NULL;
static size_t (*glibc_mbstowcs)(wchar_t *, const char *, size_t) = NULL;
static __WINT_TYPE__ (*glibc_putwc)(wchar_t, void *) = NULL;
static __WINT_TYPE__ (*glibc_putwchar)(wchar_t) = NULL;
//static int (*glibc_swprintf)(wchar_t *, size_t, const wchar_t *, ...) = NULL;
//static int (*glibc_swscanf)(const wchar_t *, const wchar_t *, ...) = NULL;
static __WINT_TYPE__ (*glibc_towlower)(__WINT_TYPE__) = NULL;
static __WINT_TYPE__ (*glibc_towupper)(__WINT_TYPE__) = NULL;
static __WINT_TYPE__ (*glibc_ungetwc)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_vfwprintf)(void*, const wchar_t*, va_list) = NULL;
static int (*glibc_vfwscanf)(void*, const wchar_t*, va_list) = NULL;
static int (*glibc_vswprintf)(wchar_t*, size_t, const wchar_t*, va_list) = NULL;
static int (*glibc_vswscanf)(const wchar_t*, const wchar_t*, va_list) = NULL;
static int (*glibc_vwprintf)(const wchar_t*, va_list) = NULL;
static int (*glibc_vwscanf)(const wchar_t*, va_list) = NULL;
static wchar_t* (*glibc_wcpcpy)(wchar_t*, const wchar_t *) = NULL;
static wchar_t* (*glibc_wcpncpy)(wchar_t*, const wchar_t *, size_t) = NULL;
static size_t (*glibc_wcrtomb)(char *, wchar_t, mbstate_t *) = NULL;
static int (*glibc_wcscasecmp)(const wchar_t *, const wchar_t *) = NULL;
static int (*glibc_wcscasecmp_l)(const wchar_t *, const wchar_t *, void*) = NULL;
static wchar_t *(*glibc_wcscat)(wchar_t *, const wchar_t *) = NULL;
static wchar_t *(*glibc_wcschr)(const wchar_t *, wchar_t) = NULL;
static int (*glibc_wcscmp)(const wchar_t *, const wchar_t *) = NULL;
static void *(*glibc_localeconv)(void) = NULL;
static char *(*glibc_setlocale)(int category, const char *locale) = NULL;
static int (*glibc_strcoll_l)(const char *, const char *, void*) = NULL;
static int (*glibc_strxfrm_l)(char* restrict, const char* restrict, size_t, void *) = NULL;
static int (*glibc_wcscoll_l)(const wchar_t *a, const wchar_t *b, void*c) = NULL;
static long long (*glibc_wcstoll_l)(const wchar_t *a, wchar_t **b, int c, void*d) = NULL;
static unsigned long long (*glibc_wcstoull_l)(const wchar_t *a, wchar_t **b, int c, void*d) = NULL;
static long double (*glibc_wcstold_l)(const wchar_t *a, wchar_t **b, void* c) = NULL;
static size_t (*glibc_wcsxfrm_l)(wchar_t *a, const wchar_t *b, size_t c, void*d) = NULL;
static int (*glibc_toupper)(int) = NULL;
static int (*glibc_tolower)(int) = NULL;
static int (*glibc_toupper_l)(int, void*) = NULL;
static int (*glibc_tolower_l)(int, void*) = NULL;
static int (*glibc_iswalnum_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswalpha_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswblank_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswcntrl_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswdigit_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswgraph_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswlower_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswprint_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswpunct_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswspace_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswupper_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_iswxdigit_l)(__WINT_TYPE__, void *) = NULL;
static int (*glibc_towlower_l)(int, void *) = NULL;
static int (*glibc_towupper_l)(int, void *) = NULL;
static int (*glibc_iswctype_l)(__WINT_TYPE__, wctype_t, void *) = NULL;
static wchar_t *(*glibc_wcscpy)(wchar_t *, const wchar_t *) = NULL;
static size_t (*glibc_wcscspn)(const wchar_t *, const wchar_t *) = NULL;
static size_t (*glibc_wcsftime)(wchar_t *, size_t, const wchar_t *, const void *) = NULL;
static size_t (*glibc_wcslen)(const wchar_t *) = NULL;
static int (*glibc_wcsncasecmp)(const wchar_t *, const wchar_t *, size_t) = NULL;
static int (*glibc_wcsncasecmp_l)(const wchar_t *, const wchar_t *, size_t, void*) = NULL;
static wchar_t *(*glibc_wcsncat)(wchar_t *, const wchar_t *, size_t) = NULL;
static int (*glibc_wcsncmp)(const wchar_t *, const wchar_t *, size_t) = NULL; 
static wchar_t *(*glibc_wcsncpy)(wchar_t *, const wchar_t *, size_t) = NULL;
static size_t (*glibc_wcsnrtombs)(char*, const wchar_t**, size_t, size_t, mbstate_t*) = NULL; 
static wchar_t *(*glibc_wcspbrk)(const wchar_t *, const wchar_t *) = NULL;
static wchar_t *(*glibc_wcsrchr)(const wchar_t *, wchar_t) = NULL;
static size_t (*glibc_wcsrtombs)(char*, const wchar_t**, size_t, mbstate_t*) = NULL;
static size_t (*glibc_wcsspn)(const wchar_t *, const wchar_t *) = NULL;
static wchar_t *(*glibc_wcsstr)(const wchar_t *, const wchar_t *) = NULL;
static double (*glibc_wcstod)(const wchar_t*, wchar_t**) = NULL;
static float (*glibc_wcstof)(const wchar_t*, wchar_t**) = NULL;
static wchar_t* (*glibc_wcstok)(wchar_t*, const wchar_t*, wchar_t**) = NULL;
static long (*glibc_wcstol)(const wchar_t*, wchar_t**, int) = NULL;
static long long (*glibc_wcstoll)(const wchar_t*, wchar_t**, int) = NULL;
static long double (*glibc_wcstold)(const wchar_t*, wchar_t**) = NULL;
static unsigned long (*glibc_wcstoul)(const wchar_t*, wchar_t**, int) = NULL;
static unsigned long long (*glibc_wcstoull)(const wchar_t*, wchar_t**, int) = NULL;
static int (*glibc_wcswidth)(const wchar_t *, size_t) = NULL;
static size_t (*glibc_wcsxfrm)(wchar_t *, const wchar_t *, size_t) = NULL;
static int (*glibc_wctob)(__WINT_TYPE__) = NULL;
static wctype_t (*glibc_wctype)(const char *) = NULL;
static int (*glibc_wcwidth)(wchar_t) = NULL;
static wchar_t *(*glibc_wmemchr)(const wchar_t *, wchar_t, size_t) = NULL;
static int (*glibc_wmemcmp)(const wchar_t *, const wchar_t *, size_t) = NULL; 
static wchar_t *(*glibc_wmempcpy)(wchar_t *, const wchar_t *, size_t) = NULL;
static wchar_t *(*glibc_wmemmove)(wchar_t *, const wchar_t *, size_t) = NULL;
static wchar_t *(*glibc_wmemset)(wchar_t *, wchar_t, size_t) = NULL;
//static int (*glibc_wprintf)(const wchar_t *, ...) = NULL;
//static int (*glibc_wscanf)(const wchar_t *, ...) = NULL;

static size_t (*glibc_wcslcat)(wchar_t*, const wchar_t*, size_t) = NULL;
static size_t (*glibc_wcslcpy)(wchar_t*, const wchar_t*, size_t) = NULL;

typedef void *wctrans_t;
static __WINT_TYPE__ (*glibc_towctrans)(__WINT_TYPE__, wctrans_t) = NULL;
static wctrans_t (*glibc_wctrans)(const char*) = NULL;

static int (*glibc_mblen)(const char *, size_t) = NULL;
static int (*glibc_mbtowc)(wchar_t *, const char *, size_t) = NULL;

static size_t (*glibc___ctype_get_mb_cur_max)(void) = NULL;

static int (*glibc_strcmp)(const char *s1, const char *s2) = NULL;

static void (*glibc_exit)(int) = NULL;
static void (*glibc__Exit)(int) = NULL;

static int (*glibc___cxa_at_quick_exit)(void (*)(void)) = NULL;

static char* (*glibc_getenv)(const char*) = NULL;
static int (*glibc_putenv)(char*) = NULL;
static int (*glibc_setenv)(const char*, const char*, int) = NULL;
static int (*glibc_unsetenv)(const char*) = NULL;
static int (*glibc_clearenv)(void) = NULL;

static char* (*glibc_mkdtemp)(char*) = NULL;
static char* (*glibc_mktemp)(char*) = NULL;

static int (*glibc_mkostemp64)(char*, int) = NULL;
static int (*glibc_mkostemp)(char*, int) = NULL;
static int (*glibc_mkostemps64)(char*, int, int) = NULL;
static int (*glibc_mkostemps)(char*, int, int) = NULL;
static int (*glibc_mkstemp64)(char*) = NULL;
static int (*glibc_mkstemp)(char*) = NULL;
static int (*glibc_mkstemps64)(char*, int) = NULL;
static int (*glibc_mkstemps)(char*, int) = NULL;

static long (*glibc_strtol)(const char *, char **, int) = NULL;
static long long (*glibc_strtoll)(const char *, char **, int) = NULL;
static unsigned long (*glibc_strtoul)(const char *, char **, int) = NULL;
static unsigned long long (*glibc_strtoull)(const char *, char **, int) = NULL;

static int (*glibc_posix_memalign)(void **memptr, size_t alignment, size_t size) = NULL;
static double (*glibc_atof)(const char*a) = NULL;

static double (*glibc_strtod)(const char*, char**) = NULL;
static float (*glibc_strtof)(const char*, char**) = NULL;
static long double (*glibc_strtold)(const char*, char**) = NULL;

static long double (*glibc_strtold_l)(const char *, char **, void*) = NULL;
static long long (*glibc_strtoll_l)(const char *, char **, int, void*) = NULL;
static unsigned long long (*glibc_strtoull_l)(const char *, char **, int, void*) = NULL;

typedef struct {
        int  quot;
        int  rem;
} div_t;

static div_t (*glibc_div)(int, int) = NULL;

typedef struct {
        long int  quot;
        long int  rem;
} ldiv_t;

static ldiv_t (*glibc_ldiv)(long, long) = NULL;

typedef struct {
        long long int  quot;
        long long int  rem;
} lldiv_t;


static lldiv_t (*glibc_lldiv)(long long, long long) = NULL;
static const char* (*glibc_getprogname)(void) = NULL;
static void (*glibc_setprogname)(const char*) = NULL;
static char* (*glibc_realpath)(const char* path, char* resolved) = NULL;
static int (*glibc_atexit)(void (*a)(void)) = NULL;
static int (*glibc_atoi)(const char*a) = NULL;
static long (*glibc_atol)(const char*a) = NULL;
static long long (*glibc_atoll)(const char*a) = NULL;
static int (*glibc_abs)(int a) = NULL;
static long (*glibc_labs)(long a) = NULL;
static long long (*glibc_llabs)(long long a) = NULL;
static int (*glibc_system)(const char *string) = NULL;
static void * (*glibc_bsearch)(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d)) = NULL;
static void (*glibc_qsort)(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *)) = NULL;
static uint32_t (*glibc_arc4random)(void) = NULL;
static uint32_t (*glibc_arc4random_uniform)(uint32_t a) = NULL;
static void (*glibc_arc4random_buf)(void* a, size_t b) = NULL;
static int (*glibc_rand)(void) = NULL;
static int (*glibc_rand_r)(unsigned int*a) = NULL;
static void (*glibc_srand)(unsigned int a) = NULL;
static double (*glibc_drand48)(void) = NULL;
static double (*glibc_erand48)(unsigned short a[3]) = NULL;
static long (*glibc_jrand48)(unsigned short a[3]) = NULL;
static void (*glibc_lcong48)(unsigned short a[7]) = NULL;
static long (*glibc_lrand48)(void) = NULL;
static long (*glibc_mrand48)(void) = NULL;
static long (*glibc_nrand48)(unsigned short a[3]) = NULL;
static unsigned short* (*glibc_seed48)(unsigned short a[3]) = NULL;
static void (*glibc_srand48)(long a) = NULL;
static char* (*glibc_initstate)(unsigned int a, char*b, size_t c) = NULL;
static long (*glibc_random)(void) = NULL;
static char* (*glibc_setstate)(char*a) = NULL;
static void (*glibc_srandom)(unsigned int a) = NULL;
static int (*glibc_getpt)(void) = NULL;
static int (*glibc_grantpt)(int a) = NULL;
static int (*glibc_posix_openpt)(int a) = NULL;
static char* (*glibc_ptsname)(int a) = NULL;
static int (*glibc_ptsname_r)(int a, char*b, size_t c) = NULL;
static int (*glibc_unlockpt)(int a) = NULL;
static int (*glibc_sched_yield)(void) = NULL;
static int (*glibc_pthread_mutex_destroy)(void *mutex) = NULL;
static int (*glibc_pthread_mutex_init)(void *restrict mutex, const void *restrict attr) = NULL;
static int (*glibc_pthread_mutex_trylock)(void *mutex) = NULL;
static int (*glibc_pthread_key_delete)(unsigned int key) = NULL;
static int (*glibc_pthread_join)(void *thread, void **retval) = NULL;
static int (*glibc_pthread_detach)(void *thread) = NULL;
static int (*rt_pthread_mutexattr_init)(void *attr) = NULL;
static int (*rt_pthread_mutexattr_destroy)(void *attr) = NULL;
static void *(*glibc_pthread_self)(void) = NULL;
static int (*rt_pthread_mutexattr_settype)(void *attr, int type) = NULL;
static int (*glibc_pthread_equal)(void *t1, void * t2) = NULL;
static int (*glibc_open)(const char *path, int oflag, ...) = NULL;
static int (*glibc_close)(int a) = NULL;
static ssize_t (*glibc___read_chk)(int fd, void * buf, size_t nbytes, size_t buflen) = NULL;
static void* (*glibc___memchr_chk)(const void* s, int c, size_t n, size_t buf_size) = NULL;
static void* (*glibc___memrchr_chk)(const void*, int, size_t, size_t) = NULL;
static int (*glibc_printf)(const char *format, ...) = NULL;
//static int (*glibc_dprintf)(int fd, const char *format, ...) = NULL;
//static int (*glibc_sprintf)(char *str, const char *format, ...) = NULL;
//static int (*glibc_snprintf)(char *str, size_t size, const char *format, ...) = NULL;
static int (*glibc_vprintf)(const char *format, va_list ap) = NULL;
static int (*glibc_vdprintf)(int fd, const char *format, va_list ap) = NULL;
static int (*glibc_vsprintf)(char *str, const char *format, va_list ap) = NULL;
static int (*glibc_vsnprintf)(char *str, size_t size, const char *format, va_list ap) = NULL;
static char *(*glibc_strerror)(int errnum) = NULL;
static char *(*glibc_strerror_r)(int errnum, char *buf, size_t buflen) = NULL;
static long (*glibc_sysconf)(int name) = NULL;
static int (*glibc_nanosleep)(const void *rqtp, void *rmtp) = NULL;
static int (*glibc_raise)(int sig) = NULL;

static time_t (*glibc_time)(time_t*) = NULL;
static char* (*glibc_asctime)(const void*) = NULL;
static char* (*glibc_asctime_r)(const void*, char*) = NULL;
static double (*glibc_difftime)(time_t, time_t) = NULL;
static time_t (*glibc_mktime)(void*) = NULL;
static void* (*glibc_localtime)(const time_t*) = NULL;
static void* (*glibc_localtime_r)(const time_t*, void*) = NULL;
static void* (*glibc_gmtime)(const time_t*) = NULL;
static void* (*glibc_gmtime_r)(const time_t*, void*) = NULL;
static char* (*glibc_strptime)(const char*, const char*, void*) = NULL;
static size_t (*glibc_strftime_l)(char *, size_t, const char *, const void *, void*) = NULL;
static char* (*glibc_ctime)(const time_t*) = NULL;
static char* (*glibc_ctime_r)(const time_t*, char*) = NULL;
static void (*glibc_tzset)(void) = NULL;
static __kernel_long_t (*glibc_clock)(void) = NULL;
static int (*glibc_clock_getcpuclockid)(pid_t, clockid_t*) = NULL;
static int (*glibc_clock_nanosleep)(clockid_t, int, const void*, void*) = NULL;
static int (*rt_timer_create)(int, void*, timer_t*) = NULL;
static int (*rt_timer_delete)(timer_t) = NULL;
static int (*rt_timer_settime)(timer_t, int, const void*, void*) = NULL;
static int (*rt_timer_gettime)(timer_t, void*) = NULL;
static int (*rt_timer_getoverrun)(timer_t) = NULL;
static time_t (*glibc_timelocal)(void*) = NULL;
static time_t (*glibc_timegm)(void*) = NULL;
static char* (*glibc_stpcpy)(char* restrict, const char* restrict) = NULL;
static char* (*glibc_strcpy)(char* restrict, const char* restrict) = NULL;
static int (*glibc_socket)(int domain, int type, int protocol) = NULL;
static int (*glibc_connect)(int socket, const void *address, int address_len) = NULL;
static uid_t (*glibc_getuid)(void) = NULL;
static ssize_t (*glibc_writev)(int fildes, const struct iovec *iov, int iovcnt) = NULL;
static ssize_t (*glibc_write)(int a, const void *b, size_t c) = NULL;
static void*  (*glibc_memccpy)(void* restrict, const void* restrict, int, size_t) = NULL;
static void*  (*glibc_memchr)(const void *, int, size_t) = NULL;
static void*  (*glibc_memrchr)(const void *, int, size_t) = NULL;
static int    (*glibc_memcmp)(const void *, const void *, size_t) = NULL;
static void*  (*glibc_memmem)(const void *, size_t, const void *, size_t) = NULL;
static char*  (*glibc_strchr)(const char *, int) = NULL;
static char* (*glibc___strchr_chk)(const char *, int, size_t) = NULL;
static char*  (*glibc_strrchr)(const char *, int) = NULL;
static char* (*glibc___strrchr_chk)(const char *, int, size_t) = NULL;
static size_t (*glibc___strlen_chk)(const char *, size_t) = NULL;
static int (*glibc_strcasecmp)(const char*, const char*) = NULL;
static int (*glibc_strcasecmp_l)(const char*, const char*, void*) = NULL;
static int (*glibc_strncasecmp)(const char*, const char*, size_t) = NULL;
static int (*glibc_strncasecmp_l)(const char*, const char*, size_t, void*) = NULL;
static char*  (*glibc_strdup)(const char *) = NULL;
static char*  (*glibc_strstr)(const char *, const char *) = NULL;
static char*  (*glibc_strcasestr)(const char *haystack, const char *needle) = NULL;
static char*  (*glibc_strtok)(char* restrict, const char* restrict) = NULL;
static char*  (*glibc_strtok_r)(char* restrict, const char* restrict, char** restrict) = NULL;
static char* (*glibc_strerror_l)(int, void*) = NULL;
static size_t (*glibc_strnlen)(const char *, size_t) = NULL;
static char*  (*glibc_strncat)(char* restrict a, const char* restrict b, size_t c) = NULL;
static char*  (*glibc_strndup)(const char *, size_t) = NULL;
static int    (*glibc_strncmp)(const char *, const char *, size_t) = NULL;
static char*  (*glibc_stpncpy)(char* restrict a, const char* restrict b, size_t c) = NULL;
static char*  (*glibc_strncpy)(char* restrict a, const char* restrict b, size_t c) = NULL;
static size_t (*glibc_strlcat)(char* restrict a, const char* restrict b, size_t c) = NULL;
static size_t (*glibc_strlcpy)(char* restrict a, const char* restrict b, size_t c) = NULL;
static size_t (*glibc_strcspn)(const char *, const char *) = NULL;
static char*  (*glibc_strpbrk)(const char *, const char *) = NULL;
static char*  (*glibc_strsep)(char** restrict, const char* restrict) = NULL;
static size_t (*glibc_strspn)(const char *, const char *) = NULL;
static char*  (*glibc_strsignal)(int  sig) = NULL;
static int    (*glibc_strcoll)(const char *, const char *) = NULL;
static size_t (*glibc_strxfrm)(char* restrict a, const char* restrict b, size_t c) = NULL;
static char* (*glibc___gnu_basename)(char*) = NULL;
static char* (*glibc___stpncpy_chk2)(char* restrict, const char* restrict, size_t, size_t, size_t) = NULL;
static char* (*glibc___strncpy_chk2)(char* restrict, const char* restrict, size_t, size_t, size_t) = NULL;
static size_t (*glibc___strlcpy_chk)(char *, const char *, size_t, size_t) = NULL;
static size_t (*glibc___strlcat_chk)(char* restrict, const char* restrict, size_t, size_t) = NULL;
static void* (*glibc_memmove)(void *dest, const void *src, size_t len) = NULL;
static char* (*glibc_strcat)(char* restrict dest, const char* restrict src) = NULL;
static void* (*glibc_memset)(void *s, int c, size_t n) = NULL;
static size_t (*glibc_strlen)(const char *s) = NULL;
static void *(*glibc___memcpy_chk)(void * dest, const void * src, size_t len, size_t destlen) = NULL;
static void *(*glibc_mmap)(void *addr, size_t len, int prot, int flags, int fildes, off_t off) = NULL;
static int (*glibc_munmap)(void *addr, size_t len) = NULL;
static pid_t  (*glibc_fork)(void) = NULL;
static pid_t  (*glibc_vfork)(void) = NULL;
static pid_t  (*glibc_getpid)(void) = NULL;
static pid_t  (*glibc_gettid)(void) = NULL;
static pid_t  (*glibc_getpgid)(pid_t a) = NULL;
static int    (*glibc_setpgid)(pid_t a, pid_t b) = NULL;
static pid_t  (*glibc_getppid)(void) = NULL;
static pid_t  (*glibc_getpgrp)(void) = NULL;
static int    (*glibc_setpgrp)(void) = NULL;
static pid_t  (*glibc_getsid)(pid_t a) = NULL;
static pid_t  (*glibc_setsid)(void) = NULL;
static int (*glibc_execv)(const char * a, char * const * b) = NULL;
static int (*glibc_execvp)(const char * a, char * const * b) = NULL;
static int (*glibc_execvpe)(const char * a, char * const * b, char * const * c) = NULL;
static int (*glibc_execve)(const char * a, char * const * b, char * const * c) = NULL;
static int (*glibc_execl)(const char * a, const char * b, ...) = NULL;
static int (*glibc_execlp)(const char * a, const char * b, ...) = NULL;
static int (*glibc_execle)(const char * a, const char * b, ...) = NULL;
static int (*glibc_nice)(int a) = NULL;
static int (*glibc_setuid)(uid_t a) = NULL;
static int (*glibc_seteuid)(uid_t a) = NULL;
static uid_t (*glibc_geteuid)(void) = NULL;
static int (*glibc_setgid)(gid_t a) = NULL;
static gid_t (*glibc_getgid)(void) = NULL;
static int (*glibc_setegid)(gid_t a) = NULL;
static gid_t (*glibc_getegid)(void) = NULL;
static int (*glibc_getgroups)(int a, gid_t * b) = NULL;
static int (*glibc_setgroups)(size_t a, const gid_t * b) = NULL;
static int (*glibc_setreuid)(uid_t a, uid_t b) = NULL;
static int (*glibc_setregid)(gid_t a, gid_t b) = NULL;
static int (*glibc_setresuid)(uid_t a, uid_t b, uid_t c) = NULL;
static int (*glibc_setresgid)(gid_t a, gid_t b, gid_t c) = NULL;
static int (*glibc_getresuid)(uid_t *ruid, uid_t *euid, uid_t *suid) = NULL;
static int (*glibc_getresgid)(gid_t *rgid, gid_t *egid, gid_t *sgid) = NULL;
static char* (*glibc_getlogin)(void) = NULL;
static long (*glibc_fpathconf)(int a, int b) = NULL;
static long (*glibc_pathconf)(const char* a, int b) = NULL;
static int (*glibc_access)(const char* a, int b) = NULL;
static int (*glibc_faccessat)(int a, const char* b, int c, int d) = NULL;
static int (*glibc_link)(const char* a, const char* b) = NULL;
static int (*glibc_linkat)(int a, const char* b, int c, const char* d, int e) = NULL;
static int (*glibc_unlink)(const char* a) = NULL;
static int (*glibc_unlinkat)(int a, const char* b, int c) = NULL;
static int (*glibc_chdir)(const char * a) = NULL;
static int (*glibc_fchdir)(int a) = NULL;
static int (*glibc_rmdir)(const char * a) = NULL;
static int (*glibc_pipe)(int * a) = NULL;
static int (*glibc_pipe2)(int * a, int b) = NULL;
static int (*glibc_chroot)(const char * a) = NULL;
static off_t (*glibc_lseek)(int fildes, off_t offset, int whence) = NULL;
static char* (*glibc___strncpy_chk)(char* __restrict dest, const char* __restrict src, size_t len, size_t dest_len);
static ssize_t (*glibc_pread)(int fildes, void *buf, size_t nbyte, off_t offset) = NULL;
static ssize_t (*glibc_read)(int fildes, void *buf, size_t nbyte) = NULL;
static int (*glibc_poll)(void *fds, unsigned long int nfds, int timeout) = NULL;
static void* (*glibc___memmove_chk)(void* dest, const void* src, size_t len, size_t dest_len) = NULL;
static unsigned int (*glibc_alarm)(unsigned int a) = NULL;
static int (*glibc_recvfrom)(int, void*, size_t, unsigned int, void*, void*) = NULL;
static int (*glibc_prctl)(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) = NULL;
static void *(*glibc_freopen)(const char *path, const char *mode, void *stream) = NULL;
static void *(*glibc_fdopen)(int fd, const char *mode) = NULL;
static void *(*glibc_fopen)(const char *path, const char *mode) = NULL;
static int (*glibc_fclose)(void *stream) = NULL;
static int (*glibc_sched_setscheduler)(pid_t pid, int policy, const void *param) = NULL;
static int (*glibc_sched_getscheduler)(pid_t pid) = NULL;
static int (*glibc___cxa_guard_acquire)(void *g) = NULL;
static int (*glibc___vsprintf_chk)(char * str, int flag, size_t strlen, const char * format, va_list ap) = NULL;
static int (*glibc_getpagesize)(void) = NULL;
static int (*rt_pthread_create)(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
static ssize_t (*glibc_getline)(char **lineptr, size_t *n, void *stream) = NULL;
static int (*glibc_stat)(const char *restrict path, void *restrict buf) = NULL;
static long (*glibc_syscall)(long number, ...) = NULL;
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
static int (*glibc_bind)(int socket, const void *address, unsigned int address_len);

BIONIC_FILE __sF[3];

BIONIC_FILE *stdin = &__sF[0];
BIONIC_FILE *stdout = &__sF[1];
BIONIC_FILE *stderr = &__sF[2];

void **glibc_stdin = NULL;
void **glibc_stdout = NULL;
void **glibc_stderr = NULL;

#define LOAD_GLIBC() \
    if(glibc_handle == NULL) \
    { \
        glibc_handle = dlmopen(1, "/lib/libc.so.6", RTLD_LAZY); \
    }

#define LOAD_GLIBC_SYMBOL(f) \
    if(glibc_ ## f == NULL) \
    { \
        LOAD_GLIBC(); \
        glibc_ ## f = dlsym(glibc_handle, #f); \
    }
#if 0
\
    if(glibc_fprintf == NULL) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(glibc_stderr == NULL) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p\n", #f, glibc_ ##f)
#endif

#define LOAD_RT() \
    if(rt_handle == NULL) \
    { \
        rt_handle = dlmopen(1, "/lib/librt.so.1", RTLD_LAZY); \
    }

#define LOAD_RT_SYMBOL(f) \
    if(rt_ ## f == NULL) \
    { \
        LOAD_RT(); \
        rt_ ## f = dlsym(rt_handle, #f); \
    }
#if 0
    \
    if(glibc_fprintf == NULL) glibc_fprintf = dlsym(glibc_handle, "fprintf"); \
    if(glibc_stderr == NULL) glibc_stderr = dlsym(glibc_handle, "stderr"); \
    glibc_fprintf(*glibc_stderr, "%s is at %p\n", #f, rt_ ##f)
#endif

void (*init_dlfunctions)(void *op, void *sym, void *addr, void *err, void *clo, void *it);

// provide dlopen and friends for fake libdl.so
void __attribute__ ((constructor)) default_constructor()
{
    int id = 0;
    void *libdl = dlmopen(1, "libdl.so", RTLD_LAZY);
    init_dlfunctions = dlsym(libdl, "init_dlfunctions");
    init_dlfunctions(dlmopen, dlsym, dladdr, dlerror, dlclose, dl_iterate_phdr);

    LOAD_GLIBC_SYMBOL(syscall);
}

void *_get_actual_fp(BIONIC_FILE *fp)
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

volatile int *__errno(void)
{
    return &errno;
}

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

_Unwind_Ptr __gnu_Unwind_Find_exidx(_Unwind_Ptr pc, int *pcount) SOFTFP;

_Unwind_Ptr __gnu_Unwind_Find_exidx(_Unwind_Ptr pc, int *pcount)
{
    LOAD_GLIBC_SYMBOL(__gnu_Unwind_Find_exidx);

    return glibc___gnu_Unwind_Find_exidx(pc, pcount);
}

void __cxa_finalize(void* dso_handle) SOFTFP;

void __cxa_finalize(void* dso_handle)
{
    LOAD_GLIBC_SYMBOL(__cxa_finalize);

    glibc___cxa_finalize(dso_handle);
}

int __cxa_atexit(void (*p1)(void *), void *p2, void *p3) SOFTFP;

int __cxa_atexit(void (*p1)(void *), void *p2, void *p3)
{
    LOAD_GLIBC_SYMBOL(__cxa_atexit);

    return glibc___cxa_atexit(p1, p2, p3);
}

int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso) SOFTFP;

int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso)
{
    LOAD_GLIBC_SYMBOL(__register_atfork);

    return glibc___register_atfork(prepare, parent, child, dso);
}

double ldexp(double, int) SOFTFP;

double  ldexp(double a, int b)
{
    LOAD_GLIBC_SYMBOL(ldexp);

    return glibc_ldexp(a, b);
}

void abort(void) SOFTFP;

void abort(void)
{
    LOAD_GLIBC_SYMBOL(abort);
    glibc_abort();

    for(;;); // noreturn
}

void *memcpy(void *a, void *b, size_t s) SOFTFP;

void *memcpy(void *a, void *b, size_t s)
{
    LOAD_GLIBC_SYMBOL(memcpy);

    return glibc_memcpy(a, b, s);
}

int putc(int c, BIONIC_FILE *stream) SOFTFP;

int putc(int c, BIONIC_FILE *stream)
{
    LOAD_GLIBC_SYMBOL(putc);

    return glibc_putc(c, _get_actual_fp(stream));
}

int putchar(int c) SOFTFP;

int putchar(int c)
{
    LOAD_GLIBC_SYMBOL(putchar);

    return glibc_putchar(c);
}

int puts(const char *s) SOFTFP;

int puts(const char *s)
{
    LOAD_GLIBC_SYMBOL(puts);

    return glibc_puts(s);
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

void *malloc(size_t size) SOFTFP;

void *malloc(size_t size)
{
    LOAD_GLIBC_SYMBOL(malloc);

    return glibc_malloc(size);
}

size_t __strlen_chk(const char* s, size_t s_len) SOFTFP;

size_t __strlen_chk(const char* s, size_t s_len)
{
//    LOAD_GLIBC_SYMBOL(__strlen_chk);

//    return glibc___strlen_chk(s, s_len);
    LOAD_GLIBC_SYMBOL(strlen);
    return glibc_strlen(s);
}

void free(void *ptr) SOFTFP;

void free(void *ptr)
{
    LOAD_GLIBC_SYMBOL(free);

    return glibc_free(ptr);
}

void *realloc(void *ptr, size_t size) SOFTFP;

void *realloc(void *ptr, size_t size)
{
    LOAD_GLIBC_SYMBOL(realloc);

    return glibc_realloc(ptr, size);
}

void __aeabi_memcpy(void *dest, const void *src, size_t n) SOFTFP;

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
    LOAD_GLIBC_SYMBOL(__aeabi_memcpy);

    return glibc___aeabi_memcpy(dest, src, n);
}

void __stack_chk_fail(void) SOFTFP;

void __stack_chk_fail(void)
{
    LOAD_GLIBC_SYMBOL(__stack_chk_fail);

    return glibc___stack_chk_fail();
}

void *__aeabi_memmove(void *dest, const void *src, size_t n) SOFTFP;

void *__aeabi_memmove(void *dest, const void *src, size_t n)
{
    LOAD_GLIBC_SYMBOL(__aeabi_memmove);

    return glibc___aeabi_memmove(dest, src, n);
}

int isalnum(int c) SOFTFP;

int isalnum(int c)
{
    LOAD_GLIBC_SYMBOL(isalnum);

    return glibc_isalnum(c);
}

int isalpha(int c) SOFTFP;

int isalpha(int c)
{
    LOAD_GLIBC_SYMBOL(isalpha);

    return glibc_isalpha(c);
}

int iscntrl(int c) SOFTFP;

int iscntrl(int c)
{
    LOAD_GLIBC_SYMBOL(iscntrl);

    return glibc_iscntrl(c);
}

int isdigit(int c) SOFTFP;

int isdigit(int c)
{
    LOAD_GLIBC_SYMBOL(isdigit);

    return glibc_isdigit(c);
}

int isgraph(int c) SOFTFP;

int isgraph(int c)
{
    LOAD_GLIBC_SYMBOL(isgraph);

    return glibc_isgraph(c);
}

int islower(int c) SOFTFP;

int islower(int c)
{
    LOAD_GLIBC_SYMBOL(islower);

    return glibc_islower(c);
}

int isprint(int c) SOFTFP;

int isprint(int c)
{
    LOAD_GLIBC_SYMBOL(isprint);

    return glibc_isprint(c);
}

int ispunct(int c) SOFTFP;

int ispunct(int c)
{
    LOAD_GLIBC_SYMBOL(ispunct);

    return glibc_ispunct(c);
}

int isspace(int c) SOFTFP;

int isspace(int c)
{
    LOAD_GLIBC_SYMBOL(isspace);

    return glibc_isspace(c);
}

int isupper(int c) SOFTFP;

int isupper(int c)
{
    LOAD_GLIBC_SYMBOL(isupper);

    return glibc_isupper(c);
}

int isxdigit(int c) SOFTFP;

int isxdigit(int c)
{
    LOAD_GLIBC_SYMBOL(isxdigit);

    return glibc_isxdigit(c);
}


int isascii(int c) SOFTFP;

int isascii(int c)
{
    LOAD_GLIBC_SYMBOL(isascii);

    return glibc_isascii(c);
}

int isblank(int c) SOFTFP;

int isblank(int c)
{
    LOAD_GLIBC_SYMBOL(isblank);

    return glibc_isblank(c);
}

int isalnum_l(int c, void* locale) SOFTFP;

int isalnum_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isalnum_l);

    return glibc_isalnum_l(c, locale);
}

int isalpha_l(int c, void* locale) SOFTFP;

int isalpha_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isalpha_l);

    return glibc_isalpha_l(c, locale);
}

int isblank_l(int c, void* locale) SOFTFP;

int isblank_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isblank_l);

    return glibc_isblank_l(c, locale);
}

int iscntrl_l(int c, void* locale) SOFTFP;

int iscntrl_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(iscntrl_l);

    return glibc_iscntrl_l(c, locale);
}

int isdigit_l(int c, void* locale) SOFTFP;

int isdigit_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isdigit_l);

    return glibc_isdigit_l(c, locale);
}

int isgraph_l(int c, void* locale) SOFTFP;

int isgraph_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isgraph_l);

    return glibc_isgraph_l(c, locale);
}

int islower_l(int c, void* locale) SOFTFP;

int islower_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(islower_l);

    return glibc_islower_l(c, locale);
}

int isprint_l(int c, void* locale) SOFTFP;

int isprint_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isprint_l);

    return glibc_isprint_l(c, locale);
}

int ispunct_l(int c, void* locale) SOFTFP;

int ispunct_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(ispunct_l);

    return glibc_ispunct_l(c, locale);
}

int isspace_l(int c, void* locale) SOFTFP;

int isspace_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isspace_l);

    return glibc_isspace_l(c, locale);
}

int isupper_l(int c, void* locale) SOFTFP;

int isupper_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isupper_l);

    return glibc_isupper_l(c, locale);
}

int isxdigit_l(int c, void* locale) SOFTFP;

int isxdigit_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isxdigit_l);

    return glibc_isxdigit_l(c, locale);
}

int isascii_l(int c, void* locale) SOFTFP;

int isascii_l(int c, void* locale)
{
    LOAD_GLIBC_SYMBOL(isascii_l);

    return glibc_isascii_l(c, locale);
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

void *__aeabi_memset(void *s, int c, size_t n) SOFTFP;

void *__aeabi_memset(void *s, int c, size_t n)
{
    LOAD_GLIBC_SYMBOL(__aeabi_memset);

    return glibc___aeabi_memset(s, c, n);
}

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
#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
};


static void hybris_set_mutex_attr(unsigned int android_value, void *attr)
{
    LOAD_RT_SYMBOL(pthread_mutexattr_init);
    LOAD_RT_SYMBOL(pthread_mutexattr_settype);

    /* Init already sets as PTHREAD_MUTEX_NORMAL */
    rt_pthread_mutexattr_init(attr);

    if (android_value & ANDROID_PTHREAD_RECURSIVE_MUTEX_INITIALIZER) {
        rt_pthread_mutexattr_settype(attr, PTHREAD_MUTEX_RECURSIVE);
    } else if (android_value & ANDROID_PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
        rt_pthread_mutexattr_settype(attr, PTHREAD_MUTEX_ERRORCHECK);
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
    pthread_attr_t *realattr;

    realattr = malloc(sizeof(pthread_attr_t)); 
    *((unsigned int *)__attr) = (unsigned int) realattr;

    return glibc_pthread_attr_init(realattr);
}

int pthread_attr_destroy(pthread_attr_t *__attr)
{
    int ret;
    pthread_attr_t *realattr = (pthread_attr_t *) *(unsigned int *) __attr;

    ret = pthread_attr_destroy(realattr);
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
        *(unsigned int*)__mutex = hybris_alloc_init_mutex(value);
        value = *(unsigned int*)__mutex;
    }

    int ret = glibc_pthread_mutex_lock(value);
    return ret;
}

int pthread_mutex_unlock(void *__mutex) SOFTFP;

int pthread_mutex_unlock(void *__mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_unlock);

    int ret = glibc_pthread_mutex_unlock(*(unsigned int*)__mutex);
    return ret;
}

int pthread_setspecific(unsigned int key, const void *value) SOFTFP;

int pthread_setspecific(unsigned int key, const void *value)
{
    LOAD_GLIBC_SYMBOL(pthread_setspecific);

    return glibc_pthread_setspecific(key, value);
}

void *pthread_getspecific(unsigned int key) SOFTFP;

void *pthread_getspecific(unsigned int key)
{
    LOAD_GLIBC_SYMBOL(pthread_getspecific);

    return glibc_pthread_getspecific(key);
}

int pthread_once(int *once_control, void (*init_routine)(void)) SOFTFP;
int pthread_once(int *once_control, void (*init_routine)(void))
{
    LOAD_RT_SYMBOL(pthread_once);

    return rt_pthread_once(once_control, init_routine);
}

long __set_errno_internal(int e)
{
    errno = e;
    return -1;
}

int fstatat(int a, const char *restrict p, void *restrict b, int x)
{
#warning __NR_fstatat64 has been replaced with 327
    return glibc_syscall(327,a, p, b, x);
    /*
    __asm__ volatile("\
    mov     ip, r7\n\
    ldr     r7, =327\n\
    swi     #0\n\
    mov     r7, ip\n\
    cmn     r0, #4096\n\
    bxls    lr\n\
    neg     r0, r0\n\
    b       __set_errno_internal\n");
    */
}

int stat(const char *restrict path, void *restrict buf)
{
    return fstatat(/*AT_FDCWD*/-100, path, buf, 0);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) SOFTFP;
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
    LOAD_RT_SYMBOL(pthread_create);

    pthread_attr_t *realattr = NULL;

    if (attr != NULL)
            realattr = (pthread_attr_t *) *(unsigned int *) attr;

    int ret = rt_pthread_create(thread, realattr, start_routine, arg);
    return ret;
}

int pthread_key_create(unsigned int *key, void (*destructor)(void*)) SOFTFP;

int pthread_key_create(unsigned int *key, void (*destructor)(void*))
{
    LOAD_GLIBC_SYMBOL(pthread_key_create);

    return glibc_pthread_key_create(key, destructor);
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

/*
    if (hybris_check_android_shared_cond(cvalue) ||
        hybris_check_android_shared_mutex(mvalue)) {
        LOGD("Shared condition/mutex with Android, not waiting.");
        return 0;
    }
*/

    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;
/*
    if (hybris_is_pointer_in_shm((void*)cvalue))
        realcond = (pthread_cond_t *)hybris_get_shmpointer((hybris_shm_pointer_t)cvalue);
*/
    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;
/*
    if (hybris_is_pointer_in_shm((void*)mvalue))
        realmutex = (pthread_mutex_t *)hybris_get_shmpointer((hybris_shm_pointer_t)mvalue);
*/
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
/*
    if (hybris_check_android_shared_cond(cvalue) ||
         hybris_check_android_shared_mutex(mvalue)) {
        LOGD("Shared condition/mutex with Android, not waiting.");
        return 0;
    }
*/
    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;
/*
    if (hybris_is_pointer_in_shm((void*)cvalue))
        realcond = (pthread_cond_t *)hybris_get_shmpointer((hybris_shm_pointer_t)cvalue);
*/
    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;
/*
    if (hybris_is_pointer_in_shm((void*)mvalue))
        realmutex = (pthread_mutex_t *)hybris_get_shmpointer((hybris_shm_pointer_t)mvalue);
*/
    if (mvalue <= ANDROID_TOP_ADDR_VALUE_MUTEX) {
        realmutex = hybris_alloc_init_mutex(mvalue);
        *((unsigned int *) mutex) = (unsigned int) realmutex;
    }

    return glibc_pthread_cond_timedwait(realcond, realmutex, abstime);
}

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond,
                pthread_mutex_t *mutex, const struct timespec *reltime)
{
    /* Both cond and mutex can be statically initialized, check for both */
    unsigned int cvalue = (*(unsigned int *) cond);
    unsigned int mvalue = (*(unsigned int *) mutex);

    LOAD_GLIBC_SYMBOL(pthread_cond_timedwait_relative_np);
    LOAD_GLIBC_SYMBOL(clock_gettime);
/*
    if (hybris_check_android_shared_cond(cvalue) ||
         hybris_check_android_shared_mutex(mvalue)) {
        LOGD("Shared condition/mutex with Android, not waiting.");
        return 0;
    }
*/

    pthread_cond_t *realcond = (pthread_cond_t *) cvalue;

/*
    if( hybris_is_pointer_in_shm((void*)cvalue) )
        realcond = (pthread_cond_t *)hybris_get_shmpointer((hybris_shm_pointer_t)cvalue);
*/
    if (cvalue <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    pthread_mutex_t *realmutex = (pthread_mutex_t *) mvalue;
/*    if (hybris_is_pointer_in_shm((void*)mvalue))
        realmutex = (pthread_mutex_t *)hybris_get_shmpointer((hybris_shm_pointer_t)mvalue);
*/
    if (mvalue <= ANDROID_TOP_ADDR_VALUE_MUTEX) {
        realmutex = hybris_alloc_init_mutex(mvalue);
        *((unsigned int *) mutex) = (unsigned int) realmutex;
    }

#define CLOCK_REALTIME 0
    struct timespec tv;
    glibc_clock_gettime(CLOCK_REALTIME, &tv);
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

    // TODO shared cond?
    realcond = malloc(sizeof(pthread_cond_t));

    *((unsigned int *) cond) = (unsigned int) realcond;

#if 0
    if (!pshared) {
        /* non shared, standard cond: use malloc */
        realcond = malloc(sizeof(pthread_cond_t));

        *((unsigned int *) cond) = (unsigned int) realcond;
    }
    else {
        /* process-shared condition: use the shared memory segment */
        hybris_shm_pointer_t handle = hybris_shm_alloc(sizeof(pthread_cond_t));

        *((unsigned int *)cond) = (unsigned int) handle;

        if (handle)
            realcond = (pthread_cond_t *)hybris_get_shmpointer(handle);
    }
#endif

    return glibc_pthread_cond_init(realcond, attr);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    unsigned int value = (*(unsigned int *) cond);
/*    if (hybris_check_android_shared_cond(value)) {
        LOGD("shared condition with Android, not broadcasting.");
        return 0;
    }
*/
    LOAD_GLIBC_SYMBOL(pthread_cond_broadcast);

    pthread_cond_t *realcond = (pthread_cond_t *) value;
/*
    if (hybris_is_pointer_in_shm((void*)value))
        realcond = (pthread_cond_t *)hybris_get_shmpointer((hybris_shm_pointer_t)value);
*/
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
/*
    if (hybris_check_android_shared_cond(value)) {
        LOGD("Shared condition with Android, not signaling.");
        return 0;
    }
*/
    pthread_cond_t *realcond = (pthread_cond_t *) value;
/*
    if (hybris_is_pointer_in_shm((void*)value))
        realcond = (pthread_cond_t *)hybris_get_shmpointer((hybris_shm_pointer_t)value);
*/
    if (value <= ANDROID_TOP_ADDR_VALUE_COND) {
        realcond = hybris_alloc_init_cond();
        *((unsigned int *) cond) = (unsigned int) realcond;
    }

    return glibc_pthread_cond_signal(realcond);
}

void *calloc(size_t nmemb, size_t size) SOFTFP;

void *calloc(size_t nmemb, size_t size)
{
    LOAD_GLIBC_SYMBOL(calloc);

    return glibc_calloc(nmemb, size);
}

int __cxa_thread_atexit_impl(void (*fn)(void*), void* arg, void* dso_handle) SOFTFP;

int __cxa_thread_atexit_impl(void (*fn)(void*), void* arg, void* dso_handle)
{
    LOAD_GLIBC_SYMBOL(__cxa_thread_atexit_impl);

    return glibc___cxa_thread_atexit_impl(fn, arg, dso_handle);
}

int gettimeofday(void *restrict tp, void *restrict tzp) SOFTFP;

int gettimeofday(void *restrict tp, void *restrict tzp)
{
    LOAD_GLIBC_SYMBOL(gettimeofday);

    return glibc_gettimeofday(tp, tzp);
}

int clock_getres(int clk_id, void *res) SOFTFP;

int clock_getres(int clk_id, void *res)
{
    LOAD_GLIBC_SYMBOL(clock_getres);

    return glibc_clock_getres(clk_id, res);
}

int clock_gettime(int clk_id, void *tp) SOFTFP;

int clock_gettime(int clk_id, void *tp)
{
    LOAD_GLIBC_SYMBOL(clock_gettime);

    return glibc_clock_gettime(clk_id, tp);
}

int clock_settime(int clk_id, const void *tp) SOFTFP;

int clock_settime(int clk_id, const void *tp)
{
    LOAD_GLIBC_SYMBOL(clock_settime);

    return glibc_clock_settime(clk_id, tp);
}

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n) SOFTFP;

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
    LOAD_GLIBC_SYMBOL(wmemcpy);

    return glibc_wmemcpy(dest, src, n);
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

int getchar(void) SOFTFP;

int getchar(void)
{
    LOAD_GLIBC_SYMBOL(getchar);

    return glibc_getchar();
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

// TODO?
void *uselocale(void *newloc) SOFTFP;

void *uselocale(void *newloc)
{
    LOAD_GLIBC_SYMBOL(uselocale);

    return glibc_uselocale(newloc);
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

void *newlocale(int category_mask, const char *locale, void *base) SOFTFP;

void *newlocale(int category_mask, const char *locale, void *base)
{
    LOAD_GLIBC_SYMBOL(newlocale);

    return glibc_newlocale(category_mask, locale, base);
}

void freelocale(void *locobj) SOFTFP;

void freelocale(void *locobj)
{
    LOAD_GLIBC_SYMBOL(freelocale);

    return glibc_freelocale(locobj);
}

size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr) SOFTFP;

size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr)
{
    LOAD_GLIBC_SYMBOL(strftime);

    return glibc_strftime(s, maxsize, format, timeptr);
}

__WINT_TYPE__ btowc(int a) SOFTFP;
__WINT_TYPE__ btowc(int a)
{
    LOAD_GLIBC_SYMBOL(btowc);

    return glibc_btowc(a);
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


int iswalnum(__WINT_TYPE__ a) SOFTFP;
int iswalnum(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswalnum);

    return glibc_iswalnum(a);
}


int iswalpha(__WINT_TYPE__ a) SOFTFP;
int iswalpha(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswalpha);

    return glibc_iswalpha(a);
}


int iswblank(__WINT_TYPE__ a) SOFTFP;
int iswblank(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswblank);

    return glibc_iswblank(a);
}


int iswcntrl(__WINT_TYPE__ a) SOFTFP;
int iswcntrl(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswcntrl);

    return glibc_iswcntrl(a);
}


int iswdigit(__WINT_TYPE__ a) SOFTFP;
int iswdigit(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswdigit);

    return glibc_iswdigit(a);
}


int iswgraph(__WINT_TYPE__ a) SOFTFP;
int iswgraph(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswgraph);

    return glibc_iswgraph(a);
}


int iswlower(__WINT_TYPE__ a) SOFTFP;
int iswlower(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswlower);

    return glibc_iswlower(a);
}


int iswprint(__WINT_TYPE__ a) SOFTFP;
int iswprint(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswprint);

    return glibc_iswprint(a);
}


int iswpunct(__WINT_TYPE__ a) SOFTFP;
int iswpunct(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswpunct);

    return glibc_iswpunct(a);
}


int iswspace(__WINT_TYPE__ a) SOFTFP;
int iswspace(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswspace);

    return glibc_iswspace(a);
}


int iswupper(__WINT_TYPE__ a) SOFTFP;
int iswupper(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswupper);

    return glibc_iswupper(a);
}


int iswxdigit(__WINT_TYPE__ a) SOFTFP;
int iswxdigit(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(iswxdigit);

    return glibc_iswxdigit(a);
}


int iswctype(__WINT_TYPE__ a, wctype_t b) SOFTFP;
int iswctype(__WINT_TYPE__ a, wctype_t b)
{
    LOAD_GLIBC_SYMBOL(iswctype);

    return glibc_iswctype(a, b);
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


__WINT_TYPE__ getwchar(void) SOFTFP;
__WINT_TYPE__ getwchar(void)
{
    LOAD_GLIBC_SYMBOL(getwchar);

    return glibc_getwchar();
}


int mbsinit(const mbstate_t *a) SOFTFP;
int mbsinit(const mbstate_t *a)
{
    LOAD_GLIBC_SYMBOL(mbsinit);

    return glibc_mbsinit(a);
}


size_t mbrlen(const char *a, size_t b, mbstate_t *c) SOFTFP;
size_t mbrlen(const char *a, size_t b, mbstate_t *c)
{
    LOAD_GLIBC_SYMBOL(mbrlen);

    return glibc_mbrlen(a, b, c);
}


size_t mbrtowc(wchar_t *a, const char *b, size_t c, mbstate_t *d) SOFTFP;
size_t mbrtowc(wchar_t *a, const char *b, size_t c, mbstate_t *d)
{
    LOAD_GLIBC_SYMBOL(mbrtowc);

    return glibc_mbrtowc(a, b, c, d);
}


size_t mbsrtowcs(wchar_t* a, const char** b, size_t c, mbstate_t* d) SOFTFP;
size_t mbsrtowcs(wchar_t* a, const char** b, size_t c, mbstate_t* d)
{
    LOAD_GLIBC_SYMBOL(mbsrtowcs);

    return glibc_mbsrtowcs(a, b, c, d);
}


size_t mbsnrtowcs(wchar_t*a, const char**b, size_t c, size_t d, mbstate_t*e) SOFTFP;
size_t mbsnrtowcs(wchar_t*a, const char**b, size_t c, size_t d, mbstate_t*e)
{
    LOAD_GLIBC_SYMBOL(mbsnrtowcs);

    return glibc_mbsnrtowcs(a, b, c, d, e);
}


size_t mbstowcs(wchar_t *a, const char *b, size_t c) SOFTFP;
size_t mbstowcs(wchar_t *a, const char *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(mbstowcs);

    return glibc_mbstowcs(a, b, c);
}


__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(putwc);

    return glibc_putwc(a, _get_actual_fp(fp));
}


__WINT_TYPE__ putwchar(wchar_t a) SOFTFP;
__WINT_TYPE__ putwchar(wchar_t a)
{
    LOAD_GLIBC_SYMBOL(putwchar);

    return glibc_putwchar(a);
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


__WINT_TYPE__ towlower(__WINT_TYPE__ a) SOFTFP;
__WINT_TYPE__ towlower(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(towlower);

    return glibc_towlower(a);
}


__WINT_TYPE__ towupper(__WINT_TYPE__ a) SOFTFP;
__WINT_TYPE__ towupper(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(towupper);

    return glibc_towupper(a);
}


__WINT_TYPE__ ungetwc(__WINT_TYPE__ a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ ungetwc(__WINT_TYPE__ a, BIONIC_FILE *fp)
{
    LOAD_GLIBC_SYMBOL(ungetwc);

    return glibc_ungetwc(a, _get_actual_fp(fp));
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


wchar_t* wcpcpy (wchar_t*a, const wchar_t *b) SOFTFP;
wchar_t* wcpcpy (wchar_t*a, const wchar_t *b)
{
    LOAD_GLIBC_SYMBOL(wcpcpy);

    return glibc_wcpcpy(a, b);
}


wchar_t* wcpncpy (wchar_t*a, const wchar_t *b, size_t c) SOFTFP;
wchar_t* wcpncpy (wchar_t*a, const wchar_t *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(wcpncpy);

    return glibc_wcpncpy(a, b, c);
}


size_t wcrtomb(char *a, wchar_t b, mbstate_t *c) SOFTFP;
size_t wcrtomb(char *a, wchar_t b, mbstate_t *c)
{
    LOAD_GLIBC_SYMBOL(wcrtomb);

    return glibc_wcrtomb(a, b, c);
}


int wcscasecmp(const wchar_t *a, const wchar_t *b) SOFTFP;
int wcscasecmp(const wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC_SYMBOL(wcscasecmp);

    return glibc_wcscasecmp(a, b);
}


int wcscasecmp_l(const wchar_t *a, const wchar_t *b, void *c) SOFTFP;
int wcscasecmp_l(const wchar_t *a, const wchar_t *b, void *c)
{
    LOAD_GLIBC_SYMBOL(wcscasecmp_l);

    return glibc_wcscasecmp_l(a, b, c);
}


wchar_t *wcscat(wchar_t *a, const wchar_t *b) SOFTFP;
wchar_t *wcscat(wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC_SYMBOL(wcscat);

    return glibc_wcscat(a, b);
}


wchar_t *wcschr(const wchar_t *a, wchar_t b) SOFTFP;
wchar_t *wcschr(const wchar_t *a, wchar_t b)
{
    LOAD_GLIBC_SYMBOL(wcschr);

    return glibc_wcschr(a, b);
}


int wcscmp(const wchar_t *a, const wchar_t *b) SOFTFP;
int wcscmp(const wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC_SYMBOL(wcscmp);

    return glibc_wcscmp(a, b);
}

long long wcstoll(const wchar_t*a, wchar_t**b, int c) SOFTFP;
long long wcstoll(const wchar_t*a, wchar_t**b, int c)
{
    LOAD_GLIBC_SYMBOL(wcstoll);

    return glibc_wcstoll(a, b, c);
}

long long wcstoll_l(const wchar_t *a, wchar_t **b, int c, void*d) SOFTFP;
long long wcstoll_l(const wchar_t *a, wchar_t **b, int c, void*d)
{
    LOAD_GLIBC_SYMBOL(wcstoll_l);

    return glibc_wcstoll_l(a, b, c, d);
}

size_t wcsxfrm_l(wchar_t *a, const wchar_t *b, size_t c, void*d) SOFTFP;
size_t wcsxfrm_l(wchar_t *a, const wchar_t *b, size_t c, void*d)
{
    LOAD_GLIBC_SYMBOL(wcsxfrm_l);

    return glibc_wcsxfrm_l(a, b, c, d);
}

void *localeconv(void) SOFTFP;
void *localeconv(void)
{
    LOAD_GLIBC_SYMBOL(localeconv);

    return glibc_localeconv();
}

char *setlocale(int category, const char *locale)
{
    LOAD_GLIBC_SYMBOL(setlocale);

    return glibc_setlocale(category, locale);
}

int toupper_l(int a, void *b) SOFTFP;
int toupper_l(int a, void *b)
{
    LOAD_GLIBC_SYMBOL(toupper_l);

    return glibc_toupper_l(a, b);
}

int tolower_l(int a, void *b) SOFTFP;
int tolower_l(int a, void *b)
{
    LOAD_GLIBC_SYMBOL(tolower_l);

    return glibc_tolower_l(a, b);
}

int iswalnum_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswalnum_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswalnum_l);

    return glibc_iswalnum_l(a, b);
}


int iswalpha_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswalpha_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswalpha_l);

    return glibc_iswalpha_l(a, b);
}


int iswblank_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswblank_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswblank_l);

    return glibc_iswblank_l(a, b);
}


int iswcntrl_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswcntrl_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswcntrl_l);

    return glibc_iswcntrl_l(a, b);
}


int iswdigit_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswdigit_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswdigit_l);

    return glibc_iswdigit_l(a, b);
}


int iswgraph_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswgraph_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswgraph_l);

    return glibc_iswgraph_l(a, b);
}


int iswlower_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswlower_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswlower_l);

    return glibc_iswlower_l(a, b);
}


int iswprint_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswprint_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswprint_l);

    return glibc_iswprint_l(a, b);
}


int iswpunct_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswpunct_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswpunct_l);

    return glibc_iswpunct_l(a, b);
}


int iswspace_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswspace_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswspace_l);

    return glibc_iswspace_l(a, b);
}


int iswupper_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswupper_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswupper_l);

    return glibc_iswupper_l(a, b);
}


int iswxdigit_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswxdigit_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC_SYMBOL(iswxdigit_l);

    return glibc_iswxdigit_l(a, b);
}


int towlower_l(int a, void *b) SOFTFP;

int towlower_l(int a, void *b)
{
    LOAD_GLIBC_SYMBOL(towlower_l);

    return glibc_towlower_l(a, b);
}


int towupper_l(int a, void *b) SOFTFP;

int towupper_l(int a, void *b)
{
    LOAD_GLIBC_SYMBOL(towupper_l);

    return glibc_towupper_l(a, b);
}


int iswctype_l(__WINT_TYPE__ a, wctype_t b, void *c) SOFTFP;

int iswctype_l(__WINT_TYPE__ a, wctype_t b, void *c)
{
    LOAD_GLIBC_SYMBOL(iswctype_l);

    return glibc_iswctype_l(a, b, c);
}

wchar_t *wcscpy(wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcscpy(wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC_SYMBOL(wcscpy);

    return glibc_wcscpy(a, b);
}


size_t wcscspn(const wchar_t * a,  const wchar_t * b) SOFTFP;
size_t wcscspn(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC_SYMBOL(wcscspn);

    return glibc_wcscspn(a, b);
}


size_t wcsftime(wchar_t * a,  size_t b,   const wchar_t * c,    const void * d) SOFTFP;
size_t wcsftime(wchar_t * a,  size_t b,   const wchar_t * c,    const void * d)
{
    LOAD_GLIBC_SYMBOL(wcsftime);

    return glibc_wcsftime(a, b, c, d);
}

size_t wcslen(const wchar_t * a) SOFTFP;
size_t wcslen(const wchar_t * a)
{
    LOAD_GLIBC_SYMBOL(wcslen);

    return glibc_wcslen(a);
}


int wcsncasecmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wcsncasecmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcsncasecmp);

    return glibc_wcsncasecmp(a, b, c);
}


int wcsncasecmp_l(const wchar_t * a,  const wchar_t * b,   size_t c,    void* d) SOFTFP;
int wcsncasecmp_l(const wchar_t * a,  const wchar_t * b,   size_t c,    void* d)
{
    LOAD_GLIBC_SYMBOL(wcsncasecmp_l);

    return glibc_wcsncasecmp_l(a, b, c, d);
}


wchar_t *wcsncat(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wcsncat(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcsncat);

    return glibc_wcsncat(a, b, c);
}


int wcsncmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wcsncmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcsncmp);

    return glibc_wcsncmp(a, b, c);
}


wchar_t *wcsncpy(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wcsncpy(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcsncpy);

    return glibc_wcsncpy(a, b, c);
}


size_t wcsnrtombs(char*a, const wchar_t** b,  size_t c,   size_t d,    mbstate_t* e) SOFTFP;
size_t wcsnrtombs(char*a, const wchar_t** b,  size_t c,   size_t d,    mbstate_t* e)
{
    LOAD_GLIBC_SYMBOL(wcsnrtombs);

    return glibc_wcsnrtombs(a, b, c, d, e);
}


wchar_t *wcspbrk(const wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcspbrk(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC_SYMBOL(wcspbrk);

    return glibc_wcspbrk(a, b);
}


wchar_t *wcsrchr(const wchar_t * a,  wchar_t b) SOFTFP;
wchar_t *wcsrchr(const wchar_t * a,  wchar_t b)
{
    LOAD_GLIBC_SYMBOL(wcsrchr);

    return glibc_wcsrchr(a, b);
}


size_t wcsrtombs(char* a,  const wchar_t** b,   size_t c,    mbstate_t* d) SOFTFP;
size_t wcsrtombs(char* a,  const wchar_t** b,   size_t c,    mbstate_t* d)
{
    LOAD_GLIBC_SYMBOL(wcsrtombs);

    return glibc_wcsrtombs(a, b, c, d);
}


size_t wcsspn(const wchar_t * a,  const wchar_t * b) SOFTFP;
size_t wcsspn(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC_SYMBOL(wcsspn);

    return glibc_wcsspn(a, b);
}


wchar_t *wcsstr(const wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcsstr(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC_SYMBOL(wcsstr);

    return glibc_wcsstr(a, b);
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


wchar_t* wcstok(wchar_t* a,  const wchar_t* b,   wchar_t** c) SOFTFP;
wchar_t* wcstok(wchar_t* a,  const wchar_t* b,   wchar_t** c)
{
    LOAD_GLIBC_SYMBOL(wcstok);

    return glibc_wcstok(a, b, c);
}


long wcstol(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
long wcstol(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC_SYMBOL(wcstol);

    return glibc_wcstol(a, b, c);
}


long double wcstold(const wchar_t* a,  wchar_t** b) SOFTFP;
long double wcstold(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC_SYMBOL(wcstold);

    return glibc_wcstold(a, b);
}


unsigned long wcstoul(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
unsigned long wcstoul(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC_SYMBOL(wcstoul);

    return glibc_wcstoul(a, b, c);
}


unsigned long long wcstoull(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
unsigned long long wcstoull(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC_SYMBOL(wcstoull);

    return glibc_wcstoull(a, b, c);
}


int wcswidth(const wchar_t * a,  size_t b) SOFTFP;
int wcswidth(const wchar_t * a,  size_t b)
{
    LOAD_GLIBC_SYMBOL(wcswidth);

    return glibc_wcswidth(a, b);
}


size_t wcsxfrm(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
size_t wcsxfrm(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcsxfrm);

    return glibc_wcsxfrm(a, b, c);
}


int wctob(__WINT_TYPE__ a) SOFTFP;
int wctob(__WINT_TYPE__ a)
{
    LOAD_GLIBC_SYMBOL(wctob);

    return glibc_wctob(a);
}


wctype_t wctype(const char * a) SOFTFP;
wctype_t wctype(const char * a)
{
    LOAD_GLIBC_SYMBOL(wctype);

    return glibc_wctype(a);
}


int wcwidth(wchar_t a) SOFTFP;
int wcwidth(wchar_t a)
{
    LOAD_GLIBC_SYMBOL(wcwidth);

    return glibc_wcwidth(a);
}


wchar_t *wmemchr(const wchar_t * a,  wchar_t b,   size_t c) SOFTFP;
wchar_t *wmemchr(const wchar_t * a,  wchar_t b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wmemchr);

    return glibc_wmemchr(a, b, c);
}


int wmemcmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wmemcmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wmemcmp);

    return glibc_wmemcmp(a, b, c);
}


wchar_t *wmempcpy(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wmempcpy(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wmempcpy);

    return glibc_wmempcpy(a, b, c);
}

wchar_t *wmemmove(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wmemmove(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wmemmove);

    return glibc_wmemmove(a, b, c);
}


wchar_t *wmemset(wchar_t * a,  wchar_t b,   size_t c) SOFTFP;
wchar_t *wmemset(wchar_t * a,  wchar_t b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wmemset);

    return glibc_wmemset(a, b, c);
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

unsigned long long wcstoull_l(const wchar_t * a,  wchar_t ** b,   int c,    void* d) SOFTFP;
unsigned long long wcstoull_l(const wchar_t * a,  wchar_t ** b,   int c,    void* d)
{
    LOAD_GLIBC_SYMBOL(wcstoull_l);

    return glibc_wcstoull_l(a, b, c, d);
}


long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c) SOFTFP;
long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c)
{
    LOAD_GLIBC_SYMBOL(wcstold_l);

    return glibc_wcstold_l(a, b, c);
}



int wcscoll_l(const wchar_t * a,  const wchar_t * b,   void* c) SOFTFP;
int wcscoll_l(const wchar_t * a,  const wchar_t * b,   void* c)
{
    LOAD_GLIBC_SYMBOL(wcscoll_l);

    return glibc_wcscoll_l(a, b, c);
}

size_t wcslcat(wchar_t* a,  const wchar_t* b,   size_t c) SOFTFP;
size_t wcslcat(wchar_t* a,  const wchar_t* b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcslcat);

    return glibc_wcslcat(a, b, c);
}


size_t wcslcpy(wchar_t* a,  const wchar_t* b,   size_t c) SOFTFP;
size_t wcslcpy(wchar_t* a,  const wchar_t* b,   size_t c)
{
    LOAD_GLIBC_SYMBOL(wcslcpy);

    return glibc_wcslcpy(a, b, c);
}



typedef void *wctrans_t;
__WINT_TYPE__ towctrans(__WINT_TYPE__ a,  wctrans_t b) SOFTFP;
__WINT_TYPE__ towctrans(__WINT_TYPE__ a,  wctrans_t b)
{
    LOAD_GLIBC_SYMBOL(towctrans);

    return glibc_towctrans(a, b);
}


wctrans_t wctrans(const char* a) SOFTFP;
wctrans_t wctrans(const char* a)
{
    LOAD_GLIBC_SYMBOL(wctrans);

    return glibc_wctrans(a);
}

int mblen(const char *a, size_t b) SOFTFP;
int mblen(const char *a, size_t b)
{
    LOAD_GLIBC_SYMBOL(mblen);

    return glibc_mblen(a, b);
}


int mbtowc(wchar_t *a, const char *b, size_t c) SOFTFP;
int mbtowc(wchar_t *a, const char *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(mbtowc);

    return glibc_mbtowc(a, b, c);
}

size_t __ctype_get_mb_cur_max(void) SOFTFP;
size_t __ctype_get_mb_cur_max(void)
{
    LOAD_GLIBC_SYMBOL(__ctype_get_mb_cur_max);
    return glibc___ctype_get_mb_cur_max();
}

void exit(int a) SOFTFP;

void exit(int a)
{
    LOAD_GLIBC_SYMBOL(exit);

    glibc_exit(a);
    for(;;);
}


char* mkdtemp(char*a) SOFTFP;

char* mkdtemp(char*a)
{
    LOAD_GLIBC_SYMBOL(mkdtemp);

    return glibc_mkdtemp(a);
}

char* mktemp(char*a) SOFTFP;

char* mktemp(char*a)
{
    LOAD_GLIBC_SYMBOL(mktemp);

    return glibc_mktemp(a);
}


int mkostemp64(char*a, int b) SOFTFP;

int mkostemp64(char*a, int b)
{
    LOAD_GLIBC_SYMBOL(mkostemp64);

    return glibc_mkostemp64(a, b);
}

int mkostemp(char*a, int b) SOFTFP;

int mkostemp(char*a, int b)
{
    LOAD_GLIBC_SYMBOL(mkostemp);

    return glibc_mkostemp(a, b);
}

int mkostemps64(char*a, int b, int c) SOFTFP;

int mkostemps64(char*a, int b, int c)
{
    LOAD_GLIBC_SYMBOL(mkostemps64);

    return glibc_mkostemps64(a, b, c);
}

int mkostemps(char*a, int b, int c) SOFTFP;

int mkostemps(char*a, int b, int c)
{
    LOAD_GLIBC_SYMBOL(mkostemps);

    return glibc_mkostemps(a, b, c);
}

int mkstemp64(char*a) SOFTFP;

int mkstemp64(char*a)
{
    LOAD_GLIBC_SYMBOL(mkstemp64);

    return glibc_mkstemp64(a);
}

int mkstemp(char*a) SOFTFP;

int mkstemp(char*a)
{
    LOAD_GLIBC_SYMBOL(mkstemp);

    return glibc_mkstemp(a);
}

int mkstemps64(char*a, int b) SOFTFP;

int mkstemps64(char*a, int b)
{
    LOAD_GLIBC_SYMBOL(mkstemps64);

    return glibc_mkstemps64(a, b);
}

int mkstemps(char*a, int b) SOFTFP;

int mkstemps(char*a, int b)
{
    LOAD_GLIBC_SYMBOL(mkstemps);

    return glibc_mkstemps(a, b);
}


long strtol(const char *a, char **b, int c) SOFTFP;

long strtol(const char *a, char **b, int c)
{
    LOAD_GLIBC_SYMBOL(strtol);

    return glibc_strtol(a, b, c);
}

long long strtoll(const char *a, char **b, int c) SOFTFP;

long long strtoll(const char *a, char **b, int c)
{
    LOAD_GLIBC_SYMBOL(strtoll);

    return glibc_strtoll(a, b, c);
}

unsigned long strtoul(const char *a, char **b, int c) SOFTFP;

unsigned long strtoul(const char *a, char **b, int c)
{
    LOAD_GLIBC_SYMBOL(strtoul);

    return glibc_strtoul(a, b, c);
}

unsigned long long strtoull(const char *a, char **b, int c) SOFTFP;

unsigned long long strtoull(const char *a, char **b, int c)
{
    LOAD_GLIBC_SYMBOL(strtoull);

    return glibc_strtoull(a, b, c);
}


int posix_memalign(void **memptr, size_t alignment, size_t size) SOFTFP;

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    LOAD_GLIBC_SYMBOL(posix_memalign);

    return glibc_posix_memalign(memptr, alignment, size);
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

long long strtoll_l(const char *a, char **b, int c, void*d) SOFTFP;

long long strtoll_l(const char *a, char **b, int c, void*d)
{
    LOAD_GLIBC_SYMBOL(strtoll_l);

    return glibc_strtoll_l(a, b, c, d);
}

unsigned long long strtoull_l(const char *a, char **b, int c, void*d) SOFTFP;

unsigned long long strtoull_l(const char *a, char **b, int c, void*d)
{
    LOAD_GLIBC_SYMBOL(strtoull_l);

    return glibc_strtoull_l(a, b, c, d);
}


void _Exit(int a) SOFTFP;

void _Exit(int a)
{
    LOAD_GLIBC_SYMBOL(_Exit);

    glibc__Exit(a);
    for(;;); // noreturn
}

int atexit(void (*a)(void)) SOFTFP;

int atexit(void (*a)(void))
{
    LOAD_GLIBC_SYMBOL(atexit);

    return glibc_atexit(a);
}


int at_quick_exit(void (*a)(void)) SOFTFP;

int at_quick_exit(void (*a)(void))
{
    LOAD_GLIBC_SYMBOL(__cxa_at_quick_exit);

    glibc___cxa_at_quick_exit(a);
    for(;;); // noreturn
}

char* getenv(const char* a) SOFTFP;

char* getenv(const char* a)
{
    LOAD_GLIBC_SYMBOL(getenv);

    return glibc_getenv(a);
}

int putenv(char*a) SOFTFP;

int putenv(char*a)
{
    LOAD_GLIBC_SYMBOL(putenv);

    return glibc_putenv(a);
}

int setenv(const char*a, const char*b, int c) SOFTFP;

int setenv(const char*a, const char*b, int c)
{
    LOAD_GLIBC_SYMBOL(setenv);

    return glibc_setenv(a, b, c);
}

int unsetenv(const char*a) SOFTFP;

int unsetenv(const char*a)
{
    LOAD_GLIBC_SYMBOL(unsetenv);

    return glibc_unsetenv(a);
}

int clearenv(void) SOFTFP;

int clearenv(void)
{
    LOAD_GLIBC_SYMBOL(clearenv);

    return glibc_clearenv();
}


int atoi(const char*a) SOFTFP;

int atoi(const char*a)
{
    LOAD_GLIBC_SYMBOL(atoi);

    return glibc_atoi(a);
}

long atol(const char*a) SOFTFP;

long atol(const char*a)
{
    LOAD_GLIBC_SYMBOL(atol);

    return glibc_atol(a);
}

long long atoll(const char*a) SOFTFP;

long long atoll(const char*a)
{
    LOAD_GLIBC_SYMBOL(atoll);

    return glibc_atoll(a);
}


int abs(int a) SOFTFP;

int abs(int a)
{
    LOAD_GLIBC_SYMBOL(abs);

    return glibc_abs(a);
}

long labs(long a) SOFTFP;

long labs(long a)
{
    LOAD_GLIBC_SYMBOL(labs);

    return glibc_labs(a);
}

long long llabs(long long a) SOFTFP;

long long llabs(long long a)
{
    LOAD_GLIBC_SYMBOL(llabs);

    return glibc_llabs(a);
}


char * realpath(const char *path, char *resolved) SOFTFP;

char * realpath(const char *path, char *resolved)
{
        LOAD_GLIBC_SYMBOL(realpath);

    return glibc_realpath(path, resolved);
}

int system(const char *string) SOFTFP;

int system(const char *string)
{
    LOAD_GLIBC_SYMBOL(system);

    return glibc_system(string);
}


void * bsearch(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d)) SOFTFP;

void * bsearch(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d))
{
    LOAD_GLIBC_SYMBOL(bsearch);

    return glibc_bsearch(key, base0, nmemb, size, compar);
}


void qsort(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *)) SOFTFP;

void qsort(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *))
{
    LOAD_GLIBC_SYMBOL(qsort);

    return glibc_qsort(a, b, c, d);
}


uint32_t arc4random(void) SOFTFP;

uint32_t arc4random(void)
{
    LOAD_GLIBC_SYMBOL(arc4random);

    return glibc_arc4random();
}

uint32_t arc4random_uniform(uint32_t a) SOFTFP;

uint32_t arc4random_uniform(uint32_t a)
{
    LOAD_GLIBC_SYMBOL(arc4random_uniform);

    return glibc_arc4random_uniform(a);
}

void arc4random_buf(void* a, size_t b) SOFTFP;

void arc4random_buf(void* a, size_t b)
{
    LOAD_GLIBC_SYMBOL(arc4random_buf);

    return glibc_arc4random_buf(a, b);
}


int rand(void) SOFTFP;

int rand(void)
{
    LOAD_GLIBC_SYMBOL(rand);

    return glibc_rand();
}

int rand_r(unsigned int*a) SOFTFP;

int rand_r(unsigned int*a)
{
    LOAD_GLIBC_SYMBOL(rand_r);

    return glibc_rand_r(a);
}

void srand(unsigned int a) SOFTFP;

void srand(unsigned int a)
{
    LOAD_GLIBC_SYMBOL(srand);

    return glibc_srand(a);
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

long jrand48(unsigned short a[3]) SOFTFP;

long jrand48(unsigned short a[3])
{
    LOAD_GLIBC_SYMBOL(jrand48);

    return glibc_jrand48(a);
}

void lcong48(unsigned short a[7]) SOFTFP;

void lcong48(unsigned short a[7])
{
    LOAD_GLIBC_SYMBOL(lcong48);

    return glibc_lcong48(a);
}

long lrand48(void) SOFTFP;

long lrand48(void)
{
    LOAD_GLIBC_SYMBOL(lrand48);

    return glibc_lrand48();
}

long mrand48(void) SOFTFP;

long mrand48(void)
{
    LOAD_GLIBC_SYMBOL(mrand48);

    return glibc_mrand48();
}

long nrand48(unsigned short a[3]) SOFTFP;

long nrand48(unsigned short a[3])
{
    LOAD_GLIBC_SYMBOL(nrand48);

    return glibc_nrand48(a);
}

unsigned short* seed48(unsigned short a[3]) SOFTFP;

unsigned short* seed48(unsigned short a[3])
{
    LOAD_GLIBC_SYMBOL(seed48);

    return glibc_seed48(a);
}

void srand48(long a) SOFTFP;

void srand48(long a)
{
    LOAD_GLIBC_SYMBOL(srand48);

    return glibc_srand48(a);
}


char* initstate(unsigned int a, char*b, size_t c) SOFTFP;

char* initstate(unsigned int a, char*b, size_t c)
{
    LOAD_GLIBC_SYMBOL(initstate);

    return glibc_initstate(a, b, c);
}

long random(void) SOFTFP;

long random(void)
{
    LOAD_GLIBC_SYMBOL(random);

    return glibc_random();
}

char* setstate(char*a) SOFTFP;

char* setstate(char*a)
{
    LOAD_GLIBC_SYMBOL(setstate);

    return glibc_setstate(a);
}

void srandom(unsigned int a) SOFTFP;

void srandom(unsigned int a)
{
    LOAD_GLIBC_SYMBOL(srandom);

    return glibc_srandom(a);
}


int getpt(void) SOFTFP;

int getpt(void)
{
    LOAD_GLIBC_SYMBOL(getpt);

    return glibc_getpt();
}

int grantpt(int a) SOFTFP;

int grantpt(int a)
{
    LOAD_GLIBC_SYMBOL(grantpt);

    return glibc_grantpt(a);
}

int posix_openpt(int a) SOFTFP;

int posix_openpt(int a)
{
    LOAD_GLIBC_SYMBOL(posix_openpt);

    return glibc_posix_openpt(a);
}

char* ptsname(int a) SOFTFP;

char* ptsname(int a)
{
    LOAD_GLIBC_SYMBOL(ptsname);

    return glibc_ptsname(a);
}

int ptsname_r(int a, char*b, size_t c) SOFTFP;

int ptsname_r(int a, char*b, size_t c)
{
    LOAD_GLIBC_SYMBOL(ptsname_r);

    return glibc_ptsname_r(a, b, c);
}

int unlockpt(int a) SOFTFP;

int unlockpt(int a)
{
    LOAD_GLIBC_SYMBOL(unlockpt);

    return glibc_unlockpt(a);
}

div_t div(int a, int b) SOFTFP;

div_t div(int a, int b)
{
    LOAD_GLIBC_SYMBOL(div);

    return glibc_div(a, b);
}

ldiv_t ldiv(long a, long b) SOFTFP;

ldiv_t ldiv(long a, long b)
{
    LOAD_GLIBC_SYMBOL(ldiv);

    return glibc_ldiv(a, b);
}

lldiv_t lldiv(long long a, long long b) SOFTFP;
lldiv_t lldiv(long long a, long long b)
{
    LOAD_GLIBC_SYMBOL(lldiv);

    return glibc_lldiv(a, b);
}

const char* getprogname(void) SOFTFP;

const char* getprogname(void)
{
    LOAD_GLIBC_SYMBOL(getprogname);

    return glibc_getprogname();
}

void setprogname(const char*a) SOFTFP;

void setprogname(const char*a)
{
    LOAD_GLIBC_SYMBOL(setprogname);

    return glibc_setprogname(a);
}

int sched_yield(void) SOFTFP;
int sched_yield(void)
{
    LOAD_GLIBC_SYMBOL(sched_yield);

    return glibc_sched_yield();
}

int pthread_mutex_destroy(void *mutex) SOFTFP;
int pthread_mutex_init(void *restrict mutex, const void *restrict attr) SOFTFP;
int pthread_mutex_trylock(void *mutex) SOFTFP;
int pthread_key_delete(unsigned int key) SOFTFP;
int pthread_join(void *thread, void **retval) SOFTFP;
int pthread_detach(void *thread) SOFTFP;
int pthread_mutexattr_init(void *attr) SOFTFP;
int pthread_mutexattr_destroy(void *attr) SOFTFP;
void *pthread_self(void) SOFTFP;
int pthread_mutexattr_settype(void *attr, int type) SOFTFP;
int pthread_equal(void *t1, void * t2) SOFTFP;

int pthread_mutex_destroy(void *mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_destroy);

    pthread_mutex_t *realmutex = (pthread_mutex_t *) *(unsigned int *) mutex;

    return glibc_pthread_mutex_destroy(realmutex);
}

int pthread_mutex_init(void *restrict mutex, const void *restrict attr)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_init);
    LOAD_GLIBC_SYMBOL(malloc);

    *(unsigned int *) mutex = glibc_malloc(sizeof(pthread_mutex_t));

    return glibc_pthread_mutex_init(*(unsigned int*)mutex, attr);
}

int pthread_mutex_trylock(void *mutex)
{
    LOAD_GLIBC_SYMBOL(pthread_mutex_trylock);

    return glibc_pthread_mutex_trylock(mutex);
}

int pthread_key_delete(unsigned int key)
{
    LOAD_GLIBC_SYMBOL(pthread_key_delete);

    return glibc_pthread_key_delete(key);
}

int pthread_join(void *thread, void **retval)
{
    LOAD_GLIBC_SYMBOL(pthread_join);

    return glibc_pthread_join(thread, retval);
}

int pthread_detach(void *thread)
{
    LOAD_GLIBC_SYMBOL(pthread_detach);

    return glibc_pthread_detach(thread);
}

int pthread_mutexattr_init(void *attr)
{
    LOAD_RT_SYMBOL(pthread_mutexattr_init);

    return rt_pthread_mutexattr_init(attr);
}

int pthread_mutexattr_destroy(void *attr)
{
    LOAD_RT_SYMBOL(pthread_mutexattr_destroy);

    return rt_pthread_mutexattr_destroy(attr);
}

void *pthread_self(void)
{
    LOAD_GLIBC_SYMBOL(pthread_self);

    return glibc_pthread_self();
}

int pthread_mutexattr_settype(void *attr, int type)
{
    LOAD_RT_SYMBOL(pthread_mutexattr_settype);

    return rt_pthread_mutexattr_settype(attr, type);
}

int pthread_equal(void *t1, void * t2)
{
    LOAD_GLIBC_SYMBOL(pthread_equal);

    return glibc_pthread_equal(t1, t2);
}

//#include <sys/stat.h>
#include <fcntl.h>

void gettid(void) __attribute__((naked,noinline));
void gettid(void)
{
    __asm__ volatile("\
    mov     ip, r7\n\
    ldr     r7, =224\n\
    swi     #0\n\
    mov     r7, ip\n\
    cmn     r0, #4096\n\
    bxls    lr\n\
    neg     r0, r0\n\
    b       __set_errno_internal\n");
}

int fcntl64(int a, int b, void *c)
{
    __asm__ volatile("\
    mov     ip, r7\n\
    ldr     r7, =221\n\
    swi     #0\n\
    mov     r7, ip\n\
    cmn     r0, #4096\n\
    bxls    lr\n\
    neg     r0, r0\n\
    b       __set_errno_internal\n");
}

int fcntl(int fd, int cmd, ...) SOFTFP;
int fcntl(int fd, int cmd, ...)
{
    va_list ap;
    void * arg;
    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);
    return fcntl64(fd, cmd, arg);
}

int open(const char *path, int oflag, ...) SOFTFP;

int open(const char *path, int oflag, ...)
{
    LOAD_GLIBC_SYMBOL(open);

    if (oflag & O_CREAT)
    {
        mode_t mode;
        va_list ap;
        va_start(ap, oflag);
        mode = va_arg(ap, mode_t); 
        va_end(ap);

        return glibc_open(path, oflag, mode);
    }
    else
    {
        return glibc_open(path, oflag);
    }
}

int close(int a) SOFTFP;

int close(int a)
{
    LOAD_GLIBC_SYMBOL(close);

    return glibc_close(a);
}

ssize_t __read_chk(int fd, void * buf, size_t nbytes, size_t buflen) SOFTFP;

ssize_t __read_chk(int fd, void * buf, size_t nbytes, size_t buflen)
{
    LOAD_GLIBC_SYMBOL(__read_chk);

    return glibc___read_chk(fd, buf, nbytes, buflen);
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

char *strerror_r(int errnum, char *buf, size_t buflen) SOFTFP;

char *strerror_r(int errnum, char *buf, size_t buflen)
{
    LOAD_GLIBC_SYMBOL(strerror_r);

    return glibc_strerror_r(errnum, buf, buflen);
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

int nanosleep(const void *rqtp, void *rmtp) SOFTFP;

int nanosleep(const void *rqtp, void *rmtp)
{
    LOAD_GLIBC_SYMBOL(nanosleep);

    return glibc_nanosleep(rqtp, rmtp);
}

int raise(int sig) SOFTFP;
int raise(int sig)
{
    LOAD_GLIBC_SYMBOL(raise);

    return glibc_raise(sig);
}

int __cxa_guard_acquire(void *g) SOFTFP;

int __cxa_guard_acquire(void *g)
{
    LOAD_GLIBC_SYMBOL(__cxa_guard_acquire);

    return glibc___cxa_guard_acquire(g);
}

uintptr_t __stack_chk_guard = 0;
/*
#include <sys/auxv.h>

static void __attribute__((constructor)) __init_stack_check_guard() {
    // AT_RANDOM is a pointer to 16 bytes of randomness on the stack.
    __stack_chk_guard = (uintptr_t)(getauxval(AT_RANDOM));
}
*/
time_t time(time_t*) SOFTFP;
char* asctime(const void*) SOFTFP;
char* asctime_r(const void*, char*) SOFTFP;
double difftime(time_t, time_t) SOFTFP;
void* localtime(const time_t*) SOFTFP;
void* localtime_r(const time_t*, void*) SOFTFP;
void* gmtime(const time_t*) SOFTFP;
void* gmtime_r(const time_t*, void*) SOFTFP;
char* strptime(const char*, const char*, void*) SOFTFP;
size_t strftime_l(char *, size_t, const char *, const void *, void*) SOFTFP;
char* ctime(const time_t*) SOFTFP;
char* ctime_r(const time_t*, char*) SOFTFP;
void tzset(void) SOFTFP;
__kernel_long_t clock(void) SOFTFP;
int clock_getcpuclockid(pid_t, clockid_t*) SOFTFP;
int clock_nanosleep(clockid_t, int, const void*, void*) SOFTFP;
int timer_create(int, void*, timer_t*) SOFTFP;
int timer_delete(timer_t) SOFTFP;
int timer_settime(timer_t, int, const void*, void*) SOFTFP;
int timer_gettime(timer_t, void*) SOFTFP;
int timer_getoverrun(timer_t) SOFTFP;
time_t timelocal(void*) SOFTFP;
time_t timegm(void*) SOFTFP;

time_t time(time_t* a)
{
    LOAD_GLIBC_SYMBOL(time);

    return glibc_time(a);
}

char* asctime(const void*a)
{
    LOAD_GLIBC_SYMBOL(asctime);

    return glibc_asctime(a);
}


char* asctime_r(const void*a, char*b)
{
    LOAD_GLIBC_SYMBOL(asctime_r);

    return glibc_asctime_r(a, b);
}

double difftime(time_t a, time_t b)
{
    LOAD_GLIBC_SYMBOL(difftime);

    return glibc_difftime(a, b);
}

void* localtime(const time_t*a)
{
    LOAD_GLIBC_SYMBOL(localtime);

    return glibc_localtime(a);
}

void* localtime_r(const time_t*a, void*b)
{
    LOAD_GLIBC_SYMBOL(localtime_r);

    return glibc_localtime_r(a, b);
}

void* gmtime(const time_t*a)
{
    LOAD_GLIBC_SYMBOL(gmtime);

    return glibc_gmtime(a);
}

void* gmtime_r(const time_t*a, void*b)
{
    LOAD_GLIBC_SYMBOL(gmtime_r);

    return glibc_gmtime_r(a, b);
}

char* strptime(const char*a, const char*b, void*c)
{
    LOAD_GLIBC_SYMBOL(strptime);

    return glibc_strptime(a, b, c);
}

size_t strftime_l(char *a, size_t b, const char *c, const void *d, void* e)
{
    LOAD_GLIBC_SYMBOL(strftime_l);

    return glibc_strftime_l(a, b, c, d, e);
}

char* ctime(const time_t*a)
{
    LOAD_GLIBC_SYMBOL(ctime);

    return glibc_ctime(a);
}

char* ctime_r(const time_t*a, char*b)
{
    LOAD_GLIBC_SYMBOL(ctime_r);

    return glibc_ctime_r(a, b);
}

void tzset(void)
{
    LOAD_GLIBC_SYMBOL(tzset);

    return glibc_tzset();
}

__kernel_long_t clock(void)
{
    LOAD_GLIBC_SYMBOL(clock);

    return glibc_clock();
}

int clock_getcpuclockid(pid_t a, clockid_t*b)
{
    LOAD_GLIBC_SYMBOL(clock_getcpuclockid);

    return glibc_clock_getcpuclockid(a, b);
}

int clock_nanosleep(clockid_t a, int b, const void*c, void*d)
{
    LOAD_GLIBC_SYMBOL(clock_nanosleep);

    return glibc_clock_nanosleep(a, b, c, d);
}

int timer_create(int a, void *b, timer_t*c)
{
    LOAD_RT_SYMBOL(timer_create);

    return rt_timer_create(a, b, c);
}

int timer_delete(timer_t a)
{
    LOAD_RT_SYMBOL(timer_delete);

    return rt_timer_delete(a);
}

int timer_settime(timer_t a, int b, const void* c, void* d)
{
    LOAD_RT_SYMBOL(timer_settime);

    return rt_timer_settime(a, b, c, d);
}

int timer_gettime(timer_t a, void*b)
{
    LOAD_RT_SYMBOL(timer_gettime);

    return rt_timer_gettime(a, b);
}

int timer_getoverrun(timer_t a)
{
    LOAD_RT_SYMBOL(timer_getoverrun);

    return rt_timer_getoverrun(a);
}

time_t timelocal(void*a)
{
    LOAD_GLIBC_SYMBOL(timelocal);

    return glibc_timelocal(a);
}

time_t timegm(void*a)
{
    LOAD_GLIBC_SYMBOL(timegm);

    return glibc_timegm(a);
}

time_t mktime(void*a) SOFTFP;

time_t mktime(void*a)
{
    LOAD_GLIBC_SYMBOL(mktime);

    return glibc_mktime(a);
}

int socket(int domain, int type, int protocol) SOFTFP;

int socket(int domain, int type, int protocol)
{
    LOAD_GLIBC_SYMBOL(socket);

    return glibc_socket(domain, type, protocol);
}

int connect(int socket, const void *address, int address_len) SOFTFP;

int connect(int socket, const void *address, int address_len)
{
    LOAD_GLIBC_SYMBOL(connect);

    int ret = glibc_connect(socket, address, address_len);
    return ret;
}

pid_t getpid(void) SOFTFP;

pid_t getpid(void)
{
    LOAD_GLIBC_SYMBOL(getpid);

    return glibc_getpid();
}

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt) SOFTFP;

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
    LOAD_GLIBC_SYMBOL(writev);

    int ret = 0;
    ret = glibc_writev(fildes, iov, iovcnt);

    return ret;
}

ssize_t write(int a, const void *b, size_t c) SOFTFP;

ssize_t write(int a, const void *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(write);

    return glibc_write(a, b, c);
}

int access(const char *path, int amode) SOFTFP;

int access(const char *path, int amode)
{
    LOAD_GLIBC_SYMBOL(access);

    return glibc_access(path, amode);
}

int faccessat(int fd, const char *path, int amode, int flag) SOFTFP;

int faccessat(int fd, const char *path, int amode, int flag)
{
    LOAD_GLIBC_SYMBOL(faccessat);

    return glibc_faccessat(fd, path, amode, flag);
}

void*  memccpy(void* restrict, const void* restrict, int, size_t) SOFTFP;
void*  memchr(const void *, int, size_t) SOFTFP;
void*  memrchr(const void *, int, size_t) SOFTFP;
int    memcmp(const void *, const void *, size_t) SOFTFP;
void*  memmove(void *, const void *, size_t) SOFTFP;
void*  memset(void *, int, size_t) SOFTFP;
void*  memmem(const void *, size_t, const void *, size_t) SOFTFP;
char*  strchr(const char *, int) SOFTFP;
char* __strchr_chk(const char *, int, size_t) SOFTFP;
char*  strrchr(const char *, int) SOFTFP;
char* __strrchr_chk(const char *, int, size_t) SOFTFP;
size_t strlen(const char *) SOFTFP;
int    strcmp(const char *, const char *) SOFTFP;
char*  stpcpy(char* restrict, const char* restrict) SOFTFP;
char*  strcpy(char* restrict, const char* restrict) SOFTFP;
char*  strcat(char* restrict, const char* restrict) SOFTFP;
int strcasecmp(const char*, const char*) SOFTFP;
int strcasecmp_l(const char*, const char*, void*) SOFTFP;
int strncasecmp(const char*, const char*, size_t) SOFTFP;
int strncasecmp_l(const char*, const char*, size_t, void*) SOFTFP;
char*  strdup(const char *) SOFTFP;
char*  strstr(const char *, const char *) SOFTFP;
char*  strcasestr(const char *haystack, const char *needle) SOFTFP;
char*  strtok(char* restrict, const char* restrict) SOFTFP;
char*  strtok_r(char* restrict, const char* restrict, char** restrict) SOFTFP;
char* strerror(int) SOFTFP;
char* strerror_l(int, void*) SOFTFP;
size_t strnlen(const char *, size_t) SOFTFP;
char*  strncat(char* restrict a, const char* restrict b, size_t c) SOFTFP;
char*  strndup(const char *, size_t) SOFTFP;
int    strncmp(const char *, const char *, size_t) SOFTFP;
char*  stpncpy(char* restrict a, const char* restrict b, size_t c) SOFTFP;
char*  strncpy(char* restrict a, const char* restrict b, size_t c) SOFTFP;
size_t strlcat(char* restrict a, const char* restrict b, size_t c) SOFTFP;
size_t strlcpy(char* restrict a, const char* restrict b, size_t c) SOFTFP;
size_t strcspn(const char *, const char *) SOFTFP;
char*  strpbrk(const char *, const char *) SOFTFP;
char*  strsep(char** restrict, const char* restrict) SOFTFP;
size_t strspn(const char *, const char *) SOFTFP;
char*  strsignal(int  sig) SOFTFP;
int    strcoll(const char *, const char *) SOFTFP;
size_t strxfrm(char* restrict a, const char* restrict b, size_t c) SOFTFP;
int    strcoll_l(const char *, const char *, void*) SOFTFP;
size_t strxfrm_l(char* restrict, const char* restrict, size_t, void*) SOFTFP;
char* __gnu_basename(char*) SOFTFP;
void* __memchr_chk(const void*, int, size_t, size_t) SOFTFP;
void* __memrchr_chk(const void*, int, size_t, size_t) SOFTFP;
void* memrchr(const void*, int, size_t) SOFTFP;
char* __stpncpy_chk2(char* restrict, const char* restrict, size_t, size_t, size_t) SOFTFP;
char* __strncpy_chk2(char* restrict, const char* restrict, size_t, size_t, size_t) SOFTFP;
size_t __strlcpy_chk(char *, const char *, size_t, size_t) SOFTFP;
size_t __strlcat_chk(char* restrict, const char* restrict, size_t, size_t) SOFTFP;
void* memchr(const void *s, int c, size_t n) SOFTFP;
void* memrchr(const void *s, int c, size_t n) SOFTFP;
void* memmove(void *dest, const void *src, size_t len) SOFTFP;
char* stpcpy(char* restrict dest, const char* restrict src) SOFTFP;
char* strcpy(char* restrict dest, const char* restrict src) SOFTFP;
char* stpncpy(char* restrict dest, const char* restrict src, size_t n) SOFTFP;
char* strncpy(char* restrict dest, const char* restrict src, size_t n) SOFTFP;
char* strcat(char* restrict dest, const char* restrict src) SOFTFP;
char *strncat(char* restrict dest, const char* restrict src, size_t n) SOFTFP;
size_t strlcat(char* restrict dest, const char* restrict src, size_t size) SOFTFP;
size_t strlen(const char *s) SOFTFP;
char* strchr(const char *s, int c) SOFTFP;
char* strrchr(const char *s, int c) SOFTFP;

void*  memccpy(void* restrict a, const void* restrict b, int c, size_t d)
{
    LOAD_GLIBC_SYMBOL(memccpy);

    return glibc_memccpy(a, b, c, d);
}

void*  memchr(const void * a, int b, size_t c)
{
    LOAD_GLIBC_SYMBOL(memchr);

    return glibc_memchr(a, b, c);
}

int    memcmp(const void *a, const void *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(memcmp);

    return glibc_memcmp(a, b, c);
}

void*  memmove(void *a, const void *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(memmove);

    return glibc_memmove(a, b, c);
}

void*  memset(void *a, int b, size_t c)
{
    LOAD_GLIBC_SYMBOL(memset);

    return glibc_memset(a, b, c);
}

void*  memmem(const void *a, size_t b, const void *c, size_t d)
{
    LOAD_GLIBC_SYMBOL(memmem);

    return glibc_memmem(a, b, c, d);
}

char*  strchr(const char *a, int b)
{
    LOAD_GLIBC_SYMBOL(strchr);

    return glibc_strchr(a, b);
}

char* __strchr_chk(const char *a, int b, size_t c)
{
    LOAD_GLIBC_SYMBOL(__strchr_chk);

    return glibc___strchr_chk(a, b, c);
}

char*  strrchr(const char *a, int b)
{
    LOAD_GLIBC_SYMBOL(strrchr);

    return glibc_strrchr(a, b);
}

char* __strrchr_chk(const char *a, int b, size_t c)
{
    LOAD_GLIBC_SYMBOL(__strrchr_chk);

    return glibc___strrchr_chk(a, b, c);
}

size_t strlen(const char *a)
{
    LOAD_GLIBC_SYMBOL(strlen);

    return glibc_strlen(a);
}

int    strcmp(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strcmp);

    return glibc_strcmp(a, b);
}

char*  stpcpy(char* restrict a, const char* restrict b)
{
    LOAD_GLIBC_SYMBOL(stpcpy);

    return glibc_stpcpy(a, b);
}

char*  strcpy(char* restrict a, const char* restrict b)
{
    LOAD_GLIBC_SYMBOL(strcpy);

    return glibc_strcpy(a, b);
}

char*  strcat(char* restrict a, const char* restrict b)
{
    LOAD_GLIBC_SYMBOL(strcat);

    return glibc_strcat(a, b);
}

int strcasecmp(const char*a, const char*b)
{
    LOAD_GLIBC_SYMBOL(strcasecmp);

    return glibc_strcasecmp(a, b);
}

int strcasecmp_l(const char*a, const char*b, void*c)
{
    LOAD_GLIBC_SYMBOL(strcasecmp_l);

    return glibc_strcasecmp_l(a, b, c);
}

int strncasecmp(const char*a, const char*b, size_t c)
{
    LOAD_GLIBC_SYMBOL(strncasecmp);

    return glibc_strncasecmp(a, b, c);
}

int strncasecmp_l(const char*a, const char*b, size_t c, void*d)
{
    LOAD_GLIBC_SYMBOL(strncasecmp_l);

    return glibc_strncasecmp_l(a, b, c, d);
}

char*  strdup(const char *a)
{
    LOAD_GLIBC_SYMBOL(strdup);

    return glibc_strdup(a);
}

char*  strstr(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strstr);

    return glibc_strstr(a, b);
}

char*  strcasestr(const char *haystack, const char *needle)
{
    LOAD_GLIBC_SYMBOL(strcasestr);

    return glibc_strcasestr(haystack, needle);
}

char*  strtok(char* restrict a, const char* restrict b)
{
    LOAD_GLIBC_SYMBOL(strtok);

    return glibc_strtok(a, b);
}

char*  strtok_r(char* restrict a, const char* restrict b, char** restrict c)
{
    LOAD_GLIBC_SYMBOL(strtok_r);

    return glibc_strtok_r(a, b, c);
}

char* strerror(int a)
{
    LOAD_GLIBC_SYMBOL(strerror);

    return glibc_strerror(a);
}

char* strerror_l(int a, void* b)
{
    LOAD_GLIBC_SYMBOL(strerror_l);

    return glibc_strerror_l(a, b);
}

size_t strnlen(const char *a, size_t b)
{
    LOAD_GLIBC_SYMBOL(strnlen);

    return glibc_strnlen(a, b);
}

char*  strncat(char* restrict a, const char* restrict b, size_t c)
{
    LOAD_GLIBC_SYMBOL(strncat);

    return glibc_strncat(a, b, c);
}

char*  strndup(const char *a, size_t b)
{
    LOAD_GLIBC_SYMBOL(strndup);

    return glibc_strndup(a, b);
}

int    strncmp(const char *a, const char *b, size_t c)
{
    LOAD_GLIBC_SYMBOL(strncmp);

    return glibc_strncmp(a, b, c);
}

char*  stpncpy(char* restrict a, const char* restrict b, size_t c)
{
    LOAD_GLIBC_SYMBOL(stpncpy);

    return glibc_stpncpy(a, b, c);
}

char*  strncpy(char* restrict a, const char* restrict b, size_t c)
{
    LOAD_GLIBC_SYMBOL(strncpy);

    return glibc_strncpy(a, b, c);
}

size_t strcspn(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strcspn);

    return glibc_strcspn(a, b);
}

char*  strpbrk(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strpbrk);

    return glibc_strpbrk(a, b);
}

char*  strsep(char** restrict a, const char* restrict b)
{
    LOAD_GLIBC_SYMBOL(strsep);

    return glibc_strsep(a, b);
}

size_t strspn(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strspn);

    return glibc_strspn(a, b);
}

char*  strsignal(int  sig)
{
    LOAD_GLIBC_SYMBOL(strsignal);

    return glibc_strsignal(sig);
}

int    strcoll(const char *a, const char *b)
{
    LOAD_GLIBC_SYMBOL(strcoll);

    return glibc_strcoll(a, b);
}

size_t strxfrm(char* restrict a, const char* restrict b, size_t c)
{
    LOAD_GLIBC_SYMBOL(strxfrm);

    return glibc_strxfrm(a, b, c);
}

int    strcoll_l(const char *a, const char *b, void*c)
{
    LOAD_GLIBC_SYMBOL(strcoll_l);

    return glibc_strcoll_l(a, b, c);
}

size_t strxfrm_l(char* restrict a, const char* restrict b, size_t c, void*d)
{
    LOAD_GLIBC_SYMBOL(strxfrm_l);

    return glibc_strxfrm_l(a, b, c, d);
}

char* __gnu_basename(char*a)
{
    LOAD_GLIBC_SYMBOL(__gnu_basename);

    return glibc___gnu_basename(a);
}

void* __memchr_chk(const void*a, int b, size_t c, size_t d)
{
    LOAD_GLIBC_SYMBOL(__memchr_chk);

    return glibc___memchr_chk(a, b, c, d);
}

void* __memrchr_chk(const void*a, int b, size_t c, size_t d)
{
    LOAD_GLIBC_SYMBOL(__memrchr_chk);

    return glibc___memrchr_chk(a, b, c, d);
}

void* memrchr(const void*a, int b, size_t c)
{
    LOAD_GLIBC_SYMBOL(memrchr);

    return glibc_memrchr(a, b, c);
}

char* __stpncpy_chk2(char* restrict a, const char* restrict b, size_t c, size_t d, size_t e)
{
    LOAD_GLIBC_SYMBOL(__stpncpy_chk2);

    return glibc___stpncpy_chk2(a, b, c, d, e);
}

char* __strncpy_chk(char* __restrict dest, const char* __restrict src, size_t len, size_t dest_len) SOFTFP;

char* __strncpy_chk(char* __restrict dest, const char* __restrict src, size_t len, size_t dest_len)
{
    LOAD_GLIBC_SYMBOL(__strncpy_chk);

    return glibc___strncpy_chk(dest, src, len, dest_len);
}

char* __strncpy_chk2(char* restrict a, const char* restrict b, size_t c, size_t d, size_t e)
{
    LOAD_GLIBC_SYMBOL(__strncpy_chk2);

    return glibc___strncpy_chk2(a, b, c, d, e);
}

size_t __strlcpy_chk(char *a, const char *b, size_t c, size_t d)
{
    LOAD_GLIBC_SYMBOL(__strlcpy_chk);

    return glibc___strlcpy_chk(a, b, c, d);
}

size_t __strlcat_chk(char* restrict a, const char* restrict b, size_t c, size_t d)
{
    LOAD_GLIBC_SYMBOL(__strlcat_chk);

    return glibc___strlcat_chk(a, b, c, d);
}

extern size_t strlcpy(char* restrict dest, const char* restrict src, size_t size);

extern size_t strlcat(char* restrict dest, const char* restrict src, size_t size);

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen) SOFTFP;

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen)
{
    LOAD_GLIBC_SYMBOL(__memcpy_chk);

    return glibc___memcpy_chk(dest, src, len, destlen);
}

int munmap(void *addr, size_t len) SOFTFP;

int munmap(void *addr, size_t len)
{
    LOAD_GLIBC_SYMBOL(munmap);

    return glibc_munmap(addr, len);
}

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) SOFTFP;

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    LOAD_GLIBC_SYMBOL(mmap);

    return glibc_mmap(addr, len, prot, flags, fildes, off);
}

pid_t  fork(void) SOFTFP;
pid_t  fork(void)
{
    LOAD_GLIBC_SYMBOL(fork);

    return glibc_fork();
}

pid_t  vfork(void) SOFTFP;
pid_t  vfork(void)
{
    LOAD_GLIBC_SYMBOL(vfork);

    return glibc_vfork();
}

pid_t  getpgid(pid_t a) SOFTFP;
pid_t  getpgid(pid_t a)
{
    LOAD_GLIBC_SYMBOL(getpgid);

    return glibc_getpgid(a);
}

int    setpgid(pid_t a, pid_t b) SOFTFP;
int    setpgid(pid_t a, pid_t b)
{
    LOAD_GLIBC_SYMBOL(setpgid);

    return glibc_setpgid(a, b);
}

pid_t  getppid(void) SOFTFP;
pid_t  getppid(void)
{
    LOAD_GLIBC_SYMBOL(getppid);

    return glibc_getppid();
}

pid_t  getpgrp(void) SOFTFP;
pid_t  getpgrp(void)
{
    LOAD_GLIBC_SYMBOL(getpgrp);

    return glibc_getpgrp();
}

int    setpgrp(void) SOFTFP;
int    setpgrp(void)
{
    LOAD_GLIBC_SYMBOL(setpgrp);

    return glibc_setpgrp();
}

pid_t  getsid(pid_t a) SOFTFP;
pid_t  getsid(pid_t a)
{
    LOAD_GLIBC_SYMBOL(getsid);

    return glibc_getsid(a);
}

pid_t  setsid(void) SOFTFP;
pid_t  setsid(void)
{
    LOAD_GLIBC_SYMBOL(setsid);

    return glibc_setsid();
}

int execv(const char * a, char * const * b) SOFTFP;
int execv(const char * a, char * const * b)
{
    LOAD_GLIBC_SYMBOL(execv);

    return glibc_execv(a, b);
}

int execvp(const char * a, char * const * b) SOFTFP;
int execvp(const char * a, char * const * b)
{
    LOAD_GLIBC_SYMBOL(execvp);

    return glibc_execvp(a, b);
}

int execvpe(const char * a, char * const * b, char * const * c) SOFTFP;
int execvpe(const char * a, char * const * b, char * const * c)
{
    LOAD_GLIBC_SYMBOL(execvpe);

    return glibc_execvpe(a, b, c);
}

int execve(const char * a, char * const * b, char * const * c) SOFTFP;
int execve(const char * a, char * const * b, char * const * c)
{
    LOAD_GLIBC_SYMBOL(execve);

    return glibc_execve(a, b, c);
}

int execl(const char * a, const char * b, ...) SOFTFP;
int execl(const char * a, const char * b, ...)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "XXX: stub execl: %s %s\n", a, b);
    (void)glibc_execl;
    return -1;
}

int execlp(const char * a, const char * b, ...) SOFTFP;
int execlp(const char * a, const char * b, ...)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "XXX: stub execlp: %s %s\n", a, b);
    (void)glibc_execlp;
    return -1;
}

int execle(const char * a, const char * b, ...) SOFTFP;
int execle(const char * a, const char * b, ...)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(stderr);
    glibc_fprintf(*glibc_stderr, "XXX: stub execle: %s %s\n", a, b);
    (void)glibc_execle;
    return -1;
}

int nice(int a) SOFTFP;
int nice(int a)
{
    LOAD_GLIBC_SYMBOL(nice);

    return glibc_nice(a);
}

int setuid(uid_t a) SOFTFP;
int setuid(uid_t a)
{
    LOAD_GLIBC_SYMBOL(setuid);

    return glibc_setuid(a);
}

uid_t getuid(void) SOFTFP;
uid_t getuid(void)
{
    LOAD_GLIBC_SYMBOL(getuid);

    return glibc_getuid();
}

int seteuid(uid_t a) SOFTFP;
int seteuid(uid_t a)
{
    LOAD_GLIBC_SYMBOL(seteuid);

    return glibc_seteuid(a);
}

uid_t geteuid(void) SOFTFP;
uid_t geteuid(void)
{
    LOAD_GLIBC_SYMBOL(geteuid);

    return glibc_geteuid();
}

int setgid(gid_t a) SOFTFP;
int setgid(gid_t a)
{
    LOAD_GLIBC_SYMBOL(setgid);

    return glibc_setgid(a);
}

gid_t getgid(void) SOFTFP;
gid_t getgid(void)
{
    LOAD_GLIBC_SYMBOL(getgid);

    return glibc_getgid();
}

int setegid(gid_t a) SOFTFP;
int setegid(gid_t a)
{
    LOAD_GLIBC_SYMBOL(setegid);

    return glibc_setegid(a);
}

gid_t getegid(void) SOFTFP;
gid_t getegid(void)
{
    LOAD_GLIBC_SYMBOL(getegid);

    return glibc_getegid();
}

int getgroups(int a, gid_t * b) SOFTFP;
int getgroups(int a, gid_t * b)
{
    LOAD_GLIBC_SYMBOL(getgroups);

    return glibc_getgroups(a, b);
}

int setgroups(size_t a, const gid_t * b) SOFTFP;
int setgroups(size_t a, const gid_t * b)
{
    LOAD_GLIBC_SYMBOL(setgroups);

    return glibc_setgroups(a, b);
}

int setreuid(uid_t a, uid_t b) SOFTFP;
int setreuid(uid_t a, uid_t b)
{
    LOAD_GLIBC_SYMBOL(setreuid);

    return glibc_setreuid(a, b);
}

int setregid(gid_t a, gid_t b) SOFTFP;
int setregid(gid_t a, gid_t b)
{
    LOAD_GLIBC_SYMBOL(setregid);

    return glibc_setregid(a, b);
}

int setresuid(uid_t a, uid_t b, uid_t c) SOFTFP;
int setresuid(uid_t a, uid_t b, uid_t c)
{
    LOAD_GLIBC_SYMBOL(setresuid);

    return glibc_setresuid(a, b, c);
}

int setresgid(gid_t a, gid_t b, gid_t c) SOFTFP;
int setresgid(gid_t a, gid_t b, gid_t c)
{
    LOAD_GLIBC_SYMBOL(setresgid);

    return glibc_setresgid(a, b, c);
}

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) SOFTFP;
int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid)
{
    LOAD_GLIBC_SYMBOL(getresuid);

    return glibc_getresuid(ruid, euid, suid);
}

int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) SOFTFP;
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
    LOAD_GLIBC_SYMBOL(getresgid);

    return glibc_getresgid(rgid, egid, sgid);
}

char* getlogin(void) SOFTFP;
char* getlogin(void)
{
    LOAD_GLIBC_SYMBOL(getlogin);

    return glibc_getlogin();
}

long fpathconf(int a, int b) SOFTFP;
long fpathconf(int a, int b)
{
    LOAD_GLIBC_SYMBOL(fpathconf);

    return glibc_fpathconf(a, b);
}

long pathconf(const char* a, int b) SOFTFP;
long pathconf(const char* a, int b)
{
    LOAD_GLIBC_SYMBOL(pathconf);

    return glibc_pathconf(a, b);
}

int link(const char* a, const char* b) SOFTFP;
int link(const char* a, const char* b)
{
    LOAD_GLIBC_SYMBOL(link);

    return glibc_link(a, b);
}

int linkat(int a, const char* b, int c, const char* d, int e) SOFTFP;
int linkat(int a, const char* b, int c, const char* d, int e)
{
    LOAD_GLIBC_SYMBOL(linkat);

    return glibc_linkat(a, b, c, d, e);
}

int unlink(const char* a) SOFTFP;
int unlink(const char* a)
{
    LOAD_GLIBC_SYMBOL(unlink);

    return glibc_unlink(a);
}

int unlinkat(int a, const char* b, int c) SOFTFP;
int unlinkat(int a, const char* b, int c)
{
    LOAD_GLIBC_SYMBOL(unlinkat);

    return glibc_unlinkat(a, b, c);
}

int chdir(const char * a) SOFTFP;
int chdir(const char * a)
{
    LOAD_GLIBC_SYMBOL(chdir);

    return glibc_chdir(a);
}

int fchdir(int a) SOFTFP;
int fchdir(int a)
{
    LOAD_GLIBC_SYMBOL(fchdir);

    return glibc_fchdir(a);
}

int rmdir(const char * a) SOFTFP;
int rmdir(const char * a)
{
    LOAD_GLIBC_SYMBOL(rmdir);

    return glibc_rmdir(a);
}

int pipe(int * a) SOFTFP;
int pipe(int * a)
{
    LOAD_GLIBC_SYMBOL(pipe);

    return glibc_pipe(a);
}

int pipe2(int * a, int b) SOFTFP;
int pipe2(int * a, int b)
{
    LOAD_GLIBC_SYMBOL(pipe2);

    return glibc_pipe2(a, b);
}

int chroot(const char * a) SOFTFP;
int chroot(const char * a)
{
    LOAD_GLIBC_SYMBOL(chroot);

    return glibc_chroot(a);
}

off_t lseek(int fildes, off_t offset, int whence) SOFTFP;
off_t lseek(int fildes, off_t offset, int whence)
{
    LOAD_GLIBC_SYMBOL(lseek);

    return glibc_lseek(fildes, offset, whence);
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

extern int feof(void *);

int __system_property_get(const char *name, char *value) SOFTFP;
int __system_property_get(const char *name, char *value)
{
    LOAD_GLIBC_SYMBOL(fprintf);
    LOAD_GLIBC_SYMBOL(stderr);
    LOAD_GLIBC_SYMBOL(fopen);
    LOAD_GLIBC_SYMBOL(strlen);
    LOAD_GLIBC_SYMBOL(strstr);
    LOAD_GLIBC_SYMBOL(strcpy);
    LOAD_GLIBC_SYMBOL(fclose);
    LOAD_GLIBC_SYMBOL(getline);
    
    glibc_fprintf(*glibc_stderr, "__system_property_get(%s, %s)\n", name, value);

    void *fp = glibc_fopen("/system/build.prop", "r");

    char *line = NULL;
    while(!feof(fp))
    {
        int len;
        while(glibc_getline(&line, &len, fp) >= 0)
        {
            line[1023] = 0;
            char *pos = glibc_strstr(line, name);
            if(pos != NULL)
            {
                glibc_strcpy(value, pos+strlen(name)+1);
                // trim newline
                value[glibc_strlen(value)-1] = 0;
                glibc_fprintf(*glibc_stderr, "new value: \"%s\"\n", value);
                goto exit;
            }
        }
    }

exit:
    glibc_fclose(fp);

    return glibc_strlen(value);
}

int __system_property_set(const char *name, char *value) SOFTFP;
int __system_property_set(const char *name, char *value)
{
    fprintf(stderr, "__system_property_set(%s, %s)\n", name, value);
    return 0;
}

int __system_property_foreach(void (*propfn)(const void *pi, void *cookie), void *cookie) SOFTFP;
int __system_property_foreach(void (*propfn)(const void *pi, void *cookie), void *cookie)
{
     fprintf(stderr, "__system_property_foreach\n");

     return 0;
}

unsigned int __system_property_serial(void *pi) SOFTFP;
unsigned int __system_property_serial(void *pi)
{
    fprintf(stderr, "__system_property_serial(%p)\n", pi);

    return 0;
}

void *__system_property_find_compat(const char *name) SOFTFP; 
void *__system_property_find_compat(const char *name)
{
    fprintf(stderr, "__system_property_find_compat(%s)\n", name);

    return NULL;
}

const void *__system_property_find(const char *name) SOFTFP;
const void *__system_property_find(const char *name)
{
    fprintf(stderr, "__system_property_find(%s)\n", name);

    return NULL;
}

unsigned int __system_property_area_serial(void) SOFTFP;

unsigned int __system_property_area_serial(void)
{
    fprintf(stderr, "__system_property_area_serial\n");

    return 0;
}

int __system_property_read(const void *pi, char *name, char *value)
{
    fprintf(stderr, "__system_property_read(%p, %s, %s)\n", pi, name, value);

    return 0;
}

int toupper(int a)
{
    LOAD_GLIBC_SYMBOL(toupper);

    return glibc_toupper(a);
}

int tolower(int a)
{
    LOAD_GLIBC_SYMBOL(tolower);

    return glibc_tolower(a);
}

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset) SOFTFP;

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset)
{
    LOAD_GLIBC_SYMBOL(pread);

    return glibc_pread(fildes, buf, nbyte, offset);
}

ssize_t read(int fildes, void *buf, size_t nbyte) SOFTFP;

ssize_t read(int fildes, void *buf, size_t nbyte)
{
    LOAD_GLIBC_SYMBOL(read);

    return glibc_read(fildes, buf, nbyte);
}

int poll(void *fds, unsigned long int nfds, int timeout) SOFTFP;
int poll(void *fds, unsigned long int nfds, int timeout)
{
    LOAD_GLIBC_SYMBOL(poll);

    return glibc_poll(fds, nfds, timeout);
}

void* __memmove_chk(void* dest, const void* src, size_t len, size_t dest_len) SOFTFP;

void* __memmove_chk(void* dest, const void* src, size_t len, size_t dest_len)
{
    LOAD_GLIBC_SYMBOL(__memmove_chk);

    return glibc___memmove_chk(dest, src, len, dest_len);
}

unsigned int alarm(unsigned int a) SOFTFP;

unsigned int alarm(unsigned int a)
{
    LOAD_GLIBC_SYMBOL(alarm);

    return glibc_alarm(a);
}

int recvfrom(int a, void* b, size_t c, unsigned int d, void* e, void* f) SOFTFP;

int recvfrom(int a, void* b, size_t c, unsigned int d, void* e, void* f)
{
    LOAD_GLIBC_SYMBOL(recvfrom);

    return glibc_recvfrom(a, b, c, d, e, f);
}

int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) SOFTFP;
int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    LOAD_GLIBC_SYMBOL(prctl);

    return glibc_prctl(option, arg2, arg3, arg4, arg5);
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

int sched_getscheduler(pid_t pid) SOFTFP;
int sched_getscheduler(pid_t pid)
{
    LOAD_GLIBC_SYMBOL(sched_getscheduler);

    return glibc_sched_getscheduler(pid);
}

int sched_setscheduler(pid_t pid, int policy, const void *param) SOFTFP;
int sched_setscheduler(pid_t pid, int policy, const void *param)
{
    LOAD_GLIBC_SYMBOL(sched_setscheduler);

    return glibc_sched_setscheduler(pid, policy, param);
}

long syscall(long a, ...) __attribute__((naked,noinline)) SOFTFP;

long syscall(long a, ...)
{
    __asm__ volatile("b glibc_syscall\n");
}

int getpagesize(void)
{
    LOAD_GLIBC_SYMBOL(getpagesize);

    return glibc_getpagesize();
}

int __aeabi_atexit(void *object, void (*destructor) (void *), void *dso_handle)
{
    LOAD_GLIBC_SYMBOL(__aeabi_atexit);

    return glibc___aeabi_atexit(object, destructor, dso_handle);
}

int bind(int socket, const void *address, unsigned int address_len)
{
    LOAD_GLIBC_SYMBOL(bind);

    return glibc_bind(socket, address, address_len);
}
