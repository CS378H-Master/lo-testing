// runtime abort test case
// ABI harness: reading an integer from exhausted input aborts with the documented message and exit status
// expected exit code: 111
// expected stderr substring: lo_read_int: end of input
#include "lo_abi.h"

/*
 * What this checks: the §3.7 read abort. The input sidecar holds one integer;
 * the first read prints it (expected stdout: 5), the second finds no token and
 * must abort with exit 111 and the message above.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  lo_print_int(lo_read_int(), 0); lo_println(0);   /* 5 */
  lo_print_int(lo_read_int(), 0); lo_println(0);   /* aborts: nothing left */
  return 0;
}
