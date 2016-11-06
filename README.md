# WHAT IS THIS?
this is an attempt to load android libraries with the gnu ld.so.

## changes required to glibc
* may not have any checks for hardfp / softfp mismatches
* must have a patch which makes it ingore .init_array pointers which are zero
  the android loader allows these kinds of pointers and ignores them as well

## changes required to android libs
* some android libs have to be patched with relocation_patcher from the android
  build system. any lib that uses DT_ANDROID_RELA needs to be patched

### ALTERNATIVE:
* patch glibc to support this kinds of relocations

## the following tests from libhybris have been compiled against this and work:
* test_vibrator
* test_sensors
* test_egl
* test_hwcomposer

how to run the tests:


`export EGL_PLATFORM=hwcomposer # depending on test`

`LD_LIBRARY_PATH=/path/to/fake/libc/libs:/usr/libexec/droid-hybris/system/lib:/system/lib:/vendor/lib test_XXX`

# compile
use `sb2 make` to compile the libraries. i will try to set up a nicer build
system eventually

# how does it work?
lets take `test_hwcomposer` as an example:

`test_hwcomposer` is linked against `libEGL.so.1` which is linked against
`libhardware.so`

both these libraries are linked against `libandroid_namespace.so` which
provides a function called `android_dlopen`. this function is a wrapper around
gnu `dlmopen`, the wrapper loads `libc.so` (the fake one) into a new namespace
when first called and initializes this library. then it loads the library which
was intended to be opened into the same namespace (the one passed to
`android_dlopen` as an argument). lets call this namespace `android_namespace`
from now on.

`libc.so`s init function loads `libdl.so` (the fake one) and `libdl.so.2` (the
gnu one) and then passes `libdl.so.2`s functions to `libdl.so` such that when
an android library calls `dlopen` it knows which `dlopen` to use (namely again
`dlmopen` with the android namespace as argument). `libc.so` provides most of
the bionic libc functions (with versioning) and redirects them to the
appropriate glibc functions. `setjmp` and `longjmp` were tricky but in the end
i just copied the functions into a seperate android library which i load in
the fake libc (with prefix `my_` to avoid issues).

`libEGL.so.1` loads androids `libEGL.so` via `android_dlopen` and (gnu)
`dlsym`s all the symbols it requires on demand.
`libharware.so` does the same for androids `/system/lib/libhardware.so`.

tls is provided to the android libraries in a similar way libhybris does it:
use a `__thread` variable and android calls `__get_tls_hooks`.

# WHY?
* maybe to remove the need for the android linker in hybris adaptions?
* maybe to do some crazy sfdroid stuff?

