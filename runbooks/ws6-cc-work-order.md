# Work order — WS-6 compile/runtime split + runtime-abort coverage (Claude-Code-in-`lo-testing`)

Audience: a Claude Code session in the `lo-testing` repo. Authored by SC, 2026-05-30, after
the WS-6 audit. Two parts in one PR: **(1) move** the misfiled runtime-abort tests out of
`InvalidPrograms/` into `RuntimeAbortPrograms/`, and **(2) add** the authored LO-3
runtime-abort tests that fill the empty runtime category. Scope is LO-3 and LO-4 only; LO-2
is two-category by design and is not touched. Follow `CLAUDE.md`'s flag-don't-guess rule:
where a file doesn't fit the pattern below, surface it to SC rather than guessing.

This is a runbook — steps carry pre/postconditions; don't advance a step whose precondition
fails, flag instead.

## Background (why)

The locked conventions define three categories per level (`ValidPrograms` /
`InvalidPrograms` / `RuntimeAbortPrograms`) with three distinct header contracts, but the
runtime-abort tests were authored into `InvalidPrograms/` and never sorted out. An
`InvalidPrograms` test must *fail to compile* (`// expected compile error: <E_CODE>`); a
`RuntimeAbortPrograms` test *compiles, runs, and aborts* (`// runtime abort test case` /
`// expected exit code:` / `// expected stderr substring:`). The misfiled tests carry the
runtime-abort header inside the compile-error directory — wrong against their own contract.

## Part 1 — Move the misfiled tests

**Identification rule.** Any file under `LO-3/InvalidPrograms/` or `LO-4/InvalidPrograms/`
whose first header line is `// runtime abort test case` (equivalently, carries
`// expected exit code:` / `// expected stderr substring:` rather than
`// expected compile error:`) is misfiled. Enumerate the full set in-repo:

```
grep -rl "runtime abort test case" LO-3/InvalidPrograms LO-4/InvalidPrograms
```

Confirmed examples (from project-knowledge sight; the grep is authoritative for the full
list): `LO-4/InvalidPrograms/test_1_cast_failure.lo` (exit 101) and
`LO-4/InvalidPrograms/test_5_string_repeat_negative.lo` (exit 120).

**Where each moves — by lowest level whose features it needs, not just its current level:**

- **Cast failure (101)** uses casts, an LO-4 feature → `LO-4/RuntimeAbortPrograms/`.
- **String-repeat-negative (120)** uses only String + `*` + `~`, all LO-3 → **re-level to
  `LO-3/RuntimeAbortPrograms/`** (don't leave it at LO-4; LO-3 needs the coverage and 120 is
  LO-3-reachable). This is the one move that changes level; flag to SC if you'd rather keep
  same-level and have SC author a separate LO-3 120 instead.
- Any other hits from the grep: place at the lowest level whose features the program uses.

For each move: `git mv` into the destination `RuntimeAbortPrograms/` (create the directory
if absent), assign a fresh sequential `test_<N>` within that destination dir (preserve the
descriptor suffix), and `git mv` any sibling `<name>.input.txt` to match the new basename.
The header already conforms to the RuntimeAbort contract (that's how it was identified) —
do not edit it beyond nothing.

**Do NOT move `LO-3/InvalidPrograms/test_5` ("Call a method from null").** It is a genuine
*compile* error (`null.getValue()` — the literal `null` as receiver, `E_NULL_LITERAL_RECEIVER`)
and stays in `InvalidPrograms`. Its runtime cousin (dispatch on a null *variable*) is the
separate authored test below. Key on the header, not the word "null".

**Renumbering is safe:** the misfiled tests are new, descriptor-suffixed names, not
numeric-only legacy tests, so no course handout or grading-script reference breaks.

## Part 2 — Add the authored LO-3 runtime-abort tests

SC has written the following (delivered alongside this work order). Drop each into
`LO-3/RuntimeAbortPrograms/`, assigning the final `test_<N>_<descriptor>.lo` number
sequentially within that dir (after any moved tests). Rename each paired `*.input.txt` to
match its test's final basename. They are verified to compile cleanly and abort at runtime
with the stated code + stderr substring.

| Delivered file | Intended name | Code | Stderr substring | Input fixture |
|---|---|---|---|---|
| `lo3_null_receiver.lo` | `test_<N>_null_receiver.lo` | 102 | `lo_abort_null_receiver: cannot dispatch getValue` | — |
| `lo3_read_int_malformed.lo` | `test_<N>_read_int_malformed.lo` | 110 | `lo_read_int: malformed token` | `lo3_read_int_malformed.input.txt` (`hello`) |
| `lo3_read_int_eof.lo` | `test_<N>_read_int_eof.lo` | 111 | `lo_read_int: end of input` | `lo3_read_int_eof.input.txt` (empty) |
| `lo3_read_bool_invalid.lo` | `test_<N>_read_bool_invalid.lo` | 112 | `lo_read_bool: invalid token` | `lo3_read_bool_invalid.input.txt` (`maybe`) |
| `lo3_oom_unbounded_alloc.lo` | `test_<N>_oom_unbounded_alloc.lo` | 137 | `lo_alloc: out of memory` | — |

Notes:

- **102** uses a field left at its null default (`Box.h`), so the null receiver is a genuine
  *runtime* value, not a statically-obvious literal — it won't collapse into the compile-time
  `E_NULL_LITERAL_RECEIVER` case.
- **111** pairs with an **empty** `.input.txt` (immediate EOF). Keep the file present and
  empty so the harness pipes empty stdin rather than leaving stdin attached to a terminal.
- **137** loops `head = new Node(head);` forever, keeping the whole chain reachable so no
  collector can reclaim it; the heap fills and `lo_alloc` aborts. The trailing `return 0;`
  is unreachable but present to satisfy the non-void return requirement. **Bound its
  runtime** by having the harness run this test with a small `LO_HEAP_SIZE` (e.g. a few MiB)
  rather than the 16 MiB default; flag to SC if the harness can't set per-test env.
- **Author line** is the `CS 378H course staff (Fall 2026)` placeholder, matching `test_15`;
  substitute the real attribution before merge if DC has a preferred byline.

## Coverage after the pass

- **LO-3/RuntimeAbortPrograms:** 102, 110, 111, 112, 120 (re-leveled), 137 — the full set of
  LO-3-reachable runtime aborts.
- **LO-4/RuntimeAbortPrograms:** 101 (cast). LO-4 inherits LO-3's coverage; the LO-3-reachable
  aborts are not duplicated at LO-4.
- **LO-2:** unchanged (two categories; no runtime aborts by design).

If the grep surfaces a runtime-abort code with no test after this pass, flag the gap to SC.

## Verification before declaring done

- `grep -rl "runtime abort test case" LO-3/InvalidPrograms LO-4/InvalidPrograms` returns
  nothing (all moved out).
- Each `RuntimeAbortPrograms/` test has the 5-line runtime-abort header and, where it reads
  input, a same-basename `.input.txt`.
- No `InvalidPrograms` test lost (count InvalidPrograms before/after = before − moved).
- `LO-3/InvalidPrograms/test_5` is still present (not swept).
- Build/run each new test against a reference toolchain if available; confirm exit code and
  stderr substring. (If no reference compiler is wired in `lo-testing`, this validates later
  under WS-2; note that in the PR.)

## Out of scope for this PR (planning-side, SC/DC)

The ledger deltas (activate WS-6; tighten WS-2's Cheney dependency; reconcile the stale
`lo-testing/` file-registry rows) are `planning`-repo changes, handled separately by SC/DC —
not part of this `lo-testing` PR.
