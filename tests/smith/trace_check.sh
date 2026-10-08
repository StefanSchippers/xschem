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

# trace: s_1_1 in the distinctive wave color
xschem setprop rect 2 0 node "s_1_1"
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/warm2.png $CAPBOX
xschem draw_graph 0
xschem print png \$pngdir/trace.png $CAPBOX

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
     && [ -s "$RES/baseline.png" ] && [ -s "$RES/trace.png" ]; then
    break
  fi
  echo "attempt $try/3 failed (rc=$rc)"
  sleep 2
done
if [ "$rc" -ne 0 ] || [ ! -s "$RES/trace.png" ] || [ ! -s "$RES/baseline.png" ]; then
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

# --- count wave-color pixels: trace vs grid-only baseline ---
python3 - "$RES/baseline.png" "$RES/trace.png" <<'PY'
import sys
base_fn, trace_fn = sys.argv[1], sys.argv[2]

def analyze_pil(base_fn, trace_fn):
    from PIL import Image
    from collections import Counter
    base = Image.open(base_fn).convert("RGB")
    trace = Image.open(trace_fn).convert("RGB")
    if base.size != trace.size:
        print("FAIL: capture size mismatch %s vs %s" % (base.size, trace.size))
        sys.exit(1)
    w, h = base.size
    bp, tp = base.load(), trace.load()
    diff = 0
    domdiff = Counter()
    for y in range(h):
        for x in range(w):
            a, b = bp[x, y], tp[x, y]
            if a != b:
                diff += 1
                domdiff[b] += 1
    if not domdiff:
        print("FAIL: trace and baseline identical - s_1_1 trace not rendered")
        sys.exit(1)
    wave = domdiff.most_common(1)[0][0]
    wt = sum(1 for y in range(h) for x in range(w) if tp[x, y] == wave)
    wb = sum(1 for y in range(h) for x in range(w) if bp[x, y] == wave)
    print("CAPTURE_SIZE=%dx%d" % (w, h))
    print("DIFF_PIXELS=%d" % diff)
    print("WAVE_COLOR=%s" % (wave,))
    print("WAVE_IN_TRACE=%d" % wt)
    print("WAVE_IN_BASELINE=%d" % wb)
    print("TOP_DIFF_COLORS=%s" % (domdiff.most_common(3),))
    if diff > 500 and wt > 500 and wb < 200:
        print("PASS: s_1_1 Smith trace rendered (%d wave-color px in trace, %d in grid-only baseline)" % (wt, wb))
        sys.exit(0)
    print("FAIL: not enough wave-color pixels (diff=%d trace=%d baseline=%d)" % (diff, wt, wb))
    sys.exit(1)

try:
    import numpy as np
    from PIL import Image
    a = np.asarray(Image.open(base_fn).convert("RGB"))
    b = np.asarray(Image.open(trace_fn).convert("RGB"))
    if a.shape != b.shape:
        print("FAIL: capture size mismatch %s vs %s" % (a.shape, b.shape))
        sys.exit(1)
    h, w, _ = a.shape
    diffmask = (a != b).any(axis=2)
    diff = int(diffmask.sum())
    if diff == 0:
        print("FAIL: trace and baseline identical - s_1_1 trace not rendered")
        sys.exit(1)
    diffpix = b[diffmask].reshape(-1, 3)
    uniq, counts = np.unique(diffpix, axis=0, return_counts=True)
    wave = tuple(int(v) for v in uniq[int(counts.argmax())])
    wave_in_trace = int((b == np.array(wave)).all(axis=2).sum())
    wave_in_base = int((a == np.array(wave)).all(axis=2).sum())
    order = np.argsort(-counts)
    top = [(tuple(int(v) for v in uniq[i]), int(counts[i])) for i in order[:3]]
    print("CAPTURE_SIZE=%dx%d" % (w, h))
    print("DIFF_PIXELS=%d" % diff)
    print("WAVE_COLOR=%s" % (wave,))
    print("WAVE_IN_TRACE=%d" % wave_in_trace)
    print("WAVE_IN_BASELINE=%d" % wave_in_base)
    print("TOP_DIFF_COLORS=%s" % (top,))
    if diff > 500 and wave_in_trace > 500 and wave_in_base < 200:
        print("PASS: s_1_1 Smith trace rendered (%d wave-color px in trace, %d in grid-only baseline)" % (wave_in_trace, wave_in_base))
        sys.exit(0)
    print("FAIL: not enough wave-color pixels (diff=%d trace=%d baseline=%d)" % (diff, wave_in_trace, wave_in_base))
    sys.exit(1)
except ImportError:
    analyze_pil(base_fn, trace_fn)
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
