#!/bin/bash
#
#  File: trace_check.sh
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
#  Trace-rendering check for S-parameter traces on Smith charts
#  (draw_smith_points() in src/draw.c, commits c574eace + 8dd7b54d).
#
#  Reuses the repo's headless rendering tooling (Xvfb + "xschem print png"
#  + PIL, see tests/sweep_expression/run.sh) to prove that a Smith graph
#  actually DRAWS the s_1_1 trace, not just that it accepts the property.
#
#  It renders two otherwise-identical Smith charts to PNG:
#    - baseline: the Smith grid with node="" (no trace)
#    - trace:    the Smith grid + node="s_1_1" in a distinctive wave color
#  and asserts that the trace capture contains a reasonable number of
#  pixels (>500) in that wave color which the grid-only baseline does not
#  contain. A wave that is rejected by the complex/"ac" validation in
#  draw_graph() would produce zero such pixels, so this also detects a
#  regression in the s-param-trace gating.
#
#  Usage:  ./trace_check.sh [xschem-binary]
#          default binary: <repo>/src/xschem
#  Needs:   Xvfb + gcc + X11 dev libs (soft: the pointer-warp helper is
#           reused from tests/sweep_expression/ and compiled if missing),
#           python3 + Pillow (PIL). numpy is used when available for a
#           faster pixel count.
#  Overrides:
#    SMITH_DISP          X display to use (default: first free of :99..)
#    SMITH_WAVE_COLOR    wave color index for the trace (default: 7)
#  Exit:     0 = PASS, 1 = FAIL, 2 = setup error.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
RAW="$REPO/tests/SC_Test.raw"
WAVE_COLOR=${SMITH_WAVE_COLOR:-7}   # 7 = bright red in both light & dark palettes
CAPBOX="2000 1600 -1000 -800 1000 800"   # repo-proven 1:1 schematic capture box

[ -x "$XSCHEM" ] || { echo "xschem not found or not executable: $XSCHEM" >&2; exit 2; }
[ -f "$RAW" ]    || { echo "raw file not found: $RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c 'import PIL' >/dev/null 2>&1 || { echo "python3 PIL (Pillow) required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/*.png "$RES"/trace_check.* "$RES"/trace.tcl "$RES"/xauth.$$

# --- locate the pointer-warp helper (reused from the sweep_expression test) ---
WARP="$REPO/tests/sweep_expression/warp"
if [ ! -x "$WARP" ]; then
  if command -v gcc >/dev/null && \
     gcc -o "$WARP" "$REPO/tests/sweep_expression/warp.c" -lX11 2>/dev/null; then
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
    : > "$XAUTH"   # empty authority file; Xvfb runs with -ac so it is unused
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

# --- generate the capture script ---
cat > "$RES/trace.tcl" <<EOF
set raw $RAW
set out $RES/trace_check.log
set outch [open \$out w]
set pngdir $RES
set warp $WARP

xschem raw read \$raw
puts \$outch "SIM_TYPE=[xschem raw sim_type]"
puts \$outch "IDX_S11=[xschem raw index s_1_1]"

xschem add_graph
xschem setprop rect 2 0 mode Smith
xschem setprop rect 2 0 smz0 75
xschem setprop rect 2 0 color {$WAVE_COLOR}
xschem setprop rect 2 0 x1 0
xschem setprop rect 2 0 x2 100
xschem setprop rect 2 0 y1 0
xschem setprop rect 2 0 y2 10

# position the pointer over the canvas so the graph lands in the capture box
exec \$warp
update
update
after 50
update

# baseline: Smith grid only (node="")
xschem setprop rect 2 0 node ""
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/warm1.png $CAPBOX
xschem draw_graph 0
xschem print png \$pngdir/baseline.png $CAPBOX

# trace A: s_1_1 in the distinctive wave color
xschem setprop rect 2 0 node "s_1_1"
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/warm2.png $CAPBOX
xschem draw_graph 0
xschem print png \$pngdir/trace.png $CAPBOX

# trace B: the impedance z_1_1 converted Z->Gamma with the RPN imp() operator
xschem setprop rect 2 0 node "Zin; z_1_1 imp()"
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/B.png $CAPBOX

# trace C: Z = v(net1)/i(v2) (RPN) converted Z->Gamma with imp()
xschem setprop rect 2 0 node "Zin2; v(net1) i(v2) / imp()"
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/C.png $CAPBOX

puts \$outch "CAPTURE_DONE"
close \$outch
xschem exit closewindow force 0
EOF

# --- run (a Tcl error can make xschem hang instead of exiting: retry) ---
rc=1
for try in 1 2 3; do
  rm -f "$RES/trace_check.log"
  timeout 90 "$XSCHEM" --script "$RES/trace.tcl" </dev/null \
    >"$RES/trace_check.stdout" 2>"$RES/trace_check.stderr"
  rc=$?
  if [ "$rc" -eq 0 ] && grep -q "CAPTURE_DONE" "$RES/trace_check.log" 2>/dev/null \
     && [ -s "$RES/baseline.png" ] && [ -s "$RES/trace.png" ] \
     && [ -s "$RES/B.png" ] && [ -s "$RES/C.png" ]; then
    break
  fi
  echo "attempt $try/3 failed (rc=$rc)"
  sleep 2
done
if [ "$rc" -ne 0 ] || [ ! -s "$RES/trace.png" ] || [ ! -s "$RES/baseline.png" ] \
   || [ ! -s "$RES/B.png" ] || [ ! -s "$RES/C.png" ]; then
  echo "FAIL: xschem capture did not complete (rc=$rc)"
  tail -15 "$RES/trace_check.stderr" 2>/dev/null
  exit 1
fi

# if the trace was rejected by the complex/ac validation, draw_graph() emits
# a "not a complex" info() line on stderr; that is a hard failure.
if grep -qi "not a complex" "$RES/trace_check.stderr" 2>/dev/null; then
  echo "FAIL: s_1_1 trace rejected by validation ('not a complex' in stderr):"
  grep -i "not a complex" "$RES/trace_check.stderr"
  exit 1
fi

# --- wave-color pixel checks: (1) A=s_1_1 renders vs grid-only baseline, and
# --- (2) pixel-equivalence of the imp() RPN traces B and C against A.
# B = "Zin; z_1_1 imp()" and C = "Zin2; v(net1) i(v2) / imp()" must plot the
# SAME physical curve as A (the reflection coefficient), so the wave-color
# pixel sets must overlap >95% of the smaller set.
python3 - "$RES/baseline.png" "$RES/trace.png" "$RES/B.png" "$RES/C.png" <<'PY'
import sys
base_fn, a_fn, b_fn, c_fn = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]

def die(msg):
    print("FAIL: " + msg)
    sys.exit(1)

try:
    import numpy as np
    from PIL import Image
    def load(fn):
        return np.asarray(Image.open(fn).convert("RGB"))
    base, A, B, C = load(base_fn), load(a_fn), load(b_fn), load(c_fn)
    if not (base.shape == A.shape == B.shape == C.shape):
        die("capture size mismatch %s" % ([i.shape for i in (base, A, B, C)],))
    h, w, _ = A.shape
    diffmask = (A != base).any(axis=2)
    diff = int(diffmask.sum())
    if diff == 0:
        die("A and baseline identical - s_1_1 trace not rendered")
    diffpix = A[diffmask].reshape(-1, 3)
    uniq, counts = np.unique(diffpix, axis=0, return_counts=True)
    wave = tuple(int(v) for v in uniq[int(counts.argmax())])
    wv = np.array(wave)
    m = lambda im: (im == wv).all(axis=2)
    mA, mB, mC, mb = m(A), m(B), m(C), m(base)
    ca, cb, cc, cbase = int(mA.sum()), int(mB.sum()), int(mC.sum()), int(mb.sum())
    ab, ac = int((mA & mB).sum()), int((mA & mC).sum())
    print("CAPTURE_SIZE=%dx%d" % (w, h))
    print("DIFF_PIXELS=%d" % diff)
    print("WAVE_COLOR=%s" % (wave,))
    print("WAVE_IN_TRACE(A)=%d WAVE_IN_BASELINE=%d" % (ca, cbase))
    print("COUNT_A=%d COUNT_B=%d COUNT_C=%d" % (ca, cb, cc))
    print("OVERLAP_AB=%d OVERLAP_AC=%d" % (ab, ac))
    if not (diff > 500 and ca > 500 and cbase < 200):
        die("not enough wave-color pixels (diff=%d A=%d baseline=%d)" % (diff, ca, cbase))
    print("PASS: s_1_1 Smith trace rendered (%d wave-color px in A, %d in grid-only baseline)"
          % (ca, cbase))
    rAB = ab / min(ca, cb) if min(ca, cb) > 0 else 0.0
    rAC = ac / min(ca, cc) if min(ca, cc) > 0 else 0.0
    print("AB_RATIO=%.4f AC_RATIO=%.4f" % (rAB, rAC))
    bad = 0
    for name, r in (("A~B", rAB), ("A~C", rAC)):
        if r > 0.95:
            print("PASS: %s pixel equivalence (%.1f%% > 95%%)" % (name, r * 100))
        else:
            print("FAIL: %s pixel equivalence (%.1f%% <= 95%%)" % (name, r * 100))
            bad += 1
    sys.exit(1 if bad else 0)
except ImportError:
    # numpy unavailable: fall back to PIL for the render check; the overlap
    # needs the array view, so reuse PIL pixel arrays.
    def load(fn):
        im = Image.open(fn).convert("RGB")
        return im
    base, A, B, C = load(base_fn), load(a_fn), load(b_fn), load(c_fn)
    if not (base.size == A.size == B.size == C.size):
        die("capture size mismatch %s" % ([i.size for i in (base, A, B, C)],))
    w, h = A.size
    pa, pb, pc, pbase = A.load(), B.load(), C.load(), base.load()
    domdiff = {}
    diff = 0
    for y in range(h):
        for x in range(w):
            if pa[x, y] != pbase[x, y]:
                diff += 1
                domdiff[pa[x, y]] = domdiff.get(pa[x, y], 0) + 1
    if diff == 0:
        die("A and baseline identical - s_1_1 trace not rendered")
    wave = max(domdiff, key=domdiff.get)
    ca = sum(1 for y in range(h) for x in range(w) if pa[x, y] == wave)
    cb = sum(1 for y in range(h) for x in range(w) if pb[x, y] == wave)
    cc = sum(1 for y in range(h) for x in range(w) if pc[x, y] == wave)
    cbase = sum(1 for y in range(h) for x in range(w) if pbase[x, y] == wave)
    ab = sum(1 for y in range(h) for x in range(w)
             if pa[x, y] == wave and pb[x, y] == wave)
    ac = sum(1 for y in range(h) for x in range(w)
             if pa[x, y] == wave and pc[x, y] == wave)
    print("CAPTURE_SIZE=%dx%d" % (w, h))
    print("DIFF_PIXELS=%d" % diff)
    print("WAVE_COLOR=%s" % (wave,))
    print("WAVE_IN_TRACE(A)=%d WAVE_IN_BASELINE=%d" % (ca, cbase))
    print("COUNT_A=%d COUNT_B=%d COUNT_C=%d" % (ca, cb, cc))
    print("OVERLAP_AB=%d OVERLAP_AC=%d" % (ab, ac))
    if not (diff > 500 and ca > 500 and cbase < 200):
        die("not enough wave-color pixels (diff=%d A=%d baseline=%d)" % (diff, ca, cbase))
    print("PASS: s_1_1 Smith trace rendered (%d wave-color px in A, %d in grid-only baseline)"
          % (ca, cbase))
    rAB = ab / min(ca, cb) if min(ca, cb) > 0 else 0.0
    rAC = ac / min(ca, cc) if min(ca, cc) > 0 else 0.0
    print("AB_RATIO=%.4f AC_RATIO=%.4f" % (rAB, rAC))
    bad = 0
    for name, r in (("A~B", rAB), ("A~C", rAC)):
        if r > 0.95:
            print("PASS: %s pixel equivalence (%.1f%% > 95%%)" % (name, r * 100))
        else:
            print("FAIL: %s pixel equivalence (%.1f%% <= 95%%)" % (name, r * 100))
            bad += 1
    sys.exit(1 if bad else 0)
PY
pyrc=$?

echo "--- trace_check.log ---"
cat "$RES/trace_check.log" 2>/dev/null
echo
if [ "$pyrc" -eq 0 ]; then
  exit 0
else
  exit 1
fi
