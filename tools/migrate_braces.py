#!/usr/bin/env python3
"""
WS-4 corpus migration: bring pre-redesign LO-2 / LO-3 test programs into the
redesigned, B-wide grammar.

Two purely-syntactic transformations (see migration-2026-05-28.md, the
authoritative recipe, and runbooks/ws4-corpus-migration.md, the execution spec):

  1. LO-3 only -- constructor relocation. Each method-section declaration of the
     form `void <ClassName>(<Formals>?) { ... }` whose name equals the enclosing
     class is moved into a `[ ... ]` constructor section (created between the
     field-parens and the method-braces when absent), with the `void` dropped.

  2. LO-2 and LO-3 -- brace-stripping. Each method body (and each constructor
     body) drops the inner `<Block>` brace pair, lifting its statements up one
     level. The braces of `if` / `else` / `while` `<Block>`s are kept -- they
     remain `<Block>`s under the new grammar.

The transformation is idempotent: already-redesigned methods (the body-block has
already been stripped, or the constructor already lives in `[ ]`) are left
untouched, so running over the whole tree -- including new-form files -- is a
no-op on the new-form parts.

Robustness properties (the correctness crux, per the runbook):
  * Tokenization is string- and comment-aware. A `{`/`}` inside a "..." string
    literal or a `//` comment never affects brace matching. (LO has no block
    comments -- confirmed against the corpus.)
  * The body-block is identified *structurally*, not by parsing var-decls: it is
    the first top-level `{` inside a method body whose immediately-preceding
    token is the method's outer `{` or a `;` (i.e. it follows only var-decls).
    A `{` preceded by `)` or `else` is a control block and is left alone. This
    correctly handles malformed var-decl prefixes (e.g. `while waiter;` in
    LO-3/InvalidPrograms/test_13) without mis-detecting the body-block.
  * Header comments (the harness contract) are emitted verbatim -- the preamble
    before the first declaration is never reindented or altered.

Usage:
    python3 tools/migrate_braces.py [--check] [--report PATH] [PATHS...]

With no PATHS, processes LO-2/{ValidPrograms,InvalidPrograms} and
LO-3/{ValidPrograms,InvalidPrograms}. --check does a dry run (no writes) and
exits non-zero if any file would change. --report writes a migration report.
"""

import os
import sys
import argparse

# Words that can never begin a variable declaration's type (kept for the report's
# heuristics only; body-block detection no longer relies on var-decl parsing).
RESERVED = {
    "return", "if", "else", "while", "new", "null", "true", "false",
    "this", "super", "instanceof", "extends", "class", "void",
}

DEFAULT_DIRS = [
    "LO-2/ValidPrograms", "LO-2/InvalidPrograms",
    "LO-3/ValidPrograms", "LO-3/InvalidPrograms",
]


# --------------------------------------------------------------------------- #
# Tokenizer (string/comment aware)
# --------------------------------------------------------------------------- #

def tokenize(src):
    """Return a list of (text, start, end, kind) tokens covering all of `src`.

    kind in {'ws', 'comment', 'string', 'word', 'punct'}.
    """
    toks = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c in " \t\r\n":
            j = i
            while j < n and src[j] in " \t\r\n":
                j += 1
            toks.append((src[i:j], i, j, "ws"))
            i = j
        elif c == "/" and i + 1 < n and src[i + 1] == "/":
            j = i
            while j < n and src[j] != "\n":
                j += 1
            toks.append((src[i:j], i, j, "comment"))
            i = j
        elif c == '"':
            j = i + 1
            while j < n:
                if src[j] == "\\":
                    j += 2
                    continue
                if src[j] == '"':
                    j += 1
                    break
                j += 1
            toks.append((src[i:j], i, j, "string"))
            i = j
        elif c.isalpha() or c == "_":
            j = i
            while j < n and (src[j].isalnum() or src[j] == "_"):
                j += 1
            toks.append((src[i:j], i, j, "word"))
            i = j
        else:
            toks.append((c, i, i + 1, "punct"))
            i += 1
    return toks


def significant(toks):
    """Significant tokens: drop whitespace and comments."""
    return [t for t in toks if t[3] not in ("ws", "comment")]


def match_brace(sig, k, open_ch, close_ch):
    """Index in `sig` of the closer matching the opener at index k (same type)."""
    depth = 0
    for m in range(k, len(sig)):
        t = sig[m][0]
        if t == open_ch:
            depth += 1
        elif t == close_ch:
            depth -= 1
            if depth == 0:
                return m
    raise ParseError("unbalanced %r starting at offset %d" % (open_ch, sig[k][1]))


class ParseError(Exception):
    pass


# --------------------------------------------------------------------------- #
# Declaration parsing / body-block stripping
# --------------------------------------------------------------------------- #

class Decl:
    """A method or constructor declaration within a (method or `[]`) section."""
    __slots__ = ("hdr_start_sig", "body_open_sig", "body_close_sig",
                 "name", "first_word")

    def __init__(self, hdr_start_sig, body_open_sig, body_close_sig,
                 name, first_word):
        self.hdr_start_sig = hdr_start_sig
        self.body_open_sig = body_open_sig
        self.body_close_sig = body_close_sig
        self.name = name
        self.first_word = first_word


def parse_decls(sig, start, end):
    """Parse the declarations in sig[start:end] (a method or `[]` section body)."""
    decls = []
    i = start
    while i < end:
        # Find this decl's body-opening '{' -- the first '{' at paren/bracket
        # depth 0 from the header start.
        j = i
        pdepth = 0
        body_open = -1
        while j < end:
            t = sig[j][0]
            if t in ("(", "["):
                pdepth += 1
            elif t in (")", "]"):
                pdepth -= 1
            elif t == "{" and pdepth == 0:
                body_open = j
                break
            j += 1
        if body_open == -1:
            break
        body_close = match_brace(sig, body_open, "{", "}")
        # name = the word immediately before the first '(' in the header.
        name = None
        for k in range(i, body_open):
            if sig[k][0] == "(":
                if k - 1 >= i and sig[k - 1][3] == "word":
                    name = sig[k - 1][0]
                break
        first_word = sig[i][0] if sig[i][3] == "word" else None
        decls.append(Decl(i, body_open, body_close, name, first_word))
        i = body_close + 1
    return decls


def strip_body(src, sig, body_open_sig, body_close_sig):
    """Return the body text (from outer '{' to outer '}', inclusive) with the
    inner body-block brace pair removed, if present. No-op for new-form bodies."""
    ob_off = sig[body_open_sig][1]
    oc_off = sig[body_close_sig][1]
    text = src[ob_off:oc_off + 1]

    # The body-block is the first '{' inside the body whose preceding significant
    # token is the outer '{' (no var-decls) or a ';' (after var-decls). A '{'
    # preceded by ')' / 'else' is a control block and is left alone.
    cand = -1
    for j in range(body_open_sig + 1, body_close_sig):
        if sig[j][0] == "{":
            cand = j
            break
    if cand != -1:
        prev = sig[cand - 1][0]
        if prev == "{" or prev == ";":
            bb_close = match_brace(sig, cand, "{", "}")
            d1 = sig[cand][1] - ob_off
            d2 = sig[bb_close][1] - ob_off
            for d in sorted((d1, d2), reverse=True):
                text = text[:d] + text[d + 1:]
    return text


def emit_decl(src, sig, decl, drop_void):
    """Header (verbatim, optionally dropping a leading `void`) + stripped body."""
    if drop_void:
        # Start the emitted header at the name token, dropping `void` and the
        # whitespace between it and the name.
        name_sig = None
        for k in range(decl.hdr_start_sig, decl.body_open_sig):
            if sig[k][0] == "(":
                name_sig = k - 1
                break
        hdr_start_off = sig[name_sig][1]
    else:
        hdr_start_off = sig[decl.hdr_start_sig][1]
    header = src[hdr_start_off:sig[decl.body_open_sig][1]]
    body = strip_body(src, sig, decl.body_open_sig, decl.body_close_sig)
    return header + body


# --------------------------------------------------------------------------- #
# Per-file transform
# --------------------------------------------------------------------------- #

class FileResult:
    def __init__(self):
        self.changed = False
        self.classes = 0
        self.methods = 0
        self.ctors_relocated = 0
        self.ctors_existing = 0
        self.bodies_stripped = 0
        self.new_brackets = 0  # classes that gained a fresh `[ ]` section
        self.flags = []


def _count_strips(src, sig, decls):
    n = 0
    for d in decls:
        before = strip_body(src, sig, d.body_open_sig, d.body_close_sig)
        after = src[sig[d.body_open_sig][1]:sig[d.body_close_sig][1] + 1]
        if before != after:
            n += 1
    return n


def transform_lo2(src, res):
    sig = significant(tokenize(src))
    if not sig:
        return src
    preamble = src[:sig[0][1]]
    decls = parse_decls(sig, 0, len(sig))
    res.methods += len(decls)
    res.bodies_stripped += _count_strips(src, sig, decls)
    pieces = [emit_decl(src, sig, d, drop_void=False) for d in decls]
    region = reindent("\n\n".join(pieces))
    return preamble + region + "\n"


def transform_lo3(src, res):
    sig = significant(tokenize(src))
    if not sig:
        return src
    preamble = src[:sig[0][1]]
    items = []  # list of emitted top-level declaration strings
    i = 0
    while i < len(sig):
        if sig[i][3] == "word" and sig[i][0] == "class":
            i = _parse_class(src, sig, i, items, res)
        else:
            # Top-level method (invalid in LO-3, but present, e.g.
            # LO-3/InvalidPrograms/test_6 "Method defined outside of class").
            decls = parse_decls(sig, i, len(sig))
            if not decls:
                raise ParseError("stray tokens at offset %d" % sig[i][1])
            d = decls[0]
            res.methods += 1
            res.bodies_stripped += _count_strips(src, sig, [d])
            res.flags.append("top-level method outside any class at offset %d "
                             "(handled: brace-stripped; intentional invalid test?)"
                             % sig[i][1])
            items.append(emit_decl(src, sig, d, drop_void=False))
            i = d.body_close_sig + 1
    region = reindent("\n\n".join(items))
    return preamble + region + "\n"


def _parse_class(src, sig, i, items, res):
    res.classes += 1
    cname = sig[i + 1][0]
    fp_open = i + 2
    if sig[fp_open][0] != "(":
        raise ParseError("class %s: expected '(' after name at offset %d"
                         % (cname, sig[fp_open][1]))
    fp_close = match_brace(sig, fp_open, "(", ")")
    fields_header = src[sig[i][1]:sig[fp_close][2]]

    k = fp_close + 1
    existing = []
    if k < len(sig) and sig[k][0] == "[":
        cb_open = k
        cb_close = match_brace(sig, cb_open, "[", "]")
        existing = parse_decls(sig, cb_open + 1, cb_close)
        k = cb_close + 1

    if k >= len(sig) or sig[k][0] != "{":
        raise ParseError("class %s: expected method-section '{' at offset %d"
                         % (cname, sig[k][1] if k < len(sig) else len(src)))
    ms_open = k
    ms_close = match_brace(sig, ms_open, "{", "}")
    msdecls = parse_decls(sig, ms_open + 1, ms_close)

    reloc, methods = [], []
    for d in msdecls:
        if d.name == cname:
            if d.first_word == "void":
                reloc.append(d)
            else:
                # A method named like its class but not `void` -- ambiguous.
                res.flags.append(
                    "class %s: method-section decl named like the class but not "
                    "`void` (offset %d) -- left as a method; flag to SC"
                    % (cname, sig[d.hdr_start_sig][1]))
                methods.append(d)
        else:
            methods.append(d)

    if existing and reloc:
        res.flags.append(
            "class %s: has both an existing `[ ]` section and relocatable "
            "`void %s(...)` method-section constructors -- merged; flag to SC"
            % (cname, cname))

    all_ctors = existing + reloc
    if reloc and not existing:
        res.new_brackets += 1
    res.ctors_existing += len(existing)
    res.ctors_relocated += len(reloc)
    res.methods += len(methods)
    res.bodies_stripped += _count_strips(src, sig, msdecls + existing)

    ctor_texts = [emit_decl(src, sig, d, drop_void=False) for d in existing]
    ctor_texts += [emit_decl(src, sig, d, drop_void=True) for d in reloc]
    method_texts = [emit_decl(src, sig, d, drop_void=False) for d in methods]

    out = [fields_header]
    if all_ctors:
        out.append(" [\n")
        out.append("\n".join(ctor_texts))
        out.append("\n] {\n")
    else:
        out.append(" {\n")
    out.append("\n".join(method_texts))
    out.append("\n}")
    items.append("".join(out))
    return ms_close + 1


# --------------------------------------------------------------------------- #
# Reindenter (4-space, brace/bracket depth; preserves intra-line content)
# --------------------------------------------------------------------------- #

def _line_sig(line):
    """Significant token texts on a single line (string/comment aware)."""
    return [t[0] for t in significant(tokenize(line))]


def reindent(region):
    """Reindent code lines to nesting depth * 4 spaces.

    Brace/bracket depth (`{` `[`) is the primary structural indent; paren depth
    (`(`) adds continuation indent so lines inside a multi-line `(...)` field
    header or expression indent under their opener. The two counters are kept
    separate and each is clamped to >= 0 at every line boundary, so an
    intentionally-unbalanced paren or brace (as some InvalidPrograms tests carry)
    cannot leak across lines and corrupt the structural indentation. Drops blank
    lines at depth > 0; keeps a single blank line between top-level declarations."""
    out = []  # (is_blank, depth_or_text)
    bdepth = 0  # { [ ] }
    pdepth = 0  # ( )
    for line in region.split("\n"):
        if bdepth < 0:
            bdepth = 0
        if pdepth < 0:
            pdepth = 0
        depth = bdepth + pdepth
        if line.strip() == "":
            out.append((True, depth))
            continue
        sigs = _line_sig(line)
        first = sigs[0] if sigs else None
        eff = depth - 1 if first in ("}", "]", ")") else depth
        if eff < 0:
            eff = 0
        out.append((False, "    " * eff + line.strip()))
        for t in sigs:
            if t in ("{", "["):
                bdepth += 1
            elif t in ("}", "]"):
                bdepth -= 1
            elif t == "(":
                pdepth += 1
            elif t == ")":
                pdepth -= 1
    result = []
    for is_blank, val in out:
        if is_blank:
            if val == 0 and result and result[-1] != "":
                result.append("")
        else:
            result.append(val)
    while result and result[0] == "":
        result.pop(0)
    while result and result[-1] == "":
        result.pop()
    return "\n".join(result)


# --------------------------------------------------------------------------- #
# Driver
# --------------------------------------------------------------------------- #

def transform_file(path, src):
    res = FileResult()
    level = "LO-3" if (os.sep + "LO-3" + os.sep) in (os.sep + path + os.sep) \
        or path.startswith("LO-3" + os.sep) or "/LO-3/" in path else "LO-2"
    if level == "LO-3":
        out = transform_lo3(src, res)
    else:
        out = transform_lo2(src, res)

    # Leave already-new-form files byte-identical: if there is nothing
    # structural to do (no body-block to strip, no constructor to relocate),
    # do not reindent. This keeps the regression anchors and any other
    # already-redesigned file untouched, and guarantees idempotency for free.
    if res.bodies_stripped == 0 and res.ctors_relocated == 0:
        out = src
    res.changed = (out != src)

    # --- token-conservation invariant (catches corruption without rejecting
    # the intentional brace/paren imbalance some InvalidPrograms tests carry) ---
    _check_conservation(src, out, res, path)
    # Idempotency: a second pass must be a byte-for-byte no-op.
    res2 = FileResult()
    out2 = transform_lo3(out, res2) if level == "LO-3" else transform_lo2(out, res2)
    if res2.bodies_stripped == 0 and res2.ctors_relocated == 0:
        out2 = out
    if out2 != out:
        res.flags.append("NOT IDEMPOTENT -- second pass differs; flag to SC")
    # Header preamble preserved verbatim.
    isig = significant(tokenize(src))
    osig = significant(tokenize(out))
    if isig and osig and src[:isig[0][1]] != out[:osig[0][1]]:
        res.flags.append("header preamble changed; flag to SC")
    return out, res


def _tok_counts(text):
    counts = {}
    for t in significant(tokenize(text)):
        counts[t[0]] = counts.get(t[0], 0) + 1
    return counts


def _check_conservation(src, out, res, path):
    """The migrated token multiset must equal the original minus exactly the
    body-block braces stripped and the relocated `void` keywords, plus exactly
    the brace-pairs of freshly created `[ ]` sections. Any other difference
    means tokens were lost, duplicated, or corrupted."""
    ci, co = _tok_counts(src), _tok_counts(out)
    expect = {
        "{": -res.bodies_stripped, "}": -res.bodies_stripped,
        "void": -res.ctors_relocated,
        "[": +res.new_brackets, "]": +res.new_brackets,
    }
    keys = set(ci) | set(co)
    for k in keys:
        delta = co.get(k, 0) - ci.get(k, 0)
        if delta != expect.get(k, 0):
            raise ParseError(
                "%s: token conservation failed for %r (delta %d, expected %d)"
                % (path, k, delta, expect.get(k, 0)))


def collect_files(paths):
    files = []
    for p in paths:
        if os.path.isdir(p):
            for root, _, names in os.walk(p):
                for nm in sorted(names):
                    if nm.endswith(".lo"):
                        files.append(os.path.join(root, nm))
        elif p.endswith(".lo"):
            files.append(p)
    return sorted(files)


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="*", default=None)
    ap.add_argument("--check", action="store_true",
                    help="dry run; exit 1 if any file would change")
    ap.add_argument("--report", default=None, help="write a migration report")
    args = ap.parse_args(argv)

    paths = args.paths if args.paths else DEFAULT_DIRS
    files = collect_files(paths)

    changed, skipped, all_flags = [], [], []
    totals = dict(classes=0, methods=0, ctors_relocated=0,
                  ctors_existing=0, bodies_stripped=0)
    noops = []

    for path in files:
        with open(path, "r") as f:
            src = f.read()
        try:
            out, res = transform_file(path, src)
        except ParseError as e:
            skipped.append((path, str(e)))
            all_flags.append("%s: SKIPPED -- %s" % (path, e))
            continue
        for k in totals:
            totals[k] += getattr(res, k)
        for fl in res.flags:
            all_flags.append("%s: %s" % (path, fl))
        if res.changed:
            changed.append(path)
            if not args.check:
                with open(path, "w") as f:
                    f.write(out)
        else:
            noops.append(path)

    report = render_report(files, changed, noops, skipped, totals, all_flags,
                            args.check)
    print(report)
    if args.report:
        with open(args.report, "w") as f:
            f.write(report)

    if args.check and changed:
        return 1
    if skipped:
        return 2
    return 0


def render_report(files, changed, noops, skipped, totals, flags, check):
    lines = []
    lines.append("# WS-4 brace-migration report")
    lines.append("")
    lines.append("Mode: %s" % ("dry-run (--check)" if check else "applied"))
    lines.append("")
    lines.append("Files scanned: %d" % len(files))
    lines.append("Files migrated (changed): %d" % len(changed))
    lines.append("Files already in new form (no-op): %d" % len(noops))
    lines.append("Files skipped (parse anomaly): %d" % len(skipped))
    lines.append("")
    lines.append("Totals across migrated corpus:")
    lines.append("  classes parsed:          %d" % totals["classes"])
    lines.append("  methods:                 %d" % totals["methods"])
    lines.append("  constructors relocated:  %d" % totals["ctors_relocated"])
    lines.append("  constructors (existing): %d" % totals["ctors_existing"])
    lines.append("  body-blocks stripped:    %d" % totals["bodies_stripped"])
    lines.append("")
    if noops:
        lines.append("## No-op files (already redesigned / regression anchors)")
        for p in noops:
            lines.append("  - %s" % p)
        lines.append("")
    if skipped:
        lines.append("## Skipped files")
        for p, why in skipped:
            lines.append("  - %s -- %s" % (p, why))
        lines.append("")
    if flags:
        lines.append("## Flags for SC")
        for fl in flags:
            lines.append("  - %s" % fl)
        lines.append("")
    return "\n".join(lines)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
