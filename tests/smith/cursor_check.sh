#!/bin/bash
#  File: cursor_check.sh
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
#  Smith-chart cursor-marker pixel test (draw_smith_cursor_markers in src/draw.c).
#  Renders tests/SC_Test.sch (3 waves: s_1_1, s_2_1 and the imp()-derived Zin)
#  with both graph cursors enabled (cursor1 = 100 MHz -> filled dot, cursor2 =
#  300 MHz -> cross) in its own process per capture, plus a no-cursor control.
#  Headless: Xvfb + xschem print png + PIL. Checks, per wave:
#    * a wave-color marker at the self-calibrated Smith position of Gamma(f_c)
#      (unit circle fitted from the GRIDLAYER render -> C, R; expected Gamma is
#      lerped from tests/SC_Test.raw exactly like the C code);
#    * the per-wave readout text in wave color next to the marker;
#    * the GRIDLAYER "f = ..." labels top-left of the plot square.
#  s_1_1 and Zin land on the SAME point (identical Gamma), so the marker of the
#  later-drawn wave occludes the earlier one: the as-is capture checks Zin's
#  marker there; a second capture re-orders the node (s_1_1 drawn last, colors
#  "12 9 4") so s_1_1's marker is visible instead. A fourth capture enables
#  then disables both cursors and must match the no-cursor control (no residue).
#  Usage: ./cursor_check.sh [xschem-binary]  Needs: Xvfb, gcc, X11, python3+numpy+PIL.
#  Overrides: SMITH_DISP.  Exit: 0=PASS 1=FAIL 2=setup.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
SCH="$REPO/tests/SC_Test.sch"
BASE_RAW="$REPO/tests/SC_Test.raw"
AC_RAW="$REPO/tests/SC_Test_AC.raw"
CAPBOX="2000 1600 -1000 -800 1000 800"
FC1="100e6"      # cursor 1 frequency (inside the 50k..600M sweep)
FC2="300e6"      # cursor 2 frequency

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$SCH" ]    || { echo "schematic not found: $SCH" >&2; exit 2; }
[ -f "$BASE_RAW" ] || { echo "base raw not found: $BASE_RAW" >&2; exit 2; }
[ -f "$AC_RAW" ]   || { echo "AC raw not found: $AC_RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import numpy" >/dev/null 2>&1 || { echo "python3 numpy required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 PIL required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/cursor_*.png "$RES"/cursor_*.log "$RES"/cursor_*.stderr \
      "$RES"/cursor_*.stdout "$RES"/cursor_cap.tcl "$RES"/xauth.$$

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
# ================= capture template =================
# One capture per MODE, each in its OWN fresh process (an imp() trace in a large
# raw can otherwise leave xschem unable to exit, see trace_check.sh). The .sch
# is the main schematic (graph rect 2 0, mode Smith, smz0=50, 3 waves, colors
# "4 12 9"); the base raw is read explicitly and the AC raw loads via the Zin
# expression's rawfile override. Cursor state is driven by the MODE env var:
#   off    : no cursors          (control)
#   on     : cursor1+cursor2     (as-is wave order: Zin last -> its marker
#                                 is visible at the s_1_1/Zin shared point)
#   s11    : cursors + node re-ordered so s_1_1 is drawn LAST (colors
#            "12 9 4" keep each wave its color) -> s_1_1 marker visible
#   toggle : cursors enabled then disabled again (residue control)
# Enabling a cursor zeroes its position, so enable BEFORE 'set cursor[12]_x'.
# The color table is echoed as "COL <hex>" lines for the pixel analysis.
cat > "$RES/cursor_cap.tcl" <<'EOF'
set outch [open $env(OUTLOG) w]
set warp $env(WARP)
set baseraw $env(BASE_RAW)
set outpng $env(OUTPNG)
set capbox [split $env(CAPBOX) " "]
xschem raw read $baseraw
if {$env(MODE) eq "s11"} {
  xschem setprop rect 2 0 node $env(NODE)
  xschem setprop rect 2 0 color $env(COLOR)
}
if {$env(MODE) ne "off"} {
  xschem cursor 1 1
  xschem set cursor1_x $env(FC1)
  xschem cursor 2 1
  xschem set cursor2_x $env(FC2)
}
if {$env(MODE) eq "toggle"} {
  xschem cursor 1 0
  xschem cursor 2 0
}
exec $warp
update
update
after 50
update
xschem select rect 2 0 clear nodraw
xschem draw_graph 0
# warm-up: the first png capture of a fresh graph lands on the default
# 200x200 canvas; a discarded capture + re-draw sizes it to the CAPBOX
xschem print png $outpng {*}$capbox
xschem draw_graph 0
xschem print png $outpng {*}$capbox
foreach c $tctx::colors { puts $outch "COL $c" }
puts $outch "CURSOR_DONE $env(MODE)"
close $outch
xschem exit closewindow force 0
EOF

# one capture in a fresh process: run_cap <name> <mode> <node> <color>
run_cap() {
  local name="$1" mode="$2" node="$3" color="$4" png t
  png="$RES/cursor_${name}.png"
  for t in 1 2 3; do
    rm -f "$png"
    MODE="$mode" NODE="$node" COLOR="$color" OUTPNG="$png" \
    OUTLOG="$RES/cursor_${name}.log" WARP="$WARP" BASE_RAW="$BASE_RAW" \
    FC1="$FC1" FC2="$FC2" CAPBOX="$CAPBOX" \
      timeout 60 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/cursor_cap.tcl" </dev/null \
      >"$RES/cursor_${name}.stdout" 2>"$RES/cursor_${name}.stderr"
    [ -s "$png" ] && return 0
    echo "cursor $name attempt $t/3 failed"
    sleep 2
  done
  echo "FAIL: cursor $name capture produced no png"
  tail -15 "$RES/cursor_${name}.stderr" 2>/dev/null
  return 1
}

# s11 node: same 3 waves, s_1_1 moved LAST so its marker is drawn on top
ZIN_EXPR='Zin; net1 i(v2) / -1 * imp() % -1 $netlist_dir/SC_Test_AC.raw ac'
NODE_S11=$(printf 's_2_1\n%s\ns_1_1' "$ZIN_EXPR")

run_cap off    off    "" "" || exit 1
run_cap on     on     "" "" || exit 1
run_cap s11    s11    "$NODE_S11" "12 9 4" || exit 1
run_cap toggle toggle "" "" || exit 1

# --- no validation-error lines may appear (rejected/failed wave) ---
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES"/cursor_off.stderr "$RES"/cursor_on.stderr \
      "$RES"/cursor_s11.stderr "$RES"/cursor_toggle.stderr 2>/dev/null; then
  echo "FAIL: validation error in stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" \
       "$RES"/cursor_*.stderr
  exit 1
fi
echo "PASS: no validation-error lines in stderr"
# ================= pixel analysis =================
python3 - "$RES" "$BASE_RAW" <<'PY'
import sys
import struct
import numpy as np
from PIL import Image

RES = sys.argv[1]
BASE_RAW = sys.argv[2]

def die(m):
    print("FAIL: " + m); sys.exit(1)

def load(fn):
    return np.asarray(Image.open(fn).convert("RGB")).astype(int)

# ---- color table (COL lines from the off capture log) ----
# .sch colors "4 12 9" -> s_1_1=4, s_2_1=12, Zin=9; the s11 capture keeps the
# same per-wave colors ("12 9 4" over the re-ordered node). GRIDLAYER=2.
colors = []
with open(RES + "/cursor_off.log") as f:
    for line in f:
        if line.startswith("COL "):
            colors.append(line.strip()[4:].lstrip("#"))
if len(colors) < 13:
    die("color table not found in cursor_off.log (got %d entries)" % len(colors))
def rgb(i):
    c = colors[i]
    return np.array([int(c[0:2], 16), int(c[2:4], 16), int(c[4:6], 16)], int)
COL_S11, COL_S21, COL_ZIN, COL_GRID = rgb(4), rgb(12), rgb(9), rgb(2)

off = load(RES + "/cursor_off.png")
on  = load(RES + "/cursor_on.png")
s11 = load(RES + "/cursor_s11.png")
tgl = load(RES + "/cursor_toggle.png")

def near(img, c, tol=30):
    return (np.abs(img - c).max(axis=2) <= tol)

# the off render must contain the traces (schematic + raw actually loaded)
if int(near(off, COL_S21).sum()) < 200:
    die("no s_2_1 trace in the control render (%d px) - graph not rendered"
        % int(near(off, COL_S21).sum()))

# ---- self-calibrate C, R from the unit circle (GRIDLAYER) in the control ----
# CAPBOX 2000x1600 over xschem -1000,-800..1000,800 and the .sch container
# 600,-440..1000,-40 -> container pixels x[1600,2000] y[360,760].
X0, X1, Y0, Y1 = 1600, 2000, 360, 760
gm = near(off, COL_GRID, 14)
gm[:Y0+8, :] = 0; gm[Y1-8:, :] = 0; gm[:, :X0+8] = 0; gm[:, X1-8:] = 0
ys, xs = np.nonzero(gm)
if len(xs) < 200:
    die("too few GRIDLAYER pixels to calibrate (%d)" % len(xs))
C0 = np.array([(X0 + X1) / 2.0, (Y0 + Y1) / 2.0])   # (1800, 560)
rad = np.hypot(xs - C0[0], ys - C0[1])
hist, edges = np.histogram(rad, bins=90, range=(0.0, 270.0))
strong = [i for i in range(len(hist)) if hist[i] >= 150]
if not strong:
    die("no strong ring found in the grid (calibration failed)")
pk = max(strong)                                    # outermost strong ring
rp = (edges[pk] + edges[pk + 1]) / 2.0              # ~ unit circle radius
sel = (rad >= rp - 6.0) & (rad <= rp + 6.0)
pts = np.stack([xs[sel], ys[sel]], axis=1).astype(float)
def fit_circle(p):
    x = p[:, 0]; y = p[:, 1]
    A = np.column_stack([2 * x, 2 * y, np.ones_like(x)])
    b = x * x + y * y
    s, *_ = np.linalg.lstsq(A, b, rcond=None)
    cx, cy = s[0], s[1]
    return cx, cy, float(np.sqrt(s[2] + cx * cx + cy * cy))
Cx, Cy, R = fit_circle(pts)
for _ in range(6):
    d = np.abs(np.hypot(pts[:, 0] - Cx, pts[:, 1] - Cy) - R)
    keep = pts[d <= 2.0]
    if len(keep) < 50:
        break
    Cx, Cy, R = fit_circle(keep)
if not (80.0 <= R <= 600.0):
    die("calibrated radius R=%.1f px implausible" % R)
print("CALIB  C=(%.2f, %.2f)  R=%.2f px  (ring r~%.1f, %d ring px)"
      % (Cx, Cy, R, rp, len(keep)))
# ---- expected Gamma(f_c): parse the SP raw, lerp re/im exactly like draw.c ----
def parse_raw(fn):
    data = open(fn, "rb").read()
    i = data.find(b"No. Variables:")
    nv = int(data[i:data.find(b"\n", i)].split(b":")[1].strip())
    i = data.find(b"No. Points:")
    npts = int(data[i:data.find(b"\n", i)].split(b":")[1].strip())
    vs = data.find(b"Variables:")
    vb = data[vs:data.find(b"Binary:", vs)]
    names = []
    for ln in vb.split(b"\n")[1:]:
        parts = [p for p in ln.decode("latin1").split("\t") if p.strip() != ""]
        if len(parts) >= 2:
            names.append((int(parts[0]), parts[1]))
    off_b = data.find(b"Binary:", vs) + len(b"Binary:")
    while data[off_b:off_b + 1] in (b"\n", b"\r"):
        off_b += 1
    nper = 2 * nv                     # complex vars: re,im per variable/point
    freq = []
    vals = {}
    for p in range(npts):
        row = struct.unpack_from("<%dd" % nper, data, off_b + p * nper * 8)
        freq.append(row[0])
        for k in range(1, len(names)):
            idx, name = names[k]
            vals.setdefault(name, []).append((row[2 * k], row[2 * k + 1]))
    return np.array(freq), vals

freq, vals = parse_raw(BASE_RAW)
if "s_1_1" not in vals or "s_2_1" not in vals:
    die("raw parse failed: s_1_1/s_2_1 missing (vars=%s)" % sorted(vals))
if freq[0] <= 0 or freq[-1] <= freq[0]:
    die("raw parse failed: frequency axis broken [%g .. %g]" % (freq[0], freq[-1]))
print("RAW    %d points, freq %.3g..%.3g" % (len(freq), freq[0], freq[-1]))

def gamma(name, fc):
    v = vals[name]
    for p in range(len(freq) - 1):
        if freq[p] <= fc <= freq[p + 1]:
            t = (fc - freq[p]) / (freq[p + 1] - freq[p])
            a, b = v[p], v[p + 1]
            return (a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1]))
    die("cursor freq %.3e outside sweep span for %s" % (fc, name))

FC1, FC2 = 100e6, 300e6
g_s11_c1, g_s11_c2 = gamma("s_1_1", FC1), gamma("s_1_1", FC2)
g_s21_c1, g_s21_c2 = gamma("s_2_1", FC1), gamma("s_2_1", FC2)
for tag, g in (("s_1_1@%.0fM" % (FC1/1e6), g_s11_c1), ("s_1_1@%.0fM" % (FC2/1e6), g_s11_c2),
               ("s_2_1@%.0fM" % (FC1/1e6), g_s21_c1), ("s_2_1@%.0fM" % (FC2/1e6), g_s21_c2)):
    mag = abs(complex(g[0], g[1]))
    if mag >= 1.0:
        die("%s has |Gamma|=%.3f outside the unit circle (parse error?)" % (tag, mag))
    print("GAMMA  %s = (%.4f, %.4f)" % (tag, g[0], g[1]))

def P(g):
    return Cx + g[0] * R, Cy - g[1] * R

bad = 0
def check(label, ok, detail):
    global bad
    print("%s: %-40s %s" % ("PASS" if ok else "FAIL", label, detail))
    if not ok:
        bad += 1

# wave-color pixels newly present at P relative to the control: the marker
def marker_delta(imgA, imgB, c, g, half=5):
    px, py = P(g)
    a = near(imgA, c)[max(0, int(py-half)):int(py+half+1),
                      max(0, int(px-half)):int(px+half+1)]
    b = near(imgB, c)[max(0, int(py-half)):int(py+half+1),
                      max(0, int(px-half)):int(px+half+1)]
    return int((a & ~b).sum()), int(a.sum()), px, py

# wave-color readout text: band right of the marker (ry = sy-16 / sy+8)
def readout_delta(imgA, imgB, c, g, which):
    px, py = P(g)
    ry = (py - 16.0) if which == 1 else (py + 8.0)
    a = near(imgA, c); b = near(imgB, c)
    m = (a & ~b)[max(0, int(ry-2)):int(ry+18), max(0, int(px-40)):int(px+260)]
    return int(m.sum()), px, py

# ---- on (as-is order) vs off: s_2_1 + Zin markers, readouts, f labels ----
for tag, c, g in (("s_2_1 cursor1@100M", COL_S21, g_s21_c1),
                  ("s_2_1 cursor2@300M", COL_S21, g_s21_c2),
                  ("Zin     cursor1@100M", COL_ZIN, g_s11_c1),
                  ("Zin     cursor2@300M", COL_ZIN, g_s11_c2)):
    d, tot, px, py = marker_delta(on, off, c, g)
    check("on: %s marker at P" % tag, d >= 15 and tot >= 40,
          "P=(%.1f,%.1f) new=%d boxtot=%d" % (px, py, d, tot))
for tag, c, g, w in (("s_2_1 readout c1", COL_S21, g_s21_c1, 1),
                     ("s_2_1 readout c2", COL_S21, g_s21_c2, 2),
                     ("Zin     readout c1", COL_ZIN, g_s11_c1, 1),
                     ("Zin     readout c2", COL_ZIN, g_s11_c2, 2)):
    d, px, py = readout_delta(on, off, c, g, w)
    check("on: %s" % tag, d >= 40, "new-band-px=%d (near P=(%.1f,%.1f))" % (d, px, py))

# f labels: GRIDLAYER text top-left of the plot square, once per active cursor
ml = (near(on, COL_GRID, 14) & ~near(off, COL_GRID, 14))
fx = int(Cx + (-1.05) * R + 8.0 - 5); fy = int(Cy - (1.05) * R + 4.0 - 5)
nl = int(ml[fy:fy+40, fx:fx+240].sum())
check("on: f-labels present (both cursors)", nl >= 50,
      "top-left new-GRID-px=%d" % nl)

# ---- s11 (s_1_1 drawn last) vs off: s_1_1 marker + readouts visible ----
for tag, g in (("s_1_1 cursor1@100M", g_s11_c1), ("s_1_1 cursor2@300M", g_s11_c2)):
    d, tot, px, py = marker_delta(s11, off, COL_S11, g)
    check("s11: %s marker at P" % tag, d >= 15 and tot >= 40,
          "P=(%.1f,%.1f) new=%d boxtot=%d" % (px, py, d, tot))
for tag, g, w in (("s_1_1 readout c1", g_s11_c1, 1), ("s_1_1 readout c2", g_s11_c2, 2)):
    d, px, py = readout_delta(s11, off, COL_S11, g, w)
    check("s11: %s" % tag, d >= 40, "new-band-px=%d (near P=(%.1f,%.1f))" % (d, px, py))
# ---- toggle (enabled then disabled) must match the control: no residue ----
for tag, c, g in (("s_2_1 cursor1 marker", COL_S21, g_s21_c1),
                  ("Zin     cursor1 marker", COL_ZIN, g_s11_c1),
                  ("s_2_1 cursor2 marker", COL_S21, g_s21_c2),
                  ("Zin     cursor2 marker", COL_ZIN, g_s11_c2)):
    d, tot, px, py = marker_delta(tgl, off, c, g)
    check("toggle: %s absent" % tag, d < 15,
          "new=%d boxtot=%d at P=(%.1f,%.1f)" % (d, tot, px, py))
mt = (near(tgl, COL_GRID, 14) & ~near(off, COL_GRID, 14))
nt = int(mt[fy:fy+40, fx:fx+240].sum())
check("toggle: f-labels absent", nt < 15, "top-left new-GRID-px=%d" % nt)

if bad:
    print("%d check(s) FAILED" % bad)
    sys.exit(1)
print("PASS: all Smith cursor-marker checks passed")
sys.exit(0)
PY

pyrc=$?
echo "--- cursor_on.log ---"
cat "$RES/cursor_on.log" 2>/dev/null | head -8
echo

# ================= summary ================
if [ "$pyrc" -eq 0 ]; then
  echo "ALL PASS (cursor markers dot/cross at shared frequency, readouts, f labels, no-cursor + toggle controls)"
  exit 0
else
  echo "FAIL (analysis rc=$pyrc)"
  exit 1
fi
