# ABI harness — running the runtime tests against your own `liblo_runtime.a`

These programs test your runtime **without your compiler**. Each one is a small freestanding C
program that links against the runtime archive your skeleton builds, calls the ABI entry points
directly (`lo_alloc`, `lo_gc_collect`, the string and type operations, ...), and prints what it
finds. They are graded on both targets: linked natively against your native `liblo_runtime.a`, and
linked for WebAssembly against your `wasm32-unknown-unknown` archive and run under `wasmrun`.

Everything a program needs from the ABI is declared in `lo_abi.h`; the programs include nothing
else. The header also carries the helpers a program uses to behave like compiled LO code: a shadow
frame per function (`LO_FRAME`, `LO_ROOT`), the write barrier for every pointer store, and
`LO_EMPTY_STRING` for String-typed fields. Read a program alongside Chapter 6 of the language
reference and the two should agree line by line.

## Layout

```
p3-abi-harness/
├── lo_abi.h                  the ABI declarations and codegen helpers every program includes
├── run-harness.sh            compile, link, run and check every program under a directory
├── ValidPrograms/            abi_00 … abi_12: allocation, collection, frames, barrier, strings, type ops
├── RuntimeAbortPrograms/     abi_13, abi_14: the string-repeat and out-of-memory aborts
└── smoke/                    abi_15 … abi_18: printing, reading, the null-receiver and end-of-input aborts
    ├── ValidPrograms/
    └── RuntimeAbortPrograms/
```

The `smoke/` programs exercise entry points the skeleton already provides; they check that your
archive still links and behaves as you change the runtime around them.

## What each program declares

The leading comment block is the same contract the LO test programs use:

```
// good test case                      or   // runtime abort test case
// <one sentence: what is exercised and what must hold>
// main method return value: 0         or   // expected exit code: 120
                                            // expected stderr substring: lo_string_repeat: negative count -3
// gc stress heap: 65536               (optional: the smallest LO_HEAP_SIZE it is calibrated for, native only)
```

A good test case must exit 0 and print exactly `<name>.expected.out`. An abort test must exit with
the declared status and its stderr must contain the declared text; its stdout is the output it
prints before aborting. `<name>.input.txt`, when present, is the program's stdin.

## Running one program natively

```sh
H=path/to/lo-testing/p3-abi-harness        # the directory holding lo_abi.h
RT=path/to/your/runtime/target/release/liblo_runtime.a

cc -std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -I "$H" \
   -c "$H/ValidPrograms/abi_03_collect_graph_roots.c" -o abi_03.o
cc -no-pie -o abi_03 abi_03.o "$RT" -lpthread -ldl -lm
./abi_03 < /dev/null | diff - "$H/ValidPrograms/abi_03_collect_graph_roots.expected.out" && echo PASS
```

For a program with a `gc stress heap` line, run it a second time at that heap:

```sh
LO_HEAP_SIZE=65536 ./abi_04 | diff - "$H/ValidPrograms/abi_04_collect_reclaims.expected.out"
```

For an abort test, check the exit status and stderr instead:

```sh
./abi_13; echo "exit=$?"                   # expect exit=120 and the declared message on stderr
```

## Running one program on WebAssembly

The course container has `clang`, `wasm-ld` and `wasmrun`. The compile is freestanding, so no
sysroot is needed:

```sh
RTW=path/to/your/runtime/target/wasm32-unknown-unknown/release/liblo_runtime.a

clang --target=wasm32 -std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib \
      -I "$H" -c "$H/ValidPrograms/abi_03_collect_graph_roots.c" -o abi_03.wasm.o
wasm-ld --no-entry --export=lo_entry --allow-undefined abi_03.wasm.o "$RTW" -o abi_03.wasm
wasmrun abi_03.wasm < /dev/null | diff - "$H/ValidPrograms/abi_03_collect_graph_roots.expected.out" && echo PASS
```

`LO_HEAP_SIZE` has no effect on WebAssembly: there is no environment, so the heap is always the
default 16 MiB. Every program also passes at the default heap.

## Running them all

`run-harness.sh` does the above for every program under a directory, on one target:

```sh
$H/run-harness.sh native "$RT"  "$H"
$H/run-harness.sh wasm   "$RTW" "$H"
```

It prints one line per program (`PASS <name>`, or `FAIL <name> (<why>)`), and exits non-zero if
any failed. A program that fails to link names the missing symbol: that is an entry point your
archive does not export.

## Writing against the unfinished skeleton

Against a fresh checkout, 11 of the 19 programs here pass: the ones that use only what the skeleton
provides already implemented (allocation, frames, collection, the barrier, reading, the aborts).
The other 8 call a string or type operation, so they fail until you implement those entry points;
the stub's panic shows up as a native abort (exit 134) or a WebAssembly trap. Program 15 is among
them only because it prints a string it first has to build. That is the expected starting state,
and the order in which the failures disappear is a reasonable order of work.
