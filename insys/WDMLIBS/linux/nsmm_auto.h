#ifndef NSMM_AUTO_H
#define NSMM_AUTO_H

/*
 * nsmm_auto.c
 *
 * NovaSparks - Automatic DMA buffer allocation
 *
 * Author: V. Mayatskikh <v.mayatskikh@novasparks.com>
 *
 * Copyright (c) 2011 Novasparks
 *
 * All rights reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/gfp.h>
#include <asm/io.h>

#include <linux/interrupt.h>

#include "dmachan.h"

//#include "ipcmodule.h"

typedef struct {
        unsigned long virt;
        unsigned long phys;
} mem_t;

#define ALIGN_ON(size, x)						\
        (((size) % (x)) ? (size) + ((x) - ((size) % (x))) : (size))

#define ORDER_10_PAGE_SIZE (PAGE_SIZE * (1ULL << 10))

unsigned long find_largest_segment(mem_t *pages, int pages_allocated, unsigned long *base);

bool nsmm_allocate_memory(void **ppPages, int *pPages_allocated, unsigned long min, unsigned long max,
                          unsigned long *base, unsigned long *size);

void nsmm_free_memory(void *pages, int pages_allocated);

#endif // NSMM_AUTO_H
