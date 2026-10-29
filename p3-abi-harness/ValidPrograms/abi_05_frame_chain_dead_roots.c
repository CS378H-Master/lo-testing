// good test case
// ABI harness: three nested shadow frames with zero, one and eight root slots, some slots deliberately null, are collected at every depth; each frame's objects are verified before it is popped and the chain is restored on the way out
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: the shadow-stack discipline of §3.3 from the runtime's
 * side. Frames nest four deep (0, 0, 1 and 8 roots) and a collection runs at
 * every depth, so the collector must walk the whole parent chain, scan exactly
 * num_roots slots per frame, tolerate null slots, and after each lo_pop_frame
 * resume from the right parent. Each frame verifies its own objects before it
 * pops; a wrong chain or a mis-sized scan surfaces as a negative sum or a
 * crash.
 *
 * Expected output: 1512, true, true.
 */

typedef struct Cell { Object hdr; int32_t v; int32_t pad; } Cell;
LO_DESC(CELL, "Cell", 0, sizeof(Cell), 0, 0);

static Cell *mk(int32_t v) { Cell *c = (Cell *)lo_alloc(&CELL); c->v = v; return c; }

/* Deepest frame: eight slots, only the even ones hold objects. */
static int32_t depth3(void) {
  LO_FRAME(f, 8);
  for (int i = 0; i < 8; i += 2) LO_ROOT(f, i) = &mk(300 + i)->hdr; /* odd slots stay null */
  lo_gc_collect();
  int32_t sum = 0;
  for (int i = 0; i < 8; i++) {
    if (i % 2 == 1) { if (LO_ROOT(f, i) != 0) return -1; continue; } /* null slots must stay null */
    Cell *c = (Cell *)LO_ROOT(f, i);
    if (c->hdr.class_descriptor != &CELL) return -2;
    sum += c->v;
  }
  lo_pop_frame();
  return sum; /* 300+302+304+306 = 1212 */
}

/* One slot; its object must survive the collections below and the one here. */
static int32_t depth2(void) {
  LO_FRAME(f, 1);
  LO_ROOT(f, 0) = &mk(200)->hdr;
  int32_t below = depth3();
  lo_gc_collect();
  Cell *c = (Cell *)LO_ROOT(f, 0);
  int32_t r = (c->hdr.class_descriptor == &CELL && c->v == 200) ? below + 200 : -3;
  lo_pop_frame();
  return r;
}

/* A frame with no roots at all is legal and must be walked through. */
static int32_t depth1(void) {
  LO_FRAME(f, 0);
  lo_gc_collect();
  int32_t below = depth2();
  lo_gc_collect();
  lo_pop_frame();
  return below + 100;
}

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(top, 2);
  LO_ROOT(top, 0) = &mk(7)->hdr;
  LO_ROOT(top, 1) = 0;
  int32_t r = depth1();                 /* 1212 + 200 + 100 = 1512 */
  lo_gc_collect();                      /* after the inner frames are gone */
  Cell *c = (Cell *)LO_ROOT(top, 0);
  lo_put_int_line(r);
  lo_put_bool_line(c->hdr.class_descriptor == &CELL && c->v == 7 && LO_ROOT(top, 1) == 0);
  /* Garbage with only `top` live: the popped frames' objects must be gone,
   * never scanned, and the one remaining root must survive. */
  for (int i = 0; i < 100000; i++) { (void)mk(i); }
  c = (Cell *)LO_ROOT(top, 0);
  lo_put_bool_line(c->v == 7);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
