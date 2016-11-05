
#define _POSIX_C_SOURCE 201112L
#include <dlfcn.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "helpers.h"

extern int (*glibc_readdir_r)(void *dir, struct dirent *entry, struct dirent **result);
extern void *(*glibc_readdir)(void *dir);
extern void *(*glibc_memcpy)(void *a, void *b, size_t s);
extern void (*glibc_free)(void *a);
extern void *(*glibc_malloc)(void *a);

struct bionic_dirent {
    uint64_t         d_ino;
    int64_t          d_off;
    unsigned short   d_reclen;
    unsigned char    d_type;
    char             d_name[256];
};

int my_readdir_r(DIR *dir, struct bionic_dirent *entry,
        struct bionic_dirent **result)
{
    struct dirent entry_r;
    struct dirent *result_r;

    int res = glibc_readdir_r(dir, &entry_r, &result_r);

    if (res == 0) {
        if (result_r != NULL) {
            *result = entry;

            entry->d_ino = entry_r.d_ino;
            entry->d_off = entry_r.d_off;
            entry->d_reclen = entry_r.d_reclen;
            entry->d_type = entry_r.d_type;
            glibc_memcpy(entry->d_name, entry_r.d_name, sizeof(entry->d_name));

            // Make sure the string is zero-terminated, even if cut off (which
            // shouldn't happen, as both bionic and glibc have d_name defined
            // as fixed array of 256 chars)
            entry->d_name[sizeof(entry->d_name) - 1] = '\0';
        } else {
            *result = NULL;
        }
    }

    return res;
}

struct bionic_dirent *my_readdir(DIR *dirp)
{
    /**
     * readdir(3) manpage says:
     *  The data returned by readdir() may be overwritten by subsequent calls  
     *  to readdir() for the same directory stream.
     *
     * XXX: At the moment, for us, the data will be overwritten even by
     * subsequent calls to /different/ directory streams. Eventually fix that  
     * (e.g. by storing per-DIR * bionic_dirent structs, and removing them on  
     * closedir, requires hooking of all funcs returning/taking DIR *) and     
     * handling the additional data attachment there)
     **/

    static struct bionic_dirent result;

    struct dirent *real_result = glibc_readdir(dirp);
    if (!real_result) {
        return NULL;
    }

    result.d_ino = real_result->d_ino;
    result.d_off = real_result->d_off;
    result.d_reclen = real_result->d_reclen;
    result.d_type = real_result->d_type;
    glibc_memcpy(result.d_name, real_result->d_name, sizeof(result.d_name));

    // Make sure the string is zero-terminated, even if cut off (which
    // shouldn't happen, as both bionic and glibc have d_name defined
    // as fixed array of 256 chars)
    result.d_name[sizeof(result.d_name)-1] = '\0';
    return &result;
}

static inline void swap(void **a, void **b)
{
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

extern int (*glibc_getaddrinfo)(const char *hostname, const char *servname,
    const void *hints, void **res);
extern int (*glibc_freeaddrinfo)(void *__ai);

int my_getaddrinfo(const char *hostname, const char *servname,
    const struct addrinfo *hints, struct addrinfo **res)
{
    // make a local copy of hints
    struct addrinfo *fixed_hints = (struct addrinfo*)glibc_malloc(sizeof(struct addrinfo));
    glibc_memcpy(fixed_hints, hints, sizeof(struct addrinfo));
    // fix bionic -> glibc missmatch
    swap((void**)&(fixed_hints->ai_canonname), (void**)&(fixed_hints->ai_addr));
    // do glibc getaddrinfo
    int result = glibc_getaddrinfo(hostname, servname, fixed_hints, res);
    // release the copy of hints
    glibc_free(fixed_hints);
    // fix bionic <- glibc missmatch
    struct addrinfo *it = *res;
    while(NULL != it)
    {
        swap((void**)&(it->ai_canonname), (void**)&(it->ai_addr));
        it = it->ai_next;
    }
    return result;
}

void my_freeaddrinfo(struct addrinfo *__ai)
{
    swap((void**)&(__ai->ai_canonname), (void**)&(__ai->ai_addr));
    glibc_freeaddrinfo(__ai);
}

