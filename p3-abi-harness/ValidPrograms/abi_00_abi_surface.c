// good test case
// ABI harness: every entry point and exported static the ABI names must be present in the runtime archive; a missing symbol fails the link, and the program prints how many it referenced
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: the archive exports the complete ABI surface of
 * runtime-abi.md §2.3 and §3. The test is really the link step: a missing
 * entry point is an undefined symbol and the program never runs. What does run
 * confirms the three descriptors are distinct, named and parentless.
 *
 * Expected output: 23, 4, true, true, true.
 */

typedef void (*fn_t)(void);

/* Taking the address of each symbol forces the linker to resolve it. The table
 * is volatile so the references cannot be optimised away. */
static volatile fn_t table[32];
static volatile const void *statics[4];

int32_t lo_entry(void) {
  /* One reference per entry point, §3.1 through §3.8. */
  int32_t n = 0;
  table[n++] = (fn_t)lo_runtime_init;
  table[n++] = (fn_t)lo_runtime_shutdown;
  table[n++] = (fn_t)lo_alloc;
  table[n++] = (fn_t)lo_string_new;
  table[n++] = (fn_t)lo_string_concat;
  table[n++] = (fn_t)lo_string_repeat;
  table[n++] = (fn_t)lo_string_compare;
  table[n++] = (fn_t)lo_string_reverse;
  table[n++] = (fn_t)lo_push_frame;
  table[n++] = (fn_t)lo_pop_frame;
  table[n++] = (fn_t)lo_gc_write_barrier;
  table[n++] = (fn_t)lo_gc_collect;
  table[n++] = (fn_t)lo_cast_check;
  table[n++] = (fn_t)lo_instanceof;
  table[n++] = (fn_t)lo_print_int;
  table[n++] = (fn_t)lo_print_bool;
  table[n++] = (fn_t)lo_print_string;
  table[n++] = (fn_t)lo_println;
  table[n++] = (fn_t)lo_read_int;
  table[n++] = (fn_t)lo_read_bool;
  table[n++] = (fn_t)lo_read_string;
  table[n++] = (fn_t)lo_eof;
  table[n++] = (fn_t)lo_abort_null_receiver;
  /* The four exported statics, §2.3. */
  int32_t s = 0;
  statics[s++] = &LO_STRING_CLASS;
  statics[s++] = &LO_INT_BOX_CLASS;
  statics[s++] = &LO_BOOL_BOX_CLASS;
  statics[s++] = &LO_EMPTY_STRING;

  lo_runtime_init();
  lo_print_int(n, 0);
  lo_println(0);
  lo_print_int(s, 0);
  lo_println(0);
  /* The descriptors must be distinct objects with their documented names. */
  lo_print_bool(&LO_STRING_CLASS != &LO_INT_BOX_CLASS && &LO_INT_BOX_CLASS != &LO_BOOL_BOX_CLASS, 0);
  lo_println(0);
  lo_print_bool(LO_STRING_CLASS.name_len == 6 && LO_STRING_CLASS.name[0] == 'S' && LO_STRING_CLASS.name[5] == 'g', 0);
  lo_println(0);
  lo_print_bool(LO_STRING_CLASS.parent == 0 && LO_INT_BOX_CLASS.parent == 0, 0);
  lo_println(0);
  lo_runtime_shutdown();
  return 0;
}
