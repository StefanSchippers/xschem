#!/bin/bash
#  File: trace_check.sh
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
#  Smith-chart trace rendering + exact-geometry test (draw_smith_expr_points in
#  src/draw.c). Two phases, both headless (Xvfb + xschem print png + PIL).
#  PHASE 1 -- render gating: renders a Smith chart on tests/SC_Test.raw in node
#  modes baseline(node=""), A(node="s_1_1"), B(node="Zin; z_1_1 imp()"),
#  C(node="Zin2; v(net1) i(v2) / imp()"); each capture runs in its own process
#  (a warm-up print sizes the canvas past the default 200x200), asserting A, B,
#  C each add >50 wave-color pixels the grid-only baseline lacks (a rejected
#  wave adds ~zero; the chart grid itself is not wave color).
#  PHASE 2 -- exact geometry: renders six constant-impedance complex raws
#  (fixture_raws.sh) with node="Zt; z imp()", smz0=50. Gamma=(Z-Z0)/(Z+Z0) maps
#  c0=50+0j->(0,0), short=0+0j->(-1,0), third=100+0j->(1/3,0), bot=0-50j->(0,-1),
#  top=0+50j->(0,+1), q1=50+50j->(0.2,0.4). The capture box keeps the fixed node
#  label out of frame and the unit circle in frame, so each red capture is one
#  point dot. Self-calibration (C from c0, R from short) cancels any fixed
#  offset; the other four points must land within a <=6 px tolerance.
#  Usage: ./trace_check.sh [xschem-binary]  Needs: Xvfb, gcc, X11, python3+PIL.
#  Overrides: SMITH_DISP, SMITH_WAVE_COLOR (default 7). Exit: 0=PASS 1=FAIL 2=setup.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
RAW="$REPO/tests/SC_Test.raw"
WAVE_COLOR=${SMITH_WAVE_COLOR:-7}   # 7 = bright red (#ff0000)
CAPBOX="2000 1600 -1000 -800 1000 800"   # Phase 1 capture box
FIXBOX="450 450 -280 -280 60 60"         # Phase 2 tight box (label out, circle in)

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$RAW" ]    || { echo "raw file not found: $RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 PIL required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/*.png "$RES"/fx_*.raw "$RES"/trace_check.* "$RES"/fixture.* \
      "$RES"/p1_cap.tcl "$RES"/fixture.tcl "$RES"/phase1.log \
      "$RES"/baseline.* "$RES"/B.* "$RES"/C.* "$RES"/xauth.$$

# self-contained pointer-warp helper (tests/smith/warp.c)
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


# ================= PHASE 1: render gating (A/B/C vs baseline) ==============
#  Each capture runs in its OWN xschem process: rendering several imp() traces
#  in one process on a large raw can leave xschem unable to exit, so isolating
#  each capture makes every run terminate cleanly. Node + output are passed via
#  the environment so one template serves all four captures.
cat > "$RES/p1_cap.tcl" <<EOF
set outch [open \$env(OUTLOG) a]
set raw $RAW
set warp $WARP
set node \$env(NODE)
set outpng \$env(OUTPNG)
xschem raw read \$raw
xschem add_graph
xschem setprop rect 2 0 mode Smith
xschem setprop rect 2 0 smz0 75
xschem setprop rect 2 0 color {$WAVE_COLOR}
xschem setprop rect 2 0 x1 0
xschem setprop rect 2 0 x2 100
xschem setprop rect 2 0 y1 0
xschem setprop rect 2 0 y2 10
if {\$node ne ""} { xschem setprop rect 2 0 node "\$node" }
exec \$warp
update
update
after 50
update
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
# warm-up: the first png capture of a fresh graph lands on the default
# 200x200 canvas; a discarded capture + re-draw sizes it to the CAPBOX
xschem print png \$outpng $CAPBOX
xschem draw_graph 0
xschem print png \$outpng $CAPBOX
puts \$outch "P1_DONE \$env(OUTPNG)"
close \$outch
xschem exit closewindow force 0
EOF

# one capture in a fresh process: p1_run <png-basename> <node-expression>
p1_run() {
  local png="$1" node="$2" t
  for t in 1 2 3; do
    rm -f "$RES/$png.png"
    NODE="$node" OUTPNG="$RES/$png.png" OUTLOG="$RES/phase1.log" \
      timeout 60 "$XSCHEM" -r --script "$RES/p1_cap.tcl" </dev/null \
      >"$RES/$png.stdout" 2>"$RES/$png.stderr"
    [ -s "$RES/$png.png" ] && return 0
    echo "phase1 $png attempt $t/3 failed"
    sleep 2
  done
  echo "FAIL: phase1 $png capture produced no png"
  return 1
}
rm -f "$RES/phase1.log"
p1_run baseline "" || exit 1
p1_run trace "s_1_1" || exit 1
p1_run B "Zin; z_1_1 imp()" || exit 1
p1_run C "Zin2; v(net1) i(v2) / imp()" || exit 1

if grep -qi "not a complex" "$RES"/trace.stderr "$RES"/B.stderr "$RES"/C.stderr 2>/dev/null; then
  echo "FAIL: phase-1 trace rejected (not a complex in stderr):"
  grep -i "not a complex" "$RES"/trace.stderr "$RES"/B.stderr "$RES"/C.stderr
  exit 1
fi
# --- Phase-1 checks: A/B/C each add wave-color pixels vs the grid baseline ---
python3 - "$RES" <<PY
import sys
RES = sys.argv[1]
try:
    import numpy as np
    from PIL import Image
    def cnt(fn):
        im = np.asarray(Image.open(fn).convert("RGB")).astype(int)
        m = (im[:,:,0]>=150)&(im[:,:,1]<=55)&(im[:,:,2]<=55)
        return int(m.sum())
except ImportError:
    from PIL import Image
    def cnt(fn):
        px = Image.open(fn).convert("RGB").load()
        w,h = Image.open(fn).size
        return sum(1 for y in range(h) for x in range(w)
                   if px[x,y][0]>=150 and px[x,y][1]<=55 and px[x,y][2]<=55)
def die(m):
    print("FAIL: "+m); sys.exit(1)
base = cnt(RES+"/baseline.png")
ca = cnt(RES+"/trace.png"); cb = cnt(RES+"/B.png"); cc = cnt(RES+"/C.png")
print("baseline=%d A=%d B=%d C=%d" % (base, ca, cb, cc))
bad = 0
for name, n in (("A=s_1_1", ca), ("B=z_1_1 imp()", cb), ("C=v/i imp()", cc)):
    if n > 50:
        print("PASS: %s renders (%d wave px, >50)" % (name, n))
    else:
        print("FAIL: %s not rendered (%d wave px, <=50)" % (name, n)); bad += 1
if base > 20:
    print("FAIL: baseline has %d wave px (expected ~0)" % base); bad += 1
sys.exit(1 if bad else 0)
PY

pyrc=$?
echo "--- phase1.log ---"
cat "$RES/phase1.log" 2>/dev/null
echo
# ================= PHASE 2: exact geometry (imp() Z->Gamma) ===============
"$DIR/fixture_raws.sh" "$RES"
if [ $? -ne 0 ]; then
  echo "FAIL: fixture raw generation failed"
  exit 1
fi
cat > "$RES/fixture.tcl" <<EOF
set out $RES/fixture.log
set outch [open \$out w]
set pngdir $RES
set warp $WARP
xschem raw read \$pngdir/fx_c0.raw
xschem add_graph
xschem setprop rect 2 0 mode Smith
xschem setprop rect 2 0 smz0 50
xschem setprop rect 2 0 color {$WAVE_COLOR}
xschem setprop rect 2 0 sim_type ac
xschem setprop rect 2 0 node "Zt; z imp()"
exec \$warp
update
update
after 50
update
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
xschem print png \$pngdir/fx_warm.png $FIXBOX
update
xschem setprop rect 2 0 node ""
xschem draw_graph 0
xschem print png \$pngdir/fx_bl.png $FIXBOX
update
foreach k {c0 short third q1 bot top} {
  xschem raw read \$pngdir/fx_\$k.raw
  xschem setprop rect 2 0 node "Zt; z imp()"
  xschem draw_graph 0
  xschem print png \$pngdir/fx_\$k.png $FIXBOX
  update
}
puts \$outch "FIXTURE_DONE"
close \$outch
xschem exit closewindow force 0
EOF

# --- run Phase 2 (retry) ---
rc=1
for try in 1 2 3; do
  rm -f "$RES/fixture.log"
  timeout 90 "$XSCHEM" -r --script "$RES/fixture.tcl" </dev/null \
    >"$RES/fixture.stdout" 2>"$RES/fixture.stderr"
  rc=$?
  ok=1
  for f in fx_bl fx_c0 fx_short fx_third fx_q1 fx_bot fx_top; do
    [ -s "$RES/$f.png" ] || ok=0
  done
  if [ "$rc" -eq 0 ] && grep -q "FIXTURE_DONE" "$RES/fixture.log" 2>/dev/null && [ $ok -eq 1 ]; then
    break
  fi
  echo "phase2 attempt $try/3 failed (rc=$rc)"
  sleep 2
done
if [ "$rc" -ne 0 ] || ! grep -q "FIXTURE_DONE" "$RES/fixture.log" 2>/dev/null; then
  echo "FAIL: phase-2 capture did not complete (rc=$rc)"
  tail -15 "$RES/fixture.stderr" 2>/dev/null
  exit 1
fi
if grep -qi "not a complex" "$RES/fixture.stderr" 2>/dev/null; then
  echo "FAIL: phase-2 fixture rejected (not a complex in stderr):"
  grep -i "not a complex" "$RES/fixture.stderr"
  exit 1
fi
# --- Phase-2 checks: self-calibrate C (Gamma=0), R (Gamma=-1), verify 4 pts ---
python3 - "$RES" <<PY
import sys, math
RES = sys.argv[1]
try:
    import numpy as np
    from PIL import Image
    def cen(fn):
        im = np.asarray(Image.open(fn).convert("RGB")).astype(int)
        m = (im[:,:,0]>=150)&(im[:,:,1]<=55)&(im[:,:,2]<=55)
        ys, xs = np.nonzero(m)
        if len(xs) == 0:
            return None
        return (float(xs.mean()), float(ys.mean()), int(m.sum()))
except ImportError:
    from PIL import Image
    def cen(fn):
        px = Image.open(fn).convert("RGB").load()
        w, h = Image.open(fn).size
        sx = sy = n = 0
        for y in range(h):
            for x in range(w):
                r, g, b = px[x, y]
                if r >= 150 and g <= 55 and b <= 55:
                    sx += x; sy += y; n += 1
        if n == 0:
            return None
        return (sx / n, sy / n, n)
def die(m):
    print("FAIL: " + m); sys.exit(1)
bl = cen(RES + "/fx_bl.png")
blc = bl[2] if bl else 0
if blc > 40:
    die("phase-2 baseline has %d wave px (expected ~0): region/color wrong" % blc)
print("PHASE2 baseline wave px = %d" % blc)
c0 = cen(RES + "/fx_c0.png")
short = cen(RES + "/fx_short.png")
if c0 is None:
    die("fx_c0.png has no wave px (Gamma=0 not rendered)")
if short is None:
    die("fx_short.png has no wave px (Gamma=-1 not rendered)")
R = math.hypot(short[0] - c0[0], short[1] - c0[1])
tol = min(6.0, max(3.0, 0.04 * R))
print("PHASE2 C=(%.2f,%.2f) R=%.2f px tol=%.2f px" % (c0[0], c0[1], R, tol))
if R < 60:
    die("radius R=%.1f px too small to be meaningful (<60)" % R)
checks = [("third", 1.0/3.0, 0.0), ("bot", 0.0, -1.0), ("top", 0.0, 1.0), ("q1", 0.2, 0.4)]
bad = 0
for name, u, v in checks:
    p = cen(RES + "/fx_%s.png" % name)
    if p is None:
        print("FAIL: %s - no wave px (point not rendered)" % name); bad += 1; continue
    ex = c0[0] + u * R
    ey = c0[1] - v * R
    d = math.hypot(p[0] - ex, p[1] - ey)
    ok = d <= tol
    if not ok:
        bad += 1
    print("%s: %-5s measured=(%.2f,%.2f) expected=(%.2f,%.2f) dist=%.2f px (tol %.2f)" % ("PASS" if ok else "FAIL", name, p[0], p[1], ex, ey, d, tol))
if bad:
    print("%d phase-2 geometry check(s) FAILED" % bad); sys.exit(1)
print("PASS: all four fixture points match the self-calibrated Smith geometry")
sys.exit(0)
PY
fixrc=$?
echo "--- fixture.log (phase 2) ---"
cat "$RES/fixture.log" 2>/dev/null

# ================= summary ================
if [ "$pyrc" -eq 0 ] && [ "$fixrc" -eq 0 ]; then
  echo "ALL PASS (phase 1 render gating + phase 2 exact geometry)"
  exit 0
else
  echo "FAIL (phase1 render rc=$pyrc, phase2 geometry rc=$fixrc)"
  exit 1
fi
