# Work order — WS-6 follow-up: category-consistency sweep (Claude-Code-in-`lo-testing`)

Audience: a Claude Code session in the `lo-testing` repo. Authored by SC, 2026-05-30, after
validating the WS-6 compile/runtime split (`lo-testing#7`). That PR fixed runtime-abort
tests misfiled in `InvalidPrograms`. Validation then surfaced a *second* miscategorization
on a different axis: **valid programs misfiled in `InvalidPrograms`** (plus duplicate test
numbers). This pass sorts every LO-3/LO-4 test into the directory its header declares, and
clears the numbering collisions. One PR. Scope is LO-3 and LO-4 only; LO-2 is the
non-deliverable checkpoint level and is not touched. Follow `CLAUDE.md`'s flag-don't-guess
rule.

Runbook: steps carry pre/postconditions; if a file doesn't fit the rule, flag to SC rather
than guessing.

## Background (why)

Each category directory has a header contract (README "Header-comment contract"). A file's
**first header line** declares its true category:

| Directory | Header line 1 | Contract line(s) |
|---|---|---|
| `ValidPrograms/` | `// good test case` | `// main method return value: <N>` |
| `InvalidPrograms/` | `// bad test case` | `// expected compile error: <E_CODE>` |
| `RuntimeAbortPrograms/` | `// runtime abort test case` | `// expected exit code: <N>` + `// expected stderr substring: <s>` |

A file whose header line 1 doesn't match the directory it sits in is misfiled and will be
mis-graded (e.g., a `// good test case` in `InvalidPrograms` compiles successfully when the
harness expects a compile failure). Validation confirmed several such cases in
`LO-4/InvalidPrograms/`.

## Part 1 — Sort every misfiled test into the directory its header declares

**Enumerate per directory** (the grep is authoritative; the confirmed examples are anchors):

- Valid tests misfiled in InvalidPrograms:
  `grep -rl "good test case" LO-3/InvalidPrograms LO-4/InvalidPrograms`
- Runtime tests misfiled in InvalidPrograms (should now be empty after `#7` — re-confirm):
  `grep -rl "runtime abort test case" LO-3/InvalidPrograms LO-4/InvalidPrograms`
- Invalid/runtime tests misfiled in ValidPrograms:
  `grep -rL "good test case" LO-3/ValidPrograms LO-4/ValidPrograms`
  (lists ValidPrograms files whose header is *not* `// good test case` — inspect each)
- Anything in RuntimeAbortPrograms not matching `// runtime abort test case`:
  `grep -rL "runtime abort test case" LO-3/RuntimeAbortPrograms LO-4/RuntimeAbortPrograms`

**Confirmed misfiled valid tests in `LO-4/InvalidPrograms/`** (move to `LO-4/ValidPrograms/`):

- `test_1_super_method_basic.lo` (returns 35)
- `test_1_self_method_call.lo` (returns 2)
- `test_2_hello_world.lo` (returns 0)
- `test_9_null_field_default.lo` (returns 42)
- `test_11_instanceof_unrelated_classes.lo` (returns 99)

These are SC's sight via project knowledge; the grep is authoritative for the complete set
and may surface more. **Leave genuine compile-error tests in place** — e.g.
`test_2_extends_string.lo` (`E_EXTENDS_NON_CLASS`), `test_8_reserved_variable_name.lo`,
`test_10_cast_target_not_class_type.lo` are correctly filed and stay.

For each misfiled file: `git mv` into the directory matching its header line 1, and `git mv`
any sibling `<name>.input.txt` / `<name>.expected.out` with it. Do not edit headers or
bodies during the move.

Expected outcome: LO-3 was audited during the WS-4 (A) retrofit and should be clean — if the
LO-3 greps return anything, **flag it to SC** rather than assuming. The mess is expected to
be confined to `LO-4/InvalidPrograms`.

## Part 2 — Clear duplicate test numbers

The LO-4 first-cut directories carry collisions — e.g. `LO-4/InvalidPrograms/` had two
`test_1_*` (`super_method_basic`, `self_method_call`) and two `test_2_*` (`hello_world`,
`extends_string`). After Part 1's moves, renumber within each affected directory so numbers
are unique and sequential, preserving the descriptor suffix. When a moved file lands in a
destination that already uses its number, assign the next free number in the destination.

**Renumbering safety:** only the new, descriptor-suffixed LO-4 tests (first cut 2026-05-28)
are renumbered here — these are not referenced by course handouts or grading scripts yet, so
no external reference breaks. **Do not renumber** the numeric-only legacy tests (the
pre-redesign LO-2/LO-3 `test_<N>.lo` names) — those are the stable handout identifiers and
are preserved by convention. None of the collisions involve legacy numeric-only names; if the
grep suggests otherwise, flag it.

## Part 3 (secondary, optional) — header completeness

The LO-4 first-cut tests omit the `// Author:` line that the header contract shows (e.g.
`test_11_instanceof_unrelated_classes` has three header lines, no author). Since these files
are being touched anyway, optionally add `// Author: CS 378H course staff (Fall 2026)` as the
final header line to bring them to the 4-line contract — or flag for SC to confirm the byline.
Do not alter existing descriptions or return-value lines. Skip this part if it risks widening
the PR; it is cosmetic relative to Parts 1–2.

## Verification before declaring done

- Every file under each `*/ValidPrograms/` begins with `// good test case`; every
  `*/InvalidPrograms/` with `// bad test case`; every `*/RuntimeAbortPrograms/` with
  `// runtime abort test case`. (Re-run the four greps; the "misfiled" ones return empty.)
- No duplicate `test_<N>` within any single category directory (LO-3, LO-4).
- File counts reconcile: every moved file appears exactly once at its destination and is
  gone from its source; nothing lost.
- Genuine compile-error tests (`test_2_extends_string`, `test_8_reserved_variable_name`,
  `test_10_cast_target_not_class_type`, …) remain in `InvalidPrograms`.
- The WS-6 `#7` results are untouched: the seven RuntimeAbort tests and `LO-3/InvalidPrograms/test_5` stay put.

## Out of scope (planning-side, SC/DC)

Ledger deltas — fully retiring R6 / closing WS-6 once categorization is clean — are
`planning`-repo changes handled by SC/DC, not part of this `lo-testing` PR. If the greps
surface anything beyond misfiled valid tests and number collisions (a file matching no
contract, an LO-3 surprise, a legacy-name collision), stop and flag to SC.
