# WHAT IS THIS?

this is an attempt to load android libraries with the gnu ld.so.

the example program does the following:

test_fake_libc:
    binary linked against glibc
    loads test.so via dlopen from glibc

test.so:
    binary linked against bionic libc
    test_function prints three lines, one with a floating point argument.

libc.so:
    a very small library which currently only implements a few bionic functions
    by loading glibc via dlopen and calling into it, currently it dlsyms every
    symbol and is thus probably slow. but i hope that gnu indirect functions
    can make it faster

this requires ld.so to have no checks for hardfp / softfp mismatches

## test_vibrator
there is also an example which loads libhardware_legacy.so and initiates the
vibrator. It requires one additional quirk (until i patch glibc or figure out
another way) and that is to use relocation_packer from bionic/tools to modify
libc++.so.

## test_sensors
Same as above, modified libc++.so. test taken from libhybris.

# COMPILE
compile test.so using the android build system
compile libc.so using sb2 make
compile test_fake_libc using sb2 make

# WHY?

to remove the need for the android linker in hybris implementations (i still
don't know whether it is even feasible)

