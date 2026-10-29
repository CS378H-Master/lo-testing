// good test case
// ABI harness: at a small heap, a varying amount of garbage is allocated before each string operation so that on some rounds the result's own allocation is what triggers the collection; the operands move during the call and every result must still be exact
// main method return value: 0
// gc stress heap: 65536
#include "lo_abi.h"

/*
 * What this checks: the string operations are safepoints (§3.2) and must read
 * their operands correctly even when allocating the result collects and moves
 * them. The runtime cannot rely on the caller: the operands are only in the
 * caller's root slots, so the runtime must copy or re-root them before it
 * allocates. Each round allocates a different amount of garbage first, so over
 * the sweep the result allocation lands on a collection boundary several
 * times at the 64 KiB stress heap. Every result and both operands are checked
 * every round; the four counts must all equal the number of rounds.
 *
 * Expected output: 215, 215, 215, 215.
 */

typedef struct Cell { Object hdr; int32_t v; int32_t pad; } Cell;
LO_DESC(CELL, "Cell", 0, sizeof(Cell), 0, 0);

/* n unrooted objects: pure garbage, used to position the next allocation. */
static void fill(uint32_t n) { for (uint32_t i = 0; i < n; i++) { Cell *c = (Cell *)lo_alloc(&CELL); c->v = (int32_t)i; } }

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 3);
  int32_t ok_concat = 0, ok_repeat = 0, ok_reverse = 0, rounds = 0;
  /* Sweep the pre-op garbage from 0 to 1500 cells in steps of 7 so the op's
     allocation lands on every phase of the heap cycle across the sweep. */
  for (uint32_t g = 0; g <= 1500; g += 7) {
    rounds++;
    LO_ROOT(f, 0) = lo_str("hello, ");
    LO_ROOT(f, 1) = lo_str("w\xC3\xB6rld");           /* wörld */
    fill(g);
    LO_ROOT(f, 2) = lo_string_concat(LO_ROOT(f, 0), LO_ROOT(f, 1));
    if (lo_str_eq_lit(LO_ROOT(f, 2), "hello, w\xC3\xB6rld")) ok_concat++;
    fill(g);
    LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 1), 40);
    { const uint8_t *p = lo_str_bytes(LO_ROOT(f, 2)); _Bool ok = lo_str_len(LO_ROOT(f, 2)) == 240;
      for (uint32_t i = 0; ok && i < 240; i++) ok = p[i] == (uint8_t)"w\xC3\xB6rld"[i % 6];
      if (ok) ok_repeat++; }
    fill(g);
    LO_ROOT(f, 2) = lo_string_reverse(LO_ROOT(f, 1));
    if (lo_str_eq_lit(LO_ROOT(f, 2), "dlr\xC3\xB6w")) ok_reverse++;
    /* operands themselves must be intact after all three */
    if (!lo_str_eq_lit(LO_ROOT(f, 0), "hello, ") || !lo_str_eq_lit(LO_ROOT(f, 1), "w\xC3\xB6rld")) ok_concat -= 1000;
  }
  lo_put_int_line(rounds);
  lo_put_int_line(ok_concat);
  lo_put_int_line(ok_repeat);
  lo_put_int_line(ok_reverse);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
