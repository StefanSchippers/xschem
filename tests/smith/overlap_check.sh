#!/bin/bash
#  File: overlap_check.sh
#  This file is part of XSCHEM, a schematic capture and Spice/Vhdl/Verilog
#  netlisting tool for circuit simulation. Copyright (C) 1998-2026 S.F. Schippers
#  This program is free software; you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free Software
#  Foundation; either version 2 of the License, or (at your option) any later version.
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#  PARTICULAR PURPOSE. See the GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License along with
#  this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
#  Street, Fifth Floor, Boston, MA 02110-1301 USA
#  Smith-chart SP-vs-AC-raw overlap test. Proves that on tests/SC_Test.sch the
#  SP-raw trace s_1_1 and the AC-raw, imp()-derived trace
#      Zin; net1 i(v2) / -1 * imp() % -1 $netlist_dir/SC_Test_AC.raw ac
#  land on the SAME physical curve of the Smith chart (i.e. the per-wave
#  rawfile/sim_type override + the RPN expression + imp() reproduce the raw
#  s_1_1 S-parameter).
#  Reuses the proven single-wave-color pattern from trace_check.sh: each wave is
#  rendered ALONE (node overridden on the .sch's Smith graph rect 2 0, color 7 =
#  bright red), warm-up print + real print, then we require >=95% of one wave's
#  red pixels to sit within Chebyshev distance 2 of the other wave's red pixels,
#  in BOTH directions. The two captures share the identical graph geometry (same
#  rect 2 0, same Smith plane, same CAPBOX) so their mapping is identical and only
#  the data can differ.
#  Usage: ./overlap_check.sh [xschem-binary]  Needs: Xvfb, gcc, X11, python3+PIL.
#  Overrides: SMITH_DISP, SMITH_WAVE_COLOR (default 7). Exit: 0=PASS 1=FAIL 2=setup.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
SCH="$REPO/tests/SC_Test.sch"
BASE_RAW="$REPO/tests/SC_Test.raw"
AC_RAW="$REPO/tests/SC_Test_AC.raw"
WAVE_COLOR=${SMITH_WAVE_COLOR:-7}   # 7 = bright red (#ff0000)
CAPBOX="2000 1600 -1000 -800 1000 800"

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$SCH" ]    || { echo "schematic not found: $SCH" >&2; exit 2; }
[ -f "$BASE_RAW" ] || { echo "base raw not found: $BASE_RAW" >&2; exit 2; }
[ -f "$AC_RAW" ]   || { echo "AC raw not found: $AC_RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 PIL required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/overlap_*.png "$RES"/overlap_*.stderr "$RES"/overlap_*.stdout \
      "$RES"/overlap_*.log "$RES"/overlap_cap.tcl "$RES"/overlap.log "$RES"/xauth.$$

# pointer-warp helper (reused from the sweep_expression test)
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

# pick a free X display; start Xvfb on it if the socket is missing
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

# ================= capture (one wave color) =================
# One capture per wave, each in its OWN fresh process (an imp() trace in a large
# raw can otherwise leave xschem unable to exit, see trace_check.sh). The .sch is
# loaded, the base (SP) raw read explicitly, the Smith graph's node+color
# overridden on rect 2 0, then warm-up print + real print. NODE is passed via the
# environment and quoted in braces so "$netlist_dir" stays literal for draw_graph
# to substitute (it is NOT a bash shell variable).
cat > "$RES/overlap_cap.tcl" <<'EOF'
set outch [open $env(OUTLOG) w]
set warp $env(WARP)
set baseraw $env(BASE_RAW)
set outpng $env(OUTPNG)
set capbox [split $env(CAPBOX) " "]
xschem raw read $baseraw
exec $warp
update
update
after 50
update
xschem select rect 2 0 clear nodraw
xschem setprop rect 2 0 node $env(NODENAME)
xschem setprop rect 2 0 color $env(WAVE_COLOR)
xschem draw_graph 0
# warm-up: the first png capture of a fresh graph lands on the default
# 200x200 canvas; a discarded capture + re-draw sizes it to the CAPBOX
xschem print png $outpng {*}$capbox
xschem draw_graph 0
xschem print png $outpng {*}$capbox
puts $outch "OVERLAP_DONE $env(OUTPNG)"
close $outch
xschem exit closewindow force 0
EOF

# one capture in a fresh process: run_cap <name> <node-expression>
run_cap() {
  local name="$1" node="$2" png t
  png="$RES/overlap_${name}.png"
  for t in 1 2 3; do
    rm -f "$png"
    OUTPNG="$png" OUTLOG="$RES/overlap_${name}.log" NODENAME="$node" \
    WAVE_COLOR="$WAVE_COLOR" WARP="$WARP" BASE_RAW="$BASE_RAW" CAPBOX="$CAPBOX" \
      timeout 60 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/overlap_cap.tcl" </dev/null \
      >"$RES/overlap_${name}.stdout" 2>"$RES/overlap_${name}.stderr"
    [ -s "$png" ] && return 0
    echo "overlap $name attempt $t/3 failed"
    sleep 2
  done
  echo "FAIL: overlap $name capture produced no png"
  tail -15 "$RES/overlap_${name}.stderr" 2>/dev/null
  return 1
}

run_cap A "s_1_1" || exit 1
run_cap B 'Zin; net1 i(v2) / -1 * imp() % -1 $netlist_dir/SC_Test_AC.raw ac' || exit 1

# --- no validation-error lines may appear (rejected/failed wave) ---
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES/overlap_A.stderr" "$RES/overlap_B.stderr" 2>/dev/null; then
  echo "FAIL: validation error in stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" \
       "$RES/overlap_A.stderr" "$RES/overlap_B.stderr"
  exit 1
fi
echo "PASS: no validation-error lines in stderr"

# ================= overlap analysis (PIL, numpy only) =================
python3 - "$RES" <<'PY'
import sys
RES = sys.argv[1]
import numpy as np
from PIL import Image

def redmask(fn):
    im = np.asarray(Image.open(fn).convert("RGB")).astype(int)
    # bright red family, same as trace_check.sh (color 7 = #ff0000)
    return (im[:,:,0] >= 150) & (im[:,:,1] <= 55) & (im[:,:,2] <= 55)

def linf_dist(mask):
    """Exact Chebyshev (L_inf) distance transform: for each pixel, the min
    chebyshev distance to the nearest True pixel (0 where True). 2-pass, O(n),
    no scipy."""
    H, W = mask.shape
    dist = np.where(mask, 0, 10**9).astype(np.int32)
    for y in range(1, H):                       # from above
        dist[y, :] = np.minimum(dist[y, :], dist[y-1, :] + 1)
    for x in range(1, W):                       # from the left
        dist[:, x] = np.minimum(dist[:, x], dist[:, x-1] + 1)
    for y in range(1, H):                       # from above-left (diagonal)
        dist[y, 1:] = np.minimum(dist[y, 1:], dist[y-1, :-1] + 1)
    for y in range(H-2, -1, -1):                # from below
        dist[y, :] = np.minimum(dist[y, :], dist[y+1, :] + 1)
    for x in range(W-2, -1, -1):                # from the right
        dist[:, x] = np.minimum(dist[:, x], dist[:, x+1] + 1)
    for y in range(H-2, -1, -1):                # from below-right (diagonal)
        dist[y, :W-1] = np.minimum(dist[y, :W-1], dist[y+1, 1:] + 1)
    return dist

A = redmask(RES + "/overlap_A.png")
B = redmask(RES + "/overlap_B.png")
na, nb = int(A.sum()), int(B.sum())
print("wave px   A(s_1_1, SP raw)=%d   B(Zin imp(), AC raw)=%d" % (na, nb))

bad = 0
if na > 50:
    print("PASS: A renders (%d red px, >50)" % na)
else:
    print("FAIL: A not rendered (%d red px, <=50)" % na); bad += 1
if nb > 50:
    print("PASS: B renders (%d red px, >50)" % nb)
else:
    print("FAIL: B not rendered (%d red px, <=50)" % nb); bad += 1
if bad:
    sys.exit(1)

def overlap(src, dst, label):
    d = linf_dist(dst)
    ys, xs = np.nonzero(src)
    dd = d[ys, xs]
    frac = float((dd <= 2).mean())
    print("%s: frac within chebyshev<=2 = %.4f   buckets {<=0:%.3f <=1:%.3f <=2:%.3f <=5:%.3f <=10:%.3f}  maxd=%d  n=%d"
          % (label, frac,
             (dd<=0).mean(), (dd<=1).mean(), (dd<=2).mean(), (dd<=5).mean(), (dd<=10).mean(),
             int(dd.max()), len(dd)))
    worst = np.argsort(-dd)[:10]
    print("   10 worst %s (src x,src y, min-d): %s"
          % (label, [(int(xs[i]), int(ys[i]), int(dd[i])) for i in worst]))
    return frac

fa = overlap(A, B, "A->B")
fb = overlap(B, A, "B->A")

if fa >= 0.95 and fb >= 0.95:
    print("PASS: A<->B overlap %.4f / %.4f (>= 0.95 both directions)" % (fa, fb))
    sys.exit(0)
else:
    print("FAIL: A<->B overlap %.4f / %.4f (need >= 0.95 both directions)" % (fa, fb))
    print("note: the top-left red cluster is the wave's own name label (drawn in the")
    print("      wave color); the Smith curve itself is what overlaps. A systematic")
    print("      offset of the curves would show as a large uniform min-d across the")
    print("      trace, not a localized cluster.")
    sys.exit(1)
PY
rc=$?

echo "--- overlap logs ---"
cat "$RES"/overlap_*.log 2>/dev/null
echo

if [ "$rc" -eq 0 ]; then
  echo "ALL PASS (SP s_1_1 == AC-raw Zin imp() on the Smith chart)"
  exit 0
else
  echo "FAIL (overlap analysis rc=$rc)"
  exit 1
fi
