#!/usr/bin/env bash
# =============================================================================
# run-harness.sh — run every ABI harness program under a directory on one target.
# =============================================================================
# Usage: run-harness.sh <native|wasm> <liblo_runtime.a> <dir> [more dirs...]
#
# Compiles each *.c found under the directories (recursively), links it against
# the archive, runs it with <stem>.input.txt as stdin when present and the
# `gc stress heap` from its header as LO_HEAP_SIZE on native, and compares exit
# status, stdout (<stem>.expected.out) and, for abort tests, the stderr
# substring from the header. Prints PASS/FAIL per program; exit 1 if any failed.
#
# Needs: cc (native); clang with wasm32 support, wasm-ld and wasmrun (wasm).
# Override the tools with CC, CLANG, WASM_LD, WASMRUN; RUN_TIMEOUT (seconds, default 60) bounds each run.
# =============================================================================
set -uo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
target="${1:?native|wasm}"; lib="${2:?liblo_runtime.a}"; shift 2
[ "$#" -ge 1 ] || { echo "usage: run-harness.sh <native|wasm> <liblo_runtime.a> <dir>..." >&2; exit 2; }
RUN_TIMEOUT="${RUN_TIMEOUT:-60}"
CC="${CC:-cc}"; CLANG="${CLANG:-clang}"; WASM_LD="${WASM_LD:-wasm-ld}"; WASMRUN="${WASMRUN:-wasmrun}"
CFLAGS=(-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -I "$HERE")
work="$(mktemp -d)"; trap 'rm -rf "$work"' EXIT
fail=0; total=0

header_field() { grep -i -m1 "^//[[:space:]]*$2:" "$1" | sed "s/^\/\/[[:space:]]*$2:[[:space:]]*//"; }

for dir in "$@"; do
  while IFS= read -r -d '' src; do
    total=$((total + 1))
    stem="${src%.c}"; name="$(basename "$stem")"
    exp_out="$stem.expected.out"; input="$stem.input.txt"
    abort=0; grep -qi '^//[[:space:]]*runtime abort test case' "$src" && abort=1
    want_exit=0; [ "$abort" = 1 ] && want_exit="$(header_field "$src" 'expected exit code')"
    want_err=""; [ "$abort" = 1 ] && want_err="$(header_field "$src" 'expected stderr substring')"
    heap="$(header_field "$src" 'gc stress heap')"
    case "$target" in
      native)
        "$CC" "${CFLAGS[@]}" -c "$src" -o "$work/p.o" 2>"$work/cc.err" || { echo "FAIL $name (compile)"; fail=1; continue; }
        "$CC" -no-pie -o "$work/p" "$work/p.o" "$lib" -lpthread -ldl -lm 2>"$work/ld.err" || { echo "FAIL $name (link: $(grep -o 'undefined reference to .*' "$work/ld.err" | head -1))"; fail=1; continue; }
        runner=("$work/p") ;;
      wasm)
        "$CLANG" --target=wasm32 "${CFLAGS[@]}" -nostdlib -c "$src" -o "$work/p.o" 2>"$work/cc.err" || { echo "FAIL $name (compile)"; fail=1; continue; }
        "$WASM_LD" --no-entry --export=lo_entry --allow-undefined "$work/p.o" "$lib" -o "$work/p.wasm" 2>"$work/ld.err" || { echo "FAIL $name (link: $(grep -o 'undefined symbol: .*' "$work/ld.err" | head -1))"; fail=1; continue; }
        runner=("$WASMRUN" "$work/p.wasm") ;;
      *) echo "target must be native|wasm" >&2; exit 2 ;;
    esac
    check_one() {  # check_one <label> [env assignments...]
      local label="$1"; shift
      # The program runs under an inner shell whose stderr is the capture file, so
      # the shell notice for a signal death (a normal outcome here) stays out of
      # the report; the inner shell exits with the program's status.
      local in=/dev/null; [ -f "$input" ] && in="$input"
      bash -c '"$@"; exit $?' _ timeout "$RUN_TIMEOUT" env "$@" "${runner[@]}" <"$in" >"$work/out" 2>"$work/err"
      local rc=$?
      if [ "$rc" = 124 ]; then echo "FAIL $name$label (timeout after ${RUN_TIMEOUT}s)"; return 1; fi
      if [ "$rc" != "$want_exit" ]; then echo "FAIL $name$label (exit=$rc, want $want_exit)"; return 1; fi
      if [ -f "$exp_out" ] && ! cmp -s "$work/out" "$exp_out"; then echo "FAIL $name$label (stdout differs)"; return 1; fi
      if [ -n "$want_err" ] && ! grep -qF -- "$want_err" "$work/err"; then echo "FAIL $name$label (stderr lacks: $want_err)"; return 1; fi
      return 0
    }
    ok=1
    check_one "" || ok=0
    if [ "$target" = native ] && [ -n "$heap" ]; then check_one " @heap=$heap" "LO_HEAP_SIZE=$heap" || ok=0; fi
    [ "$ok" = 1 ] && echo "PASS $name" || fail=1
  done < <(find "$dir" -name '*.c' -print0 | sort -z)
done
echo "$total programs, $([ "$fail" = 0 ] && echo all passed || echo some FAILED)"
exit "$fail"
