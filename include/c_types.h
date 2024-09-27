/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _C_TYPES_H_
#define _C_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __le16      u16

#define LOCAL       static

#ifndef NULL
#define NULL        (void *)0
#endif /* NULL */

/* probably should not put STATUS here */
typedef enum {
    OK = 0,
    FAIL,
    PENDING,
    BUSY,
    CANCEL,
} STATUS;

//#define BIT(nr)     (1UL << (nr))

#define REG_WRITE(_r, _v)       (*(volatile uint32 *)(_r)) = (_v)
#define REG_READ(_r)            (*(volatile uint32 *)(_r))

#define REG_SET_BIT(_r, _b)     (*(volatile uint32 *)(_r) |= (_b))
#define REG_CLR_BIT(_r, _b)     (*(volatile uint32 *)(_r) &= ~(_b))

//#define __packed            __attribute__((packed))
#define STORE_ATTR          __attribute__((aligned(4)))

#define SHMEM_ATTR

#ifdef ICACHE_FLASH
#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))
#else
#define ICACHE_FLASH_ATTR
#endif

#define DMEM_ATTR           __attribute__((section(".bss")))
#define IRAM_ATTR           __attribute__((section(".text")))
#define ICACHE_RODATA_ATTR  __attribute__((section(".irom.text")))

#ifndef __cplusplus
#define BOOL            bool
#define TRUE            true
#define FALSE           false
#endif /* !__cplusplus */

#ifdef __cplusplus
}
#endif

#endif /* _C_TYPES_H_ */
