# P3 GC stress tests

These programs are the public part of Project 3's **GC stress tests**: ordinary LO programs
written to put your collector under pressure. They allocate far more than the heap holds, keep
structures alive across many collections (lists, shared children, cycles, back pointers, deep
recursion, many roots per frame), build and compare strings while collections move their
operands, cast and test types after objects have moved, and store young objects into old ones
through the write barrier. Every program prints values that depend only on what the program
means (counts, sums, wrapping checksums, identity results, string equality), never on addresses
or timing, so any correct collector produces the same output.

```
p3-gc-stress-tests/
├── LO-3/ValidPrograms/    stress_01 … stress_15, stress_19 … stress_22
└── LO-4/ValidPrograms/    stress_16 … stress_18 (inheritance, instanceof and casts after moves)
```

Numbering is roughly in order of difficulty.

## What each program declares

The header is the usual ValidPrograms contract plus one line:

```
// good test case
// <what the program stresses>
// main method return value: 0
// gc stress heap: <bytes>
```

`gc stress heap` is the smallest `LO_HEAP_SIZE` the program is meant to pass at, between 64 KiB
and 16 MiB. Each program's peak live data is at most a quarter of that heap, so a collector that
can only use half of its heap at a time (a two-space copying collector) still has room. Every
program also passes at the default 16 MiB heap, and allocates at least 64 MiB in total, so it
collects many times even there.

`<stem>.expected.out` is the exact stdout; `<stem>.input.txt`, when present, is the program's
stdin.

## How a stress test is graded

Each program is run in three configurations:

1. compiled with `lo-compile` and run under `wasmrun`, at the default heap (a WebAssembly module
   has no environment, so `LO_HEAP_SIZE` cannot reach it);
2. emitted with `lo-emit-asm`, assembled and linked against your native `liblo_runtime.a`, and run
   at the default heap;
3. the same executable run again with `LO_HEAP_SIZE` set to the header's `gc stress heap`.

The test passes only if all three exit with the declared status and print exactly the expected
output. A failure names the first configuration that failed, for example
`native @ LO_HEAP_SIZE=65536: expected exit status 0, but the program crashed: SIGSEGV`.

At the default heap a program collects far less often than at its stress heap, so some bugs only
show up in the third configuration: a missed root, a field not updated after a move, or a string
operation that reads an operand after its own allocation collected.

## Running a program yourself

On WebAssembly, through your contract:

```sh
source p3-grading-contract.sh            # your filled-in contract, after lo-build
P=path/to/lo-testing/p3-gc-stress-tests/LO-3/ValidPrograms/stress_02_live_list_survives
lo-compile "$P.lo" /tmp/stress_02.wasm
lo-wasmrun /tmp/stress_02.wasm < /dev/null | cmp - "$P.expected.out" && echo ok
```

Natively, at both heaps:

```sh
lo-emit-asm "$P.lo" /tmp/stress_02.s
as --64 -o /tmp/stress_02.o /tmp/stress_02.s
cc -no-pie -o /tmp/stress_02 /tmp/stress_02.o "$LO_RUNTIME_NATIVE_LIB" -lpthread -ldl -lm
/tmp/stress_02 < /dev/null | cmp - "$P.expected.out" && echo ok
LO_HEAP_SIZE=$(sed -n 's|^// gc stress heap: ||p' "$P.lo") /tmp/stress_02 < /dev/null \
    | cmp - "$P.expected.out" && echo ok
```

Use the program's `.input.txt` instead of `/dev/null` when it has one. A smaller `LO_HEAP_SIZE`
than the header's is a good way to make a collector bug show up sooner; the programs are not
guaranteed to fit below it.
