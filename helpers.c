#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "helpers.h"

extern BIONIC_FILE __sF[3];

void *_get_actual_fp(BIONIC_FILE *fp)
{
    if (fp == &__sF[0])
        return stdin;
    else if (fp == &__sF[1])
        return stdout;
    else if (fp == &__sF[2])
        return stderr;

    return (FILE*)fp;
}

