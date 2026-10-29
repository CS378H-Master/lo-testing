// good test case
// ABI harness: instanceof and cast walk a hand-built descriptor chain five deep: up the chain is true, down and across is false, a cast that succeeds returns the very same pointer, and a null object short-circuits both operations
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_instanceof and lo_cast_check (§3.5) walk the parent
 * chain of the object's descriptor. A <- B <- C <- D <- E is the chain and X
 * is a second child of A. An E is an instance of all five; a B is not a C, E
 * or X; a successful cast returns the very pointer it was given; null is false
 * for instanceof and passes through cast unchanged. The runtime's own String
 * descriptor takes part like any other.
 *
 * Expected output: 5, 0, then six trues.
 */

LO_DESC(A, "A", 0, sizeof(Object), 0, 0);
LO_DESC(B, "B", &A, sizeof(Object), 0, 0);
LO_DESC(C, "C", &B, sizeof(Object), 0, 0);
LO_DESC(D, "D", &C, sizeof(Object), 0, 0);
LO_DESC(E, "E", &D, sizeof(Object), 0, 0);
LO_DESC(X, "X", &A, sizeof(Object), 0, 0); /* sibling branch under A */

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 3);
  LO_ROOT(f, 0) = lo_alloc(&E); /* deepest */
  LO_ROOT(f, 1) = lo_alloc(&B);
  LO_ROOT(f, 2) = lo_alloc(&X);

  /* Up the chain: an E is an E, D, C, B and A. */
  int32_t up = 0;
  up += lo_instanceof(LO_ROOT(f, 0), &E) ? 1 : 0;
  up += lo_instanceof(LO_ROOT(f, 0), &D) ? 1 : 0;
  up += lo_instanceof(LO_ROOT(f, 0), &C) ? 1 : 0;
  up += lo_instanceof(LO_ROOT(f, 0), &B) ? 1 : 0;
  up += lo_instanceof(LO_ROOT(f, 0), &A) ? 1 : 0;
  lo_put_int_line(up);                                                  /* 5 */
  /* Down and across: never. */
  int32_t down = 0;
  down += lo_instanceof(LO_ROOT(f, 1), &C) ? 1 : 0;
  down += lo_instanceof(LO_ROOT(f, 1), &E) ? 1 : 0;
  down += lo_instanceof(LO_ROOT(f, 1), &X) ? 1 : 0;   /* sibling */
  down += lo_instanceof(LO_ROOT(f, 2), &B) ? 1 : 0;   /* across */
  down += lo_instanceof(LO_ROOT(f, 0), &X) ? 1 : 0;
  lo_put_int_line(down);                                                /* 0 */
  lo_put_bool_line(lo_instanceof(LO_ROOT(f, 2), &A) && lo_instanceof(LO_ROOT(f, 2), &X));
  /* null: instanceof false, cast returns null, no abort */
  lo_put_bool_line(!lo_instanceof(0, &A) && lo_cast_check(0, &E) == 0);
  /* casts that succeed return the same pointer */
  Object *e = LO_ROOT(f, 0);
  lo_put_bool_line(lo_cast_check(e, &A) == e && lo_cast_check(e, &C) == e && lo_cast_check(e, &E) == e);
  lo_put_bool_line(lo_cast_check(LO_ROOT(f, 1), &A) == LO_ROOT(f, 1) && lo_cast_check(LO_ROOT(f, 2), &X) == LO_ROOT(f, 2));
  /* the runtime's own descriptors take part too */
  LO_ROOT(f, 1) = lo_str("s");
  lo_put_bool_line(lo_instanceof(LO_ROOT(f, 1), &LO_STRING_CLASS) && !lo_instanceof(LO_ROOT(f, 1), &A) && !lo_instanceof(LO_ROOT(f, 0), &LO_STRING_CLASS));
  lo_put_bool_line(lo_instanceof((Object *)&LO_EMPTY_STRING, &LO_STRING_CLASS) && lo_cast_check((Object *)&LO_EMPTY_STRING, &LO_STRING_CLASS) == (Object *)&LO_EMPTY_STRING);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
