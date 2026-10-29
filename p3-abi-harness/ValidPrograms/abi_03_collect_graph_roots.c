// good test case
// ABI harness: a hand-built object graph with a shared child, a cycle and a heap-allocated String field survives two forced collections with every payload, descriptor pointer and identity intact, and the immortal empty string does not move
// main method return value: 0
#include "lo_abi.h"

/*
 * What this checks: a collection (§3.4) preserves a reachable graph exactly.
 * Three nodes form a shared child and a cycle; one holds a heap String object
 * (allocated through lo_alloc with the String descriptor, so this program does
 * not depend on the string operations) reachable only through the node. After two forced collections every field
 * must still point at the (possibly moved) object it pointed at before, the
 * shared child must be one object, descriptors and payloads intact, and the
 * immortal empty string must not have moved. Dropping the last reference to a
 * node and collecting again must leave the rest untouched.
 *
 * Expected output: true, 123, then seven more trues.
 */

/* Node { Object hdr; Object* left; Object* right; Object* label; int32 value; } */
typedef struct Node {
  Object hdr;
  Object *left;
  Object *right;
  Object *label; /* a String */
  int32_t value;
} Node;
static const uint32_t node_ptrs[3] = {offsetof(Node, left), offsetof(Node, right), offsetof(Node, label)};
LO_DESC(NODE, "Node", 0, sizeof(Node), node_ptrs, 3);

static Node *new_node(int32_t v) {
  Node *n = (Node *)lo_alloc(&NODE);
  /* String field default: codegen stores LO_EMPTY_STRING after lo_alloc (§3.1). */
  lo_gc_write_barrier(&n->hdr, offsetof(Node, label), (Object *)&LO_EMPTY_STRING);
  n->value = v;
  return n;
}

int32_t lo_entry(void) {
  lo_runtime_init();
  LO_FRAME(f, 4);
  const StringObject *empty_before = &LO_EMPTY_STRING;

  /* Build: a -> left b, right c; b -> left c (shared child), right a (cycle);
   * c.label = "see". Every pointer store goes through the write barrier. */
  LO_ROOT(f, 0) = (Object *)new_node(1);                       /* a */
  LO_ROOT(f, 1) = (Object *)new_node(2);                       /* b */
  LO_ROOT(f, 2) = (Object *)new_node(3);                       /* c */
  LO_ROOT(f, 3) = lo_alloc(&LO_STRING_CLASS);                   /* a heap String of length 0, rooted while stored */
  lo_gc_write_barrier(LO_ROOT(f, 0), offsetof(Node, left), LO_ROOT(f, 1));
  lo_gc_write_barrier(LO_ROOT(f, 0), offsetof(Node, right), LO_ROOT(f, 2));
  lo_gc_write_barrier(LO_ROOT(f, 1), offsetof(Node, left), LO_ROOT(f, 2));
  lo_gc_write_barrier(LO_ROOT(f, 1), offsetof(Node, right), LO_ROOT(f, 0));
  lo_gc_write_barrier(LO_ROOT(f, 2), offsetof(Node, label), LO_ROOT(f, 3));
  LO_ROOT(f, 3) = 0; /* the label is now reachable only through c */

  /* Two forced collections: the graph may move twice. */
  lo_gc_collect();
  lo_gc_collect();

  /* Re-read every node from its root slot before touching it. */
  Node *a = (Node *)LO_ROOT(f, 0), *b = (Node *)LO_ROOT(f, 1), *c = (Node *)LO_ROOT(f, 2);
  lo_put_bool_line(a->hdr.class_descriptor == &NODE && b->hdr.class_descriptor == &NODE && c->hdr.class_descriptor == &NODE);
  lo_put_int_line(a->value * 100 + b->value * 10 + c->value);           /* 123 */
  lo_put_bool_line(a->left == &b->hdr && a->right == &c->hdr);            /* fields follow the roots */
  lo_put_bool_line(b->left == &c->hdr);                                   /* shared child: one copy */
  lo_put_bool_line(b->right == &a->hdr);                                  /* cycle closed */
  lo_put_bool_line(a->label == (Object *)&LO_EMPTY_STRING && b->label == (Object *)&LO_EMPTY_STRING);
  lo_put_bool_line(c->label->class_descriptor == &LO_STRING_CLASS && lo_str_len(c->label) == 0 && c->label != (Object *)&LO_EMPTY_STRING);
  lo_put_bool_line(&LO_EMPTY_STRING == empty_before && LO_EMPTY_STRING.length == 0);

  /* Drop b from the graph: unreachable after the next collection; a and c stay. */
  lo_gc_write_barrier(&a->hdr, offsetof(Node, left), 0);
  LO_ROOT(f, 1) = 0;
  lo_gc_collect();
  a = (Node *)LO_ROOT(f, 0); c = (Node *)LO_ROOT(f, 2);
  lo_put_bool_line(a->left == 0 && a->right == &c->hdr && c->label->class_descriptor == &LO_STRING_CLASS && lo_str_len(c->label) == 0);
  lo_pop_frame();
  lo_runtime_shutdown();
  return 0;
}
