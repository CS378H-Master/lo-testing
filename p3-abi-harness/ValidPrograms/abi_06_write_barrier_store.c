// good test case
// ABI harness: the write barrier is the only way compiled code stores a pointer into a heap object, so the value it stores must be readable from the field afterwards, for a heap object, for null and for the immortal empty string, and must survive a forced collection
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_gc_write_barrier(obj, offset, value) (§3.4) performs the
 * store, whatever else it records. Stores of a heap object, the immortal empty
 * string and null are read back from the field; then the stored pointer must
 * follow its target through two collections while it is the only path to it,
 * and replacing the only reference must let the old target be reclaimed.
 *
 * Expected output: seven trues, then 88.
 */

typedef struct Holder { Object hdr; Object *ref; Object *str; int32_t tag; int32_t pad; } Holder;
static const uint32_t holder_ptrs[2] = {offsetof(Holder, ref), offsetof(Holder, str)};
LO_DESC(HOLDER, "Holder", 0, sizeof(Holder), holder_ptrs, 2);
typedef struct Cell { Object hdr; int32_t v; int32_t pad; } Cell;
LO_DESC(CELL, "Cell", 0, sizeof(Cell), 0, 0);

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 2);
  Holder *h = (Holder *)lo_alloc(&HOLDER);
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, str), (Object *)&LO_EMPTY_STRING); /* String field default */
  h->tag = 9;
  LO_ROOT(f, 0) = &h->hdr;

  Cell *c = (Cell *)lo_alloc(&CELL); c->v = 77;
  LO_ROOT(f, 1) = &c->hdr;
  h = (Holder *)LO_ROOT(f, 0);

  /* 1. a heap pointer */
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, ref), &c->hdr);
  lo_put_bool_line(h->ref == &c->hdr);
  /* 2. the immortal, both as the default and as an explicit store */
  lo_put_bool_line(h->str == (Object *)&LO_EMPTY_STRING);
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, str), LO_ROOT(f, 1));
  lo_put_bool_line(h->str == &c->hdr);
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, str), (Object *)&LO_EMPTY_STRING);
  lo_put_bool_line(h->str == (Object *)&LO_EMPTY_STRING);
  /* 3. null */
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, ref), 0);
  lo_put_bool_line(h->ref == 0);
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, ref), &c->hdr);

  /* 4. survives collection: drop the root to c so the field is c's only path,
   * collect twice, and the field must name an intact Cell holding 77. */
  LO_ROOT(f, 1) = 0;
  lo_gc_collect();
  lo_gc_collect();
  h = (Holder *)LO_ROOT(f, 0);
  lo_put_bool_line(h->ref != 0 && h->ref->class_descriptor == &CELL && ((Cell *)h->ref)->v == 77);
  lo_put_bool_line(h->str == (Object *)&LO_EMPTY_STRING && h->tag == 9);
  /* 5. a barrier store that replaces the only reference frees the old target. */
  Cell *d = (Cell *)lo_alloc(&CELL); d->v = 88;
  h = (Holder *)LO_ROOT(f, 0);
  lo_gc_write_barrier(&h->hdr, offsetof(Holder, ref), &d->hdr);
  lo_gc_collect();
  h = (Holder *)LO_ROOT(f, 0);
  lo_put_int_line(((Cell *)h->ref)->v);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
