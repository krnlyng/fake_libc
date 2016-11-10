#ifndef __HELPERS_H__
#define __HELPERS_H__

#define NULL ((void*)0)

#define SOFTFP __attribute__((pcs("aapcs")))

#define STRINGIFY(x) #x

#define assert(x) do { if(!(x)) { glibc_fprintf(*glibc_stderr, "assertion failed: %s\n", STRINGIFY(x)); abort(); } } while(0);

#endif

