// runtime abort test case
// ABI harness: repeating a string a negative number of times aborts with the documented message naming the count, and the documented exit status
// expected exit code: 120
// expected stderr substring: lo_string_repeat: negative count -3
#include "lo_abi.h"

/*
 * What this checks: the §3.2 abort path of lo_string_repeat. The grader wants
 * exit status 120 and the message above on stderr (on WASM: the message via the
 * host, then a trap that wasmrun maps to 120). The "1" printed first is the
 * expected stdout; the "2" must never appear.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);
  LO_ROOT(f, 0) = lo_str("ab");
  lo_print_int(1, 0); lo_println(0);
  lo_string_repeat(LO_ROOT(f, 0), -3);   /* aborts here */
  /* not reached */
  lo_print_int(2, 0); lo_println(0);
  lo_pop_frame();
  return 0;
}
