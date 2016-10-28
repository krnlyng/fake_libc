#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <sys/types.h>

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

#endif

