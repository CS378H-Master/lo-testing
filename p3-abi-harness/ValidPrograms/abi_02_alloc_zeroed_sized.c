// good test case
// ABI harness: objects of several instance sizes come back with every payload byte zero, the header stamped, and storage that does not overlap any other live object
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_alloc honours instance_size (§3.1). Eight objects of
 * four sizes are allocated and kept live; each must be zero beyond the header,
 * 8-byte aligned, stamped with its descriptor, and disjoint from every other.
 * Writing all over the 4096-byte object must leave the others untouched.
 *
 * Expected output: 8, 8, 8, 28, 6.
 */

LO_DESC(C16, "C16", 0, 16, 0, 0);
LO_DESC(C24, "C24", 0, 24, 0, 0);
LO_DESC(C64, "C64", 0, 64, 0, 0);
LO_DESC(C4096, "C4096", 0, 4096, 0, 0);

static const ClassDescriptor *const descs[4] = {&C16, &C24, &C64, &C4096};

static _Bool payload_zero(Object *o, uint32_t size) {
  const uint8_t *p = (const uint8_t *)o;
  for (uint32_t i = sizeof(Object); i < size; i++)
    if (p[i] != 0) return 0;
  return 1;
}

static _Bool disjoint(Object *a, uint32_t sa, Object *b, uint32_t sb) {
  uintptr_t x = (uintptr_t)a, y = (uintptr_t)b;
  return (x + sa <= y) || (y + sb <= x);
}

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 8);
  int32_t zero_ok = 0, header_ok = 0, pairs_ok = 0, aligned_ok = 0;

  /* Two of each size, interleaved, all rooted so none may be reclaimed. */
  for (int i = 0; i < 8; i++) {
    const ClassDescriptor *d = descs[i % 4];
    Object *o = lo_alloc(d);
    LO_ROOT(f, i) = o;
    if (o->class_descriptor == d) header_ok++;
    if (payload_zero(o, d->instance_size)) zero_ok++;
    if (((uintptr_t)o % 8) == 0) aligned_ok++;
  }
  /* Every pair of the 8 objects must occupy disjoint storage: 28 pairs. */
  for (int i = 0; i < 8; i++)
    for (int j = i + 1; j < 8; j++)
      if (disjoint(LO_ROOT(f, i), descs[i % 4]->instance_size, LO_ROOT(f, j), descs[j % 4]->instance_size)) pairs_ok++;

  lo_put_int_line(header_ok);  /* 8  */
  lo_put_int_line(zero_ok);    /* 8  */
  lo_put_int_line(aligned_ok); /* 8  */
  lo_put_int_line(pairs_ok);   /* 28 */

  /* Writing into one object must not disturb another: fill the first 4096-byte
   * object, then the six objects that are not 4096-byte must still be zero. */
  uint8_t *big = (uint8_t *)LO_ROOT(f, 3);
  for (uint32_t i = sizeof(Object); i < 4096; i++) big[i] = (uint8_t)i;
  int32_t still_zero = 0;
  for (int i = 0; i < 8; i++)
    if (i != 3 && i != 7 && payload_zero(LO_ROOT(f, i), descs[i % 4]->instance_size)) still_zero++;
  lo_put_int_line(still_zero);  /* 6 */
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
