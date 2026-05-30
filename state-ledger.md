---
project: cs378h
phase: "Pre-semester course design (Fall 2026, semester starts Mon Aug 24); ~12 weeks remaining"
status: yellow
needs_dc_next: resolve flag-2 `test_88` in lo-testing (relocate to LO-2 under a fresh number, or delete if duplicate — CC can execute given the rule); apply the §Status correction to the lo-testing migration doc; once project knowledge syncs PR #3, SC completes the (A) error-code retrofit across the migrated InvalidPrograms (or run as a Claude-Code-in-lo-testing pass); advance WS-3 (LaTeX reflow, DC-side, unblocked); confirm project_instructions.md bootstrap text names CS378H-Master/planning
last_touched: 2026-05-30
deadline: 2026-08-24
cadence: deadline-bounded
---

# CS 378H — State Ledger

🟡 Pre-semester course design (Fall 2026, semester starts Mon Aug 24); ~12 weeks remaining.

**Last updated: 2026-05-30.** *Update this line every time the file changes.*

This document is the running status board for the CS 378H course-design project. It is the place to look first when resuming work after time away. The conversation transcripts in `/mnt/transcripts/` (when available in the working environment) contain the reasoning behind every decision; this ledger summarizes the outcomes. When transcripts are absent, the locked-decisions section is the only audit trail.

The ledger has three parallel structures that look similar but track different things. **Workstreams** are trackable work items with the lifecycle in `common_sop_v7.md` § Session continuity (Queued → Active → Completed/Dropped). **Decisions** have the lifecycle in `common_sop_v7.md` § Decision discipline (Proposed → Pending → Locked, etc.). **Risks** have the lifecycle in `common_sop_v7.md` § Conditional / Deadline-bounded (Identified → Mitigated → Closed). The three are linked: most workstreams have a risk shadow, and decisions in flight may block workstreams.

---

## At-a-glance workstreams

WS-1 (runtime skeletons) is ✅ Completed and WS-4 (conformance suite) is 🟡 Active; the remaining workstreams are Queued (⚫). The project status is 🟡 — the deadline (2026-08-24) is real and most pre-semester work is still ahead. WS-4's (B) brace migration has merged; its (A) error-code retrofit is the live SC thread (gated on the PR-#3 project-knowledge sync). WS-3 (LaTeX reflow) is unblocked and DC-side; WS-2 and WS-6 remain gated on WS-4. DC's next moves: the WS-4 follow-ups (the CC work order for `test_88` / §Status / `test_6`) and advancing WS-3 in parallel.

| ID | Workstream | Status | Needs DC |
|---|---|---|---|
| WS-1 | Build and test three runtime skeletons (Rust, Zig, C++) to the shared ABI | ✅ Completed 2026-05-29 | CC reports all three skeletons + shared corpus + cross-skeleton C harness merged, CI-green. SC reconciled the five spec/runbook deltas CC surfaced (see "WS-1 spec deltas" below) on 2026-05-29 |
| WS-2 | Cross-skeleton conformance test: each skeleton + Cheney passes shared suite | ⚫ Queued | WS-1 dependency now satisfied (skeletons merged); still needs the conformance suite (WS-4) and a Cheney collector before it can activate |
| WS-3 | Reflow LaTeX grammar source for LO-3 redesign + LO-4 productions | ⚫ Queued | yes — unblocked by 2026-05-28 LO finalization; DC-side LaTeX edit |
| WS-4 | Update LO-3 / LO-4 conformance test suite for redesigned LO-3 and new LO-4 | 🟡 Active | 53 new redesigned tests in. (B) systematic brace migration **COMPLETE** — CC PR `lo-testing#3` merged (`tools/migrate_braces.py`; 147 files migrated, 5 no-ops, 54 constructors relocated, 419 body-blocks stripped; LO-4 untouched). (A) error-code retrofit now unblocked for the full migrated corpus — `test_4`/`test_8` annotated, `test_6` queued; rest pending project-knowledge sync of PR #3 (or a Claude-Code-in-`lo-testing` pass) |
| WS-5 | Spot-check best-effort URLs in `reading-list.md` | ⚫ Queued | low priority; SC can drive when activated |
| WS-6 | Confirm conformance suite is in course repository at right granularity | ⚫ Queued | depends on WS-4 |
| WS-7 | Source physical or library copies of three required supplementary books | ⚫ Queued | DC-side |
| WS-8 | Confirm UT library / ACM DL access for student paper readings | ⚫ Queued | DC-side |
| WS-9 | Prepare bio-form template for student team-formation | ⚫ Queued | yes — SC can draft |
| WS-10 | TA outreach: walk TAs through project structure and artifacts | ⚫ Queued | DC-side |

## Active workstreams

**WS-4 — Update LO-3/LO-4 conformance test suite. Active (since 2026-05-29).** Owner: SC, with CC for mechanical passes. The 53 new redesigned LO-3/LO-4 tests are in; the (B) systematic brace migration of the pre-redesign corpus merged (CC PR `lo-testing#3`). Remaining: the (A) error-code retrofit across the migrated `InvalidPrograms` — gated on project knowledge syncing PR #3, or run as a Claude-Code-in-`lo-testing` pass. Target: suite complete and internally consistent before WS-2 / WS-6 activate. (Per `common_sop_v7.md` § Session continuity, this section and heading are preserved for the cross-project board's regen workflow.)

**Recently completed.** WS-1 (runtime skeletons) ran and merged CI-green; CC surfaced five spec/runbook deltas, all reconciled by SC on 2026-05-29 (see "WS-1 spec deltas — ratified" under Locked decisions). The skeleton implementation is the source of truth where it diverged from the draft runbook; the runbook and ABI were corrected to match, not the reverse.

**WS-4 (B) — corpus migration, completed and merged 2026-05-29** (CC PR `lo-testing#3`): idempotent `tools/migrate_braces.py` + 147 migrated `.lo` files (54 constructors relocated, 419 body-blocks stripped), README current-state updated, `cs378h:`→`planning:` doc prefixes fixed. Three flags returned to SC, resolved this turn:
- **§Status overstated prior progress** — only 3 worked examples (`LO-3` `test_18`/`test_4`/`test_8`) were actually on disk, not the 14 the doc/ledger listed; the other 11 were listed-but-not-migrated and CC's pass migrated them. This was an SC-side recording inaccuracy; §Status correction text handed back for the `lo-testing` doc. (The "14 worked-example migrations applied" framing in this session's earlier record is superseded — 3 real, full corpus now migrated.)
- **`LO-3/ValidPrograms/test_88.lo`** is a misfiled class-free LO-2 program (invalid as LO-3 → `E_NO_MAIN_CLASS`). SC decision: relocate to `LO-2/ValidPrograms/` under the next free number (test_88 taken there) unless it duplicates an existing LO-2 test, in which case delete; do not rewrite as LO-3. Handed to CC with the decision rule (or SC executes post-sync).
- **`LO-3/InvalidPrograms/test_6.lo`** ("method outside class") preserved correctly; queued for (A) as `E_PARSE_PHASE_OTHER` (top-level method = parse error; `<Program>` is `(<ClassDecl>)*` only).

**Sync caveat:** project knowledge has not yet synced PR #3 — SC's current view of `lo-testing` reflects the pre-migration state. The full (A) retrofit across the migrated `InvalidPrograms` waits on that sync, or runs as a Claude-Code-in-`lo-testing` task. SC's `outputs/lo-testing/` working copies are now behind the repo (CC's PR + any DC-applied A-annotations) and should not be treated as canonical — the repo is.

---

## Locked decisions

These are settled and should not be re-litigated except via `common_sop_v7.md` § Decision discipline's reopening gate (named trigger; cascade check; locked entry annotated). All entries below are **Locked** unless flagged otherwise.

### Course structure — **Locked**

- 14-week semester, 28 lecture sessions (90 minutes × 2 sessions per week).
- Five projects (P1-P5) plus a mini-conference replacing the final exam.
- Static and dynamic linking are lecture topics, not a separate project.

### Semester calendar — **Locked**

- Semester runs Mon 8/24 through Mon 12/7.
- Two breaks: **Labor Day Mon 9/7** shifts W3.1 onward by one position; **Thanksgiving Mon 11/23 - Fri 11/27** shifts W13.2 onward by two positions.
- Net: 28 sessions preserved; W{n}.1 lands on Wednesday and W{n}.2 on Monday from W3 onward.
- Project deadlines map to: P1 Mon 9/14, P2 Mon 10/12, P3 Mon 11/2, P4 Mon 11/30. Mini-conference held in UT's finals period after Mon 12/7.
- Full mapping in `calendar.md`.

### Midterm — **Locked**

- **Format:** take-home, 48-hour window, open book, AI tools permitted with mandatory disclosure (light "AI use disclosure" section in submission, naming tools and which questions). Carries over the disclosure mechanism the instructor uses in other courses.
- **Scope:** through W7 — front-end through register allocation.
- **Placement:** distributed Fri 10/16 5pm, due Sun 10/18 11:59pm. Falls between W8.1 (FS II) and W8.2 (GC fundamentals); 4 days after the P2 deadline.
- **Shape (proposed, not locked):** 4-5 questions spanning the scope — front-end / FS I, IR / SSA, instruction selection, register allocation, plus one cross-phase synthesis question. To be finalized closer to the date as lectures are taught.

### Project arc — **Locked**

- **P1 (W1-3):** Front-end + LO-3 / LO-4 WASM codegen + tree-walker scaffold. Codegen-during-parse, single-pass.
- **P2 (W4-7):** Native AOT + 3-address IR + linear-scan register allocation. SSA taught, not implemented.
- **P3 (W8-10):** Shared GC runtime; one moving collector; shadow stack.
- **P4 (W11-13):** ptrace-based debugger + DWARF subset.
- **P5 (W13-14):** LO-5 capstone implementation.

### LO grammar — **Locked**

- LO-0 through LO-3 pre-exist from previous offerings; LO-3 is **redesigned** for Fall 2026 to use a three-section class declaration: fields in `( )`, optional explicit constructors in `[ ]`, methods in `{ }`. The LO-3 redesign is strictly more capable than the prior LO-3, but previous-LO-3 programs require mechanical migration to remain valid: the prior form declared constructors as `void ClassName(args) { body }` in the method section (called automatically by `new ClassName(args)`); the redesigned form moves these into the `[ ClassName(args) { body } ]` bracket section. The migration is mechanical (move the declaration, drop the `void` keyword), and the semantic outcome is unchanged. *Note: an earlier characterization of this lock claimed every previous-LO-3 program with its "implicit 1-1 constructor from the field list" remained valid; that was inaccurate — previous LO-3 programs use explicit constructors in the old form. The inaccuracy was surfaced 2026-05-28 during the lo-testing/ survey.*
- The redesign also introduces P45 `<ObjName>` to name the method-call receiver role, and rewrites P20 to take an `<ObjName>` rather than the previous syntactic `<ClassName>` placeholder. `<ObjName>` admits a `<Var>`, the keyword `this`, or a parenthesized expression `( <Expr> )`; the parenthesized alternative composes with P25 to allow any expression as a method-call receiver, preserving LO's parens-heavy compound-expression style.
- LO-4: single inheritance, `super()` and `this()` first-statement delegation in constructors, `super.m()` in methods (lookup walks the static parent chain for inherited-method targets), casts with runtime check (class-types only, no sibling casts; `((T) null)` returns null without abort; cast-failure aborts with a specific message and exit code 101 on native / `unreachable` trap on WASM), `instanceof` expression, no field shadowing, invariant method overrides, no abstract classes / methods, no visibility modifiers, no method overloading, `String` not extensible (treated as primitive-equivalent for inheritance). Inheriting classes are required to declare an explicit constructor section (`[ ]` brackets); no implicit super-call insertion.
- LO-5: capped allocation menu (12 features in 4 groups, max 2 teams per feature) plus "Other" path; algebraic effect handlers excluded as too risky.
- **Cascade warning:** the LaTeX source for the grammar tables (P1-P46 with LO-3 redesigned, including the new P45 `<ObjName>`, and LO-4 added: P3 cast, P21 super-method, P46 instanceof) is reflowed as a one-time pre-semester task (WS-3).

### LO-4 finalization edges — **Locked 2026-05-28**

Six edges in `lo-4-reference.md` that the first-iteration document left implicit or under-specified. Resolved together in the 2026-05-28 session and committed to the reference document the same day.

- **`super.m()` walks the static parent chain.** When the immediate parent does not declare `m` directly, the compile-time lookup walks up the inheritance chain to the nearest ancestor that does declare it. Matches Java's `super` semantics; rejected alternative (require declaration at the immediate parent) considered as the simpler-implementation route but rejected: would surprise readers used to OOP intuitions, and break common idioms like deep-hierarchy `super.toString()`. Cascade: changes nothing in code generation (the walk happens entirely at compile time; runtime sees a direct call).
- **`lo_cast_check` failure abort contract.** On a failed downcast, the runtime writes `lo_cast_check: cannot cast <runtime-class-name> to <target-class-name>` to stderr, then exits with status code 101 on native (matching Rust's default panic exit) or executes `unreachable` on WASM. Rejected: silent-abort (no message). The specific exit code is the conformance-suite signal for distinguishing cast-failure aborts from other crashes.
- **`((T) null)` passes the runtime check.** `lo_cast_check(null, _)` returns `null` rather than aborting, matching `null`'s assignment-compatibility with any class-typed target. Rejected: treat null as failing the cast (would break the symmetry with `null instanceof T` returning `false`).
- **`String` is not extensible.** `class C extends String { ... }` is a compile error. String is treated as primitive-equivalent for subtyping even though it is pointer-backed at runtime. Rejected: allow String subclasses (would require runtime String-op handling for subclass instances; no LO-4 pedagogical win).
- **Class-typed fields default to null.** Explicit specification of the LO-3 §4.1 type-default convention as it applies to class types: a class-typed field uninitialized by any constructor holds `null`. String fields default to the empty string per the preserved LO-3 §4.1 spec (see LO-3 conventions below).
- **Same-name methods in unrelated classes — worked example added.** No semantic change; a §5.6 worked example was added to `lo-4-reference.md` showing that two unrelated classes may both declare a method with the same name, with each class's method dispatched independently (no shared vtable slot, no inheritance involvement).

Cascade-if-reopened: any of these edges feed into the conformance test suite (WS-4) and the runtime ABI's cast-failure contract (`runtime-abi.md` §3.5). The grammar productions are unaffected by these edges — they are static- and dynamic-semantics clarifications.

### LO-3 conventions finalized — **Locked 2026-05-28**

Four LO-3 conventions surfaced during planning of the LO-4 conformance test suite. Each was either undocumented or in tension with the LO-3 reference's existing text; resolution committed to `lo-3-reference.md` the same day.

- **Entry point: `void Main.main()`.** Every LO program must declare a class named `Main` with a no-arg method `void main()` (declared directly on `Main`; not inherited). `Main` must have a zero-arg constructor (implicit or explicit). The compiler synthesizes a C-level `main` wrapper that calls `lo_runtime_init`, allocates `new Main()`, invokes `main()` on the instance, calls `lo_runtime_shutdown` (optional), and exits 0. Preserves the convention from previous offerings. Rejected: designated method name `lo_main` discoverable on any class; compile-flag `--entry=ClassName`. Both would diverge from existing convention without benefit. *Status: Superseded 2026-05-28 by the int-Main.main revision below; the "preserves the convention" justification was incorrect — the existing lo-testing/LO-2 and LO-3 tests use `int main()` returning the exit status, not `void main()`.*
- **Entry point (revised): `int Main.main()`.** Every LO program must declare a class named `Main` with a no-arg method `int main()` (declared directly on `Main`; not inherited). `Main` must have a zero-arg constructor (implicit or explicit) and may not extend any class. The compiler synthesizes a C-level `main` wrapper that calls `lo_runtime_init`, allocates `new Main()`, invokes `main()` on the instance, reads the returned `int`, calls `lo_runtime_shutdown` (optional), and exits the process with that integer as its status. *Trigger for supersession:* survey of the existing `lo-testing/LO-2/` and `lo-testing/LO-3/` conformance tests (added to the repo 2026-05-28) revealed they use `int main()` with the exit code as the harness signal (header comment `// main method return value: N`); preserving compatibility with this convention rules out `void main()`. Cascade-if-reopened: `lo-3-reference.md` §4.6, `runtime-abi.md` §3.6, and the conformance suite's header-comment contract in `lo-testing/README.md`.
- **Standard preamble: synthetic `IO` class.** Compiler injects an implicit `class IO ( ) { ... }` whose methods (`print_int`, `print_bool`, `print_string`, `println`, `read_int`, `read_bool`, `read_string`, `eof`) codegen recognizes by name and lowers to the corresponding runtime ABI entry points (`lo_print_*`, `lo_read_*`, `lo_eof`). Students instantiate the class — `IO out; out = new IO(); out.print_int(42);` — and may instantiate multiple references for symmetry with potential LO-5 stream extensions. The class name `IO` is reserved (along with `Main`); a user declaration of `class IO` is a compile error. Rejected: built-in pre-bound `io` singleton (magic name); free-standing print/read expressions (substantial grammar addition). The read methods consume stdin in the canonical way; semantics specified in `lo-3-reference.md` §4.6 and `runtime-abi.md` §3.7. *Status: Superseded 2026-05-28 by the two-class Input/Output revision below; the single-class instantiable shape adds substantial boilerplate to every IO-exercising program and fails to statically catch role mismatches like `out.read_int()`.*
- **Standard preamble (revised): two synthetic classes `Input` and `Output`, three pre-bound names `in`/`out`/`err`.** Compiler injects two synthetic classes: `Input` with read methods (`read_int`, `read_bool`, `read_string`, `eof`) and `Output` with write methods (`print_int`, `print_bool`, `print_string`, `println`); plus three pre-bound program-scope names — `in: Input` (singleton reading stdin), `out: Output` (singleton writing stdout), `err: Output` (singleton writing stderr) — initialized once by the synthesized `main` wrapper before `Main.main()` is called. Reserved class names: `Main`, `Input`, `Output`, `String`. Reserved variable names: `in`, `out`, `err` (a user field, formal, or local with any of these names is a compile error). User code may pass `out`, `err`, or `in` as actuals to methods that take `Output` or `Input` formals (e.g., logging utilities that take a sink as a parameter). User code never instantiates `Input` or `Output` directly; these classes are not extensible. *Trigger for supersession:* code review of the conformance-suite first cut showed the `IO out; out = new IO();` boilerplate appearing in every IO-exercising test, accounting for a non-trivial fraction of small test programs' line count. The split into Input/Output classes lets the static type checker catch role mismatches (`out.read_int()` becomes `E_UNKNOWN_METHOD`); pre-bound names eliminate the instantiation boilerplate. Rejected: single class with both read and write methods on each instance — would type-check role-confusing code that should be caught statically. Cascade-if-reopened: `lo-3-reference.md` §3.4 (reserved names) and §4.6 (preamble); `runtime-abi.md` §3.7's framing paragraph; the conformance test programs that use `in`/`out`/`err`.
- **P20 receiver refactor: `<ObjName>` introduced.** P20 changes from `<ClassName> . <MethodName>(<Actuals>?)` (a syntactic placeholder the semantic phase resolved as a variable) to `<ObjName> . <MethodName>(<Actuals>?)`, with the new production `P45 <ObjName> → <Var> | this | ( <Expr> )`. Names the role accurately; permits `this`-as-receiver explicitly; permits any expression as a receiver via the parenthesized form. Direct method-call chaining without parens (`a.foo().bar()`) remains a syntax error — chains require parens (`(a.foo()).bar()`) consistent with LO's parens-heavy compound-expression style (P22, P23, P24, P25 all wear parens). Total LO-3 production count: 45. LO-4's `instanceof` shifts from P45 to P46 accordingly. *Trigger for an intra-session revision:* an initial lock of `<ObjName> → <Var> | this` was reopened on observation that LO's existing P25 (parens-around-expression) makes the restrictive form structurally inconsistent — P25 trivially wraps a `<Var>` and produces an `<Expr>`, but the restrictive `<ObjName>` wouldn't admit it. Extended to `<Var> | this | ( <Expr> )` to match the rest of LO's grammar style. Rejected: receiver = `<Expr>` directly (would require left-recursion handling in the LL(2) parser and break the parens-heavy convention by allowing direct chaining).
- **String default value: preserved as empty string.** `lo-3-reference.md` §4.1 specifies the type-default for `String` as the empty string. This was tested against an intra-session alternative proposal (default to null, symmetric with class-typed fields) and the empty-string spec was upheld: rejected null because it conflicts with an existing canonical spec, breaks the `String <: String` identity-only subtyping treatment, and would require String operations to check for null receivers. The runtime now exports `LO_EMPTY_STRING` as a pre-allocated canonical instance (`runtime-abi.md` §2.3); codegen initializes String-typed fields to this pointer after `lo_alloc` returns, before the constructor body runs.

Cascade-if-reopened: each entry has its own cascade. The Main.main convention and IO preamble cascade into codegen's program-entry wrapper and method-name-recognition pattern. The P20 receiver refactor cascades into the LaTeX grammar source (WS-3), the LO-4 reference's grammar diff, and the conformance test suite (WS-4). The String default cascades into runtime-abi.md (`LO_EMPTY_STRING` export) and codegen (post-alloc String-field init).

### Conformance suite conventions — **Locked 2026-05-28**

The conformance test suite's structural conventions, surfaced during WS-4's first cut against the existing `lo-testing/` repository content (added 2026-05-28).

- **Directory shape: three-way categorization per level.** `lo-testing/LO-<N>/{ValidPrograms,InvalidPrograms,RuntimeAbortPrograms}/`. ValidPrograms compile, run, exit with the declared code. InvalidPrograms fail to compile. RuntimeAbortPrograms compile, run, abort with the declared signal. The third category is new in this round — LO-4 introduces real runtime aborts (cast failure exit 101); pre-existing LO-2 and LO-3 directories carry only the first two categories. The runtime-abort category is added to LO-3 and LO-4 directories; LO-2 stays two-way because LO-2 has no runtime mechanisms that can abort.
- **Test naming: `test_<N>_<optional-descriptor>.lo`.** Sequential numbering within each category directory is the primary identifier. The optional snake_case descriptor suffix summarizes what the test exercises. Existing tests with numeric-only names (`test_32.lo`) are preserved unchanged to keep course handouts and external references valid; new tests added in this round use the underscored-descriptor form. Numbering may have gaps as tests are added or removed.
- **Hybrid test contract.** Each test declares its contract in a header comment parsed by the harness. ValidPrograms: `// main method return value: N` declaring expected exit code; optional sibling `<test_name>.expected.out` for IO-exercising tests (byte-matched against stdout); optional sibling `<test_name>.input.txt` piped to stdin. InvalidPrograms: `// expected compile error: <E_CODE>` substring-matched against compiler stderr. RuntimeAbortPrograms: `// expected exit code: <N>` and `// expected stderr substring: <substring>` matched against the running binary's exit and stderr. Existing InvalidPrograms tests predating the error-code vocabulary have no `expected compile error:` line; the harness treats their contract as "compilation must fail" without an E-code check.
- **Error-category vocabulary.** Roughly 35 compile-error codes organized by compiler phase (parse, well-formedness, name resolution, type check, inheritance check, cast/instanceof check, entry-point) plus seven runtime-abort exit codes (101 cast failure, 102 null receiver, 110-112 read failures, 120 string-repeat negative, 137 OOM). Documented canonically in `lo-testing/error-codes.md`; cross-referenced from `runtime-abi.md` §3.8. Each phase carries an `E_<PHASE>_OTHER` sentinel for errors that don't fit a specific code. Tests prefer specific codes over sentinels (specific codes catch cross-phase-mistake failures that sentinel-based tests would miss).
- **LO-2 scaffolding role.** The `lo-testing/LO-2/` tests serve as an intermediate-checkpoint suite for students testing a procedural-only language level during P1 front-end work, before LO-3 features are wired in. Preserved as-is; not actively expanded under WS-4. The active conformance work in this round is LO-3 (with pre-redesign tests migrated to the new constructor form) and LO-4 (new).

Cascade-if-reopened: WS-4 (current first cut + ~25 planned tests). No cascade into language specs or runtime ABI from these conventions; they are test-suite-internal.

### Class-type reference equality — **Locked 2026-05-28**

- **`=` on class types is reference equality; `<` and `>` are not legal.** Two class-typed values are equal under `=` iff they refer to the same heap object on the runtime heap. `null` compares equal to `null` and to any class-typed reference that is null at runtime; a non-null reference compares unequal to `null`. `<` and `>` between class-typed values (or between a class-typed value and `null`) are not legal operators and a compile error. Locked to fill an enumeration gap in the canonical grammar preamble — `liveoak-grammar.tex`'s binop semantics list enumerated int / bool / string but not class types, and a strict reading made `(d = null)`-style null-checks ill-formed despite their being a near-universal LO-3 idiom. Rejected: (a) forbid `=` on class types and provide a built-in `isNull(x)` or analogous null-check primitive — bigger spec change, would require a grammar production and is still missing in the proposal; (b) defer to LO-4 and require `instanceof` for null-checks — leaves LO-3 with no null-check mechanism, which doesn't work since class-typed fields default to null. *Trigger:* surfacing of the canonical grammar's preamble during 2026-05-28 cascade work — the .tex source's "no other combination is legal" rule, applied strictly, would have invalidated several conformance tests already written. Cascade: `liveoak-grammar.tex` preamble (binop bullet list extended with a new "for class types" item).

### Method and constructor body shape — **Locked 2026-05-28**

- **Single-braced bodies; `<Body>` retires.** A method body or constructor body is a single brace pair containing zero-or-more `<VarDecl>`s followed by one-or-more `<Stmt>`s — `{ (<VarDecl>)* (<Stmt>)+ }`. The prior canonical form `{ <Body> }` where `<Body> → (<VarDecl>)* <Block>` and `<Block> → { (<Stmt>)+ }` — a double-braced shape — is retired. The change is "B-wide" in the framing: it applies to LO-2 as well as LO-3 (and forward to LO-4), so the brace pattern stays continuous across the levels students traverse. `<Block>` at P10 is preserved with its braces because `if` (P12) and `while` (P13) bodies still use it; the change is local to `<MethodDecl>` (P5) and `<ConstructorDecl>` (P2 in redesigned LO-3). `<Body>` is unused after this change and retires; its P6 slot becomes empty.

  *Trigger:* discrepancy surfaced 2026-05-28 between the canonical grammar's double-braced `MethodDecl` and the prose worked examples in `lo-3-reference.md` §5 plus the 43 new conformance tests written this round (all single-braced). One of the two had to give. The discrepancy could have been resolved by Option A (canonical wins, fix prose + tests to double-braced) or B (relax canonical to single-braced). DC chose B; the LL(2) preservation argument confirmed B is locally clean (the LL(2) disambiguations between `<VarDecl>` and `<Stmt>` are unchanged; only the brace count is different). B-wide rather than B-narrow because preserving brace-shape continuity across the LO-2 → LO-3 transition is pedagogically clearer than a discontinuity. Rejected: Option A (preserve double-braced canonical) — proportionally larger mechanical fix-up on the new tests and the §5 examples without any user-facing improvement; Option C (allow both single and double-braced forms via parser disambiguation) — adds a special case to the parser for marginal benefit.

  Cascade: `liveoak-grammar.tex` LO-2 and LO-3 tables (P5 inlined, P6 retired); `lo-3-reference.md` §2 grammar table (P5/P2 updated, P6 retired) and the parser-disambiguation paragraph; the conformance test corpora — `lo-testing/LO-2/` tests (existing, double-braced) require brace-stripping migration; `lo-testing/LO-3/` tests (existing pre-redesign, double-braced) require brace-stripping plus the constructor relocation; the new tests written 2026-05-28 are already single-braced and need no change; `lo-3-reference.md` §5 worked examples are already single-braced and need no change.

### Repository topology — **Locked 2026-05-28; org + cutover executed 2026-05-29**

The course's GitHub repos and how they relate. Driven by the constraint that GitHub repo visibility is per-repo (no per-directory setting) while the current `cs378h` repo mixes instructor-only planning with student-facing specs.

Locked:
- **The language references stay in the planning repo and are published.** `liveoak-grammar.tex/pdf`, `lo-3-reference.md`, `lo-4-reference.md`, and `runtime-abi.md` remain in the planning repo (`CS378H-Master/planning`) as their canonical home; students receive them via publication (course site or read-only export), not via direct repo access. Chosen over splitting the references into a separate student-facing `lo-spec` repo specifically to avoid disrupting the SC workflow, which relies on project knowledge syncing the references from the planning repo. The planning repo stays the canonical source-of-truth that project knowledge syncs from.
- **`lo-testing` splits out into its own repo.** The conformance suite, currently a subdirectory of `cs378h` (deducible from its files appearing in project-knowledge search, which syncs only from `cs378h`), becomes a standalone student-visible repo parallel to `lo-runtime`. Same audience logic that makes `lo-runtime` separate: students clone the suite and run their compilers against it.
- **`lo-runtime` is a separate student-visible repo** (as specified in the WS-1 docs): the three runtime skeletons, versioned/tagged for the semester per `runtime-abi.md`'s "ABI is the contract for Fall 2026" line.

Resolved 2026-05-29:
- **Org, not independent personal repos.** DC chose a named GitHub organization to host the planning repo, `lo-runtime`, and `lo-testing`, plus any GitHub Classroom per-student project repos. Confirms SC's recommendation: cleaner audience-based visibility, team-based access control for CC, durability across offerings.
- **Org name: `CS378H-Master`.** Per DC's established `CSxxx-Master` convention for course orgs in other courses; reported available. Persistent across offerings (per-offering separation handled by tags/branches on infrastructure repos and by archiving prior student project repos, not by new orgs).
- **Planning-repo name: `planning`** (full path `CS378H-Master/planning`). Bare `planning` chosen over `cs378h-planning` since the org already carries the course number; matches the prefix-free naming of `lo-runtime` and `lo-testing`. The documentation-only cross-reference prefix is therefore `planning:` (e.g., `planning:lo-3-reference.md`); `CLAUDE.md` and `ws1-build-skeletons.md` were updated 2026-05-29 (the prior `cs378h:` prefix and prose repo-references replaced).

Repo cutover — **executed 2026-05-29** (DC confirmed). The org and all three repos exist (`CS378H-Master/{planning, lo-runtime, lo-testing}`); WS-1 ran and merged in `lo-runtime`; the `lo-testing/` subtree was split out to its own repo; and project knowledge now syncs from `planning` plus **`lo-runtime` and `lo-testing` as second sources**, so SC retains visibility into all three (the split's visibility consequence is mitigated — SC can drive test work via project knowledge, and Claude-Code-in-`lo-testing` remains the better home for mechanical passes like the brace migration). The cutover steps, for the record:
- `cs378h` → `CS378H-Master/planning` (rename + move). Done.
- Project-knowledge sync re-pointed to `CS378H-Master/planning` (+ the two second sources). Done.
- `lo-testing/` split out to `CS378H-Master/lo-testing` (history-preserving). Done.
- `lo-runtime` created; WS-1 merged. Done.
- Bootstrap text: `handoff-menu.md` updated 2026-05-29. **`project_instructions.md` still DC-side** (not in SC's working set) — confirm it names `CS378H-Master/planning`.

Cascade complete: `CLAUDE.md` and `ws1-build-skeletons.md` cross-references done 2026-05-29; `handoff-menu.md` bootstrap text done 2026-05-29. The `lo-testing` internal docs' old `cs378h:` prefixes were fixed in CC's PR `lo-testing#3` (`migration-2026-05-28.md`, `README.md`). Remaining: `project_instructions.md` bootstrap text (DC-side).

### WS-1 spec deltas — **Ratified 2026-05-29**

CC's WS-1 skeleton work surfaced five points; SC resolved each. The merged skeleton implementation is authoritative where it diverged from the draft runbook — the runbook/ABI were corrected to match it.

Runbook/ABI corrections (now in the specs):
- **(1) ShadowFrame layout.** `roots` is an *inline* array after the `{ parent, num_roots }` header, not a pointer field — the draft runbook step 1.3 wrongly said `*mut *mut Object`. A pointer field would make a team's GC scan the wrong memory. Fixed in `ws1-build-skeletons.md` step 1.3 and stated in `runtime-abi.md` §3.3.
- **(2) `lo_abort_null_receiver`.** Provided per ABI §4.4/§3.8 (exit 102 / WASM trap) but missing from the runbook step lists. Added to the provided-set summary and as an explicit step (under runtime init), with a note that Phases 2–3 mirror it. It is *provided*, not stubbed.
- **(3) C++ harness link.** Runbook §4.4 verification snippet used `cc` for all skeletons; the C++ static lib needs the `c++`/`$CXX` driver to pull in the C++ stdlib. Snippet corrected; `tests/c_harness/run.sh` named as authoritative.

Decisions blessed/recorded:
- **(5a) Per-language I/O backends — BLESSED.** Rust/Zig std I/O, C++ `<cstdio>`, not raw libc. Rationale documented in `runtime-abi.md` §3.7 and runbook step 1.8: `feof` reports EOF only *after* a consumed read, so it cannot implement `lo_eof`'s peek-without-consume semantics; the language stdlibs express the buffered peek cleanly, bare libc `stdin` globals do not portably. Observable behavior and abort codes identical across skeletons.
- **(5b) Offset-of, not size-of, for flexible tails — BLESSED + documented.** Inline `data`/`roots` begin at `offset_of(...)` (String data = 20, ShadowFrame roots = 16 on 64-bit), not `size_of` (24, includes trailing padding). Stated in `runtime-abi.md` §2.2/§3.3 and runbook steps 1.3/1.4; `LO_STRING_CLASS.instance_size` corrected to the offset-of base. Confirmed by `cpp/tests/abi_tests.cpp` (`string_data_offset()==20`, `shadow_frame_roots_offset()==16`).
- **(5c) Heap 16 MiB / `LO_HEAP_SIZE` override — BLESSED** (kept as specified; no change).
- **(5d) C++ test framework Catch2, vendored single-header — BLESSED** (implementation choice; no spec impact).
- **(5e) WASM stub trap shape — KEEP AS-IS (not unified).** SC's open question resolved: language-native trap shapes for stubs are *not* part of the conformance contract — a stub only fires during skeleton development. Recorded in runbook (stub-form bullet). Revisit only if a future harness must distinguish "stub not implemented" from a real abort.

Item (4) — the three placeholder `tests/lo_programs/*.lo` fixtures used non-LO-3 syntax (`class C { method m() : int }`, `var s : String`, `out.printString`). SC rewrote all three to grammar-correct LO-3 preserving the stable expected-output contract (`alloc_basic`→42, `string_basic`→hello, `class_basic`→7); delivered in `lo-runtime-fixtures/`. They replace the placeholders in `lo-runtime/tests/lo_programs/`; the `TODO(SC)` markers in those files and `tests/README.md` can then be cleared.

DC-side resolutions (CC flagged): **C++/WASM toolchain — LOCKED 2026-05-29: wasm-clang, not Emscripten** (`wasm32-unknown-unknown` first to match Rust's freestanding host-import model; wasi-sdk `wasm32-wasi` fallback only if the freestanding link needs C++ runtime-support symbols, keeping I/O on the project's host imports either way; C++ WASM branch traps via `__builtin_trap()`/`abort()` rather than `throw`, since freestanding wasm32 has no exception unwinder and the trap shape isn't part of the conformance contract). Recorded in `ws1-build-skeletons.md` (Phase 3.1, Phase 3.6–3.11, the Phase 0 toolchain note, and the resolved open-items list). Still DC-side/open: build-box Homebrew broken, toolchains provisioned user-local (informational).

### LO-5 selection process — **Locked**

- Ranked top-3 preferences with justification, due end of Week 3.
- Instructor allocates by start of Week 4. Caps published transparently.
- "Other" submissions have an earlier deadline (~Week 2.5) and may require iteration.
- FSD due end of Week 5.
- Mid-semester teaching lecture on the feature in one of three LO-5 talk blocks (W10.2, W13.2, W14.2).
- Mini-conference presentation at finals week.

### Implementation languages — **Locked**

- **Toolchain first tier:** Rust, OCaml, Haskell, Scala, F#.
- **Toolchain second tier (with approval):** Kotlin, Java, Swift, modern C++.
- **Toolchain discouraged:** Python, JavaScript, Ruby, plain C.
- **Runtime:** Rust (recommended), Zig, or modern C++.
- Per-team unification: one toolchain language per team; runtime may differ.

### Runtime infrastructure — **Locked**

- Three parallel skeleton crates — Rust, Zig, modern C++ — sharing one ABI; teams pick by language preference.
- Mono-repo with `rust/`, `zig/`, `cpp/` subdirectories and shared `tests/` plus `runtime-abi.md` at root.
- Object header: 16 bytes on 64-bit (8-byte class-descriptor pointer + 4-byte GC bits + 4-byte flags reserved for LO-5 extensions).
- Shadow stack: linked frames per function, allocated on the C stack at function entry.
- Class descriptor with explicit `pointer_offsets` array for GC scanning (not a bitmap).
- Strings: objects with class pointer + length + inline UTF-8 bytes. No small-string optimization. `lo_string_reverse` operates on codepoints, preserving UTF-8 validity; rejected alternative (byte-reversed) considered and rejected on 2026-05-28: byte-reversal of multi-byte sequences yields malformed UTF-8, surfacing as misbehavior in any downstream string operation on non-ASCII input.
- `LO_EMPTY_STRING` exported as a canonical empty-string instance; codegen uses it to initialize String fields post-`lo_alloc`. Added 2026-05-28 alongside the LO-3 String-default lock.
- Pre-semester acceptance test for each skeleton: implement Cheney, run shared test suite, all tests pass.
- **Cascade warning:** reopening object header size or shadow stack design cascades through codegen specs (`architecture-reference.md`), the runtime ABI (`runtime-abi.md`), and the conformance test suite. Not a casual reopen.

### Target architecture — **Locked**

- Native code targets **x86-64 only**, on Linux, using the System V AMD64 ABI. AArch64 considered and rejected: matching Brand's *Building a Debugger* (the primary P4 text, x64-only) outweighs AArch64's pedagogical and aesthetic advantages.
- Architecture subset defined in `architecture-reference.md`: roughly 40 instructions, 16-register GPR set, RIP-relative addressing, no SIMD, no x87, no atomics, no system instructions. `INT3` for breakpoints.
- Assembly syntax convention: **Intel** (matches Brand, Intel SDM, Microsoft docs). AT&T appears in `objdump`/`gdb` output; students read but do not write it.
- Students with M-series Macs or AArch64 hardware develop against an x86-64 VM, container, or remote machine.

### GC menu (P3) — **Locked**

- **Cheney's semispace** — canonical default, ★★★☆☆.
- **Mark-compact, Lisp 2 sliding** — three-pass; no half-heap waste, ★★★☆☆.
- **Generational, Cheney nursery + Cheney tenured** — adds remembered sets, write-barrier logic, ★★★★☆.
- Open choice; no allocation caps.

### Write barriers — **Locked**

- `lo_gc_write_barrier(obj, field_offset, value)` mandated in P1 codegen at every pointer store.
- Bare-store implementation for non-generational collectors; recorded write into remembered set / card table for generational.
- Mandate from P1 keeps GC algorithm choice decoupled from codegen.

### Team formation — **Locked**

- Hybrid allocation.
- Structured bio form: skills, experience, schedule, optional prefer-with / not-with (no justification required).
- 4-6 members per team, median 5.
- Bio due end of Week 1; allocation by start of Week 2.

### Peer evaluation and grading — **Locked**

- Five milestones across the semester.
- Three axes (effort, technical contribution, collaboration) on a 1-5 scale plus free text.
- Confidential; running feedback returned each milestone.
- `project_grade = 60% correctness + 25% code review + 15% viva`.
- `individual_score = project_grade × peer_factor`, with `peer_factor ∈ [0.85, 1.05]`. Later milestones weighted more heavily.

### Drop policy — **Locked**

- Weeks 1-2: free re-form.
- Weeks 3-7: instructor redistributes from teams losing members.
- Week 8+: team continues smaller with proportional scope adjustment.

### Lecture sequence — **Locked**

- 28 sessions detailed in `lecture-sequence.md`.
- W1.1: Course intro + Theory of Computation hybrid (~25/65 split).
- W1.2: GFG-based parsing using the Bilardi-Pingali approach (instructor teaches).
- W3.1, W8.1: Formal semantics I and II.
- W5.1: SSA + multi-level IR concepts (MLIR-style) — lecture only, not implemented.
- W14.1: Production toolchain survey (JIT, optimization, concurrency).
- Three student LO-5 talk blocks: W10.2, W13.2, W14.2.

### Reading list — **Locked**

- **Core textbook:** Cooper & Torczon, *Engineering a Compiler* (3rd ed., 2022).
- **Required supplementary books:** Pierce *TAPL*; Jones, Hosking, Moss *Garbage Collection Handbook* (2nd ed., 2023); Brand *Building a Debugger* (2024).
- **Recommended (optional):** Scott *Programming Language Pragmatics* OR Ramsey *Programming Languages: Build, Prove, Compare*.
- Per-lecture supplementary papers and specs in `reading-list.md`.
- Per-paper URL verification is **Pending-data** (see § In flight); the reading-list selection itself is locked.

### Review chain — **Locked**

- **Default reviewer:** DC.
- **Pedagogical-clarity path:** TAs review student-facing materials for clarity, density, pedagogical landing; activated during TA onboarding; advisory, not gating.
- **Future-instructor-portability path:** one peer expert (named when bound) gives consultative reads when materials are stable enough; advisory, not gating.
- Documented in `project_instructions.md` § Quality assurance.

---

## In flight (pending decisions)

### Reading list per-paper URL verification — **Pending-data**

- Best-effort ACM DOI URLs in `reading-list.md` need spot-check: Chaitin (1982); Briggs et al. (1994); Poletto-Sarkar (1999); Cheney (1970); Lieberman-Hewitt (1983); Ungar (1984); Aho-Ganapathi-Tjiang (1989); Click-Paleczny (1995); Wright-Felleisen (1994); Aycock (2003).
- The Levine *Linkers and Loaders* URL (<https://www.iecc.com/linker/>) needs a freshness check — author page is old.
- Reading density may need trimming in W7 (graph coloring + linear scan both as papers) and W11 (Levine + Sy Brand both as primary).
- Tracked as workstream WS-5; awaiting URL-check data before this decision can lock.

### W14.1 toolchain survey content — **Pending**

- Editorial choice of which production systems to highlight (HotSpot, Graal, V8, LuaJIT, etc.). Currently Aycock JIT survey + HotSpot + Graal + LuaJIT, but this can be revised.
- No empirical input gates this; awaiting DC's editorial judgment.

### Planning-repo name — **Resolved 2026-05-29**

- Resolved: planning repo is `planning` (full path `CS378H-Master/planning`). Cross-reference prefix is `planning:`; `CLAUDE.md` and `ws1-build-skeletons.md` updated. See "Repository topology" locked section for the cutover steps (re-point project-knowledge sync; `lo-testing` subtree move; `project_instructions.md` bootstrap-text update — DC-side).
- All repo-topology naming is now settled: org `CS378H-Master`, repos `planning` / `lo-runtime` / `lo-testing`.

### WS-1 toolchain environment ownership — **Resolved 2026-05-29**

- Resolved: **CC builds the local toolchain environment** (Rust via rustup + `wasm32-unknown-unknown`; Zig stable ~0.13; C++ clang 18+/gcc 14+, CMake 3.28+, clang-format, clang-tidy) as a Phase 0 step, **with DC as fallback** if CC's environment lacks the privileges to install toolchains.
- Encoded in `ws1-build-skeletons.md` Phase 0 (toolchain-environment prerequisite paragraph) and the WS-1 workstream row. The contingency: CC attempts install at Phase 0; if blocked by environment permissions, CC surfaces immediately and DC provisions before CC proceeds.
- This was the actual gate on WS-1 starting alongside org/repo creation; with ownership assigned, the remaining gate is org + repo creation (DC).

### W3.2 WASM intro reading — **Pending**

- Primary reading is currently the WASM core spec, which is dry. A better tutorial-style intro would improve the lecture; candidate not yet identified.
- Awaiting a candidate identification — could fold into WS-5's reading-list work.

---

## Risk register

The risk register seeds from the pre-semester action items: each item has a risk shadow (what happens if the work isn't done in time, or is done badly). The workstreams above are the mitigations. Risk lifecycle per `common_sop_v7.md` § Conditional / Deadline-bounded.

Most risks remain **Identified**. Two have moved as work activated: **R1** (runtime skeletons) is **Mitigated** — WS-1's three skeletons are built, tested, and merged CI-green; full closure awaits WS-2's cross-skeleton + Cheney validation. **R4** (conformance suite) is **Mitigated** — WS-4's 53 redesigned tests exercise the new LO-3/LO-4 features and the pre-redesign corpus is migrated (PR `lo-testing#3`); the (A) error-code retrofit and WS-6 granularity remain. Severity is the rough cost-of-failure with the deadline-bounded conditional active.

| ID | Risk | Severity | Mitigation | Lifecycle |
|---|---|---|---|---|
| R1 | Runtime skeletons not built and tested by P3 start (Week 8); P3 has nothing to run against | high | WS-1 — three skeletons (Rust/Zig/C++) built, tested, merged CI-green 2026-05-29 | Mitigated |
| R2 | Cross-skeleton implementations diverge on ABI; the three baselines disagree | high | WS-2 — depends on R1; conformance test against shared LO programs | Identified |
| R3 | LaTeX grammar source not reflowed for LO-3 redesign + LO-4 productions; student handouts don't match the language | medium | WS-3 — one-time edit, now unblocked by 2026-05-28 LO finalization | Identified |
| R4 | LO-3 / LO-4 conformance test suite doesn't exercise the new features; students pass tests but ship bugs | medium | WS-4 — 53 redesigned tests landed (constructors, `super(...)`, casts, `instanceof`, P20 receivers, IO round-trips); pre-redesign corpus migrated; (A) retrofit + WS-6 granularity remain | Mitigated |
| R5 | Best-effort reading-list URLs rot between now and semester; students hit 404s | low | WS-5 — spot-check sweep before semester | Identified |
| R6 | Conformance suite present in course repository at wrong granularity for student submissions (per-class vs. per-method, etc.) | medium | WS-6 — depends on WS-4 | Identified |
| R7 | Required supplementary book copies not available to students at start of semester | low | WS-7 — physical / library / ebook procurement | Identified |
| R8 | UT library / ACM DL access not confirmed; student paper readings blocked | low | WS-8 — DC confirms access pre-semester | Identified |
| R9 | Bio-form template not ready for end of Week 1; team formation slips | medium | WS-9 — SC drafts; DC reviews | Identified |
| R10 | TAs onboarded incompletely; grading or office hours falter once projects ship | medium | WS-10 — DC walks TAs through repo and artifacts | Identified |

Risks that materialize exit the register as incidents in their own workstreams (per the SOP), with process notes for prevention.

---

## File registry

All canonical files in the project repo. Outputs land in `/mnt/user-data/outputs/` during sessions and are committed to the repo by DC via the reconciliation flow in `project_instructions.md`.

| File | Purpose | Status |
|------|---------|--------|
| `common_sop_v7.md` | Universal layer; cross-project norms | v7 issued 2026-05-23 (replaces v6 added 2026-05-21); active references in this ledger aligned 2026-05-28 |
| `project_instructions.md` | Project-specific norms layered on the SOP; entry point for new collaborators | Added 2026-05-21 (rectification); supersedes prior claude.ai project-settings copy |
| `state-ledger.md` | This file; running status board with workstreams, locked decisions, in-flight pending decisions, risk register, file registry, process notes | Restructured 2026-05-21; LO finalization + conformance suite + cascade updates 2026-05-28; org/cutover/WS-1-deltas/WS-4 updates 2026-05-29; accuracy review (dates, headlines, risk + registry staleness) 2026-05-30 |
| `master-plan.md` | Course design overview document | Stable |
| `handoff-menu.md` | Bootstrap menu for new SC instances: DC pre-handoff checklist plus five parameterized opening-message modes (cold start, resume in-flight, workstream-focused, decision review, discrete task) | Added 2026-05-28; sync-source reference updated 2026-05-29 |
| `cutover-checklist.md` | Sequenced DC operational checklist for the repo cutover (org creation, planning rename, lo-testing split, lo-runtime creation, ABI copy) and WS-1 kickoff; critical steps flagged. Operational / largely one-time | Added 2026-05-29; cutover executed 2026-05-29 |
| `ws4-corpus-migration.md` | CC execution spec for the WS-4 systematic brace-migration of the pre-redesign LO-2/LO-3 corpus (the robust-script layer on top of `migration-2026-05-28.md`); destined for `lo-testing/runbooks/`. Run as Claude-Code-in-`lo-testing` | Added 2026-05-29 |
| `lo-testing-CLAUDE.md` | Repo-orientation doc for Claude-Code sessions in `lo-testing` (purpose, role split, authoritative docs, header/naming/grammar conventions, cross-ref prefix, active work); destined for `lo-testing/CLAUDE.md` | Added 2026-05-29 |
| `lecture-sequence.md` | 28-session schedule with topics per session | Stable |
| `calendar.md` | Session-to-date mapping plus project / midterm / LO-5 deadlines | First iteration |
| `course-policies.md` | Cross-cutting policies (team formation, peer eval, languages, runtime, GC menu) | Stable |
| `reading-list.md` | Per-lecture reading list with books, papers, specs | Draft awaiting review; URL spot-check open (WS-5) |
| `lo-3-reference.md` | LO-3 language reference: redesigned with three-section class declarations; entry-point and IO conventions; P20 receiver `<ObjName>` | First iteration; revised 2026-05-28 (initial LO-3 conventions + IO-redesign cascade reverting void→int Main.main and IO single-class → Input/Output two-class with `in`/`out`/`err` pre-bound) |
| `lo-4-reference.md` | LO-4 language reference: additive extension of LO-3 with single inheritance; finalization edges resolved | First iteration revised 2026-05-28 |
| `runtime-abi.md` | Runtime ABI specification (object layout, entry points, build configuration); `LO_EMPTY_STRING`, cast-failure contract, read I/O, abort exit codes, `lo_abort_null_receiver` | Revised 2026-05-28 (LO_EMPTY_STRING + cast contract + read I/O + codepoint reverse + abort exit codes 102/110/111/112/120/137 + `lo_abort_null_receiver` helper + Input/Output framing); revised 2026-05-29 (WS-1 deltas: offset-of-not-size-of for flexible tails §2.2/§3.3, per-language I/O backend rationale §3.7). Canonical in `planning`; working copy in `lo-runtime` (keep in sync) |
| — *`CS378H-Master/lo-testing` (separate repo)* | The conformance suite split out of the planning repo on 2026-05-29 (history-preserving) and is now its own student-visible sibling repo, synced into project knowledge as a second source. Contents: `README.md`, `error-codes.md` (~35 compile + 7 runtime-abort codes), `migration-2026-05-28.md` (recipe), `runbooks/` (CC specs incl. `ws4-corpus-migration.md`), `CLAUDE.md`, `tools/migrate_braces.py`, and the `LO-2` / `LO-3` / `LO-4` test trees. Pre-redesign LO-2/LO-3 corpus migration **complete** (PR `lo-testing#3`); (A) error-code retrofit in progress. No longer tracked file-by-file here — this registry's scope is the `planning` repo | Split executed 2026-05-29; now a sibling repo |
| `architecture-reference.md` | x86-64 subset spec: instructions, addressing, calling convention, syntax | First iteration |
| `lo5-feature-briefings.md` | Per-feature briefings with references and URLs, grouped by design dimension | Stable |
| `lo5-fsd-template.md` | FSD template students fill in for their LO-5 feature | Stable |
| `scripts/hooks/pre-push` | Local git hook: blocks direct push to `main` | Added 2026-05-21 (bootstrap) |
| `scripts/install-hooks.sh` | One-shot installer for local git hooks; run after clone | Added 2026-05-21 (bootstrap) |
| `.gitignore` | Excludes macOS / Windows / editor noise from staging | Added 2026-05-21 (bootstrap) |
| `liveoak-grammar.tex` | Canonical grammar source: LO-0 through LO-3 production tables in LaTeX, with shared preamble specifying binop and unop semantics per operand type. Authoritative for grammar productions and operator semantics; the markdown reference docs (`lo-3-reference.md`, `lo-4-reference.md`) prose-describe what this LaTeX source formally specifies. WS-3 modifies this file to add the LO-3 redesign (P2 `<ConstructorDecl>`, P4 three-section, P45 `<ObjName>`) and LO-4 productions (P3 cast, P21 super-method, P46 instanceof) | Surfaced into context 2026-05-28 — covers pre-redesign LO-3; awaiting WS-3 reflow |
| `liveoak-grammar.pdf` | Compiled output of `liveoak-grammar.tex`; the rendered grammar reference distributed to students | Build artifact; regenerated by WS-3 |

---

## Process notes

Generalizations captured for future sessions. cs378h-specific lessons live here; SOP-level lessons that should bubble up to the meta-project sit in the `## Back-to-meta surfaces` section below.

- **Status interpretation when all workstreams are Queued.** The cross-project board's worst-workstream rule renders an all-Queued project as ⚫. The intent of the rule is "if active work is going badly, project status reflects that." When no work is active but real pre-semester work is pending with a real deadline, the project's intrinsic status is 🟡 — the rule's reading and the project's reality diverge. This ledger sets `status: yellow` to reflect the latter. If the cross-project board's automation overrides to ⚫, that override is meaningful information about engagement (DC has not yet kicked off workstreams) rather than a bug.
- **Workstream / decision / risk separation.** Workstreams track work-to-do. Decisions track choices-to-make. Risks track failure-modes-of-work-not-done-in-time. The three are linked but distinct; an item can appear in two of them with different framings (e.g., the reading-list URL check is WS-5 as work, Pending-data as a decision, R5 as a risk). This is by design — the three views serve different readers.
- **Cascade warnings on structural locks.** The runtime ABI and the LO grammar are the two areas with the strongest cascade effects. Casual reopening of either has been called out explicitly in the locked entries; SC's enforcement role per the reopening gate applies most forcefully here.
- **The lo-3/lo-4 vs lo5 filename asymmetry is known.** Tracked as a separate consistency item, not addressed in the 2026-05-21 rectification or the 2026-05-28 LO finalization. Renaming touches every cross-reference in every document; cost outweighs the benefit until a natural break point.
- **Transcripts directory empty.** Without transcripts, decisions whose reasoning isn't captured in the locked-decisions entries are effectively irrecoverable. New locked entries should capture enough rationale that the entry alone is the audit trail.
- **In-app pointer text root cause (bootstrap misdiagnosis cascade).** The cs378h Claude.ai project's in-app instructions field initially carried a pointer line reading "Read it via the GitHub connector at session start." That phrasing primed every new chat session to expect a live GitHub MCP tool, look for it, fail to find it, and report the absence as if it were a setup gap — repeating across roughly six hours of sessions. The pointer was rewritten 2026-05-21 to be access-mechanism-agnostic: *"Read `project_instructions.md` and `state-ledger.md` at session start. Both are in project knowledge."* If the in-app pointer is touched in the future, preserve mechanism-agnosticism — don't name specific tools in instruction text.
- **Bootstrap operational verification.** The drift-recovery sentinel test (commit a uniquely-marked line to the repo, watch for re-indexing into project knowledge, verify a fresh chat session can find it) confirmed end-to-end that the integration delivers commits into chat-session-readable knowledge. Re-run if a future commit ever appears not to land in new sessions; the test design is in the back-to-meta notes below.
- **Pre-check canonical specs before composing option-lists for related decisions.** The 2026-05-28 LO finalization session surfaced two intra-session reopenings of decisions that had been locked minutes earlier: the String-default decision (locked as null, reopened on finding LO-3 §4.1 already specified empty-string) and the P20-receiver decision (locked as `<ObjName> = <Var> | this`, reopened on observation that LO's parens-heavy style and existing P25 wrapping made the restrictive form structurally inconsistent). Both reopenings were valid — DC named the trigger in each case (existing canonical spec; structural inconsistency with the rest of the grammar) — but both could have been avoided if SC had pulled the relevant existing specs and structural constraints before composing the option-list. The lesson: when posing options on a topic that has any existing spec or structural neighbor, search the spec corpus first. The option-list should reflect the existing state, not just the abstract design space.
- **Verify stated conventions against existing artifacts.** When locking a convention that has an existing artifact corpus (test files, code, prior offerings), pull a representative sample before locking — even when DC asserts the convention from memory. The 2026-05-28 int-`Main.main` supersession traces to SC accepting DC's stated "preserve the `void Main.main()` convention" without checking the actual `lo-testing/LO-3/` programs, which used `int main()` with the return value as the harness signal. Both sides contributed to the gap: DC's recall was off, SC didn't verify against the artifacts. The cost of a one-search verification is small; the cost of supersession (audit trail, cascade across `lo-3-reference.md` + `runtime-abi.md` + the conformance test rewrite) is meaningful. Apply this even when the stated convention sounds right — particularly when it sounds right, since plausibility lowers the bar for verification.
- **The markdown reference docs are not the full spec — the LaTeX grammar source is.** Surfaced 2026-05-28: SC drafted a conformance test for codepoint-aware string reversal and deferred it as "syntax unclear, lo-3-reference.md doesn't enumerate per-type unop semantics" — then DC surfaced `liveoak-grammar.tex` and its preamble explicitly documents that `~` on strings is reversal (alongside `~` on int = negation, `!` on bool = NOT). The markdown docs are prose layers that describe the grammar's structure and the spec's intent; the LaTeX source is where operators and productions are formally enumerated. When a question is about formal grammar (productions, operator semantics, type-of-operand rules), check the LaTeX source first; the markdown docs are for prose context and worked examples.
- **Multi-edge sessions surface cross-cutting integrity issues.** Both the LO-4 and LO-3 finalizations in the 2026-05-28 session were initiated as small targeted reviews ("resolve a few open edge cases") and grew during planning into substantial multi-document updates as cross-cutting issues surfaced (entry point, I/O calling convention, P20 receiver, String defaults). The growth was right — these were real gaps in the first-iteration documents — but the planning underestimated scope. Future "small clarification" sessions should anticipate scope growth and budget accordingly.

---

## Back-to-meta surfaces (post-bootstrap)

Items this rectification's execution surfaced for the meta-project's SOP v7 queue. The rectification memo's existing back-to-meta items still stand; these are *additional*, surfaced during bootstrap rather than during inventory.

- **Sync-vs-live integration shape for verification design.** The Claude.ai GitHub Integration is sync-into-project-knowledge, not live MCP. Verification of a sync-based integration is content match against project knowledge plus a drift-recovery test (commit a unique sentinel, confirm re-indexing). Verification of a live-MCP integration is a source-distinguishing question (commit metadata, queries the local mirror can't answer). cs378h's bootstrap conflated the two and proposed a live-MCP-style test for a sync-based integration. Candidate v7 amendment to § Session continuity: name which integration shape is in use before naming a verification; the SOP's "GitHub-Claude connector required" line should be augmented to acknowledge that the same connector delivers content via different paths depending on use mode (Chat / Projects / Claude Code).
- **Before/after distinguishability requirement in verification design.** A verification with identical before- and after-states is not a test. cs378h's bootstrap proposed a date-bump test where the date wouldn't change because the ledger's `last_touched` already matched today's date — collapsed before/after, no signal possible. Candidate v7 amendment to § Working norms: *"Before proposing a verification, mentally run pass and fail through it — confirm both that the integration path is actually exercised and that the test's before- and after-states differ in a way the test can detect. Either failure mode renders the verification incoherent."*
- **Pointer / instruction text should be access-mechanism-agnostic.** Don't name specific tools in instruction text. Naming a tool primes every reader to expect that tool; if the integration delivers the same content via a different mechanism, the named-tool framing creates a recurring phantom failure mode that repeats in every session reading the instruction. cs378h's six-hour misdiagnosis cascade traces to one such line in the in-app pointer ("via the GitHub connector"). Candidate v7 amendment to § Artifact conventions: *"Instruction text and pointer text should describe what to read, not how to access. Access mechanism is the integration's concern; instruction text that names tools is fragile to integration changes and creates phantom failure modes when reality and naming diverge."*
- **Bootstrap instructions need a built-in `.gitignore`.** The original bootstrap sequence had DC run `git add .` without a `.gitignore` in place, sweeping up `.DS_Store` (macOS Finder metadata). Caught at the staging check, but cleaner to seed `.gitignore` in the initial scaffolding alongside `scripts/hooks/`. Candidate v7 amendment to § Version control's bootstrap protocol: *"Initial repo scaffolding includes a project-appropriate `.gitignore` (at minimum: platform noise files for the OS the human collaborator runs) before the first `git add .`."*
- **Pre-spec-check before composing option-lists.** Surfaced 2026-05-28: two LO finalization decisions were locked briefly and then reopened within the same session because SC's option-list didn't reflect existing canonical specs (LO-3 §4.1 String empty-string default) or structural constraints (P25's existing role making the restrictive `<ObjName>` inconsistent). The intra-session reopening protocol worked correctly — both reopenings had named triggers and were valid — but the better preventive is: pull the spec corpus before composing the option-list, not after. Candidate v7 amendment to § Working norms: *"When composing an option-list for a decision that has any existing spec or structural neighbor, search the spec corpus and walk the structural neighbors first; the option-list should reflect existing state, not just the abstract design space. Intra-session supersessions are a signal that the option-list was under-researched."*

Further surfaces will be collected as cs378h's pre-semester action items execute and comp-panini's bootstrap begins.

---

## Notes on the working session producing this state

The original design discussion was a sounding-board session in late April / early May 2026, conducted between DC and SC. Decisions were made through structured option-then-pick exchanges; the full reasoning was in the conversation transcripts (now unavailable in this working environment).

The 2026-05-21 restructure applied the cs378h rectification spec against `common_sop_v6.md`: YAML front matter added; sections reorganized under SOP-aligned headings; lifecycle labels applied to decisions; risk register seeded from pre-semester action items; `## Active workstreams` and `## Process notes` headings added per the SOP's session-continuity expectations. Content (locked decisions, file registry, the working-session provenance above) was preserved.

The rectification's infrastructure deltas all landed on 2026-05-21: GitHub repo created (`sidchatterjee/cs378h`, private, under DC's personal account); content migrated from `/mnt/project/` plus the rectification artifacts as the initial-import commit on `main`; `.gitignore` added covering macOS / Windows / editor noise; `scripts/hooks/pre-push` and `scripts/install-hooks.sh` installed for local branch protection (no direct push to `main`); first-push gitleaks scan clean; Claude.ai GitHub Integration wired to the cs378h project with the repo synced into project knowledge; in-app pointer text rewritten to mechanism-agnostic phrasing after diagnosing it as the root cause of a recurring misdiagnosis pattern in new chat sessions; drift-recovery sentinel test confirmed re-indexing works end-to-end. Bootstrap is operationally complete; subsequent work flows through the reconciliation path documented in `project_instructions.md`.

The post-bootstrap ledger update (2026-05-21, same calendar day) removed the drift-recovery sentinel, updated this closing note to reflect bootstrap completion, dropped the connector item from `needs_dc_next`, added `scripts/` and `.gitignore` to the file registry, and recorded the bootstrap's lessons in `## Process notes` and `## Back-to-meta surfaces`.

The 2026-05-28 LO finalization session activated as the first substantive design work after bootstrap, against the queued workstream board. Originally scoped as a small LO-4 review pass before drafting the CC spec for WS-1 (runtime skeletons), the session grew during planning into a multi-document update covering LO-4 finalization (six edges in §3 and §4 of `lo-4-reference.md`), LO-3 convention finalization (entry point, IO preamble, P20 receiver, String defaults — four conventions previously implicit or undocumented), and the runtime ABI changes that cascade from them (`LO_EMPTY_STRING` export, `lo_cast_check` failure contract, read I/O entry points). Two decisions were briefly locked and reopened within the session — the String default (null → empty-string, on finding the existing LO-3 §4.1 spec) and the P20 receiver (`<ObjName> = <Var> | this` → `<ObjName> = <Var> | this | ( <Expr> )`, on observation that P25's existing role made the restrictive form structurally inconsistent); both reopenings had named triggers and were valid per the reopening protocol. The session shipped four file updates (`lo-3-reference.md`, `lo-4-reference.md`, `runtime-abi.md`, this state ledger) and left WS-3 unblocked and WS-4's first cut as the immediate next item.

Continued 2026-05-28 work produced the WS-1 CC engagement spec (`CLAUDE.md` and `runbooks/ws1-build-skeletons.md`, both targeted at the lo-runtime repo) and the WS-4 conformance suite first cut (`lo-testing/README.md`, `lo-testing/error-codes.md`, plus 9 representative test programs across `LO-3/ValidPrograms/`, `LO-4/{ValidPrograms,InvalidPrograms,RuntimeAbortPrograms}/`). The conformance suite work triggered three further LO-3 supersessions, all rooted in surveying the existing `lo-testing/LO-2/` and `lo-testing/LO-3/` test files added to the repo the same day:

- Entry-point convention reverted from `void Main.main()` to `int Main.main()` — the existing tests use `int main()` with the return value as the harness signal.
- IO preamble redesigned from a single instantiable `IO` class to two synthetic classes `Input` and `Output` with three pre-bound names `in`/`out`/`err` — eliminates the `IO out; out = new IO();` boilerplate in every IO-exercising test and lets the static type checker catch role mismatches like `out.read_int()`.
- The "every previous-LO-3 program with implicit constructor remains valid" characterization in the LO grammar locked entry was corrected — prior LO-3 programs use explicit `void ClassName(args)` constructors in the method section and require mechanical migration to the redesigned `[ ]` bracket form.

A separate cascade landed `lo_string_reverse`'s semantics change from byte-reversal to codepoint-reversal (consistent with the §2.2 UTF-8 commitment) and locked the runtime-abort exit codes 102 (null receiver), 110-112 (read failures), 120 (string-repeat negative), and 137 (OOM) — propagated into `runtime-abi.md` §3.1, §3.2, §3.7, and a new §3.8 introducing the `lo_abort_null_receiver` helper. Cross-referenced in `lo-testing/error-codes.md`. The day's two largest surface areas — conformance suite + IO/entry-point cascade — settled in a single coherent push, with the supersessions documented per the SOP's prime convention.
