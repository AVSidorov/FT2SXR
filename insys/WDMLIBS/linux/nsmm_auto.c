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

#include "nsmm_auto.h"

static int mem_t_cmp(const void *a, const void *b)
{
        return (((mem_t *)a)->phys >= ((mem_t *)b)->phys) ? 0 : -1;
}

static void mem_t_swap(void *a, void *b, int size)
{
        mem_t t = *(mem_t *)a;
        *(mem_t *)a = *(mem_t *)b;
        *(mem_t *)b = t;
}

static void reserve_page10(unsigned long addr, int reserve)
{
        int i;

        /* Reserved pages are not compacted */
        for (i = 0; i < (1ULL << 10); i++, addr += PAGE_SIZE) {
                reserve ? SetPageReserved(virt_to_page(phys_to_virt(addr)))
                        : ClearPageReserved(virt_to_page(phys_to_virt(addr)));
        }
}

unsigned long find_largest_segment(mem_t *pages, int pages_allocated, unsigned long *base)
{
        unsigned long addr, start, seg_len;
        unsigned long seg_base, seg_size;
        int i;

        sort(pages, pages_allocated, sizeof(mem_t), mem_t_cmp, mem_t_swap);

        start = seg_base = pages[0].phys;
        seg_size = seg_len = ORDER_10_PAGE_SIZE;

        for (i = 1; i < pages_allocated; i++) {
                addr = pages[i].phys;
                if (i == pages_allocated - 1 && /* last page */
                    start + seg_len == addr) {
                        seg_len += ORDER_10_PAGE_SIZE;
                        addr = (unsigned long)-1;
                }
                if (start + seg_len != addr) {
                        if (seg_size < seg_len) {
                                seg_base = start;
                                seg_size = seg_len;
                        }
                        start = addr;
                        seg_len = 0;
                }
                seg_len += ORDER_10_PAGE_SIZE;
        }
        *base = seg_base;

        return seg_size;
}

bool nsmm_allocate_memory(void **ppPages, int *pPages_allocated,
                          unsigned long min, unsigned long max,
                          unsigned long *base, unsigned long *size)
{
        mem_t *pages = 0;
        int i, max_pages = ALIGN_ON(max, ORDER_10_PAGE_SIZE) / ORDER_10_PAGE_SIZE;
        unsigned long addr, seg_base, seg_size = 0;
        int freed;

     //   err_msg(err_trace, "%s(): pages = 0x%p\n", __FUNCTION__, pages);

        dbg_msg(1, "trying to allocate up to %d order-10 pages", max_pages);
        pages = kzalloc(max_pages * sizeof(mem_t), GFP_KERNEL);
        if (!pages) {
                err_msg("can't allocate space for pages");
                return false;
        }

   //     err_msg(err_trace, "%s(): pages = 0x%p\n", __FUNCTION__, pages);

        for (*pPages_allocated = 0, i = 0; i < max_pages; i++) {
                addr = __get_free_pages(GFP_DMA32, 10);
                if (!addr)
                        break;
                (*pPages_allocated)++;
                pages[i].virt = addr;
                pages[i].phys = virt_to_phys((void*)addr);
                seg_size = find_largest_segment(pages, *pPages_allocated, &seg_base);
                if (seg_size >= min)
                        break;
        }

        dbg_msg(1, "allocated %d order-10 pages", *pPages_allocated);

        if (*pPages_allocated == 0)
                goto error;

        if (seg_size < min) {  /* if min can't be allocated */
                err_msg(1, "asked to allocate %lu bytes, could do only %lu",
                       min, seg_size);
                goto error;
        }

        /* keep the longest segment and free the rest*/
        for (i = 0, freed = 0; i < (*pPages_allocated); i++) {
                if (pages[i].phys < seg_base ||
                    pages[i].phys >= seg_base + seg_size) {
                        free_pages(pages[i].virt, 10);
                        pages[i].phys = 0;
                        freed++;
                } else {
                        reserve_page10(pages[i].phys, 1);
                }
        }
        dbg_msg(1, "freed %d unused order-10 pages", freed);
        *base = seg_base;
        *size = seg_size;
        dbg_msg(1, "found segment %p %lu mb", (void *)seg_base, (seg_size >> 20ULL));

        *ppPages = pages;

        return true;
error:
        nsmm_free_memory(pages, *pPages_allocated);
        *ppPages = 0;
        return false;
}

void nsmm_free_memory(void *pages, int pages_allocated)
{
        int freed = 0, i;
        mem_t *p = (mem_t *)pages;

        for (i = 0 ; i < pages_allocated; i++) {
                if (p[i].phys) {
                        reserve_page10(p[i].phys, 0);
                        free_pages((unsigned long)phys_to_virt(p[i].phys), 10);
                        freed++;
                }
        }
        dbg_msg(1, "freed %d order-10 pages", freed);
        if (pages) {
                kfree(pages);
                pages = NULL;
        }
}
