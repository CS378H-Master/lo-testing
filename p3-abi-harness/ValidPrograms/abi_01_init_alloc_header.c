// good test case
// ABI harness: the runtime links from C, initialises, stamps a fresh object's header with the descriptor it was allocated with, and exports the immortal empty string with the String descriptor and length zero
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: the smallest possible use of the runtime, the same shape as
 * the skeleton's own tests/c_harness/main.c. Initialise, allocate one object,
 * confirm the header stamp (§3.1) and the LO_EMPTY_STRING export (§2.3), print
 * 42. Any non-zero return before the print pinpoints which check failed.
 *
 * Expected output: 42.
 */

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 1);

  /* lo_alloc must return a non-null object whose header names the descriptor. */
  LO_ROOT(f, 0) = lo_alloc(&LO_INT_BOX_CLASS);
  if (LO_ROOT(f, 0) == 0) return 2;
  if (LO_ROOT(f, 0)->class_descriptor != &LO_INT_BOX_CLASS) return 3;

  /* The immortal empty string is a String of length 0 outside the heap. */
  if (LO_EMPTY_STRING.header.class_descriptor != &LO_STRING_CLASS) return 4;
  if (LO_EMPTY_STRING.length != 0) return 5;

  lo_print_int(42, 0);
  lo_println(0);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
