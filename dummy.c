#include "helpers.h"

/*#define DUMMY(f) \
    void f(void) __attribute__((naked,noinline)) SOFTFP; \
    void f(void) \
    { \
        __asm__ volatile("blx " #f "@GLIBC_2.19");\
    }
*/

#define DUMMY(f, ...) \
    __asm__(".symver "#f "," #f "@LIBC"); \
    __asm__(".symver glibc_" #f "," #f "@GLIBC_2.4"); \
    void f(void) __attribute__((naked,noinline)); \
    void f(void) \
    { \
        __asm__ volatile ("blx " #f "\n"); \
    }

// TODO
DUMMY(lstat)
DUMMY(__FD_SET_chk)
DUMMY(__FD_ISSET_chk)
DUMMY(__open_2)
DUMMY(mknod)
DUMMY(fstatat)

//TODO
DUMMY(opendir);
DUMMY(readdir_r);
DUMMY(closedir);
DUMMY(sleep);
DUMMY(daemon);
DUMMY(getcwd);
DUMMY(capset);

// TODO
DUMMY(fseek);
DUMMY(ftell);
DUMMY(fileno);
DUMMY(fsync);

DUMMY(waitpid);
DUMMY(init_module);
DUMMY(delete_module);
DUMMY(socketpair);
DUMMY(dup);
DUMMY(tcgetattr);
DUMMY(tcsetattr);
DUMMY(mkdir);
DUMMY(chmod);
DUMMY(chown);
DUMMY(rename);
DUMMY(mkdirat);
DUMMY(openat);
DUMMY(setsockopt);
DUMMY(bind);
DUMMY(listen);
DUMMY(gethostbyname);
DUMMY(select);
DUMMY(getsockopt);
DUMMY(sync);
DUMMY(setmntent);
DUMMY(getmntent);
DUMMY(endmntent);
DUMMY(usleep);
DUMMY(reboot);
DUMMY(ioctl);
DUMMY(strtoimax);
DUMMY(recvmsg);
DUMMY(inet_ntoa);
DUMMY(__strcpy_chk);
DUMMY(inet_addr);

// TODO
DUMMY(getaddrinfo);
DUMMY(freeaddrinfo);

DUMMY(if_nametoindex);
DUMMY(send);
DUMMY(sendmsg);
DUMMY(perror);

