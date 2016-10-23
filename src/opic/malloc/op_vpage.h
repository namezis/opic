/* op_vpage.h --- 
 * 
 * Filename: op_vpage.h
 * Description: 
 * Author: Felix Chern
 * Maintainer: 
 * Copyright: (c) 2016 Felix Chern
 * Created: Sat Oct 22, 2016
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
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Code: */


/* op_vpage.h --- 
 * 
 * Filename: op_vpage.h
 * Description: 
 * Author: Felix Chern
 * Maintainer: 
 * Copyright: (c) 2016 Felix Chern
 * Created: Tue Oct 11 2016
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
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Code: */

#ifndef OP_VPAGE_H
#define OP_VPAGE_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "opic/common/op_macros.h"
#include "opic/common/op_assert.h"
#include "opic/common/op_log.h"
#include "opic/malloc/op_pspan.h"
#include <assert.h>

OP_BEGIN_DECLS

typedef struct OPVPage OPVPage;

struct OPVPage 
{
  OPVPage* prev;
  OPVPage* next;
  atomic_uint_fast64_t occupy_bmap[8];
  atomic_uint_fast64_t header_bmap[8];
  atomic_int_fast8_t refcnt[512];
};

static_assert(sizeof(OPVPage) == 656, "OPVPage size should be 656\n");

OPVPage* OPVPageInit(void* addr)
  __attribute__((nonnull));

UnaryPSpan* OPVPageAllocPSpan(OPVPage* restrict self,
                              int16_t sc_idx,
                              uint16_t obj_size,
                              unsigned int span_cnt)
  __attribute__((nonnull));

bool OPVPageFree(OPVPage* restrict self, void* addr)
  __attribute__((nonnull));




OP_END_DECLS

#endif
/* op_vpage.h ends here */

/* op_vpage.h ends here */
