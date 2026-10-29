// runtime abort test case
// ABI harness: the null-receiver helper compiled code calls before a dispatch aborts with the documented message naming the method and the documented exit status
// expected exit code: 102
// expected stderr substring: lo_abort_null_receiver: cannot dispatch foo
#include "lo_abi.h"

/*
 * What this checks: lo_abort_null_receiver (§3.8), which compiled code calls
 * when a method receiver is null. It must write the message naming the method
 * and exit 102 (trap on WASM). The "1" printed first is the expected stdout.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  lo_print_int(1, 0); lo_println(0);
  lo_abort_null_receiver((const uint8_t *)"foo", 3);   /* never returns */
}
