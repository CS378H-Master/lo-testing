// good test case
// ABI harness: string comparison follows lexicographic UTF-8 byte order; equal strings, a strict prefix, a multi-byte character against an ASCII one, and the empty string are each compared in both directions and only the sign of the result is read
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_string_compare (§3.2) orders by raw UTF-8 bytes. Only
 * the sign of the result is specified, so only the sign is printed. The pairs
 * cover equality of distinct objects, a strict prefix, a multi-byte first byte
 * (0xC3) against 'z' (0x7A), and the empty string against anything, each in
 * both directions. Comparison must not disturb the operands.
 *
 * Expected output: 0 0 -1 1 1 -1 -1 1 0 0, then true.
 */

static int32_t sgn(int32_t x) { return (x > 0) - (x < 0); }

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 6);
  LO_ROOT(f, 0) = lo_str("abc");
  LO_ROOT(f, 1) = lo_str("abc");
  LO_ROOT(f, 2) = lo_str("abcd");
  LO_ROOT(f, 3) = lo_str("\xC3\xA9");  /* é : 0xC3 0xA9 */
  LO_ROOT(f, 4) = lo_str("z");
  LO_ROOT(f, 5) = lo_str("");

  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 0), LO_ROOT(f, 1))));   /* 0  equal, distinct objects */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 0), LO_ROOT(f, 0))));   /* 0  same object */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 0), LO_ROOT(f, 2))));   /* -1 strict prefix is smaller */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 2), LO_ROOT(f, 0))));   /* 1 */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 3), LO_ROOT(f, 4))));   /* 1  0xC3 > 'z' (0x7A): bytes, not codepoints or locale */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 4), LO_ROOT(f, 3))));   /* -1 */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 5), LO_ROOT(f, 0))));   /* -1 empty is smallest */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 0), LO_ROOT(f, 5))));   /* 1 */
  lo_put_int_line(sgn(lo_string_compare(LO_ROOT(f, 5), (Object *)&LO_EMPTY_STRING)));  /* 0  heap-empty vs immortal */
  lo_put_int_line(sgn(lo_string_compare((Object *)&LO_EMPTY_STRING, (Object *)&LO_EMPTY_STRING))); /* 0 */
  /* Comparison allocates nothing and leaves the operands alone. */
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 0), "abc") && lo_str_eq_lit(LO_ROOT(f, 2), "abcd"));
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
