# Test Corpus Migration — 2026-05-28

This document specifies the mechanical migration that brings the existing pre-redesign LO-2 and LO-3 test corpora into conformance with the **B-wide** brace-shape decision locked in `cs378h:state-ledger.md` under "Method and constructor body shape — Locked 2026-05-28".

Under the prior canonical grammar, `<MethodDecl>` had the form `<Type> <MethodName> ( <Formals>? ) { <Body> }` where `<Body> → (<VarDecl>)* <Block>` and `<Block> → { (<Stmt>)+ }`. A method body with no var-decls therefore contained two brace pairs:

```
int area() {        // outer braces (MethodDecl's { Body })
    {               //   inner braces (Block's own braces)
        return 42;
    }
}
```

Under the new (B-wide) grammar, `<MethodDecl>` inlines its contents directly: `<Type> <MethodName> ( <Formals>? ) { (<VarDecl>)* (<Stmt>)+ }`. The `<Body>` nonterminal retires. Constructor declarations in redesigned LO-3 follow the same shape via P2. `<Block>` at P10 is preserved with its braces because `if` (P12) and `while` (P13) still use it.

The corresponding edits to the test corpora are mechanical.

## LO-2: brace-stripping only

For every test in `lo-testing/LO-2/ValidPrograms/` and `lo-testing/LO-2/InvalidPrograms/`, each method body has its inner `<Block>` brace pair stripped while preserving any `<VarDecl>`s that precede the Block. Statements that were inside the Block move up one level of nesting (i.e., become direct children of the method's outer brace pair).

**Example: `LO-2/ValidPrograms/test_88.lo`** (already migrated in this directory):

Before:
```
int doSomethingCoolAndExciting(int var1, int var2, bool shouldAdd){
  {
    if ((((!shouldAdd)) & (!(var2 = 0)))){
        return ((var1 / (var2)));
    }
    else{
        return (var1 + var2);
    }
    return (~1);
  }
}

int main(){
  int var1, var2;
  bool shouldAdd;
  {
    var1 = (90);
    var2 = 45;
    shouldAdd = true;
    return doSomethingCoolAndExciting(var1, var2, (!shouldAdd));
  }
}
```

After (the inner `{` and `}` of each method are removed; var-decls in `main()` stay where they are):
```
int doSomethingCoolAndExciting(int var1, int var2, bool shouldAdd){
    if ((((!shouldAdd)) & (!(var2 = 0)))){
        return ((var1 / (var2)));
    }
    else{
        return (var1 + var2);
    }
    return (~1);
}

int main(){
    int var1, var2;
    bool shouldAdd;
    var1 = (90);
    var2 = 45;
    shouldAdd = true;
    return doSomethingCoolAndExciting(var1, var2, (!shouldAdd));
}
```

The braces inside `if` / `else` / `while` are unchanged — those are `<Block>` instances that retain their braces under the new grammar.

## LO-3: brace-stripping plus constructor relocation

For every test in `lo-testing/LO-3/ValidPrograms/` and `lo-testing/LO-3/InvalidPrograms/`, two transformations are applied in sequence.

First, every method-section declaration of the form `void <ClassName>(<Formals>?) { <body> }` (where `<ClassName>` matches the enclosing class's name) is **relocated** from the method section to the constructor section. The `void` keyword is dropped. If the class had no constructor section, one is created with a single brace pair `[ ... ]`. If the class had multiple `void <ClassName>(...)` declarations, all are moved (in source order) into one constructor section.

Second, the inner `<Block>` brace pair is stripped from each method body **and** each constructor body, exactly as in the LO-2 pass.

**Example: `LO-3/ValidPrograms/test_18.lo`** (already migrated in this directory):

Before:
```
class circle(int radius;) {

  void circle(int data){
    {
      radius = data;
    }
  }

   int area() {
       {
        return (3 * (radius * radius));
      }
   }
}

class Main() {
  int main() {
    circle c;
       {
        c = new circle (4);
        return c.area();
       }
  }
}
```

After:
```
class circle(int radius;) [
    circle(int data){
        radius = data;
    }
] {
    int area() {
        return (3 * (radius * radius));
    }
}

class Main() {
    int main() {
        circle c;
        c = new circle(4);
        return c.area();
    }
}
```

Note three changes: `void circle(int data){ { ... } }` moved out of the method section into a new bracket section `[ circle(int data){ ... } ]` with `void` dropped; `int area()`'s Block braces stripped; `Main.main()`'s Block braces stripped (the `circle c;` var-decl is preserved before the (now-direct) statements).

A class with no `void <ClassName>(...)` method-section declarations needs no relocation step — the bracket section stays absent (the class continues to use the implicit constructor matching its field list). Only the brace-stripping pass applies to its methods.

## Pseudocode for the transformation

```
for each .lo file in lo-testing/LO-2/ and lo-testing/LO-3/:
    tokens = tokenize(file_content)
    output = []
    
    if level == "LO-3":
        # Pass 1: per-class constructor relocation
        for each class declaration in tokens:
            class_name = class declaration's name
            constructors_to_move = []
            for each method declaration inside the class's method section:
                if method's return type is "void" AND method's name == class_name:
                    constructors_to_move.append(method)
            for each c in constructors_to_move:
                remove c from method section
                drop "void" keyword from c
                if class has no [ ... ] section:
                    insert empty [ ] section between fields and methods
                append c to the bracket section
    
    # Pass 2: brace-stripping (applies to LO-2 and LO-3 alike)
    for each method declaration in tokens (and for LO-3, each constructor declaration):
        find the method's opening { (the outer brace from MethodDecl / ConstructorDecl)
        skip any <VarDecl> lines that follow (lines matching: Type Ident (, Ident)* ;)
        the next non-trivial token should be { (the Block's opening brace)
        remove that { 
        track brace depth from this point;
        when depth returns to the level before the removed {, that closing } is the Block's closing brace —
        remove it too
    
    write tokens back to file
```

The two-pass structure ensures that the LO-3 brace-stripping pass sees the relocated constructors as constructor declarations in the bracket section (whose bodies also need brace-stripping), rather than as method-section entries that would no longer match the migration pattern after relocation.

## Header comments preserved

Header comments (`// good test case`, `// main method return value: N`, `// expected compile error: E_...`, `// Author: ...`) are preserved verbatim across the migration. The hybrid harness contract in `lo-testing/README.md` is unaffected by the brace change.

## Verifying a migration

After applying the migration to a file:
- The file should be parseable under the new grammar (`liveoak-grammar.tex` post-2026-05-28).
- The semantic meaning of the program — what it computes, what error it triggers, what it prints — is unchanged. The migration is purely syntactic.
- For ValidPrograms tests, the header's `// main method return value: N` (or `expected.out` byte sequence) remains the correctness criterion.
- For InvalidPrograms tests, the header's `// expected compile error: E_...` remains the correctness criterion; the bug being demonstrated by the test is preserved through the migration.

## Status of the migration in this directory

Worked-example migrations applied 2026-05-28 (these files are the result of applying the recipe to the corresponding pre-redesign source):

- `LO-2/ValidPrograms/test_48.lo`, `test_86.lo`, `test_88.lo`
- `LO-2/InvalidPrograms/test_3.lo`, `test_8.lo`, `test_11.lo`
- `LO-3/ValidPrograms/test_15.lo`, `test_18.lo`, `test_20.lo`, `test_21.lo`, `test_22.lo`, `test_36.lo`
- `LO-3/InvalidPrograms/test_4.lo`, `test_8.lo`

Pre-redesign tests not yet migrated remain in the source repository in their original form. Applying the recipe above (or implementing it as a script and running it against the corpus) brings them to current-grammar conformance.

## Pre-redesign tests that may need semantic review beyond the mechanical pass

Two of the migrated examples in this directory contain pre-existing bugs being intentionally demonstrated by the test:

- `LO-3/InvalidPrograms/test_4.lo` — the program contains `a = Tester(5);` (missing `new`) and `int getA{` (missing parentheses for formals). The error categories these trigger are not the same as "missing new in constructor" suggested by the original header comment — the latter would be E_MISSING_NEW (a hypothetical code, not enumerated in current `error-codes.md`). A future pass should annotate these with a specific `expected compile error:` line referencing whatever code the LL(2) parser actually emits, replacing the vague "Missing new in constructor" comment.
- `LO-3/InvalidPrograms/test_8.lo` — declares `class A(){...}` twice in the same program. Should be annotated `// expected compile error: E_DUPLICATE_CLASS_NAME` once that code is added to `error-codes.md` (currently the codebook does not list a duplicate-class-name code; the closest is `E_RESERVED_CLASS_NAME` which is the wrong category).

These are part of the "retrofit error codes to existing pre-redesign InvalidPrograms tests" workstream-internal item — a separate pass from the brace migration. The mechanical brace migration applies cleanly to both; the error-code annotation is the follow-on pass.
