/*
 * Copyright (c) 2012 Carsten Munk <carsten.munk@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* EGL function pointers */
#define EGL_EGLEXT_PROTOTYPES
/* For RTLD_DEFAULT */
#define _GNU_SOURCE
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "ws.h"
#include "helper.h"
#include <assert.h>


#include <string.h>

#include <system/window.h>
#include "logging.h"

#define FP_ATTRIB __attribute__((pcs("aapcs")))

#define HYBRIS_DLSYSM(name, fptr, sym) \
    if (!name##_handle) \
        hybris_##name##_initialize(); \
    if (*(fptr) == NULL) \
    { \
        *(fptr) = (void *) dlsym(name##_handle, sym); \
    }

#define HYBRIS_IMPLEMENT_FUNCTION0(name, return_type, symbol) \
    return_type symbol() \
    { \
        static return_type (*f)() FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION1(name, return_type, symbol, a1) \
    return_type symbol(a1 n1) \
    { \
        static return_type (*f)(a1) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION2(name, return_type, symbol, a1, a2) \
    return_type symbol(a1 n1, a2 n2) \
    { \
        static return_type (*f)(a1, a2) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION3(name, return_type, symbol, a1, a2, a3) \
    return_type symbol(a1 n1, a2 n2, a3 n3) \
    { \
        static return_type (*f)(a1, a2, a3) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION4(name, return_type, symbol, a1, a2, a3, a4) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4) \
    { \
        static return_type (*f)(a1, a2, a3, a4) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION5(name, return_type, symbol, a1, a2, a3, a4, a5) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION6(name, return_type, symbol, a1, a2, a3, a4, a5, a6) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION7(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION8(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION9(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION10(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION11(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION12(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION13(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION14(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION15(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION16(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION17(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION18(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17, a18 n18) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18); \
    }


#define HYBRIS_IMPLEMENT_FUNCTION19(name, return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) \
    return_type symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17, a18 n18, a19 n19) \
    { \
        static return_type (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        return f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION0(name, symbol) \
    void symbol() \
    { \
        static void (*f)() FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION1(name, symbol, a1) \
    void symbol(a1 n1) \
    { \
        static void (*f)(a1) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION2(name, symbol, a1, a2) \
    void symbol(a1 n1, a2 n2) \
    { \
        static void (*f)(a1, a2) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION3(name, symbol, a1, a2, a3) \
    void symbol(a1 n1, a2 n2, a3 n3) \
    { \
        static void (*f)(a1, a2, a3) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION4(name, symbol, a1, a2, a3, a4) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4) \
    { \
        static void (*f)(a1, a2, a3, a4) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION5(name, symbol, a1, a2, a3, a4, a5) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5) \
    { \
        static void (*f)(a1, a2, a3, a4, a5) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION6(name, symbol, a1, a2, a3, a4, a5, a6) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION7(name, symbol, a1, a2, a3, a4, a5, a6, a7) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION8(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION9(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION10(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION11(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION12(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION13(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION14(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION15(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION16(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION17(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION18(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17, a18 n18) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18); \
    }


#define HYBRIS_IMPLEMENT_VOID_FUNCTION19(name, symbol, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) \
    void symbol(a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8, a9 n9, a10 n10, a11 n11, a12 n12, a13 n13, a14 n14, a15 n15, a16 n16, a17 n17, a18 n18, a19 n19) \
    { \
        static void (*f)(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) FP_ATTRIB = NULL; \
        HYBRIS_DLSYSM(name, &f, #symbol); \
        f(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19); \
    }


static void *egl_handle = NULL;
static void *glesv2_handle = NULL;
static void *_hybris_libgles1 = NULL;
static void *_hybris_libgles2 = NULL;
static int _egl_context_client_version = 1;

static EGLint  (*_eglGetError)(void) = NULL;

static EGLDisplay  (*_eglGetDisplay)(EGLNativeDisplayType display_id) = NULL;
static EGLBoolean  (*_eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor) = NULL;
static EGLBoolean  (*_eglTerminate)(EGLDisplay dpy) = NULL;

static const char *  (*_eglQueryString)(EGLDisplay dpy, EGLint name) = NULL;

static EGLBoolean  (*_eglGetConfigs)(EGLDisplay dpy, EGLConfig *configs,
		EGLint config_size, EGLint *num_config) = NULL;
static EGLBoolean  (*_eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list,
		EGLConfig *configs, EGLint config_size,
		EGLint *num_config) = NULL;
static EGLBoolean  (*_eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config,
		EGLint attribute, EGLint *value) = NULL;

static EGLSurface  (*_eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config,
		EGLNativeWindowType win,
		const EGLint *attrib_list) = NULL;
static EGLSurface  (*_eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config,
		const EGLint *attrib_list) = NULL;
static EGLSurface  (*_eglCreatePixmapSurface)(EGLDisplay dpy, EGLConfig config,
		EGLNativePixmapType pixmap,
		const EGLint *attrib_list) = NULL;
static EGLBoolean  (*_eglDestroySurface)(EGLDisplay dpy, EGLSurface surface) = NULL;
static EGLBoolean  (*_eglQuerySurface)(EGLDisplay dpy, EGLSurface surface,
		EGLint attribute, EGLint *value) = NULL;

static EGLBoolean  (*_eglBindAPI)(EGLenum api) = NULL;
static EGLenum  (*_eglQueryAPI)(void) = NULL;

static EGLBoolean  (*_eglWaitClient)(void) = NULL;

static EGLBoolean  (*_eglReleaseThread)(void) = NULL;

static EGLSurface  (*_eglCreatePbufferFromClientBuffer)(
		EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
		EGLConfig config, const EGLint *attrib_list) = NULL;

static EGLBoolean  (*_eglSurfaceAttrib)(EGLDisplay dpy, EGLSurface surface,
		EGLint attribute, EGLint value) = NULL;
static EGLBoolean  (*_eglBindTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer) = NULL;
static EGLBoolean  (*_eglReleaseTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer) = NULL;


static EGLBoolean  (*_eglSwapInterval)(EGLDisplay dpy, EGLint interval) = NULL;


static EGLContext  (*_eglCreateContext)(EGLDisplay dpy, EGLConfig config,
		EGLContext share_context,
		const EGLint *attrib_list) = NULL;
static EGLBoolean  (*_eglDestroyContext)(EGLDisplay dpy, EGLContext ctx) = NULL;
static EGLBoolean  (*_eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw,
		EGLSurface read, EGLContext ctx) = NULL;

static EGLContext  (*_eglGetCurrentContext)(void) = NULL;
static EGLSurface  (*_eglGetCurrentSurface)(EGLint readdraw) = NULL;
static EGLDisplay  (*_eglGetCurrentDisplay)(void) = NULL;
static EGLBoolean  (*_eglQueryContext)(EGLDisplay dpy, EGLContext ctx,
		EGLint attribute, EGLint *value) = NULL;

static EGLBoolean  (*_eglWaitGL)(void) = NULL;
static EGLBoolean  (*_eglWaitNative)(EGLint engine) = NULL;
static EGLBoolean  (*_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = NULL;
static EGLBoolean  (*_eglCopyBuffers)(EGLDisplay dpy, EGLSurface surface,
		EGLNativePixmapType target) = NULL;


static EGLImageKHR (*_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list) = NULL;
static EGLBoolean (*_eglDestroyImageKHR) (EGLDisplay dpy, EGLImageKHR image) = NULL;

static void (*_glEGLImageTargetTexture2DOES) (GLenum target, GLeglImageOES image) = NULL;

static __eglMustCastToProperFunctionPointerType (*_eglGetProcAddress)(const char *procname) = NULL;

extern void *android_dlopen(const char *name, int flags);

static inline void hybris_egl_initialize()
{
	egl_handle = (void *) android_dlopen(getenv("LIBEGL") ? getenv("LIBEGL") : "libEGL.so", RTLD_LAZY);
}

static inline void hybris_glesv2_initialize()
{
	glesv2_handle = (void *) android_dlopen(getenv("LIBGLESV2") ? getenv("LIBGLESV2") : "libGLESv2.so", RTLD_LAZY);
}

static void * _android_egl_dlsym(const char *symbol)
{
	if (egl_handle == NULL)
		_init_androidegl();

	return dlsym(egl_handle, symbol);
}

struct ws_egl_interface hybris_egl_interface = {
	_android_egl_dlsym,
	egl_helper_has_mapping,
	egl_helper_get_mapping,
};

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLint, eglGetError);

#define _EGL_MAX_DISPLAYS 100

struct _EGLDisplay *_displayMappings[_EGL_MAX_DISPLAYS];

void _addMapping(struct _EGLDisplay *display_id)
{
	int i;
	for (i = 0; i < _EGL_MAX_DISPLAYS; i++)
	{
		if (_displayMappings[i] == NULL)
		{
			_displayMappings[i] = display_id;
			return;
		}
	}
}

struct _EGLDisplay *hybris_egl_display_get_mapping(EGLDisplay display)
{
	int i;
	for (i = 0; i < _EGL_MAX_DISPLAYS; i++)
	{
		if (_displayMappings[i])
		{
			if (_displayMappings[i]->dpy == display)
			{
				return _displayMappings[i];
			}

		}
	}
	return EGL_NO_DISPLAY;
}

EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
	HYBRIS_DLSYSM(egl, &_eglGetDisplay, "eglGetDisplay");
	EGLNativeDisplayType real_display;

	real_display = (*_eglGetDisplay)(EGL_DEFAULT_DISPLAY);
	if (real_display == EGL_NO_DISPLAY)
	{
		return EGL_NO_DISPLAY;
	}

	struct _EGLDisplay *dpy = hybris_egl_display_get_mapping(real_display);
	if (!dpy) {
		dpy = ws_GetDisplay(display_id);
		if (!dpy) {
			return EGL_NO_DISPLAY;
		}
		dpy->dpy = real_display;
		_addMapping(dpy);
	}

	return real_display;
}

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglInitialize, EGLDisplay, EGLint *, EGLint *);

EGLBoolean eglTerminate(EGLDisplay dpy)
{
	HYBRIS_DLSYSM(egl, &_eglTerminate, "eglTerminate");

	struct _EGLDisplay *display = hybris_egl_display_get_mapping(dpy);
	ws_Terminate(display);
	return (*_eglTerminate)(dpy);
}

const char * eglQueryString(EGLDisplay dpy, EGLint name)
{
	HYBRIS_DLSYSM(egl, &_eglQueryString, "eglQueryString");
	return ws_eglQueryString(dpy, name, _eglQueryString);
}

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglGetConfigs, EGLDisplay, EGLConfig *, EGLint, EGLint *);
HYBRIS_IMPLEMENT_FUNCTION5(egl, EGLBoolean, eglChooseConfig, EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglGetConfigAttrib, EGLDisplay, EGLConfig, EGLint, EGLint *);

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
		EGLNativeWindowType win,
		const EGLint *attrib_list)
{
	HYBRIS_DLSYSM(egl, &_eglCreateWindowSurface, "eglCreateWindowSurface");

	HYBRIS_TRACE_BEGIN("hybris-egl", "eglCreateWindowSurface", "");
	struct _EGLDisplay *display = hybris_egl_display_get_mapping(dpy);
	win = ws_CreateWindow(win, display);

	assert(((struct ANativeWindowBuffer *) win)->common.magic == ANDROID_NATIVE_WINDOW_MAGIC);

	HYBRIS_TRACE_BEGIN("native-egl", "eglCreateWindowSurface", "");
	EGLSurface result = (*_eglCreateWindowSurface)(dpy, config, win, attrib_list);
	HYBRIS_TRACE_END("native-egl", "eglCreateWindowSurface", "");
	egl_helper_push_mapping(result, win);

	HYBRIS_TRACE_END("hybris-egl", "eglCreateWindowSurface", "");
	return result;
}

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLSurface, eglCreatePbufferSurface, EGLDisplay, EGLConfig, const EGLint *);
HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLSurface, eglCreatePixmapSurface, EGLDisplay, EGLConfig, EGLNativePixmapType, const EGLint *);

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	HYBRIS_DLSYSM(egl, &_eglDestroySurface, "eglDestroySurface");
	EGLBoolean result = (*_eglDestroySurface)(dpy, surface);

	/**
         * If the surface was created via eglCreateWindowSurface, we must
         * notify the ws about surface destruction for clean-up.
	 **/
	if (egl_helper_has_mapping(surface)) {
	    ws_DestroyWindow(egl_helper_pop_mapping(surface));
	}

	return result;
}

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglQuerySurface, EGLDisplay, EGLSurface, EGLint, EGLint *);
HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglBindAPI, EGLenum);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLenum, eglQueryAPI);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglWaitClient);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglReleaseThread);
HYBRIS_IMPLEMENT_FUNCTION5(egl, EGLSurface, eglCreatePbufferFromClientBuffer, EGLDisplay, EGLenum, EGLClientBuffer, EGLConfig, const EGLint *);
HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglSurfaceAttrib, EGLDisplay, EGLSurface, EGLint, EGLint);
HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglBindTexImage, EGLDisplay, EGLSurface, EGLint);
HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglReleaseTexImage, EGLDisplay, EGLSurface, EGLint);

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	EGLBoolean ret;
	EGLSurface surface;
	EGLNativeWindowType win;
	HYBRIS_TRACE_BEGIN("hybris-egl", "eglSwapInterval", "=%d", interval);

	/* Some egl implementations don't pass through the setSwapInterval
	 * call.  Since we may support various swap intervals internally, we'll
	 * call it anyway and then give the wrapped egl implementation a chance
	 * to chage it. */
	HYBRIS_DLSYSM(egl, &_eglGetCurrentSurface, "eglGetCurrentSurface");
	surface = (*_eglGetCurrentSurface)(EGL_DRAW);
	if (egl_helper_has_mapping(surface))
	    ws_setSwapInterval(dpy, egl_helper_get_mapping(surface), interval);

	HYBRIS_TRACE_BEGIN("native-egl", "eglSwapInterval", "=%d", interval);
	HYBRIS_DLSYSM(egl, &_eglSwapInterval, "eglSwapInterval");
	ret = (*_eglSwapInterval)(dpy, interval);
	HYBRIS_TRACE_END("native-egl", "eglSwapInterval", "");
	HYBRIS_TRACE_END("hybris-egl", "eglSwapInterval", "");
	return ret;
}

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config,
		EGLContext share_context,
		const EGLint *attrib_list)
{
	HYBRIS_DLSYSM(egl, &_eglCreateContext, "eglCreateContext");

	const EGLint *p = attrib_list;
	while (p != NULL && *p != EGL_NONE) {
		if (*p == EGL_CONTEXT_CLIENT_VERSION) {
			_egl_context_client_version = p[1];
		}
		p += 2;
	}

	return (*_eglCreateContext)(dpy, config, share_context, attrib_list);
}

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglDestroyContext, EGLDisplay, EGLContext);
HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglMakeCurrent, EGLDisplay, EGLSurface, EGLSurface, EGLContext);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLContext, eglGetCurrentContext);
HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLSurface, eglGetCurrentSurface, EGLint);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLDisplay, eglGetCurrentDisplay);
HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglQueryContext, EGLDisplay, EGLContext, EGLint, EGLint *);
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglWaitGL);
HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglWaitNative, EGLint);

EGLBoolean _my_eglSwapBuffersWithDamageEXT(EGLDisplay dpy, EGLSurface surface, EGLint *rects, EGLint n_rects)
{
	EGLNativeWindowType win;
	EGLBoolean ret;
	HYBRIS_TRACE_BEGIN("hybris-egl", "eglSwapBuffersWithDamageEXT", "");
	HYBRIS_DLSYSM(egl, &_eglSwapBuffers, "eglSwapBuffers");

	if (egl_helper_has_mapping(surface)) {
		win = egl_helper_get_mapping(surface);
		ws_prepareSwap(dpy, win, rects, n_rects);
		ret = (*_eglSwapBuffers)(dpy, surface);
		ws_finishSwap(dpy, win);
	} else {
		ret = (*_eglSwapBuffers)(dpy, surface);
	}
	HYBRIS_TRACE_END("hybris-egl", "eglSwapBuffersWithDamageEXT", "");
	return ret;
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	EGLBoolean ret;
	HYBRIS_TRACE_BEGIN("hybris-egl", "eglSwapBuffers", "");
	ret = _my_eglSwapBuffersWithDamageEXT(dpy, surface, NULL, 0);
	HYBRIS_TRACE_END("hybris-egl", "eglSwapBuffers", "");
	return ret;
}

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglCopyBuffers, EGLDisplay, EGLSurface, EGLNativePixmapType);


static EGLImageKHR _my_eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list)
{
	HYBRIS_DLSYSM(egl, &_eglCreateImageKHR, "eglCreateImageKHR");
	EGLContext newctx = ctx;
	EGLenum newtarget = target;
	EGLClientBuffer newbuffer = buffer;
	const EGLint *newattrib_list = attrib_list;

	ws_passthroughImageKHR(&newctx, &newtarget, &newbuffer, &newattrib_list);

	EGLImageKHR eik = (*_eglCreateImageKHR)(dpy, newctx, newtarget, newbuffer, newattrib_list);

	if (eik == EGL_NO_IMAGE_KHR) {
		return EGL_NO_IMAGE_KHR;
	}

	struct egl_image *image;
	image = malloc(sizeof *image);
	image->egl_image = eik;
	image->egl_buffer = buffer;
	image->target = target;

	return (EGLImageKHR)image;
}

static void _my_glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	HYBRIS_DLSYSM(glesv2, &_glEGLImageTargetTexture2DOES, "glEGLImageTargetTexture2DOES");
	struct egl_image *img = image;
	(*_glEGLImageTargetTexture2DOES)(target, img ? img->egl_image : NULL);
}

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *procname)
{
	HYBRIS_DLSYSM(egl, &_eglGetProcAddress, "eglGetProcAddress");
	if (strcmp(procname, "eglCreateImageKHR") == 0)
	{
		return (__eglMustCastToProperFunctionPointerType) _my_eglCreateImageKHR;
	}
	else if (strcmp(procname, "eglDestroyImageKHR") == 0)
	{
		return (__eglMustCastToProperFunctionPointerType) eglDestroyImageKHR;
	}
	else if (strcmp(procname, "eglSwapBuffersWithDamageEXT") == 0)
	{
		return (__eglMustCastToProperFunctionPointerType) _my_eglSwapBuffersWithDamageEXT;
	}
	else if (strcmp(procname, "glEGLImageTargetTexture2DOES") == 0)
	{
		return (__eglMustCastToProperFunctionPointerType) _my_glEGLImageTargetTexture2DOES;
	}

	__eglMustCastToProperFunctionPointerType ret = NULL;

	switch (_egl_context_client_version) {
		case 1:  // OpenGL ES 1.x API
			if (_hybris_libgles1 == NULL) {
				_hybris_libgles1 = (void *) dlopen(getenv("HYBRIS_LIBGLESV1") ?: "libGLESv1_CM.so.1", RTLD_LAZY);
			}
			ret = _hybris_libgles1 ? dlsym(_hybris_libgles1, procname) : NULL;
			break;
		case 2:  // OpenGL ES 2.0 API
			if (_hybris_libgles2 == NULL) {
				_hybris_libgles2 = (void *) dlopen(getenv("HYBRIS_LIBGLESV2") ?: "libGLESv2.so.2", RTLD_LAZY);
			}
			ret = _hybris_libgles2 ? dlsym(_hybris_libgles2, procname) : NULL;
			break;
		case 3:  // OpenGL ES 3.x API
			// TODO: Load from libGLESv3.so once we have OpenGL ES 3.0/3.1 support
			break;
		default:
			HYBRIS_WARN("Unknown EGL context client version: %d", _egl_context_client_version);
			break;
	}

	if (ret == NULL) {
		ret = ws_eglGetProcAddress(procname);
	}

	if (ret == NULL) {
		ret = (*_eglGetProcAddress)(procname);
	}

	return ret;
}

EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image)
{
	HYBRIS_DLSYSM(egl, &_eglDestroyImageKHR, "eglDestroyImageKHR");
	struct egl_image *img = image;
	EGLBoolean ret = (*_eglDestroyImageKHR)(dpy, img ? img->egl_image : NULL);
	if (ret == EGL_TRUE) {
		free(img);
		return EGL_TRUE;
	}
	return ret;
}

// vim:ts=4:sw=4:noexpandtab
