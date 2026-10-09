#!/bin/bash
#
#  File: expr_checks.sh
#
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
#  simulation.
#  Copyright (C) 1998-2026 Stefan Frederik Schippers
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
#  Miscasing checks for the Smith-chart imp() RPN operator
#  (draw_smith_expr_points() in src/draw.c, commits 1b43f044 + 9590914d).
#
#  A single xschem run (tests/smith/expr_checks.tcl) sets the graph "node"
#  attribute to five distinct RPN expressions and forces exactly one redraw
#  per expression. The info() lines land on xschem's stderr, which this
#  script captures and asserts:
#    (a) "Zin; v(net1) i(v2) / imp()"  -> no error/warning line
#    (b) "Zin; nosuchvar imp()"        -> exactly one "not supported ... nosuchvar"
#    (c) "Zin; v(net1)"                -> no error (depth 1: V plotted as Gamma)
#    (d) "Zin; v(net1) i(v2) /"        -> no "unbalanced" (valid Z=v/i)
#    (d2)"Zin; v(net1) i(v2) / /"      -> exactly one "unbalanced"
#
#  The expression-validation path runs inside the graph node-draw loop, which
#  only executes when a (virtual) X display drives drawing. A pure "-q -x -r"
#  run with no display emits no info() lines, so a display is required; this
#  script reuses the repo's Xvfb machinery (as trace_check.sh does).
#  -r (no_readline) is used so xschem does not
#  detach and discard its own stderr.
#
#  Usage:  ./expr_checks.sh [xschem-binary]
#          default binary: <repo>/src/xschem
#  Needs:   Xvfb + the self-contained pointer-warp helper (tests/smith/warp.c)
#  Overrides: SMITH_DISP  X display to use (default: first free of :99..)
#  Exit:     0 = PASS, 1 = FAIL, 2 = setup error.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
RAW="$REPO/tests/SC_Test.raw"
TCL="$DIR/expr_checks.tcl"

[ -x "$XSCHEM" ] || { echo "xschem not found or not executable: $XSCHEM" >&2; exit 2; }
[ -f "$RAW" ]    || { echo "raw file not found: $RAW" >&2; exit 2; }
[ -f "$TCL" ]    || { echo "tcl script not found: $TCL" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/expr_checks.* "$RES"/xauth.$$

# --- self-contained pointer-warp helper (tests/smith/warp.c) ---
WARP="$REPO/tests/smith/warp"
if [ ! -x "$WARP" ]; then
  if command -v gcc >/dev/null && \
     gcc -o "$WARP" "$REPO/tests/smith/warp.c" -lX11 2>/dev/null; then
    :
  else
    echo "SKIP: pointer-warp helper unavailable (need $WARP or gcc + X11 libs)"
    exit 2
  fi
fi

# --- pick a free X display; start Xvfb on it if the socket is missing ---
DISP="${SMITH_DISP:-}"
if [ -z "$DISP" ]; then
  for cand in 99 98 97 96 95 94; do
    if [ ! -e "/tmp/.X11-unix/X$cand" ]; then DISP=":$cand"; break; fi
  done
  [ -n "$DISP" ] || DISP=:99
fi
SOCK="/tmp/.X11-unix/X${DISP#:}"
XVFB_PID=""
OURED=0
XAUTH="$RES/xauth.$$"
cleanup() {
  [ -n "${XVFB_PID:-}" ] && kill "$XVFB_PID" 2>/dev/null
  rm -f "$XAUTH"
}
trap cleanup EXIT

if [ ! -e "$SOCK" ]; then
  if command -v Xvfb >/dev/null; then
    : > "$XAUTH"
    setsid Xvfb "$DISP" -screen 0 1920x1200x24 -ac -nolisten tcp </dev/null >/dev/null 2>&1 &
    XVFB_PID=$!
    OURED=1
    for i in $(seq 1 20); do [ -e "$SOCK" ] && break; sleep 0.25; done
  fi
fi
export DISPLAY="$DISP"
[ "$OURED" = 1 ] && export XAUTHORITY="$XAUTH"

if ! timeout 5 xset q >/dev/null 2>&1; then
  echo "SKIP: no usable X display on $DISP (install xvfb, or set SMITH_DISP / DISPLAY)"
  exit 2
fi

# --- run: one xschem process drives all five checks; -r keeps stderr ours ---
rc=1
for try in 1 2 3; do
  rm -f "$RES/expr_checks.log"
  SMITH_RAW="$RAW" SMITH_WARP="$WARP" SMITH_OUT="$RES/expr_checks.log" \
    timeout 90 "$XSCHEM" -r --script "$TCL" </dev/null \
      >"$RES/expr_checks.stdout" 2>"$RES/expr_checks.stderr"
  rc=$?
  if [ "$rc" -eq 0 ] && grep -q "ALL_CHECKS_DONE" "$RES/expr_checks.log" 2>/dev/null; then
    break
  fi
  echo "attempt $try/3 failed (rc=$rc)"
  sleep 2
done
if [ "$rc" -ne 0 ] || ! grep -q "ALL_CHECKS_DONE" "$RES/expr_checks.log" 2>/dev/null; then
  echo "FAIL: xschem run did not complete (rc=$rc)"
  tail -15 "$RES/expr_checks.stderr" 2>/dev/null
  exit 1
fi

ERR="$RES/expr_checks.stderr"
echo "--- raw stderr (info lines) ---"
grep -E "Smith chart|not supported|unbalanced|not a complex|requires an ac|too long" "$ERR" || echo "(no Smith-chart info lines)"
echo "--------------------------------"

fail=0
chk() { # $1=pattern $2=expected_count $3=label
  local n; n=$(grep -cE "$1" "$ERR" 2>/dev/null || true)
  if [ "$n" -eq "$2" ]; then
    echo "PASS: $3 (found $n, expected $2)"
  else
    echo "FAIL: $3 (found $n, expected $2)"
    fail=$((fail + 1))
  fi
}

# (b) exactly one "not supported" naming the bad variable
chk "Smith chart: expression token 'nosuchvar' not supported - skipped" 1 "(b) unknown variable -> one 'not supported' line"
# (d2) exactly one "unbalanced" line for the genuinely unbalanced expression
chk "Smith chart: unbalanced expression" 1 "(d2) unbalanced expression -> one 'unbalanced' line"
# (a)/(c)/(d-literal) must be clean: no complex/ac/length gating errors
chk "not a complex" 0 "(a/c/d) no 'not a complex' rejection"
chk "requires an ac raw file" 0 "(a/c/d) no 'requires an ac raw file' rejection"
chk "expression too long" 0 "(a/c/d) no 'expression too long' rejection"

# the total 'not supported' count must be exactly one (only check b)
chk "not supported - skipped" 1 "(b) total 'not supported' lines"

echo "--- expr_checks.log ---"
cat "$RES/expr_checks.log" 2>/dev/null
echo

if [ "$fail" -eq 0 ]; then
  echo "ALL PASS (miscas checks)"
  exit 0
fi
echo "$fail check(s) FAILED (miscas checks)"
exit 1
