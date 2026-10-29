// runtime abort test case
// ABI harness: at a 64 KiB heap a reachable chain grows until no collection can make room; the allocator must abort with the documented message and exit status rather than return null or corrupt the heap
// expected exit code: 137
// expected stderr substring: lo_alloc: out of memory
// gc stress heap: 65536
#include "lo_abi.h"

/*
 * What this checks: the §3.1 out-of-memory contract. First a hundred heaps of
 * garbage prove the collector reclaims (a runtime that never frees aborts
 * before printing "1"); then a chain in which every node is reachable from the
 * root grows until a full collection cannot make room. The only acceptable end
 * is exit 137 with the message above; a null return prints -1 and exits 3.
 *
 * Expected stdout before the abort: 1.
 */

typedef struct Link { Object hdr; Object *next; int32_t v; int32_t pad; } Link;
static const uint32_t link_ptrs[1] = {offsetof(Link, next)};
LO_DESC(LINK, "Link", 0, sizeof(Link), link_ptrs, 1);

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);
  LO_ROOT(f, 0) = 0;
  /* First prove collection works: 100 heaps of garbage with an empty root set. */
  for (int i = 0; i < 100 * 65536 / 32; i++) { Link *g = (Link *)lo_alloc(&LINK); g->v = i; }
  lo_print_int(1, 0); lo_println(0);
  /* Then grow a reachable chain forever. Every node is reachable from the root. */
  for (int32_t i = 0;; i++) {
    Link *n = (Link *)lo_alloc(&LINK);
    if (n == 0) { lo_print_int(-1, 0); lo_println(0); return 3; }
    lo_gc_write_barrier(&n->hdr, offsetof(Link, next), LO_ROOT(f, 0));
    n->v = i;
    LO_ROOT(f, 0) = &n->hdr;
  }
}
