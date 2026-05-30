# WS-4 brace-migration — CC notes for SC/DC review

**Task:** `runbooks/ws4-corpus-migration.md` (the (B) systematic brace migration).
**Recipe:** `migration-2026-05-28.md`. **Implemented by:** `tools/migrate_braces.py`.
**Run:** 2026-05-29. **Machine-generated counts:** `cc-ws4-migration-report.md`.

## What ran

`tools/migrate_braces.py` applied two purely-syntactic transformations to
`LO-2/{ValidPrograms,InvalidPrograms}` and `LO-3/{ValidPrograms,InvalidPrograms}`:

1. **LO-3 constructor relocation** — `void <ClassName>(<Formals>?) { … }` method-section
   declarations moved into a `[ … ]` section (created when absent), `void` dropped.
2. **LO-2 + LO-3 brace-stripping** — each method/constructor body's inner `<Block>`
   brace pair removed, lifting its statements up one level. `if`/`else`/`while`
   block braces are kept.

Headers (`// …` contract comments, including `// expected compile error:` and
`// Author:`) are emitted verbatim — the preamble before the first declaration is
never reindented or altered. Migrated bodies are reindented to 4-space.

### Totals

- Files scanned: **152**. Migrated: **147**. Already-new-form no-ops: **5**. Skipped: **0**.
- Classes parsed: 111 · methods: 375 · constructors relocated: 54 · existing `[ ]`
  constructors: 4 · body-blocks stripped: 419.

## Verification

- **Regression anchors.** The genuinely-pre-migrated worked examples
  `LO-3/ValidPrograms/test_18.lo`, `LO-3/InvalidPrograms/test_4.lo`,
  `LO-3/InvalidPrograms/test_8.lo` are **byte-identical no-ops** (confirmed via
  `git status`: untouched). `LO-3/ValidPrograms/test_14_string_reverse_codepoint.lo`
  (a new redesigned test) is likewise a no-op.
- **Idempotency.** Re-running over the migrated tree reports 0 changed, 0 skipped.
- **Token conservation (anti-corruption invariant).** For every file, the migrated
  significant-token multiset equals the original minus exactly the stripped
  body-block braces and relocated `void`s, plus exactly the brace pairs of any
  freshly created `[ ]` section — nothing else added, lost, or duplicated. This
  invariant deliberately does **not** require global brace/paren balance, so
  InvalidPrograms tests carrying intentional imbalance (see below) are not rejected.
- **Hand spot-checks** across LO-2/LO-3 × Valid/Invalid: var-decl prefixes preserved,
  `if`/`else`/`while` braces retained, string literals (incl. ones containing
  punctuation) preserved, multi-line field headers and constructor relocation correct.

## Flags for SC (surfaced, not resolved)

1. **The 2026-05-28 "worked example" migrations were mostly not on disk.**
   `migration-2026-05-28.md` §Status lists 14 files as already migrated. In fact only
   **3** were in redesigned form on disk (`LO-3/ValidPrograms/test_18`,
   `LO-3/InvalidPrograms/test_4`, `LO-3/InvalidPrograms/test_8`). The other 11 —
   `LO-2/{ValidPrograms/test_48,test_86,test_88; InvalidPrograms/test_3,test_8,test_11}`
   and `LO-3/ValidPrograms/{test_15,test_20,test_21,test_22,test_36}` — were still
   pre-redesign (double-braced / old-form constructors) and were migrated by **this**
   pass. No action needed beyond awareness; the recipe doc's §Status is now stale.

2. **`LO-3/ValidPrograms/test_88.lo` is a misplaced LO-2 program.** It is a class-free,
   procedural program (`int doSomethingCoolAndExciting(...)`, `int main()`) already in
   single-braced form — byte-for-byte the redesigned form of `LO-2/ValidPrograms/test_88.lo`.
   It parses as two top-level methods with no class and no `Main.main()` entry point, so
   it does not fit the LO-3 ValidPrograms contract. The migration pass left it
   **byte-identical** (nothing structural to do) and flagged it.
   **RESOLVED (follow-up, 2026-05-30):** confirmed an exact duplicate of
   `LO-2/ValidPrograms/test_88.lo` — identical 106-token significant-token stream and
   byte-identical header (same description and `// Author: Aaron Wollman`). Per the
   follow-up work order (clear exact dup → delete), `LO-3/ValidPrograms/test_88.lo` was
   deleted with `git rm`.

3. **`LO-3/InvalidPrograms/test_6.lo` — "Method defined outside of class".** Has a
   top-level `int add(int a, int b){…}` before `class Main`. This is the intentional
   bug the test demonstrates. The script handled it (brace-stripped the top-level
   method like an LO-2 method) and migrated the rest normally; the demonstrated error
   is preserved. Flagged only so SC knows the top-level-method shape was encountered
   intentionally, not as a parse failure.

## Notes (handled cleanly, no action needed)

- **`LO-2/InvalidPrograms/test_20.lo`** ("Unbalanced parentheses") carries a genuine
  extra `)` as its bug. Brace-stripping and reindent preserve the imbalance exactly;
  the reindenter tracks brace and paren depth in separate, per-line-clamped counters
  so the stray paren can't corrupt structural indentation.
- **`LO-3/InvalidPrograms/test_13.lo`** ("reserved words as identifiers") has a
  malformed var-decl prefix (`while waiter;`). Body-block detection is structural (the
  first top-level `{` inside a body whose preceding token is the outer `{` or a `;`),
  not var-decl-parsing, so the malformed prefix did not derail the strip. The
  reserved-word misuse is preserved.
- **Boundary with (A).** No `expected compile error:` line was added, removed, or edited;
  `error-codes.md` was not touched. Error-code retrofit remains SC's separate (A) pass.

## Cosmetic residue (acceptable, idempotent)

- Inline old-form bodies such as `void X() { { ; } }` migrate to `X() {  ;  }` (double
  space where the inner braces were). Valid and stable under re-runs.
- Multi-line expressions (e.g. the nested `new treeCell(...)` in
  `LO-3/ValidPrograms/test_17.lo`) have continuation lines reindented by paren depth
  rather than aligned to the original column. Cosmetic only; semantics unchanged.
