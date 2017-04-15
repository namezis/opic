/* op_malloc.c ---
 *
 * Filename: op_malloc.c
 * Description:
 * Author: Felix Chern
 * Maintainer: Felix Chern
 * Created: Sat Oct  1 16:25:47 2016
 * Version:
 * Package-Requires: ()
 * Last-Updated:
 *           By:
 *     Update #: 0
 * URL:
 * Doc URL:
 * Keywords:
 * Compatibility:
 *
 */

/* Commentary:
 *
 *
 *
 */

/* Change Log:
 *
 *
 */

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "opic/op_malloc.h"
#include "opic/common/op_utils.h"
#include "opic/malloc/objdef.h"

bool
OPHeapNew(OPHeap** heap_ref)
{
  void *addr, *map_addr;
  addr = NULL + OPHEAP_SIZE;
  map_addr = MAP_FAILED;
  for (int i = 0; i < (1<<15); i++)
    {
      map_addr = mmap(addr, OPHEAP_SIZE,
                      PROT_READ | PROT_WRITE,
                      MAP_ANON | MAP_PRIVATE | MAP_FIXED,
                      -1, 0);
      if (map_addr != MAP_FAILED)
        {
          *heap_ref = map_addr;
          memset(*heap_ref, 0, sizeof(OPHeap));
          (*heap_ref)->version = OPHEAP_VERSION;
          (*heap_ref)->hpage_num = HPAGE_BMAP_NUM * 64;
          return true;
        }
      else
        addr += OPHEAP_SIZE;
    }
  return false;
}

bool
OPHeapNewFromFile(OPHeap** heap_ref, FILE* fd)
{
  return false;
}

void
OPHeapShrinkShadow(OPHeap* heap)
{
  int max_hpage;
  int hpage_bmidx, hpage_bmbit, bm_padding_bit;
  uint64_t bmap;

  hpage_bmidx = (heap->hpage_num - 1) / 64;
  bm_padding_bit = heap->hpage_num % 64;

  bmap = atomic_load_explicit(&heap->occupy_bmap[hpage_bmidx],
                              memory_order_relaxed);
  if (bm_padding_bit)
    {
      bmap &= ((1UL << bm_padding_bit) - 1);
      if (bmap)
        {
          hpage_bmbit = 64 - __builtin_clzl(bmap) - 1;
          max_hpage = hpage_bmidx * 64 + hpage_bmbit;
          goto found_max_hpage;
        }
    }

  for (int i = hpage_bmidx; i >= 0; i--)
    {
      if ((bmap = atomic_load_explicit(&heap->occupy_bmap[i],
                                       memory_order_relaxed)))
        {
          hpage_bmbit = 64 - __builtin_clzl(bmap) - 1;
          max_hpage = i * 64 + hpage_bmbit;
          goto found_max_hpage;
        }
    }
  max_hpage = 1;

 found_max_hpage:
  heap->hpage_num = max_hpage;
  hpage_bmidx = max_hpage / 64;
  hpage_bmbit = max_hpage % 64;

  if (hpage_bmidx < HPAGE_BMAP_NUM)
    {
      if (hpage_bmbit)
        atomic_fetch_or_explicit(&heap->occupy_bmap[hpage_bmidx],
                                 ~((1UL << hpage_bmbit) - 1),
                                 memory_order_relaxed);

      for (int bmidx = round_up_div(heap->hpage_num, 64);
           bmidx < heap->hpage_num; bmidx++)
        atomic_store_explicit(&heap->occupy_bmap[bmidx], ~0ULL,
                              memory_order_relaxed);
    }
}

void
OPHeapWriteToFile(OPHeap* heap, FILE* fd)
{
  OPHeap heap_copy;
  uintptr_t heap_base;
  heap_base = (uintptr_t)heap;
  memcpy(&heap_copy, heap, sizeof(OPHeap));
  OPHeapShrinkShadow(&heap_copy);

  fwrite(&heap_copy, sizeof(OPHeap), 1, fd);
  fwrite((void*)(heap_base + sizeof(OPHeap)),
         HPAGE_SIZE - sizeof(OPHeap), 1, fd);
  if (heap_copy.hpage_num > 1)
    fwrite((void*)(heap_base + HPAGE_SIZE),
           HPAGE_SIZE, heap_copy.hpage_num - 1, fd);
}

void
OPHeapDestroy(OPHeap* heap)
{
  munmap(heap, heap->hpage_num * HPAGE_SIZE);
}

/* op_malloc.c ends here */
