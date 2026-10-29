// good test case
// ABI harness: the read family consumes integers and booleans as whitespace-separated tokens and strings as whole lines, including a blank line, and eof reports end of input only once everything has been consumed
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: the read entry points of §3.7 against the input sidecar
 *   "42\n  -7 true\tfalse\nhello world\n\ncafé\n".
 * Tokens skip whitespace; lo_read_string takes the rest of the current line
 * (empty after the last token), then whole lines including a blank one;
 * lo_eof is false while a line remains and true after it, and reading at
 * end of input returns the empty string without aborting.
 *
 * Expected output: 35, true, 0, hello world, 0, false, true, true, 0.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);
  int32_t a = lo_read_int();          /* 42 */
  int32_t b = lo_read_int();          /* -7, after spaces/newlines */
  _Bool t = lo_read_bool();            /* true */
  _Bool u = lo_read_bool();            /* false */
  lo_put_int_line(a + b);              /* 35 */
  lo_put_bool_line(t && !u);
  /* The rest of the current line after "false" is empty; read_string consumes it. */
  LO_ROOT(f, 0) = lo_read_string();
  lo_put_int_line((int32_t)lo_str_len(LO_ROOT(f, 0)));   /* 0: remainder of the token line */
  LO_ROOT(f, 0) = lo_read_string();                       /* "hello world" */
  lo_print_string(LO_ROOT(f, 0), 0); lo_println(0);
  LO_ROOT(f, 0) = lo_read_string();                       /* blank line */
  lo_put_int_line((int32_t)lo_str_len(LO_ROOT(f, 0)));   /* 0 */
  lo_put_bool_line(lo_eof());                             /* false: one more line */
  LO_ROOT(f, 0) = lo_read_string();                       /* "caf\xC3\xA9" */
  lo_put_bool_line(lo_str_eq_lit(LO_ROOT(f, 0), "caf\xC3\xA9"));
  lo_put_bool_line(lo_eof());                             /* true */
  LO_ROOT(f, 0) = lo_read_string();                       /* at EOF: empty string, no abort */
  lo_put_int_line((int32_t)lo_str_len(LO_ROOT(f, 0)));
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
