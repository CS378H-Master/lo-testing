// good test case
// ABI harness: twenty default heaps' worth of unrooted objects are allocated, at the default heap and at a 64 KiB heap; the collector must reclaim them rather than abort, while one rooted object keeps its payload throughout
// main method return value: 0
// gc stress heap: 65536
#include "lo_abi.h"

/*
 * What this checks: lo_alloc's exhaustion path collects and retries (§3.1)
 * rather than aborting, and what it reclaims is exactly the unreachable
 * objects. One object is rooted for the whole run; everything else allocated
 * here is garbage as soon as the loop moves on. A runtime that never frees
 * exits 137 partway through the first loop on every configuration.
 *
 * Expected output: 13981013, 6990507, true, true.
 */

typedef struct Cell { Object hdr; int32_t v; int32_t w; } Cell;
LO_DESC(CELL, "Cell", 0, sizeof(Cell), 0, 0);

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);
  /* The one survivor, rooted in the only slot. */
  Cell *keep = (Cell *)lo_alloc(&CELL);
  keep->v = 1234; keep->w = -5;
  LO_ROOT(f, 0) = &keep->hdr;

  /* Twenty default heaps (20 x 16 MiB) of garbage in CELL-sized pieces, so the
   * loop collects many times even at the default heap and on WASM; at the
   * 64 KiB stress heap that is over five thousand collections. Nothing
   * allocated here is ever rooted. */
  uint32_t per = (uint32_t)sizeof(Cell); if (per % 8) per += 8 - per % 8;
  uint32_t count = (20u * 16777216u) / per;
  int32_t sum = 0;
  for (uint32_t i = 0; i < count; i++) {
    Cell *g = (Cell *)lo_alloc(&CELL); /* never rooted: garbage after this iteration */
    g->v = (int32_t)i;
    sum += g->v & 1;
  }
  /* Re-read the survivor from its root slot: it may have moved. */
  keep = (Cell *)LO_ROOT(f, 0);
  lo_put_int_line((int32_t)count);
  lo_put_int_line(sum);
  lo_put_bool_line(keep->hdr.class_descriptor == &CELL && keep->v == 1234 && keep->w == -5);

  /* Now the same with a forced collection between batches: still no abort,
   * and the survivor still intact. */
  for (int round = 0; round < 20; round++) {
    for (uint32_t i = 0; i < count / 10; i++) { Cell *g = (Cell *)lo_alloc(&CELL); g->w = round; }
    lo_gc_collect();
  }
  keep = (Cell *)LO_ROOT(f, 0);
  lo_put_bool_line(keep->v == 1234 && keep->w == -5);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
