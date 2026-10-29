// good test case
// ABI harness: the print family writes integers including the extremes, booleans, strings and newlines to standard output, and anything sent through the stderr selector must not appear on standard output
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: the print entry points of §3.7, which the skeleton provides
 * but a team can break. Integers at both extremes, two prints on one line with
 * no separator, booleans, a multi-byte string, the immortal empty string, and
 * blank lines. Everything sent with to_stderr = 1 must stay off stdout, which
 * is the only stream compared.
 *
 * Expected output: 0, -1, 2147483647, -2147483648, 12-34, true, false,
 * "café 😀", an empty line, two empty lines, end.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);
  lo_print_int(0, 0); lo_println(0);
  lo_print_int(-1, 0); lo_println(0);
  lo_print_int(2147483647, 0); lo_println(0);
  lo_print_int((int32_t)(-2147483647 - 1), 0); lo_println(0);
  lo_print_int(12, 0); lo_print_int(-34, 0); lo_println(0);  /* no separator: "12-34" */
  lo_print_bool(1, 0); lo_println(0);
  lo_print_bool(0, 0); lo_println(0);
  LO_ROOT(f, 0) = lo_str("caf\xC3\xA9 \xF0\x9F\x98\x80");
  lo_print_string(LO_ROOT(f, 0), 0); lo_println(0);
  lo_print_string((Object *)&LO_EMPTY_STRING, 0); lo_println(0);     /* an empty line */
  lo_print_string(lo_str("to stderr"), 1); lo_println(1);           /* must not reach stdout */
  lo_print_int(99, 1); lo_println(1);
  lo_print_bool(1, 1); lo_println(1);
  lo_println(0); lo_println(0);                                     /* two empty lines */
  lo_print_string(lo_str("end"), 0); lo_println(0);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
