# CS 378H — LO Error-Code Vocabulary

**Fall 2026. First cut, 2026-05-28.**

This document defines the compile-error and runtime-abort categories used in conformance-test contracts (see `README.md` for how test headers reference these). Student compilers emit the corresponding code in their stderr output; the conformance harness substring-matches against the code rather than the human-readable message, so student compilers retain freedom over diagnostic phrasing while the suite still validates that the right category of error is being caught.

Codes are stable across the semester: a code added in this vocabulary will not be renumbered or removed without a course-wide announcement. The vocabulary may grow as edge cases surface during the semester.

## Compile-error categories

Organized by compiler phase. A given compile error must emit at least one of these codes; emitting multiple codes is acceptable when an error genuinely spans phases (e.g., a missing constructor flagged at both well-formedness and inheritance check time). The harness matches any of the declared codes appearing in stderr.

### Parse-phase errors

| Code | Trigger |
|------|---------|
| `E_RESERVED_KEYWORD_AS_IDENTIFIER` | An LO reserved keyword used where an identifier is required — `this`, `super`, `null`, `new`, `instanceof`, `extends`, or a type keyword (`int`, `bool`, `String`, `void`). Most commonly a type keyword in `<ClassName>` position, e.g., `extends String`, `extends int`, or `class String`. |
| `E_MALFORMED_CLASS_DECL` | A `<ClassDecl>` lacks one of its required sections, has them in the wrong order, or has empty `[ ]` brackets (constructor section must have at least one declaration when present). |
| `E_MALFORMED_CONSTRUCTOR` | A constructor declaration whose name does not equal the enclosing class's name. |
| `E_DELEGATION_BOTH_SUPER_AND_THIS` | A constructor body contains both a `super(...)` and a `this(...)` delegation. The grammar admits at most one delegation, only as the optional first statement, so a body with both is rejected during parsing. |
| `E_DELEGATION_NOT_FIRST_STATEMENT` | A `super(...)` or `this(...)` delegation appears anywhere other than the optional first statement of a constructor body. The grammar admits the delegation prefix only in first position, so a misplaced delegation is rejected during parsing. |
| `E_INVALID_UNICODE_ESCAPE` | A `\u{...}` string escape denotes a value that is not a Unicode scalar value — greater than `U+10FFFF`, or in the surrogate range `U+D800`–`U+DFFF`. |
| `E_PARSE_PHASE_OTHER` | Parse or lexer failure not covered by a more specific category in this phase. The sentinel for the parse phase. |

### Well-formedness errors

| Code | Trigger |
|------|---------|
| `E_DUPLICATE_CLASS_NAME` | Two class declarations with the same name in one program. |
| `E_DUPLICATE_FIELD` | Two fields with the same name in one class's field-parens. |
| `E_DUPLICATE_METHOD` | Two methods with the same name in one class (LO has no method overloading). |
| `E_DUPLICATE_CONSTRUCTOR_ARITY` | Two constructors in one class with the same parameter arity. |
| `E_FIELD_TYPED_VOID` | A field declared with type `void`. Only method return types may be `void`. |
| `E_FORMAL_TYPED_VOID` | A formal parameter declared with type `void`. |
| `E_RETURN_IN_VOID_METHOD` | A `return <Expr>;` statement in a `void`-returning method. (LO has no `return;` form; void methods have no return statements.) |
| `E_RETURN_MISSING` | A non-`void` method body where some control path reaches the end without returning. |
| `E_RETURN_IN_CONSTRUCTOR` | A `return` statement in a constructor body. |
| `E_LOCAL_SHADOWS_FORMAL` | A local variable redeclares the name of a formal parameter within the same scope. |
| `E_WELL_FORMEDNESS_OTHER` | Well-formedness violation not covered by a more specific category in this phase. The sentinel for the well-formedness phase. |

### Name-resolution errors

| Code | Trigger |
|------|---------|
| `E_UNKNOWN_CLASS` | A class name appears in a type position, `new` expression, `extends` clause, cast, or `instanceof` without being declared anywhere in the program. |
| `E_UNKNOWN_METHOD` | A method name appears in a method-invocation position and is not in the receiver's effective method set. |
| `E_UNKNOWN_VARIABLE` | A bare identifier appears in expression position and resolves to no field, formal, local, or pre-bound name. |
| `E_RESERVED_CLASS_NAME` | A class declaration uses a reserved class *identifier* (`Input`, `Output`, or another reserved name; `Main` is permitted but must satisfy the entry-point shape). `String` is a type keyword, not an identifier, so a class named `String` is a parse error (`E_RESERVED_KEYWORD_AS_IDENTIFIER`), not this code. |
| `E_RESERVED_VARIABLE_NAME` | A user declaration (field, formal, local) uses a reserved variable name (`in`, `out`, `err`). |
| `E_THIS_OUTSIDE_INSTANCE` | The expression `this` appears outside a method or constructor body. |
| `E_NAME_RESOLUTION_OTHER` | Name-resolution failure not covered by a more specific category in this phase. The sentinel for the name-resolution phase. |

### Type-check errors

| Code | Trigger |
|------|---------|
| `E_TYPE_MISMATCH` | An expression's type is incompatible with its context, in a case not covered by a more specific code below. |
| `E_ASSIGN_TYPE_MISMATCH` | The right-hand side of an assignment is not assignment-compatible with the left-hand side. |
| `E_RETURN_TYPE_MISMATCH` | A returned expression's type is not assignment-compatible with the method's declared return type. |
| `E_ACTUAL_TYPE_MISMATCH` | An actual argument's type is not assignment-compatible with the corresponding formal parameter's type. (Applies to method calls, `new`, `super(...)`, `this(...)`.) |
| `E_BINOP_TYPE_MISMATCH` | An operand of a binary operator has a type the operator doesn't accept. |
| `E_UNOP_TYPE_MISMATCH` | An operand of a unary operator has a type the operator doesn't accept. |
| `E_CONDITIONAL_TYPE_MISMATCH` | The two branches of a ternary `( e1 ? e2 : e3 )` have types with no common supertype. |
| `E_ARITY_MISMATCH` | A method call, `new`, `super(...)`, or `this(...)` provides a wrong number of actual arguments. |
| `E_RECEIVER_NOT_CLASS_TYPE` | The receiver in a method-call expression has a non-class static type. |
| `E_NULL_LITERAL_RECEIVER` | The receiver in a method-call expression is the literal `null` (statically detectable null dispatch). |
| `E_NONVOID_CALL_AS_STATEMENT` | A method-call statement (P19) invokes a non-`void` method; LO requires a non-`void` result to be used, so it may not be discarded as a bare statement. |
| `E_VOID_CALL_IN_EXPRESSION` | A call to a `void`-returning method appears in expression / value position; a `void` call may appear only as a call statement. |
| `E_TYPE_CHECK_OTHER` | Type-check failure not covered by a more specific category in this phase. The sentinel for the type-check phase. |

### Inheritance-check errors (LO-4)

| Code | Trigger |
|------|---------|
| `E_INHERITANCE_CYCLE` | A class declaration's `extends` chain transitively reaches the same class. |
| `E_FIELD_SHADOWING` | A subclass declares a field whose name appears anywhere in an ancestor's effective fields. |
| `E_OVERRIDE_SIGNATURE_MISMATCH` | A subclass declares a method whose name matches an ancestor's but whose parameter types or return type differ (LO requires invariant overrides). |
| `E_MISSING_CONSTRUCTOR_IN_INHERITING_CLASS` | A class with an `extends` clause has no `[ ]` constructor section. |
| `E_SUPER_IN_ROOT_CLASS` | A `super(...)` call appears in a constructor of a class with no `extends` clause. |
| `E_SUPER_METHOD_IN_ROOT_CLASS` | A `super.<MethodName>(...)` expression appears in a method body of a class with no `extends` clause. |
| `E_SUPER_METHOD_UNRESOLVED` | A `super.<MethodName>(...)` call where `<MethodName>` does not appear in any ancestor's effective method set. |
| `E_DELEGATION_CYCLE` | A `this(...)` delegation chain within one class forms a cycle (includes a constructor that delegates to itself). Applies from LO-3, where `this(...)` is introduced. |
| `E_DELEGATION_ARITY_MISMATCH` | A `super(...)` or `this(...)` call's actual arity matches no constructor's formal arity in the target. Because the implicit constructor is suppressed once any explicit constructor is declared, a `this(...)` in a single-explicit-constructor class has no valid target and fails here. The `this(...)` cases apply from LO-3. |
| `E_INHERITANCE_CHECK_OTHER` | Inheritance-check failure not covered by a more specific category in this phase. The sentinel for the inheritance-check phase. |

*Retired 2026-06-02: `E_EXTENDS_NON_CLASS`. The only non-class types are the reserved type keywords (`int`, `bool`, `String`, `void`); since `extends` takes a `<ClassName>` (an identifier) and those are keywords, `extends <non-class>` is rejected by the parser as `E_RESERVED_KEYWORD_AS_IDENTIFIER` — there is no semantic path to a separate extends-non-class error. Casts differ: `( ( T ) e )` takes a `<Type>`, so `((String) e)` parses and is caught semantically as `E_CAST_TARGET_NOT_CLASS`.*

### Cast and instanceof errors (LO-4)

| Code | Trigger |
|------|---------|
| `E_CAST_TARGET_NOT_CLASS` | A cast `( ( T ) e )` where `T` is not a class type (`int`, `bool`, `String`, `void`). |
| `E_CAST_SOURCE_NOT_CLASS` | A cast `( ( T ) e )` where `e`'s static type is not a class type. |
| `E_CAST_UNRELATED_TYPES` | A cast `( ( T ) e )` where neither `T <: static-type-of-e` nor `static-type-of-e <: T` (sibling cast). |
| `E_INSTANCEOF_SOURCE_NOT_CLASS` | An `instanceof` expression `( e instanceof T )` where `e`'s static type is not a class type. |
| `E_CAST_INSTANCEOF_OTHER` | Cast or `instanceof` check failure not covered by a more specific category in this phase. The sentinel for the cast-and-instanceof check phase. |

*Retired 2026-06-04: `E_INSTANCEOF_TARGET_NOT_CLASS`. The `instanceof` target is a `<ClassName>` (an identifier), so a non-class type can never reach the cast/instanceof phase in target position — a reserved type keyword (`int`, `bool`, `String`, `void`) is a parse error (`E_RESERVED_KEYWORD_AS_IDENTIFIER`) and an identifier naming no class is `E_UNKNOWN_CLASS` at name resolution. Confirmed unreachable during the WS-13 LO-4 held-out authoring (the code was defined in the reference compiler but never emitted). Parallel to the retired `E_EXTENDS_NON_CLASS`. The source-side `E_INSTANCEOF_SOURCE_NOT_CLASS` stays (reachable — a primitive left operand). Casts differ: a cast takes a `<Type>`, so `E_CAST_TARGET_NOT_CLASS` is reachable.*

### Entry-point and program-shape errors

| Code | Trigger |
|------|---------|
| `E_NO_MAIN_CLASS` | The program does not declare a class named `Main`. |
| `E_MAIN_CLASS_EXTENDS` | The `Main` class declaration includes an `extends` clause. |
| `E_NO_MAIN_METHOD` | The `Main` class does not declare a method `int main()` with the expected signature (no formals, `int` return). |
| `E_MAIN_METHOD_SIGNATURE` | The `Main` class declares a method named `main` but with wrong signature (wrong return type, has formals). |
| `E_MAIN_NO_ZERO_ARG_CONSTRUCTOR` | The `Main` class lacks a zero-arg constructor reachable via `new Main()`. |
| `E_ENTRY_POINT_OTHER` | Entry-point or program-shape failure not covered by a more specific category in this phase. The sentinel for the entry-point check phase. |

## Runtime-abort signals

Runtime aborts have native exit codes documented in `runtime-abi.md`. The corresponding header-comment line in a `RuntimeAbortPrograms/` test references the exit code directly; the names here are for cross-reference in this document.

| Name | Native exit code | Trigger |
|------|------------------|---------|
| `ABORT_CAST_FAILURE` | 101 | `lo_cast_check` called with an object whose runtime class is neither `target` nor a descendant. Per `runtime-abi.md` §3.5. |
| `ABORT_NULL_RECEIVER` | 102 | Method invocation on a null receiver. The runtime detects on entry to the dispatched method. |
| `ABORT_READ_INT_MALFORMED` | 110 | `lo_read_int` encounters input that is not a valid integer token. |
| `ABORT_READ_INT_EOF` | 111 | `lo_read_int` encounters end-of-input before any integer characters. |
| `ABORT_READ_BOOL_MALFORMED` | 112 | `lo_read_bool` encounters a token other than `true` or `false`. |
| `ABORT_STRING_REPEAT_NEGATIVE` | 120 | `lo_string_repeat` called with negative count. |
| `ABORT_OOM` | 137 | Heap exhausted; allocation failed even after a collection. 137 matches Linux's SIGKILL convention used by many OOM-killers, which makes it discoverable for students familiar with that pattern. |

These exit codes are mirrored in `runtime-abi.md` at each emitting function (§3.1 for OOM, §3.2 for string-repeat-negative, §3.5 for cast-failure and null-receiver, §3.7 for read failures). Locked 2026-05-28.

## Adding a new code

When the conformance suite needs to test for a new error category, the workflow is:

1. SC drafts the new code's row in this document with the trigger description.
2. DC reviews and approves the addition (low ceremony — usually a one-line confirmation).
3. The code lands in this document; tests using it can be added in the same commit.
4. Student compilers add support for emitting the new code in their next development cycle.

Removing or renaming a code requires more care: any test referencing the old code breaks, and external materials (handouts, lecture slides) may name codes by their stable identifier. Removal happens only at semester boundaries, with a course-wide announcement.
