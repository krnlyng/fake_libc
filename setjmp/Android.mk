LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libsetjmp

LOCAL_SRC_FILES := setjmp.S \
					assert.cpp \
					sigaddset.cpp  sigdelset.cpp    sigfillset.cpp  sigismember.cpp  signalfd.cpp    sigprocmask.cpp  sigsetmask.c    sigtimedwait.cpp  sigwaitinfo.cpp \
					sigaction.cpp  sigblock.c     sigemptyset.cpp  siginterrupt.c  signal.cpp       sigpending.cpp  sigqueue.cpp     sigsuspend.cpp  sigwait.cpp __restore.S \
					___rt_sigqueueinfo.S errno_internal.c


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../bionic/libc

LOCAL_CXX_STL := none

include $(BUILD_SHARED_LIBRARY)

