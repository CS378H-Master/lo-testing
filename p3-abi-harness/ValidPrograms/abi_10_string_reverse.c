// good test case
// ABI harness: reversal works on codepoints, not bytes: ASCII, two-, three- and four-byte UTF-8 sequences, the empty string and a single codepoint all reverse correctly and reverse back to the original
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_string_reverse (§3.2) reverses the order of codepoints
 * while keeping each multi-byte sequence intact; a byte-wise reversal fails
 * the 2-, 3- and 4-byte cases. The result is a new String, the source is
 * unchanged, and reversing twice returns the original bytes.
 *
 * Expected output: seven trues, then "éfac".
 */

static _Bool is_str(Object *s) { return s != 0 && s->class_descriptor == &LO_STRING_CLASS; }

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 3);
  /* ASCII: a new object with the characters reversed. */
  LO_ROOT(f, 0) = lo_str("abc");
  LO_ROOT(f, 1) = lo_string_reverse(LO_ROOT(f, 0));
  lo_put_bool_line(is_str(LO_ROOT(f, 1)) && lo_str_eq_lit(LO_ROOT(f, 1), "cba") && LO_ROOT(f, 1) != LO_ROOT(f, 0));
  /* a é b : the 2-byte sequence stays intact */
  LO_ROOT(f, 0) = lo_str("a\xC3\xA9" "b");
  LO_ROOT(f, 1) = lo_string_reverse(LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 1), "b\xC3\xA9" "a"));
  /* 3-byte (€ U+20AC) and 4-byte (U+1F600) sequences swap places whole */
  LO_ROOT(f, 0) = lo_str("x\xE2\x82\xAC\xF0\x9F\x98\x80y");
  LO_ROOT(f, 1) = lo_string_reverse(LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_len(LO_ROOT(f, 1)) == 9 && lo_str_eq_lit(LO_ROOT(f, 1), "y\xF0\x9F\x98\x80\xE2\x82\xACx"));
  /* round trip, and the source is untouched */
  LO_ROOT(f, 2) = lo_string_reverse(LO_ROOT(f, 1));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 2), "x\xE2\x82\xAC\xF0\x9F\x98\x80y") && lo_str_eq_lit(LO_ROOT(f, 0), "x\xE2\x82\xAC\xF0\x9F\x98\x80y"));
  /* empty, heap and immortal */
  LO_ROOT(f, 1) = lo_string_reverse(lo_str(""));
  lo_put_bool_line(is_str(LO_ROOT(f, 1)) && lo_str_len(LO_ROOT(f, 1)) == 0);
  LO_ROOT(f, 1) = lo_string_reverse((Object *)&LO_EMPTY_STRING);
  lo_put_bool_line(is_str(LO_ROOT(f, 1)) && lo_str_len(LO_ROOT(f, 1)) == 0);
  /* one codepoint reverses to itself */
  LO_ROOT(f, 1) = lo_string_reverse(lo_str("\xF0\x9F\x98\x80"));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 1), "\xF0\x9F\x98\x80"));
  LO_ROOT(f, 1) = lo_string_reverse(lo_str("caf\xC3\xA9"));
  lo_print_string(LO_ROOT(f, 1), 0); lo_println(0);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
