#!/usr/bin/env bash
# =============================================================================
# CS 378H — P1 grading contract
# =============================================================================
# This file is the interface between your submission and the autograder.
# Every submission builds and runs differently, so instead of forcing a common
# starter layout, we ask you to fill in four shell functions that tell the
# grader how to drive *your* compiler.
#
# Submit this file, filled in, at the TOP LEVEL of your repository, keeping the
# name `p1-grading-contract.sh`.
#
# -----------------------------------------------------------------------------
# What the grader does
# -----------------------------------------------------------------------------
# Once per submission:
#
#     $ source p1-grading-contract.sh
#     $ lo-build                                       # build everything, once
#
# Then, for each test program:
#
#     $ lo-check   prog.lo                             # front-end only
#     $ lo-compile prog.lo /tmp/artifacts/prog.wasm    # front-end, and if good,
#                                                      # must emit .wasm at $2
#     $ lo-wasmrun /tmp/artifacts/prog.wasm            # run it
#
# Not every program runs every step: programs that are supposed to be REJECTED
# are only ever passed to `lo-check`.
#
# -----------------------------------------------------------------------------
# Rules that apply to this file as a whole
# -----------------------------------------------------------------------------
#  * It must be safe to `source`. Do NOT do any work at the top level: no
#    building, no `cd`, no output. Only variable assignments and function
#    definitions. The grader sources this file once and then calls the
#    functions many times.
#  * Do NOT print anything while being sourced. Anything your functions print
#    on stdout is compared byte-for-byte against expected program output, and a
#    stray banner will fail tests that would otherwise pass.
#  * Do NOT use `set -e` at the top level. A non-zero exit from `lo-check` is a
#    NORMAL, EXPECTED result (it means "this program has an error"), and `set -e`
#    would tear down the grader's shell. Use `set -e` inside a function body if
#    you want fail-fast there.
#  * Do not rename the functions or change their argument order.
#  * Paths given to you may be absolute and may contain no spaces, but quote
#    "$1" / "$2" anyway.
# =============================================================================

set -uo pipefail   # deliberately NOT -e; see above

# Absolute path to your repository root, so the functions work no matter what
# directory the grader happens to be in. Leave this as-is.
# To be safe, consider using $LO_ROOT in your commands to refer to your submission's
# root (review filled-in example at the end).
LO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


# -----------------------------------------------------------------------------
# lo-build
# -----------------------------------------------------------------------------
# Build your compiler and anything it needs (your runtime, libraries, ...).
# Called ONCE, before any other function.
#
#   arguments : none
#   stdout    : ignored (build logs are fine here)
#   stderr    : ignored (shown to you if the build fails)
#   exit      : 0 on success, non-zero on failure.
#
# If this fails, the grader reports "your submission did not build", shows you
# the tail of this output, and skips every other test. Get this right first.
#
# The grading container has a network connection and a warm Cargo registry, but
# builds are on a slow, single-core machine — keep it under ~10 minutes.
# -----------------------------------------------------------------------------
lo-build() {
    : # TODO: build your compiler, e.g.  ( cd "$LO_ROOT" && cargo build --release )
}


# -----------------------------------------------------------------------------
# lo-check  <source.lo>
# -----------------------------------------------------------------------------
# Run ONLY the front end: parse, resolve names, type check. Do not generate or
# write any output file.
#
#   $1        : path to a .lo source file
#   stdout    : ignored
#   stderr    : on rejection, must contain the error code for the problem you
#               found (see error-codes.md), e.g. E_UNKNOWN_VARIABLE. The grader
#               searches stderr for the code as a substring, so you are free to
#               word the surrounding message however you like.
#   exit      : 0  the program is well-formed and type-correct
#               1  the program has an error (and stderr names the code)
#
# Exit exactly 1 to reject. Any other non-zero status is treated as your
# compiler having crashed rather than having diagnosed the problem, and will
# not earn credit — this is deliberate, since a segfault on a malformed program
# is not the same as catching the error.
# -----------------------------------------------------------------------------
lo-check() {
    local src="$1"
    : # TODO: e.g.  "$LO_ROOT/target/release/loc" --check "$src"
}


# -----------------------------------------------------------------------------
# lo-compile  <source.lo>  <output.wasm>
# -----------------------------------------------------------------------------
# Compile a program all the way to a linked WebAssembly module: run the front
# end, generate code, and link against your runtime.
#
#   $1        : path to a .lo source file
#   $2        : path where the finished .wasm MUST be written
#   stdout    : ignored
#   stderr    : shown to you on failure
#   exit      : 0 on success, non-zero on failure
#
# $2 is always of the form /tmp/artifacts/<name>.wasm. The grader creates and
# empties /tmp/artifacts before each program, so you may assume the directory
# exists and is writable, and you do not need to clean up after yourself.
#
# Write the module to EXACTLY $2 — not to a fixed path of your own choosing,
# and not next to the source file. Immediately after this function returns, the
# grader checks that $2 exists and is a non-empty regular file, and reports that
# check as its own test. If you exit 0 but write nothing, you will see
# "no .wasm produced" rather than a confusing failure later on.
#
# The finished module must export `lo_entry` (returning i32) and a `memory`,
# because that is what the `wasmrun` host harness looks for.
# -----------------------------------------------------------------------------
lo-compile() {
    local src="$1" out="$2"
    : # TODO: compile $src and write the .wasm to $out
}


# -----------------------------------------------------------------------------
# lo-wasmrun  <module.wasm>
# -----------------------------------------------------------------------------
# Run a compiled module under the course's host harness.
#
# This one is already written, and you should NOT change it. `wasmrun` is
# installed in the course container and in the grading image; it supplies the
# `host` I/O imports your runtime is written against, calls `lo_entry`, and
# exits with whatever your program returned (or with a documented abort code
# such as 101 or 110 if your runtime trapped).
#
#   $1        : path to a linked .wasm module
#   stdin     : the grader may pipe a test's input in; wasmrun forwards it
#   stdout    : your program's output, compared byte-for-byte where a test
#               declares expected output
#   stderr    : searched for the expected message on abort tests
#   exit      : your program's exit status — must be passed through unchanged
#
# Do not wrap this in anything that swallows, renumbers, or remaps the exit
# status; the grader compares it against the value the test declares.
# -----------------------------------------------------------------------------
lo-wasmrun() {
    wasmrun "$1"
}


# =============================================================================
# Worked example
# =============================================================================
# For reference, here is a complete fill-in for a Rust compiler named `loc`
# that emits a relocatable wasm object and links it against a runtime staticlib.
# Yours will differ — this is only to show the shape. Pay special attention
# to the return codes of each function.
#
#   lo-build() {
#       ( cd "$LO_ROOT" && cargo build --release ) || return 1
#       ( cd "$LO_ROOT/runtime" &&
#         cargo build --release --target wasm32-unknown-unknown --lib ) || return 1
#   }
#
#   lo-check() {
#       "$LO_ROOT/target/release/loc" --check "$1"
#   }
#
#   lo-compile() {
#       local obj; obj="$(mktemp)" || return 1
#       "$LO_ROOT/target/release/loc" --emit-obj "$obj" "$1" || { rm -f "$obj"; return 1; }
#       "$LO_ROOT/tools/wasm-ld" "$obj" "$LO_ROOT/runtime/liblo_runtime.a" -o "$2" \
#           --export=lo_entry --no-entry --allow-undefined
#       local rc=$?; rm -f "$obj"; return $rc
#   }
# =============================================================================
