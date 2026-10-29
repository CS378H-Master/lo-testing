// good test case
// ABI harness: strings are built from C byte buffers, including the empty buffer and four-byte UTF-8 codepoints, and concatenated in every combination with the empty string; each result carries the String descriptor, the exact length and the exact bytes
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: lo_string_new copies the caller's bytes into a String
 * object (§2.2, §3.2) and lo_string_concat produces a new String with the
 * operands' bytes in order. Edge cases: the empty buffer, a 4-byte UTF-8
 * sequence treated as opaque bytes, the caller's buffer mutated after the
 * call, and the empty string as either operand (heap-allocated or immortal).
 *
 * Expected output: eleven trues, then "hellohello".
 */

static const char hi[] = "hello";
static const char emoji[] = "a\xF0\x9F\x98\x80z"; /* a, U+1F600 (4 bytes), z */

static _Bool is_str(Object *s) { return s != 0 && s->class_descriptor == &LO_STRING_CLASS; }

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 4);
  /* lo_string_new: class, length and bytes. */
  LO_ROOT(f, 0) = lo_string_new((const uint8_t *)hi, 5);
  LO_ROOT(f, 1) = lo_string_new((const uint8_t *)emoji, 6);
  LO_ROOT(f, 2) = lo_string_new((const uint8_t *)"", 0);
  lo_put_bool_line(is_str(LO_ROOT(f, 0)) && lo_str_len(LO_ROOT(f, 0)) == 5 && lo_str_eq_lit(LO_ROOT(f, 0), "hello"));
  lo_put_bool_line(is_str(LO_ROOT(f, 1)) && lo_str_len(LO_ROOT(f, 1)) == 6 && lo_str_bytes(LO_ROOT(f, 1))[1] == 0xF0 && lo_str_bytes(LO_ROOT(f, 1))[5] == 'z');
  lo_put_bool_line(is_str(LO_ROOT(f, 2)) && lo_str_len(LO_ROOT(f, 2)) == 0);
  /* The source buffer is the caller's: mutating it afterwards changes nothing. */
  char buf[4] = {'a', 'b', 'c', 0};
  LO_ROOT(f, 3) = lo_string_new((const uint8_t *)buf, 3);
  buf[0] = 'X';
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 3), "abc"));

  /* lo_string_concat: both orders, multi-byte bytes kept intact. */
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 0), LO_ROOT(f, 1));
  lo_put_bool_line(is_str(LO_ROOT(f, 3)) && lo_str_len(LO_ROOT(f, 3)) == 11 && lo_str_eq_lit(LO_ROOT(f, 3), "helloa\xF0\x9F\x98\x80z"));
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 1), LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 3), "a\xF0\x9F\x98\x80zhello"));
  /* Empty operands, both heap-empty and the immortal. */
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 0), LO_ROOT(f, 2));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 3), "hello"));
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 2), LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 3), "hello"));
  LO_ROOT(f, 3) = lo_string_concat((Object *)&LO_EMPTY_STRING, LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 3), "hello"));
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 2), (Object *)&LO_EMPTY_STRING);
  lo_put_bool_line(is_str(LO_ROOT(f, 3)) && lo_str_len(LO_ROOT(f, 3)) == 0);
  /* Operands are unchanged by concatenation and the result is a new object. */
  LO_ROOT(f, 3) = lo_string_concat(LO_ROOT(f, 0), LO_ROOT(f, 0));
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 0), "hello") && LO_ROOT(f, 3) != LO_ROOT(f, 0) && lo_str_eq_lit(LO_ROOT(f, 3), "hellohello"));
  lo_print_string(LO_ROOT(f, 3), 0); lo_println(0);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
