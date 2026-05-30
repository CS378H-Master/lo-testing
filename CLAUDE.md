# CLAUDE.md — `CS378H-Master/lo-testing`

Orientation for a Claude-Code session working in this repo. Read this first, then
the document relevant to your task (usually `README.md` and the runbook you were
handed).

## What this repo is

The **LO conformance test suite** for CS 378H (Fall 2026). It verifies that a
*student's* LO toolchain — parser, type checker, codegen — implements the LO
language references correctly. Each test is a self-contained LO program whose
contract is declared in its leading header comment; the instructor's grading
harness compiles/runs each program and checks the contract.

It covers LO-2, redesigned LO-3, and LO-4. It is **student-visible** (a public repo
in the org); students clone it and run their compilers against it during
development, and the instructor runs it for grading.

Do not confuse this suite with `lo-runtime/tests/lo_programs/`: that small set
exercises the *runtime* against a known-correct compiler; **this** suite exercises a
student's *compiler* against a known-correct runtime. They are independent.

The grading **harness implementation** is the instructor's infrastructure and lives
elsewhere — it is **out of scope** for this repo. What lives here is test *content*
written to the harness contract, plus tooling (e.g. the migration script). Don't try
to build or modify the harness here.

## Roles

CC is the implementer here. SC drafts and specs (test conventions, content, error
vocabulary); DC binds decisions. Where a task is underspecified or a file doesn't fit
the documented pattern, **flag it to SC rather than guessing** — the same
flag-don't-guess protocol the WS-1 work used. This repo is implementation/content;
language design happens in `planning`.

## Authoritative documents in this repo

- `README.md` — suite purpose, directory structure, the three-way categorization, and
  the **header-comment contract** (three formats, one per category). This is the
  contract every test file obeys.
- `error-codes.md` — the canonical compile-error and runtime-abort **vocabulary**
  (~35 compile codes by phase + the runtime-abort exit codes). `InvalidPrograms`
  tests reference these; the harness substring-matches the code in compiler stderr.
- `migration-2026-05-28.md` — the mechanical recipe (with worked before/after pairs
  and pseudocode) for bringing the pre-redesign LO-2/LO-3 corpus to the current
  (B-wide, three-section) grammar.
- `runbooks/` — CC execution specs (e.g. `ws4-corpus-migration.md`, the systematic
  migration task).
- `LO-2/`, `LO-3/`, `LO-4/` — the test trees.

The **language definition** these tests encode lives in `planning`, not here:
`planning:lo-3-reference.md`, `planning:lo-4-reference.md`, `planning:liveoak-grammar.tex`
(formal grammar), and `planning:runtime-abi.md` (the source of the runtime-abort exit
codes). You don't have direct `planning` access; reference those docs by the
`planning:` prefix (see cross-references below) and rely on what project knowledge
surfaces.

## Conventions you must not break

- **Header-comment contract.** Every test declares its contract in leading `//`
  comments the harness parses. Three formats (see `README.md`): ValidPrograms
  (`// main method return value: N`), InvalidPrograms (`// expected compile error:
  E_…`), RuntimeAbortPrograms (`// expected exit code: N` + `// expected stderr
  substring: …`). **Preserve headers verbatim** — including `// Author:` attribution —
  when transforming files. Do not reflow or rewrite them.
- **Error codes come from `error-codes.md`.** Never invent a code. If a test seems to
  need a category that isn't in the vocabulary, flag it to SC; adding a code is an
  SC/DC decision, not a CC one. The harness substring-matches, so a program may emit a
  more specific diagnostic as long as the declared category code appears.
- **Test naming:** `test_<N>_<optional_descriptor>.lo`, sequential within a category
  dir. Existing numeric-only names (`test_32.lo`) are **preserved unchanged** so course
  handouts stay valid; new tests use the underscored-descriptor form.
- **Grammar the tests are written in:** redesigned LO-3 (three-section classes
  `( fields ) [ constructors ] { methods }`, with the bracket section *omitted* — not
  empty — when there's no explicit constructor; empty `[ ]` is a syntax error) and
  **B-wide single-braced** method/constructor bodies. Entry point is `int Main.main()`
  (LO-3/LO-4) or top-level `int main()` (LO-2). Reserved class names `Main`, `Input`,
  `Output`, `String`; reserved variable names `in`, `out`, `err`; `Output` methods are
  `print_int`/`print_bool`/`print_string`/`println`; `Input` has
  `read_int`/`read_bool`/`read_string`/`eof`. When in doubt, the `planning:` references
  are authoritative.
- **Directory shape:** `LO-<N>/{ValidPrograms,InvalidPrograms,RuntimeAbortPrograms}/`.
  LO-2 is two-way only (no `RuntimeAbortPrograms` — LO-2 has no runtime mechanism that
  aborts). There is no `LO-1/` (LO-1 is exercised implicitly by simpler LO-2 programs).

## Cross-references

To `planning`-repo docs, link by repo-relative path with a `planning:` prefix
(`planning:lo-3-reference.md`); the prefix is documentation-only — you don't have
direct `planning` access except where a doc is synced/visible. Within this repo, link
by relative path.

> Known cleanup: some docs here (`migration-2026-05-28.md`, `README.md`) still carry
> the **old `cs378h:` prefix** from before the repo rename. Update those to `planning:`
> when you're next editing them (folded into the WS-4 migration PR).

## Workflow

`main` is protected (PR required; branch protection is free on this public repo). Work
on a branch, open a PR for DC review. A local pre-push hook may exist as advisory
fast-feedback, but server-side protection is the real gate.

## Current state / active work

- 53 new tests in the redesigned LO-3 / LO-4 grammar are in (`README.md` "Current
  state" has the breakdown).
- **(B) systematic brace migration** of the remaining pre-redesign LO-2/LO-3 corpus is
  the active CC task — spec in `runbooks/ws4-corpus-migration.md`, recipe in
  `migration-2026-05-28.md`. Idempotent script; preserve headers; don't touch error
  codes.
- **(A) error-code retrofit** of `InvalidPrograms` tests is an SC pass that follows the
  migration; `test_4`/`test_8` are already annotated as worked examples. Don't add
  `expected compile error:` lines during the migration — that's the separate (A) pass.
