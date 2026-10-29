// good test case
// ABI harness: repetition by zero, one and a thousand, of a plain string, a multi-byte string and the empty string, each checked for the String descriptor, the exact length and the exact bytes
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_string_repeat(s, n) (§3.2) for n = 0, 1, 333 and 1000,
 * on an ASCII string, a string whose byte length differs from its codepoint
 * count, and the empty string. The result must be a new String whose bytes
 * are exactly n copies of the source; the source must be unchanged.
 *
 * Expected output: five trues, "ababab", true.
 */

static _Bool is_str(Object *s) { return s != 0 && s->class_descriptor == &LO_STRING_CLASS; }

/* True iff r is exactly n copies of the C string unit. */
static _Bool repeats_of(Object *r, const char *unit, uint32_t n) {
  uint32_t ul = lo_cstrlen(unit);
  if (!is_str(r) || lo_str_len(r) != ul * n) return 0;
  const uint8_t *p = lo_str_bytes(r);
  for (uint32_t i = 0; i < ul * n; i++)
    if (p[i] != (uint8_t)unit[i % ul]) return 0;
  return 1;
}

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 3);
  LO_ROOT(f, 0) = lo_str("ab");
  LO_ROOT(f, 1) = lo_str("\xC3\xA9!"); /* é! : 3 bytes, 2 codepoints */

  LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 0), 0);        /* zero copies: an empty String */
  lo_put_bool_line(is_str(LO_ROOT(f, 2)) && lo_str_len(LO_ROOT(f, 2)) == 0);
  LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 0), 1);        /* one copy, but a new object */
  lo_put_bool_line(repeats_of(LO_ROOT(f, 2), "ab", 1) && LO_ROOT(f, 2) != LO_ROOT(f, 0));
  LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 0), 1000);
  lo_put_bool_line(repeats_of(LO_ROOT(f, 2), "ab", 1000));
  LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 1), 333);      /* multi-byte unit, odd count */
  lo_put_bool_line(repeats_of(LO_ROOT(f, 2), "\xC3\xA9!", 333));
  LO_ROOT(f, 2) = lo_string_repeat((Object *)&LO_EMPTY_STRING, 1000); /* the immortal as source */
  lo_put_bool_line(is_str(LO_ROOT(f, 2)) && lo_str_len(LO_ROOT(f, 2)) == 0);
  LO_ROOT(f, 2) = lo_string_repeat(LO_ROOT(f, 0), 3);
  lo_print_string(LO_ROOT(f, 2), 0); lo_println(0);
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 0), "ab"));      /* source untouched */
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
