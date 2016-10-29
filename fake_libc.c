#include <dlfcn.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include "helpers.h"

#define NULL ((void*) 0)
#define SOFTFP __attribute__((pcs("aapcs")))

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
static int (*glibc_vfprintf)(void *fp, const char *fmt, va_list ap) = NULL;
static int (*glibc_fprintf)(void *stream, const char *format, ...) = NULL;
static _Unwind_Ptr (*glibc___gnu_Unwind_Find_exidx)(_Unwind_Ptr pc, int *pcount) = NULL;
static void (*glibc___cxa_finalize)(void *dso_handle) = NULL;
static int (*glibc___cxa_atexit)(void (*)(void *), void *, void *) = NULL;
static int (*glibc___register_atfork)(void (*prepare)(void), void (*parent)(void), void (*child)(void), void* dso) = NULL;
static double (*glibc_ldexp)(double, int) = NULL;
static void *(*glibc_memset)(void *s, int c, size_t n) = NULL;
static void *(*glibc___aeabi_memset)(void *s, int c, size_t n) = NULL;
static int (*glibc_isxdigit)(int c) = NULL;
static void (*glibc_abort)(void) = NULL;
static void *(*glibc_memcpy)(void *, void *, size_t) = NULL;
static int (*glibc_fputs)(const char *s, void *stream) = NULL;
static int (*glibc_fputc)(int c, void *stream) = NULL;
static int (*glibc_vasprintf)(char **strp, const char *fmt, va_list ap) = NULL;
static void *(*glibc_malloc)(size_t size) = NULL;
static size_t (*glibc___strlen_chk)(const char* s, size_t s_len) = NULL;
static void (*glibc_free)(void *ptr) = NULL;
static void *(*glibc_realloc)(void *ptr, size_t size) = NULL;
static void (*glibc___aeabi_memcpy)(void *dest, const void *src, size_t n) = NULL;
static void (*glibc___stack_chk_fail)(void) = NULL;
static int (*glibc_memcmp)(const void *s1, const void *s2, size_t n) = NULL;
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
static int (*glibc_pthread_once)(int *once_control, void (*init_routine)(void)) = NULL;
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
static size_t (*glibc_strftime_l)(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr,void *locale) = NULL;
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
//static long long (*glibc_wcstoll)(const wchar_t*, wchar_t**, int) = NULL;
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
float (*glibc_strtof)(const char*, char**) = NULL;
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
int (*glibc_atoi)(const char*a) = NULL;
long (*glibc_atol)(const char*a) = NULL;
long long (*glibc_atoll)(const char*a) = NULL;
int (*glibc_abs)(int a) = NULL;
long (*glibc_labs)(long a) = NULL;
long long (*glibc_llabs)(long long a) = NULL;
int (*glibc_system)(const char *string) = NULL;
void * (*glibc_bsearch)(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d)) = NULL;
void (*glibc_qsort)(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *)) = NULL;
uint32_t (*glibc_arc4random)(void) = NULL;
uint32_t (*glibc_arc4random_uniform)(uint32_t a) = NULL;
void (*glibc_arc4random_buf)(void* a, size_t b) = NULL;
int (*glibc_rand)(void) = NULL;
int (*glibc_rand_r)(unsigned int*a) = NULL;
void (*glibc_srand)(unsigned int a) = NULL;
double (*glibc_drand48)(void) = NULL;
double (*glibc_erand48)(unsigned short a[3]) = NULL;
long (*glibc_jrand48)(unsigned short a[3]) = NULL;
void (*glibc_lcong48)(unsigned short a[7]) = NULL;
long (*glibc_lrand48)(void) = NULL;
long (*glibc_mrand48)(void) = NULL;
long (*glibc_nrand48)(unsigned short a[3]) = NULL;
unsigned short* (*glibc_seed48)(unsigned short a[3]) = NULL;
void (*glibc_srand48)(long a) = NULL;
char* (*glibc_initstate)(unsigned int a, char*b, size_t c) = NULL;
long (*glibc_random)(void) = NULL;
char* (*glibc_setstate)(char*a) = NULL;
void (*glibc_srandom)(unsigned int a) = NULL;
int (*glibc_getpt)(void) = NULL;
int (*glibc_grantpt)(int a) = NULL;
int (*glibc_posix_openpt)(int a) = NULL;
char* (*glibc_ptsname)(int a) = NULL;
int (*glibc_ptsname_r)(int a, char*b, size_t c) = NULL;
int (*glibc_unlockpt)(int a) = NULL;


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
    if(glibc_ ## f == NULL) \
    { \
        glibc_ ## f = dlsym(glibc_handle, #f); \
        assert(glibc_ ## f != NULL); \
    }

BIONIC_FILE __sF[3];

BIONIC_FILE *stdin = &__sF[0];
BIONIC_FILE *stdout = &__sF[1];
BIONIC_FILE *stderr = &__sF[2];

volatile int *__errno(void)
{
    return &errno;
}

void *_get_actual_fp(BIONIC_FILE *fp);

int vfprintf(BIONIC_FILE *fp, const char *fmt, va_list ap) SOFTFP;

int vfprintf(BIONIC_FILE *fp, const char *fmt, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfprintf);

    return glibc_vfprintf(_get_actual_fp(fp), fmt, ap);
}

int fprintf(BIONIC_FILE *fp, const char *fmt, ...) SOFTFP;

int fprintf(BIONIC_FILE *fp, const char *fmt, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfprintf);

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

int putc(int c, BIONIC_FILE *stream) SOFTFP;

int putc(int c, BIONIC_FILE *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(putc);

    return glibc_putc(c, _get_actual_fp(stream));
}

int putchar(int c) SOFTFP;

int putchar(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(putchar);

    return glibc_putchar(c);
}

int puts(const char *s) SOFTFP;

int puts(const char *s)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(puts);

    return glibc_puts(s);
}

int fputs(const char *s, void *stream) SOFTFP;

int fputs(const char *s, void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fputs);

    return glibc_fputs(s, _get_actual_fp(stream));
}

int fputc(int c, void *stream) SOFTFP;

int fputc(int c, void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fputc);

    return glibc_fputc(c, _get_actual_fp(stream));
}

int vasprintf(char **strp, const char *fmt, va_list ap) SOFTFP;

int vasprintf(char **strp, const char *fmt, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vasprintf);

    return glibc_vasprintf(strp, fmt, ap);
}

void android_set_abort_message(const char* msg) SOFTFP;

void android_set_abort_message(const char* msg)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fprintf);

    glibc_fprintf(stderr, "stub: android_set_abort_message(%s)\n", msg);
}

void openlog(const char* log_tag, int options, int facility) SOFTFP;

void openlog(const char* log_tag, int options, int facility)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fprintf);

    glibc_fprintf(stderr, "stub: openlog(%s %d %d)\n", log_tag, options, facility);
}

void syslog(int priority, const char* fmt, ...) SOFTFP;

void syslog(int priority, const char* fmt, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fprintf);
    LOAD_GLIBC_FUNCTION(vfprintf);

    glibc_fprintf(stderr, "%d: ", priority);
    va_list args;
    va_start(args, fmt);
    glibc_vfprintf(stderr, fmt, args);
    va_end(args);
}

void closelog(void) SOFTFP;

void closelog()
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fprintf);

    glibc_fprintf(stderr, "stub: closelog()\n");
}

void *malloc(size_t size) SOFTFP;

void *malloc(size_t size)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(malloc);

    return glibc_malloc(size);
}

size_t __strlen_chk(const char* s, size_t s_len) SOFTFP;

size_t __strlen_chk(const char* s, size_t s_len)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__strlen_chk);

    return glibc___strlen_chk(s, s_len);
}

void free(void *ptr) SOFTFP;

void free(void *ptr)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(free);

    return glibc_free(ptr);
}

void *realloc(void *ptr, size_t size) SOFTFP;

void *realloc(void *ptr, size_t size)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(realloc);

    return glibc_realloc(ptr, size);
}

void __aeabi_memcpy(void *dest, const void *src, size_t n) SOFTFP;

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__aeabi_memcpy);

    return __aeabi_memcpy(dest, src, n);
}

void __stack_chk_fail(void) SOFTFP;

void __stack_chk_fail(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__stack_chk_fail);

    return glibc___stack_chk_fail();
}

int memcmp(const void *s1, const void *s2, size_t n) SOFTFP;

int memcmp(const void *s1, const void *s2, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(memcmp);

    return glibc_memcmp(s1, s2, n);
}

void *__aeabi_memmove(void *dest, const void *src, size_t n) SOFTFP;

void *__aeabi_memmove(void *dest, const void *src, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__aeabi_memmove);

    return glibc___aeabi_memmove(dest, src, n);
}

int isalnum(int c) SOFTFP;

int isalnum(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isalnum);

    return glibc_isalnum(c);
}

int isalpha(int c) SOFTFP;

int isalpha(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isalpha);

    return glibc_isalpha(c);
}

int iscntrl(int c) SOFTFP;

int iscntrl(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iscntrl);

    return glibc_iscntrl(c);
}

int isdigit(int c) SOFTFP;

int isdigit(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isdigit);

    return glibc_isdigit(c);
}

int isgraph(int c) SOFTFP;

int isgraph(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isgraph);

    return glibc_isgraph(c);
}

int islower(int c) SOFTFP;

int islower(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(islower);

    return glibc_islower(c);
}

int isprint(int c) SOFTFP;

int isprint(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isprint);

    return glibc_isprint(c);
}

int ispunct(int c) SOFTFP;

int ispunct(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ispunct);

    return glibc_ispunct(c);
}

int isspace(int c) SOFTFP;

int isspace(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isspace);

    return glibc_isspace(c);
}

int isupper(int c) SOFTFP;

int isupper(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isupper);

    return glibc_isupper(c);
}

int isxdigit(int c) SOFTFP;

int isxdigit(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isxdigit);

    return glibc_isxdigit(c);
}


int isascii(int c) SOFTFP;

int isascii(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isascii);

    return glibc_isascii(c);
}

int isblank(int c) SOFTFP;

int isblank(int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isblank);

    return glibc_isblank(c);
}

int isalnum_l(int c, void* locale) SOFTFP;

int isalnum_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isalnum_l);

    return glibc_isalnum_l(c, locale);
}

int isalpha_l(int c, void* locale) SOFTFP;

int isalpha_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isalpha_l);

    return glibc_isalpha_l(c, locale);
}

int isblank_l(int c, void* locale) SOFTFP;

int isblank_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isblank_l);

    return glibc_isblank_l(c, locale);
}

int iscntrl_l(int c, void* locale) SOFTFP;

int iscntrl_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iscntrl_l);

    return glibc_iscntrl_l(c, locale);
}

int isdigit_l(int c, void* locale) SOFTFP;

int isdigit_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isdigit_l);

    return glibc_isdigit_l(c, locale);
}

int isgraph_l(int c, void* locale) SOFTFP;

int isgraph_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isgraph_l);

    return glibc_isgraph_l(c, locale);
}

int islower_l(int c, void* locale) SOFTFP;

int islower_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(islower_l);

    return glibc_islower_l(c, locale);
}

int isprint_l(int c, void* locale) SOFTFP;

int isprint_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isprint_l);

    return glibc_isprint_l(c, locale);
}

int ispunct_l(int c, void* locale) SOFTFP;

int ispunct_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ispunct_l);

    return glibc_ispunct_l(c, locale);
}

int isspace_l(int c, void* locale) SOFTFP;

int isspace_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isspace_l);

    return glibc_isspace_l(c, locale);
}

int isupper_l(int c, void* locale) SOFTFP;

int isupper_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isupper_l);

    return glibc_isupper_l(c, locale);
}

int isxdigit_l(int c, void* locale) SOFTFP;

int isxdigit_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isxdigit_l);

    return glibc_isxdigit_l(c, locale);
}

int isascii_l(int c, void* locale) SOFTFP;

int isascii_l(int c, void* locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(isascii_l);

    return glibc_isascii_l(c, locale);
}

int __snprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...) SOFTFP;

int __snprintf_chk(char* dest, size_t supplied_size, int flags, size_t dest_len_from_compiler, const char* format, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__vsnprintf_chk);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__vsnprintf_chk);

    return glibc___vsnprintf_chk(dest, supplied_size, flags, dest_len_from_compiler, format, va);
}

void *__aeabi_memset(void *s, int c, size_t n) SOFTFP;

void *__aeabi_memset(void *s, int c, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__aeabi_memset);

    return glibc___aeabi_memset(s, c, n);
}

// PTHREAD TODO!

int pthread_mutex_lock(void *__mutex) SOFTFP;

int pthread_mutex_lock(void *__mutex)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_mutex_lock);

    return glibc_pthread_mutex_lock(__mutex);
}

int pthread_mutex_unlock(void *__mutex) SOFTFP;

int pthread_mutex_unlock(void *__mutex)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_mutex_unlock);

    return glibc_pthread_mutex_unlock(__mutex);
}

int pthread_setspecific(unsigned int key, const void *value) SOFTFP;

int pthread_setspecific(unsigned int key, const void *value)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_setspecific);

    return glibc_pthread_setspecific(key, value);
}

void *pthread_getspecific(unsigned int key) SOFTFP;

void *pthread_getspecific(unsigned int key)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_getspecific);

    return glibc_pthread_getspecific(key);
}

int pthread_once(int *once_control, void (*init_routine)(void)) SOFTFP;

int pthread_once(int *once_control, void (*init_routine)(void))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_once);

    return glibc_pthread_once(once_control, init_routine);
}

int pthread_key_create(unsigned int *key, void (*destructor)(void*)) SOFTFP;

int pthread_key_create(unsigned int *key, void (*destructor)(void*))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_key_create);

    return glibc_pthread_key_create(key, destructor);
}

int pthread_cond_timedwait(void *restrict cond, void *restrict mutex, void *restrict abstime) SOFTFP;

int pthread_cond_timedwait(void *restrict cond, void *restrict mutex, void *restrict abstime)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_timedwait);

    return glibc_pthread_cond_timedwait(cond, mutex, abstime);
}

int pthread_cond_wait(void *restrict cond, void *restrict mutex) SOFTFP;

int pthread_cond_wait(void *restrict cond, void *restrict mutex)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_wait);

    return glibc_pthread_cond_wait(cond, mutex);
}

int pthread_cond_broadcast(void *cond) SOFTFP;

int pthread_cond_broadcast(void *cond)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_broadcast);

    return glibc_pthread_cond_broadcast(cond);
}

int pthread_cond_signal(void *cond) SOFTFP;

int pthread_cond_signal(void *cond)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_signal);

    return glibc_pthread_cond_signal(cond);
}

int pthread_cond_destroy(void *cond) SOFTFP;

int pthread_cond_destroy(void *cond)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_destroy);

    return glibc_pthread_cond_destroy(cond);
}

int pthread_cond_init(void *restrict cond, const void *restrict attr) SOFTFP;

int pthread_cond_init(void *restrict cond, const void *restrict attr)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(pthread_cond_init);

    return glibc_pthread_cond_init(cond, attr);
}

void *calloc(size_t nmemb, size_t size) SOFTFP;

void *calloc(size_t nmemb, size_t size)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(calloc);

    return glibc_calloc(nmemb, size);
}

int __cxa_thread_atexit_impl(void (*fn)(void*), void* arg, void* dso_handle) SOFTFP;

int __cxa_thread_atexit_impl(void (*fn)(void*), void* arg, void* dso_handle)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__cxa_thread_atexit_impl);

    return glibc___cxa_thread_atexit_impl(fn, arg, dso_handle);
}

int gettimeofday(void *restrict tp, void *restrict tzp) SOFTFP;

int gettimeofday(void *restrict tp, void *restrict tzp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(gettimeofday);

    return glibc_gettimeofday(tp, tzp);
}

int clock_getres(int clk_id, void *res) SOFTFP;

int clock_getres(int clk_id, void *res)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(clock_getres);

    return glibc_clock_getres(clk_id, res);
}

int clock_gettime(int clk_id, void *tp) SOFTFP;

int clock_gettime(int clk_id, void *tp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(clock_gettime);

    return glibc_clock_gettime(clk_id, tp);
}

int clock_settime(int clk_id, const void *tp) SOFTFP;

int clock_settime(int clk_id, const void *tp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(clock_settime);

    return glibc_clock_settime(clk_id, tp);
}

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n) SOFTFP;

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmemcpy);

    return glibc_wmemcpy(dest, src, n);
}

int fgetc(void *stream) SOFTFP;

int fgetc(void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fgetc);

    return glibc_fgetc(_get_actual_fp(stream));
}

char *fgets(char *s, int size, void *stream) SOFTFP;

char *fgets(char *s, int size, void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fgets);

    return glibc_fgets(s, size, _get_actual_fp(stream));
}

int getc(void *stream) SOFTFP;

int getc(void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getc);

    return glibc_getc(_get_actual_fp(stream));
}

int getchar(void) SOFTFP;

int getchar(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getchar);

    return glibc_getchar();
}

int ungetc(int c, void *stream) SOFTFP;

int ungetc(int c, void *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ungetc);

    return glibc_ungetc(c, _get_actual_fp(stream));
}

size_t fread(void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream) SOFTFP;

size_t fread(void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fread);

    return glibc_fread(ptr, size, nmemb, _get_actual_fp(stream));
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream) SOFTFP;

size_t fwrite(const void *ptr, size_t size, size_t nmemb, BIONIC_FILE *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fwrite);

    return glibc_fwrite(ptr, size, nmemb, _get_actual_fp(stream));
}

int fflush(BIONIC_FILE *stream) SOFTFP;

int fflush(BIONIC_FILE *stream)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fflush);

    return fflush(_get_actual_fp(stream));
}

// TODO?
void *uselocale(void *newloc) SOFTFP;

void *uselocale(void *newloc)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(uselocale);

    return glibc_uselocale(newloc);
}

int scanf(const char *format, ...) SOFTFP;

int scanf(const char *format, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(sscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vscanf);

    return glibc_vscanf(format, ap);
}

int vsscanf(const char *str, const char *format, va_list ap) SOFTFP;

int vsscanf(const char *str, const char *format, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vsscanf);

    return glibc_vsscanf(str, format, ap);
}

int vfscanf(BIONIC_FILE *stream, const char *format, va_list ap) SOFTFP;

int vfscanf(BIONIC_FILE *stream, const char *format, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfscanf);

    return glibc_vfscanf(_get_actual_fp(stream), format, ap);
}

void *newlocale(int category_mask, const char *locale, void *base) SOFTFP;

void *newlocale(int category_mask, const char *locale, void *base)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(newlocale);

    return glibc_newlocale(category_mask, locale, base);
}

void freelocale(void *locobj) SOFTFP;

void freelocale(void *locobj)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(freelocale);

    return glibc_freelocale(locobj);
}

size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr) SOFTFP;

size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strftime);

    return glibc_strftime(s, maxsize, format, timeptr);
}

size_t strftime_l(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr,void *locale) SOFTFP;

size_t strftime_l(char *restrict s, size_t maxsize, const char *restrict format, const void *restrict timeptr,void *locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strftime_l);

    return glibc_strftime_l(s, maxsize, format, timeptr, locale);
}

__WINT_TYPE__ btowc(int a) SOFTFP;
__WINT_TYPE__ btowc(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(btowc);

    return glibc_btowc(a);
}


int fwprintf(BIONIC_FILE *fp, const wchar_t *format, ...) SOFTFP;
int fwprintf(BIONIC_FILE *fp, const wchar_t *format, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfwprintf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfwscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswalnum);

    return glibc_iswalnum(a);
}


int iswalpha(__WINT_TYPE__ a) SOFTFP;
int iswalpha(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswalpha);

    return glibc_iswalpha(a);
}


int iswblank(__WINT_TYPE__ a) SOFTFP;
int iswblank(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswblank);

    return glibc_iswblank(a);
}


int iswcntrl(__WINT_TYPE__ a) SOFTFP;
int iswcntrl(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswcntrl);

    return glibc_iswcntrl(a);
}


int iswdigit(__WINT_TYPE__ a) SOFTFP;
int iswdigit(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswdigit);

    return glibc_iswdigit(a);
}


int iswgraph(__WINT_TYPE__ a) SOFTFP;
int iswgraph(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswgraph);

    return glibc_iswgraph(a);
}


int iswlower(__WINT_TYPE__ a) SOFTFP;
int iswlower(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswlower);

    return glibc_iswlower(a);
}


int iswprint(__WINT_TYPE__ a) SOFTFP;
int iswprint(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswprint);

    return glibc_iswprint(a);
}


int iswpunct(__WINT_TYPE__ a) SOFTFP;
int iswpunct(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswpunct);

    return glibc_iswpunct(a);
}


int iswspace(__WINT_TYPE__ a) SOFTFP;
int iswspace(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswspace);

    return glibc_iswspace(a);
}


int iswupper(__WINT_TYPE__ a) SOFTFP;
int iswupper(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswupper);

    return glibc_iswupper(a);
}


int iswxdigit(__WINT_TYPE__ a) SOFTFP;
int iswxdigit(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswxdigit);

    return glibc_iswxdigit(a);
}


int iswctype(__WINT_TYPE__ a, wctype_t b) SOFTFP;
int iswctype(__WINT_TYPE__ a, wctype_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswctype);

    return glibc_iswctype(a, b);
}


__WINT_TYPE__ fgetwc(BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ fgetwc(BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fgetwc);

    return glibc_fgetwc(_get_actual_fp(fp));
}


wchar_t *fgetws(wchar_t *a, int b, BIONIC_FILE *fp) SOFTFP;
wchar_t *fgetws(wchar_t *a, int b, BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fgetws);

    return glibc_fgetws(a, b, _get_actual_fp(fp));
}


__WINT_TYPE__ fputwc(wchar_t a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ fputwc(wchar_t a, BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fputwc);

    return glibc_fputwc(a, _get_actual_fp(fp));
}


int fputws(const wchar_t *a, BIONIC_FILE *fp) SOFTFP;
int fputws(const wchar_t *a, BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fputws);

    return glibc_fputws(a, _get_actual_fp(fp));
}


int fwide(BIONIC_FILE *fp, int a) SOFTFP;
int fwide(BIONIC_FILE *fp, int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(fwide);

    return glibc_fwide(_get_actual_fp(fp), a);
}


__WINT_TYPE__ getwc(BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ getwc(BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getwc);

    return glibc_getwc(_get_actual_fp(fp));
}


__WINT_TYPE__ getwchar(void) SOFTFP;
__WINT_TYPE__ getwchar(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getwchar);

    return glibc_getwchar();
}


int mbsinit(const mbstate_t *a) SOFTFP;
int mbsinit(const mbstate_t *a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbsinit);

    return glibc_mbsinit(a);
}


size_t mbrlen(const char *a, size_t b, mbstate_t *c) SOFTFP;
size_t mbrlen(const char *a, size_t b, mbstate_t *c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbrlen);

    return glibc_mbrlen(a, b, c);
}


size_t mbrtowc(wchar_t *a, const char *b, size_t c, mbstate_t *d) SOFTFP;
size_t mbrtowc(wchar_t *a, const char *b, size_t c, mbstate_t *d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbrtowc);

    return glibc_mbrtowc(a, b, c, d);
}


size_t mbsrtowcs(wchar_t* a, const char** b, size_t c, mbstate_t* d) SOFTFP;
size_t mbsrtowcs(wchar_t* a, const char** b, size_t c, mbstate_t* d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbsrtowcs);

    return glibc_mbsrtowcs(a, b, c, d);
}


size_t mbsnrtowcs(wchar_t*a, const char**b, size_t c, size_t d, mbstate_t*e) SOFTFP;
size_t mbsnrtowcs(wchar_t*a, const char**b, size_t c, size_t d, mbstate_t*e)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbsnrtowcs);

    return glibc_mbsnrtowcs(a, b, c, d, e);
}


size_t mbstowcs(wchar_t *a, const char *b, size_t c) SOFTFP;
size_t mbstowcs(wchar_t *a, const char *b, size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbstowcs);

    return glibc_mbstowcs(a, b, c);
}


__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ putwc(wchar_t a, BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(putwc);

    return glibc_putwc(a, _get_actual_fp(fp));
}


__WINT_TYPE__ putwchar(wchar_t a) SOFTFP;
__WINT_TYPE__ putwchar(wchar_t a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(putwchar);

    return glibc_putwchar(a);
}


int swprintf(wchar_t * a, size_t b, const wchar_t *c, ...) SOFTFP;
int swprintf(wchar_t * a, size_t b, const wchar_t *c, ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vswprintf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vswscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(towlower);

    return glibc_towlower(a);
}


__WINT_TYPE__ towupper(__WINT_TYPE__ a) SOFTFP;
__WINT_TYPE__ towupper(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(towupper);

    return glibc_towupper(a);
}


__WINT_TYPE__ ungetwc(__WINT_TYPE__ a, BIONIC_FILE *fp) SOFTFP;
__WINT_TYPE__ ungetwc(__WINT_TYPE__ a, BIONIC_FILE *fp)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ungetwc);

    return glibc_ungetwc(a, _get_actual_fp(fp));
}


int vfwprintf(BIONIC_FILE*a, const wchar_t*b, va_list ap) SOFTFP;
int vfwprintf(BIONIC_FILE*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfwprintf);

    return glibc_vfwprintf(a, b, ap);
}


int vfwscanf(BIONIC_FILE*a, const wchar_t*b, va_list ap) SOFTFP;
int vfwscanf(BIONIC_FILE*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vfwscanf);

    return glibc_vfwscanf(a, b, ap);
}


int vswprintf(wchar_t* a, size_t b, const wchar_t* c, va_list ap) SOFTFP;
int vswprintf(wchar_t* a, size_t b, const wchar_t* c, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vswprintf);

    return glibc_vswprintf(a, b, c, ap);
}


int vswscanf(const wchar_t*a, const wchar_t*b, va_list ap) SOFTFP;
int vswscanf(const wchar_t*a, const wchar_t*b, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vswscanf);

    return glibc_vswscanf(a, b, ap);
}


int vwprintf(const wchar_t*a, va_list ap) SOFTFP;
int vwprintf(const wchar_t*a, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vwprintf);

    return glibc_vwprintf(a, ap);
}


int vwscanf(const wchar_t*a, va_list ap) SOFTFP;
int vwscanf(const wchar_t*a, va_list ap)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vwscanf);

    return glibc_vwscanf(a, ap);
}


wchar_t* wcpcpy (wchar_t*a, const wchar_t *b) SOFTFP;
wchar_t* wcpcpy (wchar_t*a, const wchar_t *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcpcpy);

    return glibc_wcpcpy(a, b);
}


wchar_t* wcpncpy (wchar_t*a, const wchar_t *b, size_t c) SOFTFP;
wchar_t* wcpncpy (wchar_t*a, const wchar_t *b, size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcpncpy);

    return glibc_wcpncpy(a, b, c);
}


size_t wcrtomb(char *a, wchar_t b, mbstate_t *c) SOFTFP;
size_t wcrtomb(char *a, wchar_t b, mbstate_t *c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcrtomb);

    return glibc_wcrtomb(a, b, c);
}


int wcscasecmp(const wchar_t *a, const wchar_t *b) SOFTFP;
int wcscasecmp(const wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscasecmp);

    return glibc_wcscasecmp(a, b);
}


int wcscasecmp_l(const wchar_t *a, const wchar_t *b, void *c) SOFTFP;
int wcscasecmp_l(const wchar_t *a, const wchar_t *b, void *c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscasecmp_l);

    return glibc_wcscasecmp_l(a, b, c);
}


wchar_t *wcscat(wchar_t *a, const wchar_t *b) SOFTFP;
wchar_t *wcscat(wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscat);

    return glibc_wcscat(a, b);
}


wchar_t *wcschr(const wchar_t *a, wchar_t b) SOFTFP;
wchar_t *wcschr(const wchar_t *a, wchar_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcschr);

    return glibc_wcschr(a, b);
}


int wcscmp(const wchar_t *a, const wchar_t *b) SOFTFP;
int wcscmp(const wchar_t *a, const wchar_t *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscmp);

    return glibc_wcscmp(a, b);
}

long long wcstoll_l(const wchar_t *a, wchar_t **b, int c, void*d) SOFTFP;
long long wcstoll_l(const wchar_t *a, wchar_t **b, int c, void*d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstoll_l);

    return glibc_wcstoll_l(a, b, c, d);
}

size_t wcsxfrm_l(wchar_t *a, const wchar_t *b, size_t c, void*d) SOFTFP;
size_t wcsxfrm_l(wchar_t *a, const wchar_t *b, size_t c, void*d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsxfrm_l);

    return glibc_wcsxfrm_l(a, b, c, d);
}

void *localeconv(void) SOFTFP;
void *localeconv(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(localeconv);

    return glibc_localeconv();
}

char *setlocale(int category, const char *locale)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(setlocale);

    return setlocale(category, locale);
}

int strcoll_l(const char *a, const char *b, void *c) SOFTFP;

int strcoll_l(const char *a, const char *b, void *c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strcoll_l);

    return strcoll_l(a, b, c);
}

size_t strxfrm_l(char* restrict a, const char* restrict b, size_t c, void *d) SOFTFP;

size_t strxfrm_l(char* restrict a, const char* restrict b, size_t c, void *d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strxfrm_l);

    return glibc_strxfrm_l(a, b, c, d);
}

int toupper_l(int a, void *b) SOFTFP;
int toupper_l(int a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(toupper_l);

    return glibc_toupper_l(a, b);
}

int tolower_l(int a, void *b) SOFTFP;
int tolower_l(int a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(tolower_l);

    return glibc_tolower_l(a, b);
}

int iswalnum_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswalnum_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswalnum_l);

    return glibc_iswalnum_l(a, b);
}


int iswalpha_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswalpha_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswalpha_l);

    return glibc_iswalpha_l(a, b);
}


int iswblank_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswblank_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswblank_l);

    return glibc_iswblank_l(a, b);
}


int iswcntrl_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswcntrl_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswcntrl_l);

    return glibc_iswcntrl_l(a, b);
}


int iswdigit_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswdigit_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswdigit_l);

    return glibc_iswdigit_l(a, b);
}


int iswgraph_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswgraph_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswgraph_l);

    return glibc_iswgraph_l(a, b);
}


int iswlower_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswlower_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswlower_l);

    return glibc_iswlower_l(a, b);
}


int iswprint_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswprint_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswprint_l);

    return glibc_iswprint_l(a, b);
}


int iswpunct_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswpunct_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswpunct_l);

    return glibc_iswpunct_l(a, b);
}


int iswspace_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswspace_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswspace_l);

    return glibc_iswspace_l(a, b);
}


int iswupper_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswupper_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswupper_l);

    return glibc_iswupper_l(a, b);
}


int iswxdigit_l(__WINT_TYPE__ a, void *b) SOFTFP;

int iswxdigit_l(__WINT_TYPE__ a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswxdigit_l);

    return glibc_iswxdigit_l(a, b);
}


int towlower_l(int a, void *b) SOFTFP;

int towlower_l(int a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(towlower_l);

    return glibc_towlower_l(a, b);
}


int towupper_l(int a, void *b) SOFTFP;

int towupper_l(int a, void *b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(towupper_l);

    return glibc_towupper_l(a, b);
}


int iswctype_l(__WINT_TYPE__ a, wctype_t b, void *c) SOFTFP;

int iswctype_l(__WINT_TYPE__ a, wctype_t b, void *c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(iswctype_l);

    return glibc_iswctype_l(a, b, c);
}

wchar_t *wcscpy(wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcscpy(wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscpy);

    return glibc_wcscpy(a, b);
}


size_t wcscspn(const wchar_t * a,  const wchar_t * b) SOFTFP;
size_t wcscspn(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscspn);

    return glibc_wcscspn(a, b);
}


size_t wcsftime(wchar_t * a,  size_t b,   const wchar_t * c,    const void * d) SOFTFP;
size_t wcsftime(wchar_t * a,  size_t b,   const wchar_t * c,    const void * d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsftime);

    return wcsftime(a, b, c, d);
}

size_t wcslen(const wchar_t * a) SOFTFP;
size_t wcslen(const wchar_t * a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcslen);

    return glibc_wcslen(a);
}


int wcsncasecmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wcsncasecmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsncasecmp);

    return glibc_wcsncasecmp(a, b, c);
}


int wcsncasecmp_l(const wchar_t * a,  const wchar_t * b,   size_t c,    void* d) SOFTFP;
int wcsncasecmp_l(const wchar_t * a,  const wchar_t * b,   size_t c,    void* d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsncasecmp_l);

    return glibc_wcsncasecmp_l(a, b, c, d);
}


wchar_t *wcsncat(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wcsncat(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsncat);

    return glibc_wcsncat(a, b, c);
}


int wcsncmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wcsncmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsncmp);

    return glibc_wcsncmp(a, b, c);
}


wchar_t *wcsncpy(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wcsncpy(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsncpy);

    return glibc_wcsncpy(a, b, c);
}


size_t wcsnrtombs(char*a, const wchar_t** b,  size_t c,   size_t d,    mbstate_t* e) SOFTFP;
size_t wcsnrtombs(char*a, const wchar_t** b,  size_t c,   size_t d,    mbstate_t* e)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsnrtombs);

    return glibc_wcsnrtombs(a, b, c, d, e);
}


wchar_t *wcspbrk(const wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcspbrk(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcspbrk);

    return glibc_wcspbrk(a, b);
}


wchar_t *wcsrchr(const wchar_t * a,  wchar_t b) SOFTFP;
wchar_t *wcsrchr(const wchar_t * a,  wchar_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsrchr);

    return glibc_wcsrchr(a, b);
}


size_t wcsrtombs(char* a,  const wchar_t** b,   size_t c,    mbstate_t* d) SOFTFP;
size_t wcsrtombs(char* a,  const wchar_t** b,   size_t c,    mbstate_t* d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsrtombs);

    return glibc_wcsrtombs(a, b, c, d);
}


size_t wcsspn(const wchar_t * a,  const wchar_t * b) SOFTFP;
size_t wcsspn(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsspn);

    return glibc_wcsspn(a, b);
}


wchar_t *wcsstr(const wchar_t * a,  const wchar_t * b) SOFTFP;
wchar_t *wcsstr(const wchar_t * a,  const wchar_t * b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsstr);

    return glibc_wcsstr(a, b);
}


double wcstod(const wchar_t* a,  wchar_t** b) SOFTFP;
double wcstod(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstod);

    return glibc_wcstod(a, b);
}


float wcstof(const wchar_t* a,  wchar_t** b) SOFTFP;
float wcstof(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstof);

    return glibc_wcstof(a, b);
}


wchar_t* wcstok(wchar_t* a,  const wchar_t* b,   wchar_t** c) SOFTFP;
wchar_t* wcstok(wchar_t* a,  const wchar_t* b,   wchar_t** c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstok);

    return glibc_wcstok(a, b, c);
}


long wcstol(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
long wcstol(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstol);

    return glibc_wcstol(a, b, c);
}


long double wcstold(const wchar_t* a,  wchar_t** b) SOFTFP;
long double wcstold(const wchar_t* a,  wchar_t** b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstold);

    return glibc_wcstold(a, b);
}


unsigned long wcstoul(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
unsigned long wcstoul(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstoul);

    return glibc_wcstoul(a, b, c);
}


unsigned long long wcstoull(const wchar_t* a,  wchar_t** b,   int c) SOFTFP;
unsigned long long wcstoull(const wchar_t* a,  wchar_t** b,   int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstoull);

    return glibc_wcstoull(a, b, c);
}


int wcswidth(const wchar_t * a,  size_t b) SOFTFP;
int wcswidth(const wchar_t * a,  size_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcswidth);

    return glibc_wcswidth(a, b);
}


size_t wcsxfrm(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
size_t wcsxfrm(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcsxfrm);

    return glibc_wcsxfrm(a, b, c);
}


int wctob(__WINT_TYPE__ a) SOFTFP;
int wctob(__WINT_TYPE__ a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wctob);

    return glibc_wctob(a);
}


wctype_t wctype(const char * a) SOFTFP;
wctype_t wctype(const char * a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wctype);

    return glibc_wctype(a);
}


int wcwidth(wchar_t a) SOFTFP;
int wcwidth(wchar_t a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcwidth);

    return glibc_wcwidth(a);
}


wchar_t *wmemchr(const wchar_t * a,  wchar_t b,   size_t c) SOFTFP;
wchar_t *wmemchr(const wchar_t * a,  wchar_t b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmemchr);

    return glibc_wmemchr(a, b, c);
}


int wmemcmp(const wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
int wmemcmp(const wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmemcmp);

    return glibc_wmemcmp(a, b, c);
}


wchar_t *wmempcpy(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wmempcpy(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmempcpy);

    return glibc_wmempcpy(a, b, c);
}

wchar_t *wmemmove(wchar_t * a,  const wchar_t * b,   size_t c) SOFTFP;
wchar_t *wmemmove(wchar_t * a,  const wchar_t * b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmemmove);

    return glibc_wmemmove(a, b, c);
}


wchar_t *wmemset(wchar_t * a,  wchar_t b,   size_t c) SOFTFP;
wchar_t *wmemset(wchar_t * a,  wchar_t b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wmemset);

    return glibc_wmemset(a, b, c);
}


int wprintf(const wchar_t * a,  ...) SOFTFP;
int wprintf(const wchar_t * a,  ...)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vwprintf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(vwscanf);

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
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstoull_l);

    return glibc_wcstoull_l(a, b, c, d);
}


long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c) SOFTFP;
long double wcstold_l(const wchar_t * a,  wchar_t ** b,   void*  c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcstold_l);

    return glibc_wcstold_l(a, b, c);
}



int wcscoll_l(const wchar_t * a,  const wchar_t * b,   void* c) SOFTFP;
int wcscoll_l(const wchar_t * a,  const wchar_t * b,   void* c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcscoll_l);

    return glibc_wcscoll_l(a, b, c);
}

size_t wcslcat(wchar_t* a,  const wchar_t* b,   size_t c) SOFTFP;
size_t wcslcat(wchar_t* a,  const wchar_t* b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcslcat);

    return glibc_wcslcat(a, b, c);
}


size_t wcslcpy(wchar_t* a,  const wchar_t* b,   size_t c) SOFTFP;
size_t wcslcpy(wchar_t* a,  const wchar_t* b,   size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wcslcpy);

    return glibc_wcslcpy(a, b, c);
}



typedef void *wctrans_t;
__WINT_TYPE__ towctrans(__WINT_TYPE__ a,  wctrans_t b) SOFTFP;
__WINT_TYPE__ towctrans(__WINT_TYPE__ a,  wctrans_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(towctrans);

    return glibc_towctrans(a, b);
}


wctrans_t wctrans(const char* a) SOFTFP;
wctrans_t wctrans(const char* a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(wctrans);

    return glibc_wctrans(a);
}

int mblen(const char *a, size_t b) SOFTFP;
int mblen(const char *a, size_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mblen);

    return glibc_mblen(a, b);
}


int mbtowc(wchar_t *a, const char *b, size_t c) SOFTFP;
int mbtowc(wchar_t *a, const char *b, size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mbtowc);

    return glibc_mbtowc(a, b, c);
}

size_t __ctype_get_mb_cur_max(void) SOFTFP;
size_t __ctype_get_mb_cur_max(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__ctype_get_mb_cur_max);
    return glibc___ctype_get_mb_cur_max();
}

int strcmp(const char *s1, const char *s2) SOFTFP;
int strcmp(const char *s1, const char *s2)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strcmp);

    return glibc_strcmp(s1, s2);
}

void exit(int a) SOFTFP;

void exit(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(exit);

    glibc_exit(a);
    for(;;);
}


char* mkdtemp(char*a) SOFTFP;

char* mkdtemp(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkdtemp);

    return glibc_mkdtemp(a);
}

char* mktemp(char*a) SOFTFP;

char* mktemp(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mktemp);

    return glibc_mktemp(a);
}


int mkostemp64(char*a, int b) SOFTFP;

int mkostemp64(char*a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkostemp64);

    return glibc_mkostemp64(a, b);
}

int mkostemp(char*a, int b) SOFTFP;

int mkostemp(char*a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkostemp);

    return glibc_mkostemp(a, b);
}

int mkostemps64(char*a, int b, int c) SOFTFP;

int mkostemps64(char*a, int b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkostemps64);

    return glibc_mkostemps64(a, b, c);
}

int mkostemps(char*a, int b, int c) SOFTFP;

int mkostemps(char*a, int b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkostemps);

    return glibc_mkostemps(a, b, c);
}

int mkstemp64(char*a) SOFTFP;

int mkstemp64(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkstemp64);

    return glibc_mkstemp64(a);
}

int mkstemp(char*a) SOFTFP;

int mkstemp(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkstemp);

    return glibc_mkstemp(a);
}

int mkstemps64(char*a, int b) SOFTFP;

int mkstemps64(char*a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkstemps64);

    return glibc_mkstemps64(a, b);
}

int mkstemps(char*a, int b) SOFTFP;

int mkstemps(char*a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mkstemps);

    return glibc_mkstemps(a, b);
}


long strtol(const char *a, char **b, int c) SOFTFP;

long strtol(const char *a, char **b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtol);

    return glibc_strtol(a, b, c);
}

long long strtoll(const char *a, char **b, int c) SOFTFP;

long long strtoll(const char *a, char **b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtoll);

    return glibc_strtoll(a, b, c);
}

unsigned long strtoul(const char *a, char **b, int c) SOFTFP;

unsigned long strtoul(const char *a, char **b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtoul);

    return glibc_strtoul(a, b, c);
}

unsigned long long strtoull(const char *a, char **b, int c) SOFTFP;

unsigned long long strtoull(const char *a, char **b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtoull);

    return glibc_strtoull(a, b, c);
}


int posix_memalign(void **memptr, size_t alignment, size_t size) SOFTFP;

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(posix_memalign);

    return glibc_posix_memalign(memptr, alignment, size);
}


double atof(const char*a) SOFTFP;
double atof(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(atof);

    return glibc_atof(a);
}

double strtod(const char*a, char**b) SOFTFP;

double strtod(const char*a, char**b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtod);

    return glibc_strtod(a, b);
}

long double strtold(const char*a, char**b) SOFTFP;

long double strtold(const char*a, char**b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtold);

    return glibc_strtold(a, b);
}


long double strtold_l(const char *a, char **b, void*c) SOFTFP;

long double strtold_l(const char *a, char **b, void*c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtold_l);

    return glibc_strtold_l(a, b, c);
}

long long strtoll_l(const char *a, char **b, int c, void*d) SOFTFP;

long long strtoll_l(const char *a, char **b, int c, void*d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtoll_l);

    return glibc_strtoll_l(a, b, c, d);
}

unsigned long long strtoull_l(const char *a, char **b, int c, void*d) SOFTFP;

unsigned long long strtoull_l(const char *a, char **b, int c, void*d)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(strtoull_l);

    return glibc_strtoull_l(a, b, c, d);
}


void _Exit(int a) SOFTFP;

void _Exit(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(_Exit);

    glibc__Exit(a);
    for(;;); // noreturn
}

int atexit(void (*a)(void)) SOFTFP;

int atexit(void (*a)(void))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(atexit);

    return glibc_atexit(a);
}


int at_quick_exit(void (*a)(void)) SOFTFP;

int at_quick_exit(void (*a)(void))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(__cxa_at_quick_exit);

    glibc___cxa_at_quick_exit(a);
    for(;;); // noreturn
}

char* getenv(const char* a) SOFTFP;

char* getenv(const char* a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getenv);

    return glibc_getenv(a);
}

int putenv(char*a) SOFTFP;

int putenv(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(putenv);

    return glibc_putenv(a);
}

int setenv(const char*a, const char*b, int c) SOFTFP;

int setenv(const char*a, const char*b, int c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(setenv);

    return glibc_setenv(a, b, c);
}

int unsetenv(const char*a) SOFTFP;

int unsetenv(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(unsetenv);

    return glibc_unsetenv(a);
}

int clearenv(void) SOFTFP;

int clearenv(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(clearenv);

    return glibc_clearenv();
}


int atoi(const char*a) SOFTFP;

int atoi(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(atoi);

    return glibc_atoi(a);
}

long atol(const char*a) SOFTFP;

long atol(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(atol);

    return glibc_atol(a);
}

long long atoll(const char*a) SOFTFP;

long long atoll(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(atoll);

    return glibc_atoll(a);
}


int abs(int a) SOFTFP;

int abs(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(abs);

    return glibc_abs(a);
}

long labs(long a) SOFTFP;

long labs(long a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(labs);

    return glibc_labs(a);
}

long long llabs(long long a) SOFTFP;

long long llabs(long long a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(llabs);

    return glibc_llabs(a);
}


char * realpath(const char *path, char *resolved) SOFTFP;

char * realpath(const char *path, char *resolved)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(realpath);

    return glibc_realpath(path, resolved);
}

int system(const char *string) SOFTFP;

int system(const char *string)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(system);

    return glibc_system(string);
}


void * bsearch(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d)) SOFTFP;

void * bsearch(const void *key, const void *base0, size_t nmemb,  size_t size,  int (*compar)(const void * c,  const void * d))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(bsearch);

    return glibc_bsearch(key, base0, nmemb, size, compar);
}


void qsort(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *)) SOFTFP;

void qsort(void *a, size_t b,  size_t c,  int (*d)(const void *,  const void *))
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(qsort);

    return glibc_qsort(a, b, c, d);
}


uint32_t arc4random(void) SOFTFP;

uint32_t arc4random(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(arc4random);

    return glibc_arc4random();
}

uint32_t arc4random_uniform(uint32_t a) SOFTFP;

uint32_t arc4random_uniform(uint32_t a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(arc4random_uniform);

    return glibc_arc4random_uniform(a);
}

void arc4random_buf(void* a, size_t b) SOFTFP;

void arc4random_buf(void* a, size_t b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(arc4random_buf);

    return glibc_arc4random_buf(a, b);
}


int rand(void) SOFTFP;

int rand(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(rand);

    return glibc_rand();
}

int rand_r(unsigned int*a) SOFTFP;

int rand_r(unsigned int*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(rand_r);

    return glibc_rand_r(a);
}

void srand(unsigned int a) SOFTFP;

void srand(unsigned int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(srand);

    return glibc_srand(a);
}


double drand48(void) SOFTFP;

double drand48(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(drand48);

    return glibc_drand48();
}

double erand48(unsigned short a[3]) SOFTFP;

double erand48(unsigned short a[3])
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(erand48);

    return glibc_erand48(a);
}

long jrand48(unsigned short a[3]) SOFTFP;

long jrand48(unsigned short a[3])
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(jrand48);

    return glibc_jrand48(a);
}

void lcong48(unsigned short a[7]) SOFTFP;

void lcong48(unsigned short a[7])
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(lcong48);

    return glibc_lcong48(a);
}

long lrand48(void) SOFTFP;

long lrand48(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(lrand48);

    return glibc_lrand48();
}

long mrand48(void) SOFTFP;

long mrand48(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(mrand48);

    return glibc_mrand48();
}

long nrand48(unsigned short a[3]) SOFTFP;

long nrand48(unsigned short a[3])
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(nrand48);

    return glibc_nrand48(a);
}

unsigned short* seed48(unsigned short a[3]) SOFTFP;

unsigned short* seed48(unsigned short a[3])
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(seed48);

    return glibc_seed48(a);
}

void srand48(long a) SOFTFP;

void srand48(long a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(srand48);

    return glibc_srand48(a);
}


char* initstate(unsigned int a, char*b, size_t c) SOFTFP;

char* initstate(unsigned int a, char*b, size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(initstate);

    return glibc_initstate(a, b, c);
}

long random(void) SOFTFP;

long random(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(random);

    return glibc_random();
}

char* setstate(char*a) SOFTFP;

char* setstate(char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(setstate);

    return glibc_setstate(a);
}

void srandom(unsigned int a) SOFTFP;

void srandom(unsigned int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(srandom);

    return glibc_srandom(a);
}


int getpt(void) SOFTFP;

int getpt(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getpt);

    return glibc_getpt();
}

int grantpt(int a) SOFTFP;

int grantpt(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(grantpt);

    return glibc_grantpt(a);
}

int posix_openpt(int a) SOFTFP;

int posix_openpt(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(posix_openpt);

    return glibc_posix_openpt(a);
}

char* ptsname(int a) SOFTFP;

char* ptsname(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ptsname);

    return glibc_ptsname(a);
}

int ptsname_r(int a, char*b, size_t c) SOFTFP;

int ptsname_r(int a, char*b, size_t c)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ptsname_r);

    return glibc_ptsname_r(a, b, c);
}

int unlockpt(int a) SOFTFP;

int unlockpt(int a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(unlockpt);

    return glibc_unlockpt(a);
}

div_t div(int a, int b) SOFTFP;

div_t div(int a, int b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(div);

    return glibc_div(a, b);
}

ldiv_t ldiv(long a, long b) SOFTFP;

ldiv_t ldiv(long a, long b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(ldiv);

    return glibc_ldiv(a, b);
}

lldiv_t lldiv(long long a, long long b) SOFTFP;
lldiv_t lldiv(long long a, long long b)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(lldiv);

    return glibc_lldiv(a, b);
}

const char* getprogname(void) SOFTFP;

const char* getprogname(void)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(getprogname);

    return glibc_getprogname();
}

void setprogname(const char*a) SOFTFP;

void setprogname(const char*a)
{
    LOAD_GLIBC();
    LOAD_GLIBC_FUNCTION(setprogname);

    return glibc_setprogname(a);
}
