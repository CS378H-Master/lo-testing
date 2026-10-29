/*
 * lo_abi.h — the LO runtime ABI as a C client sees it, plus the helpers every
 * harness program uses to behave like compiled LO code.
 *
 * Spec of record: runtime-abi.md (§2 layouts and exports, §3 entry points,
 * §4.2 linkage). This header declares only what the ABI publishes; it includes
 * no runtime-internal header and no libc. Programs that include it are
 * freestanding: compile with -ffreestanding -nostdlib -fno-builtin, natively
 * with cc and for WASM with clang --target=wasm32.
 *
 * A harness program is a model code generator. It must do what compiled LO
 * code does: call lo_runtime_init first, keep every live heap pointer in a
 * shadow-frame root slot across every call that may collect, leave dead slots
 * null, pop its frame on exit, store heap pointers into heap objects only
 * through lo_gc_write_barrier, and initialise String-typed fields to
 * LO_EMPTY_STRING after lo_alloc. The LO_FRAME / LO_ROOT helpers below exist
 * so each program states those obligations explicitly.
 */
#ifndef LO_ABI_H
#define LO_ABI_H

#include <stddef.h>
#include <stdint.h>

/* ------------------------------------------------------------------------ */
/* §2 Object layout                                                          */
/* ------------------------------------------------------------------------ */

typedef struct ClassDescriptor ClassDescriptor;

typedef struct Object {
  const ClassDescriptor *class_descriptor;
  uint32_t gc_bits;
  uint32_t flags;
} Object;

struct ClassDescriptor {
  const char *name;              /* UTF-8, null-terminated */
  uint32_t name_len;
  const ClassDescriptor *parent; /* null at the root of the hierarchy */
  uint32_t instance_size;        /* bytes, header included */
  const uint32_t *pointer_offsets;
  uint32_t pointer_count;
  uint32_t vtable_size;
  const void *const *vtable;
};

typedef struct StringObject {
  Object header; /* class_descriptor == &LO_STRING_CLASS */
  uint32_t length;
  /* `length` UTF-8 bytes follow inline at LO_STRING_DATA_OFFSET */
} StringObject;

#define LO_STRING_DATA_OFFSET (offsetof(StringObject, length) + sizeof(uint32_t))

/* Layout locks: 16-byte header / data at 20 on 64-bit, 12 / 16 on wasm32. */
#if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
_Static_assert(sizeof(Object) == 16, "Object is 16 bytes on 64-bit");
_Static_assert(LO_STRING_DATA_OFFSET == 20, "string data starts at 20 on 64-bit");
#else
_Static_assert(sizeof(Object) == 12, "Object is 12 bytes on wasm32");
_Static_assert(LO_STRING_DATA_OFFSET == 16, "string data starts at 16 on wasm32");
#endif

/* §2.3 Exports. LO_EMPTY_STRING is the object itself, not a pointer to it. */
extern const ClassDescriptor LO_STRING_CLASS;
extern const ClassDescriptor LO_INT_BOX_CLASS;
extern const ClassDescriptor LO_BOOL_BOX_CLASS;
extern const StringObject LO_EMPTY_STRING;

/* ------------------------------------------------------------------------ */
/* §3 Entry points                                                           */
/* ------------------------------------------------------------------------ */

extern void lo_runtime_init(void);
extern void lo_runtime_shutdown(void);

extern Object *lo_alloc(const ClassDescriptor *cls);

extern Object *lo_string_new(const uint8_t *bytes, uint32_t len);
extern Object *lo_string_concat(Object *a, Object *b);
extern Object *lo_string_repeat(Object *s, int32_t n);
extern int32_t lo_string_compare(Object *a, Object *b);
extern Object *lo_string_reverse(Object *s);

typedef struct ShadowFrame {
  struct ShadowFrame *parent;
  uint32_t num_roots;
  Object *roots[];
} ShadowFrame;

extern void lo_push_frame(ShadowFrame *frame);
extern void lo_pop_frame(void);

extern void lo_gc_write_barrier(Object *obj, uint32_t field_offset, Object *value);
extern void lo_gc_collect(void);

extern Object *lo_cast_check(Object *obj, const ClassDescriptor *target);
extern _Bool lo_instanceof(Object *obj, const ClassDescriptor *target);

extern void lo_print_int(int32_t n, int32_t to_stderr);
extern void lo_print_bool(_Bool b, int32_t to_stderr);
extern void lo_print_string(Object *s, int32_t to_stderr);
extern void lo_println(int32_t to_stderr);

extern int32_t lo_read_int(void);
extern _Bool lo_read_bool(void);
extern Object *lo_read_string(void);
extern _Bool lo_eof(void);

extern void lo_abort_null_receiver(const uint8_t *method_name, uint32_t method_name_len)
    __attribute__((noreturn));

/* ------------------------------------------------------------------------ */
/* Entry. Every program defines lo_entry; its result is the exit status.     */
/* On WASM wasmrun calls the exported lo_entry; natively main forwards.      */
/* ------------------------------------------------------------------------ */

int32_t lo_entry(void);

#ifndef __wasm__
int main(void) { return (int)lo_entry(); }
#endif

/* ------------------------------------------------------------------------ */
/* Shadow-frame helpers (codegen obligation §3.3)                            */
/* ------------------------------------------------------------------------ */

/* LO_FRAME(name, N): declare and push a frame with N root slots, all null.
 * The struct has the ShadowFrame prefix, so the cast is layout-exact.        */
#define LO_FRAME(name, N)                                         \
  struct {                                                        \
    ShadowFrame *parent;                                          \
    uint32_t num_roots;                                           \
    Object *roots[(N) > 0 ? (N) : 1];                             \
  } name = {0, (N), {0}};                                         \
  lo_push_frame((ShadowFrame *)&name)

/* Root slot access: LO_ROOT(frame, i) is an lvalue of type Object*. Keep a
 * live pointer there across every call; read it back after the call.        */
#define LO_ROOT(name, i) ((name).roots[(i)])

/* ------------------------------------------------------------------------ */
/* Descriptor helpers                                                        */
/* ------------------------------------------------------------------------ */

/* LO_DESC(ident, cname, parentp, size, offsets, count): a static descriptor
 * with no methods. `offsets` is a static const uint32_t[] or NULL.          */
#define LO_DESC(ident, cname, parentp, size, offsets, count)              \
  static const ClassDescriptor ident = {                                   \
      cname, (uint32_t)(sizeof(cname) - 1), (parentp), (uint32_t)(size), \
      (offsets), (uint32_t)(count), 0, (const void *const *)0}

/* ------------------------------------------------------------------------ */
/* Byte helpers (no libc)                                                    */
/* ------------------------------------------------------------------------ */

static inline uint32_t lo_cstrlen(const char *s) {
  uint32_t n = 0;
  while (s[n] != 0) n++;
  return n;
}

static inline uint8_t *lo_str_bytes(Object *s) {
  return (uint8_t *)s + LO_STRING_DATA_OFFSET;
}

static inline uint32_t lo_str_len(Object *s) {
  return ((StringObject *)s)->length;
}

/* A fresh heap string from a C literal. Allocates: a safepoint.             */
static inline Object *lo_str(const char *lit) {
  return lo_string_new((const uint8_t *)lit, lo_cstrlen(lit));
}

/* True iff heap string `s` holds exactly the bytes of C string `lit`.       */
static inline _Bool lo_str_eq_lit(Object *s, const char *lit) {
  uint32_t n = lo_cstrlen(lit);
  if (lo_str_len(s) != n) return 0;
  const uint8_t *p = lo_str_bytes(s);
  for (uint32_t i = 0; i < n; i++)
    if (p[i] != (uint8_t)lit[i]) return 0;
  return 1;
}

/* Print a C literal on stdout. Allocates a heap string: a safepoint.        */
static inline void lo_puts(const char *lit) {
  lo_print_string(lo_str(lit), 0);
  lo_println(0);
}

/* Print "label: value\n" without allocating (label is printed as an int tag
 * by the caller); kept trivial so GC-sensitive programs can report safely.  */
static inline void lo_put_int_line(int32_t v) {
  lo_print_int(v, 0);
  lo_println(0);
}

static inline void lo_put_bool_line(_Bool b) {
  lo_print_bool(b, 0);
  lo_println(0);
}

#endif /* LO_ABI_H */
