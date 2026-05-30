# WS-4 — Pre-redesign corpus migration (CC execution spec)

**Audience:** CC, routed by DC. **Drafted by:** SC, 2026-05-29.
**Repo:** `CS378H-Master/lo-testing` (this is a Claude-Code-in-`lo-testing` task).
**Destination of this file:** `lo-testing/runbooks/ws4-corpus-migration.md`.

## Objective

Bring the remaining pre-redesign LO-2 and LO-3 test programs into conformance with
the redesigned grammar (B-wide single-braced bodies; LO-3 three-section classes)
by implementing the migration recipe as a **robust, idempotent script** and running
it across the corpus. The transformation is purely syntactic — every program's
computed result, triggered compile error, or printed output is unchanged.

The recipe, with worked before/after pairs and pseudocode, is authoritative:
**`migration-2026-05-28.md`** in this repo. This spec is the execution layer on top
of it — how to implement the recipe correctly and what to verify. Read the recipe
first; do not re-derive the transformation from scratch.

Authoritative references (all in `planning`, visible via project knowledge):
`planning:lo-3-reference.md` (redesigned grammar prose), `planning:liveoak-grammar.tex`
(formal grammar), and this repo's `README.md` (categories, header-comment contract)
and `migration-2026-05-28.md` (the recipe).

Role split: SC drafts/specs, DC binds, CC implements. Where the script's behavior is
underspecified or a file doesn't fit the pattern, **flag it to SC rather than guessing**
(see "Flag to SC" below) — same protocol as the WS-1 deltas.

## Scope

In scope — migrate files under:
- `LO-2/ValidPrograms/`, `LO-2/InvalidPrograms/`
- `LO-3/ValidPrograms/`, `LO-3/InvalidPrograms/`

Out of scope — do **not** transform:
- `LO-4/**` — written directly in redesigned form; nothing to migrate.
- `LO-3/RuntimeAbortPrograms/` and the new `LO-3` tests `test_1`…`test_15` — already
  redesigned form; the script must **no-op** on them (see idempotency).
- The 14 already-migrated worked examples (listed in `migration-2026-05-28.md` §"Status")
  — the script must no-op on them too; they are your regression anchors.

Idempotency is a hard requirement: running the script over the whole tree, including
already-migrated files, must leave the migrated/new-form files byte-identical (or
re-emit them identically). The corpus is mixed old/new form, so the script must detect
form per method, not per run.

## The transformation (summary; recipe is authoritative)

Two passes, per `migration-2026-05-28.md`:

1. **LO-3 only — constructor relocation.** Each method-section declaration of the form
   `void <ClassName>(<Formals>?) { … }` whose name equals the enclosing class is moved
   into a `[ … ]` constructor section (created between the field parens and the method
   braces if absent), with the `void` dropped. Multiple such declarations all move, in
   source order. A class with none keeps no bracket section (implicit constructor).
2. **LO-2 and LO-3 — brace-stripping.** Each method body (and each relocated constructor
   body) drops the inner `<Block>` brace pair, lifting its statements up one level while
   preserving any leading `<VarDecl>`s. Braces of `if` / `else` / `while` `<Block>`s are
   **kept** — they remain `<Block>`s under the new grammar.

## Implementation guidance — the robustness crux

A naïve regex/brace-counter will corrupt the corpus. The script must:

- **Tokenize with string/comment awareness.** A `{` or `}` inside a string literal
  (`"…"`, including escaped quotes) or a comment (`//` to end-of-line; check
  `liveoak-grammar.tex` for whether block comments exist and handle them if so) must
  not affect brace matching. This is the single most important correctness property.
- **Match braces to distinguish the Body-Block from inner Blocks.** Under the old
  grammar a method is `Type name(Formals?) { (VarDecl)* { (Stmt)+ } }` — an outer brace,
  optional var-decls, then the Body's `<Block>` brace pair. Strip *that* inner pair only.
  `if`/`while`/`else` Blocks nest deeper and stay. Identify the Body-Block as the first
  `{` after the method's outer `{` and any leading var-decl lines.
- **Detect already-migrated methods and skip them.** If, after the outer `{` and any
  leading var-decls, the next token is a statement (not a `{`), the method is already
  single-braced — leave it untouched. This is what makes the run idempotent.
- **Relocate constructors structurally,** not by text search: parse class headers to get
  the class name, scan the method section for `void <ClassName>(…)`, and move whole
  declarations. Preserve order; create the `[ ]` section only when at least one moves.
- **Preserve header comments verbatim** — `// good test case`, `// main method return
  value: N`, `// expected compile error: E_…`, `// Author: …`. The harness parses these;
  do not reflow or alter them.
- **Emit clean, consistently-indented output** (4-space, matching the worked examples).
  Indentation is not semantic but the corpus is read by graders and students.

Keep the script in the repo (e.g. `tools/migrate_braces.py`) — it is a record and is
re-runnable; do not delete it after the run.

## Boundary with (A) — error-code retrofit

The error-code annotation of `InvalidPrograms` tests is a **separate SC-owned pass (A)**,
not part of this task. This migration must **not** add, remove, or edit any
`expected compile error:` line, and must not touch `error-codes.md`. Preserve whatever
header each file already has. If a migrated file's demonstrated bug looks mis-annotated
or un-annotated, flag it to SC — do not fix it here.

## Verification

There is no LO compiler in-tree yet, so "parses under the new grammar" cannot be machine-
checked by a real parser. Verify by:
- **Invariants the script asserts on its own output:** balanced braces (string/comment-
  aware); no remaining old-form double-brace method body; header block byte-identical to
  input; file count and per-file class/method counts unchanged.
- **Regression anchors:** running over the 14 already-migrated worked examples is a no-op
  (diff is empty).
- **Hand spot-check:** review a sample across LO-2/LO-3 × Valid/Invalid against the recipe's
  worked pairs.
- **Report:** emit a migration report (files touched, methods/constructors transformed,
  files skipped as already-new-form, anything flagged) for SC/DC review in the PR.

Real parse-validation lands later when a reference parser exists (or when WS-2 wires the
suite); the worked examples and these invariants are the pre-parser guard.

## Deliverables

- `tools/migrate_braces.py` (or equivalent) committed.
- The migrated `LO-2/{Valid,Invalid}` and `LO-3/{Valid,Invalid}` corpus.
- A migration report (counts + flagged files), e.g. `runbooks/notes/cc-ws4-migration-notes.md`.
- `README.md` "Current state" updated: the pre-redesign corpus is now migrated; note any
  files left flagged for SC.
- One PR to `lo-testing` `main` for DC review (branch protection: PR required).

## Flag to SC (don't guess)

Surface these rather than resolving them:
- A method named like its class but **not** `void` (ambiguous: constructor or anomaly?).
- Any file whose structure doesn't fit the recipe (e.g., a post-var-decl `{` that isn't a
  simple Body-Block; unexpected nesting).
- `InvalidPrograms` tests whose demonstrated bug is mis- or un-annotated — e.g. the recipe
  already flags `LO-3/InvalidPrograms/test_4` (missing-`new` / missing-formal-parens →
  likely a syntax error, not a semantic code) and `test_8` (duplicate class → needs a code
  not yet in `error-codes.md`). These are SC's (A) pass; just confirm the brace migration
  applied cleanly and leave the headers alone.
- Any file that does not round-trip idempotently.

## Secondary cleanup (while you're in the repo)

The `lo-testing` docs (`migration-2026-05-28.md`, `README.md`) still use the old
`cs378h:` cross-reference prefix from before the repo rename. Update those to `planning:`
in the same PR — mechanical, low-risk.
