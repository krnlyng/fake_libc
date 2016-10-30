/*
 * Copyright (C) 2014 Jolla Ltd.
 * Contact: Simonas Leleiva <simonas.leleiva@jollamobile.com>
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

#define _GNU_SOURCE
#include <dlfcn.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define LIBHARDWARE_LEGACY_PATH "/system/lib/libhardware_legacy.so"

#define SOFTFP __attribute__((pcs("aapcs")))

static void *libhardware_legacy_handle = NULL;

static int (*libhardware_legacy_vibrator_exists)(void) SOFTFP;
static int (*libhardware_legacy_vibrator_on)(int) SOFTFP;
static int (*libhardware_legacy_vibrator_off)(int) SOFTFP;

#define LOAD_LIBHARDWARE_LEGACY() \
	if(!libhardware_legacy_handle) \
	{ \
		libhardware_legacy_handle = dlmopen(LM_ID_NEWLM, LIBHARDWARE_LEGACY_PATH, RTLD_LAZY); \
        \
	}

#define LOAD_LIBHARDWARE_LEGACY_FUNCTION(f) \
	if(!libhardware_legacy_ ## f) \
	{ \
		libhardware_legacy_ ## f = dlsym(libhardware_legacy_handle, "vibrator_on"); \
	}

int vibrator_exists(void)
{
    LOAD_LIBHARDWARE_LEGACY();

    LOAD_LIBHARDWARE_LEGACY_FUNCTION(vibrator_exists);

    return libhardware_legacy_vibrator_exists();
}

int vibrator_on(int arg)
{
    LOAD_LIBHARDWARE_LEGACY();

    LOAD_LIBHARDWARE_LEGACY_FUNCTION(vibrator_on);

    return libhardware_legacy_vibrator_on(arg);
}

int vibrator_off(int arg)
{
    LOAD_LIBHARDWARE_LEGACY();

    LOAD_LIBHARDWARE_LEGACY_FUNCTION(vibrator_off);

    return libhardware_legacy_vibrator_off(arg);
}

