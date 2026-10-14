# P2 spill tests

These programs are the public part of Project 2's **spill tests** (stage 3 of the P2
autograder): they measure how well your register allocator keeps values out of stack
memory. `instructor/` holds programs written by the course staff; `contributed/` holds
programs built from tests contributed by students this term. Both are graded the same way,
1 point per program.

## What each program looks like

Every program reads one integer `n` from stdin, runs a loop body exactly `n` times, prints a
checksum, and returns 0. The loop body is where the register pressure is: many values live
at once, values live across a call or a division, a deeply nested expression, and so on.
The second header line says what each program stresses.

For each program `<stem>.lo` there are three input/expected pairs:

| File | Contents |
|---|---|
| `<stem>.input.<N>.txt` | the loop count `N` (1000, 3000 or 5000) |
| `<stem>.expected.<N>.out` | the exact stdout for that input |

The header carries one extra line beyond the usual ValidPrograms contract:

```
// good test case
// <what the program stresses>
// main method return value: 0
// expected spills: less than N (plus configured slack)
```

## How a spill test is graded

1. `lo-emit-asm` writes your assembly; the grader assembles and links it against the
   runtime, exactly as in the native tier.
2. The executable runs once per input under `valgrind --tool=callgrind`. Wrong output or a
   non-zero exit fails the test (there is no spill verdict for a wrong program).
3. The grader counts the stack loads and stores executed by **your** code (the runtime is
   not counted) in each run. The difference between runs, divided by the difference in `N`,
   is your cost per loop iteration, `s`. Everything outside the loop cancels out.
4. You pass if `s < N + slack`. `N` in the header is the reference compiler's own cost plus
   one, so `s < N` means "as good as the reference"; the slack is an allowance set by the
   course staff and printed with every result.

The result line reads, for example:

```
Passed: spills per loop iteration=21, test bound=28 (reference spills=21, configured slack=6)
```

followed by every instruction of yours that contributed to `s`, with its share per
iteration, so you can see where the stack traffic comes from.

**What counts** as a stack access: loads and stores of stack slots (addressed from `rbp` or
`rsp`, including the red zone), `push`/`pop` of values, saving caller-saved registers around
calls, and static memory used as a spill slot. A read-modify-write such as
`add dword ptr [rbp-8], ecx` counts once as a load and once as a store. **What does not
count**: `call`/`ret`, the frame link (`push rbp` / `leave`), and saving a callee-saved
register on function entry and restoring it before returning. Only instructions that
actually run are counted.

If the grader cannot measure a run reliably (for example, an instruction valgrind cannot
emulate), that test scores 0 with a message saying your instructor will review it by hand.

## Checking a program yourself

You cannot reproduce the spill count without the grader, but you can check correctness:

```sh
lo-emit-asm p2-spill-tests/instructor/L_k4_nocall.lo /tmp/L_k4_nocall.s
as --64 -o /tmp/L_k4_nocall.o /tmp/L_k4_nocall.s
cc -no-pie -o /tmp/L_k4_nocall /tmp/L_k4_nocall.o <your liblo_runtime.a> -lpthread -ldl -lm
/tmp/L_k4_nocall < p2-spill-tests/instructor/L_k4_nocall.input.1000.txt \
    | cmp - p2-spill-tests/instructor/L_k4_nocall.expected.1000.out && echo ok
```
