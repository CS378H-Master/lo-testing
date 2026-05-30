# CS 378H — LO Conformance Test Suite

**Fall 2026.**

This suite verifies that a student's LO toolchain — parser, type checker, codegen — implements the LO language references correctly. It covers LO-2, LO-3 (redesigned), and LO-4. Each test is a self-contained LO program with a contract declared in its header comment.

The instructor runs the suite against student submissions for correctness grading; teams run it locally during development. The suite is independent of the runtime-skeleton tests in the `lo-runtime` repo's `tests/lo_programs/`; those exercise the runtime against a known-correct compiler. This suite exercises a student's compiler against a known-correct runtime.

## Directory structure

```
lo-testing/
├── README.md                       # this document
├── error-codes.md                  # the compile-error category vocabulary
├── LO-2/
│   ├── ValidPrograms/              # compile, run, exit with declared code
│   └── InvalidPrograms/            # fail to compile
├── LO-3/
│   ├── ValidPrograms/
│   ├── InvalidPrograms/
│   └── RuntimeAbortPrograms/       # compile, run, abort with declared signal
└── LO-4/
    ├── ValidPrograms/
    ├── InvalidPrograms/
    └── RuntimeAbortPrograms/
```

LO-2 has two categories (Valid / Invalid) because LO-2 has no runtime mechanisms that can abort by design. LO-3 and LO-4 carry the third category for runtime aborts — `lo_cast_check` failure (LO-4), null-receiver method call, malformed `read_int` input, and similar.

There is no LO-1 directory; the LO-1 grammar subset is implicitly exercised by the simpler LO-2 programs that use only LO-1 features. The LO-2 tests themselves are scaffolding for students who want or need an intermediate checkpoint at the procedural-only language level (no classes, just top-level functions) — useful during P1 front-end work before LO-3 features are wired in. The LO-2 corpus is preserved as-is; the active conformance work under WS-4 is LO-3 and LO-4.

## Test naming

`test_<N>_<optional_descriptor>.lo`. The number `<N>` is sequential within a category directory and is the primary identifier (handouts and grading scripts refer to tests by number). The optional descriptor is a short snake_case phrase summarizing what the test exercises — useful for `git log`, file-tree scanning, and discussion.

Existing tests with numeric-only names (`test_32.lo`, `test_38.lo`) are preserved unchanged so course handouts and external references remain valid. New tests added in this round and going forward use the underscored-descriptor form.

## Header-comment contract (hybrid)

Every test's contract is declared in its leading comments. The harness parses the header before compiling. Three header formats, one per category.

### ValidPrograms

```
// good test case
// <one-line description>
// main method return value: <N>
// Author: <name> (<offering>)
```

The harness compiles the program, runs the binary, and verifies the exit code equals `<N>`. Where the test exercises IO, an `<test_name>.expected.out` file lives alongside `<test_name>.lo`; the harness additionally byte-matches the program's stdout against that file. Where the test reads from stdin, an `<test_name>.input.txt` file lives alongside; the harness pipes it to the program's stdin. Both auxiliary files are optional: presence signals "match this"; absence signals "no stdout / stdin check."

The entry-point convention is `int Main.main()` for LO-3 and LO-4 (and top-level `int main()` for LO-2). The integer return is the test's success signal; tests typically choose a memorable non-zero value (line number, identifier-derived hash) for the success path and distinct values for failure paths, so a wrong exit code points at which path executed.

### InvalidPrograms

```
// bad test case
// <one-line description>
// expected compile error: <E_CODE>
// Author: <name> (<offering>)
```

The harness compiles the program and expects compilation to fail. The student compiler's stderr must contain the literal string `<E_CODE>` somewhere in its output. Categories are documented in `error-codes.md`; student compilers emit the code alongside their human-readable diagnostic message in whatever format the compiler prefers. The substring match keeps the suite portable across compilers with different diagnostic styles while ensuring the right category of error is being detected.

Existing InvalidPrograms tests that predate the error-code vocabulary have no `expected compile error:` line; the harness treats their contract as "compilation must fail" without an error-code check. Adding error codes to existing tests is a separate retrofit pass; new tests carry the code.

### RuntimeAbortPrograms

```
// runtime abort test case
// <one-line description>
// expected exit code: <N>
// expected stderr substring: <substring>
// Author: <name> (<offering>)
```

The harness compiles the program, runs the binary (with `<test_name>.input.txt` piped to stdin if present), and verifies the exit code and stderr-substring per the header. Exit codes match the runtime ABI's documented abort signals (e.g., 101 for `lo_cast_check` failure per `runtime-abi.md` §3.5). For WASM tests, the exit code line reads `wasm-trap` instead of an integer; the host harness catches the `unreachable` instruction and matches the trap message.

## Conventions across categories

**Tests use `int Main.main()` as the entry point** (or `int main()` for LO-2). The synthesized C-level `main` wrapper reads the LO entry method's return value and exits with that as its status. See `planning:lo-3-reference.md` §4.6.

**Tests minimize IO.** The conformance suite is primarily for the language; IO machinery (the pre-bound `in` / `out` / `err`) is exercised in a small set of dedicated tests (e.g., `LO-3/ValidPrograms/test_2_hello_world.lo`). Most tests communicate pass/fail through the exit code alone. Lower I/O dependence means each test is self-contained, faster to run, and easier to debug when something breaks in the toolchain.

**Tests target a single language level.** A test in `LO-3/` may use any LO-3 feature but no LO-4 feature; a test in `LO-4/` may use any LO-4 feature (which is a superset of LO-3). This keeps the per-level cutoff sharp for grading midpoint projects and for partial-implementation testing.

**Tests prefer specific error codes over phase sentinels.** Each phase in `error-codes.md` carries an `E_<PHASE>_OTHER` sentinel for errors that don't fit a specific category in that phase. New tests should declare the specific code (e.g., `E_FIELD_SHADOWING`) rather than the sentinel — this catches student compilers that detect a problem from the wrong phase, where a sentinel-based test would still pass. Use a sentinel only when no specific code exists yet for the case being tested, and consider drafting one in that case via the workflow in `error-codes.md` § Adding a new code.

**Numbering may have gaps.** As tests are added, removed, or moved between categories, numbers may skip. The descriptor suffix carries semantic identity; the number carries reference identity. Re-numbering an existing test breaks external references and is avoided.

## Migration of pre-redesign LO-3 tests

The LO-3 tests from previous offerings (Summer 2021 / Fall 2021 authors) used the pre-redesign grammar — constructors declared as `void ClassName(args) { body }` in the method section, called automatically by `new ClassName(args)`. The redesigned LO-3 moves constructors into a `[ ClassName(args) { body } ]` bracket section.

Migration is mechanical: locate any method-section declaration whose name equals the enclosing class name, drop the `void` keyword, and move the declaration into a new `[ ]` section between the field-parens and the method-braces. The state-ledger's earlier characterization that "every previous-LO-3 program with its implicit 1-1 constructor remains valid" was inaccurate — the existing programs use explicit constructors in the old form, and they require mechanical migration. The semantic outcome is unchanged: every previously-valid LO-3 program migrates to an equivalently-valid redesigned-LO-3 program by this transformation.

Migration of all pre-redesign tests in `LO-3/ValidPrograms/` and `LO-3/InvalidPrograms/` is a workstream-internal task; new tests added in this round use the redesigned form directly.

## Harness contract (informational)

The harness implementation is the instructor's grading infrastructure and is out of scope for this suite. The contract above is what the harness implements; the test files in this directory are written to that contract.

A reference harness invocation, for orientation:

```
# Valid test:
$ student_compiler LO-3/ValidPrograms/test_38_simple_use_of_this.lo -o /tmp/test_38
$ /tmp/test_38
$ echo $?                          # must equal the header's declared value (36)

# Invalid test:
$ student_compiler LO-3/InvalidPrograms/test_9_duplicate_main.lo 2>compiler_stderr
$ test $? -ne 0                    # compilation must fail
$ grep -q "E_RESERVED_CLASS_NAME" compiler_stderr  # if header declares an E-code

# Runtime abort test:
$ student_compiler LO-4/RuntimeAbortPrograms/test_1_cast_failure.lo -o /tmp/test_abort
$ /tmp/test_abort 2>abort_stderr
$ test $? -eq 101                  # matches header's expected exit code
$ grep -q "lo_cast_check: cannot cast Dog to Cat" abort_stderr
```

## Current state

As of 2026-05-29, the suite covers:

- `LO-2/` — existing tests from prior offerings, **now fully migrated** to the B-wide brace-shape (locked 2026-05-28; see `migration-2026-05-28.md`). The systematic pass was applied by `tools/migrate_braces.py` (see `runbooks/ws4-corpus-migration.md`); all `ValidPrograms/` and `InvalidPrograms/` tests are in single-braced form. The transformation is purely syntactic — each test's computed result or triggered error is unchanged.
- `LO-3/ValidPrograms/` — new tests written directly in the redesigned grammar, plus the pre-redesign tests **now fully migrated** (body-blocks stripped, explicit `void ClassName(...)` constructors relocated into `[ ]` sections).
- `LO-3/InvalidPrograms/` — new tests, each carrying an `expected compile error:` line referencing a code in `error-codes.md`, plus pre-redesign tests **now fully migrated**. The worked examples `test_4.lo` (missing-`new` case) and `test_8.lo` (duplicate-class case) are annotated; the remaining pre-redesign InvalidPrograms tests carry no E-code annotation — retrofitting codes is a separate (A) pass following this brace migration.
- `LO-4/ValidPrograms/` — new tests covering inheritance basics, constructor chains, polymorphic dispatch, casts, `instanceof`, and same-name methods across unrelated classes. Written directly in redesigned form; not part of the migration.
- `LO-4/InvalidPrograms/` — new tests covering field shadowing, extends-non-class, inheritance cycle, missing constructor, super arity, super in root, delegation-not-first, super-and-this-both, sibling cast, cast-to-non-class.

The mechanical migration recipe for the pre-redesign LO-2 and LO-3 corpora is in `migration-2026-05-28.md`, with worked-example before/after pairs and pseudocode; it is implemented as the idempotent, re-runnable `tools/migrate_braces.py`. The migration report (counts, flagged files) is in `runbooks/notes/`.

> Migration note: the doc-recorded "worked example" migrations of 2026-05-28 had in
> fact only landed on disk for `LO-3/{ValidPrograms/test_18, InvalidPrograms/test_4,
> InvalidPrograms/test_8}` — the other listed files were still pre-redesign and were
> migrated by this pass. A stray `LO-3/ValidPrograms/test_88.lo` — a procedural
> (class-free) LO-2 program with no `Main.main()` — was found to be an exact duplicate
> (identical token stream and header) of `LO-2/ValidPrograms/test_88.lo` and was deleted
> in the follow-up pass.
