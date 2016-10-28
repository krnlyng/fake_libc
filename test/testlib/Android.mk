LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test
LOCAL_SRC_FILES := test.c

LOCAL_CXX_STL := none

$(combo_2nd_arch_prefix)TARGET_GLOBAL_CFLAGS = -fno-exceptions -Wno-multichar -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2 -fno-short-enums -no-canonical-prefixes -fno-canonical-system-headers -mcpu=cortex-a15 -mfpu=neon-vfpv4 -D__ARM_FEATURE_LPAE=1 -mfloat-abi=softfp -include build/core/combo/include/arch/linux-arm/AndroidConfig.h -I build/core/combo/include/arch/linux-arm/ -fno-builtin-sin -fno-strict-volatile-bitfields -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -fdiagnostics-color -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG

LOCAL_LDFLAGS := -Wl,--exclude-libs,libgcc.a -Wl,--no-warn-mismatch

include $(BUILD_SHARED_LIBRARY)

