#!/bin/bash
#  File: drag_check.sh
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
#
#  Smith-chart cursor DRAG test (src/callback.c waves_callback, mode-3 gated).
#  Proves the 'a'/cursor1 drag works end-to-end: press on cursor1's marker,
#  drag to a DIFFERENT trace point, release -> the cursor snaps to the grabbed
#  point's swept frequency. Fully headless: Xvfb + the C event dispatcher
#  `xschem callback .drw <event> <px> <py> 0 <button> 0 <state>` -- the very
#  dispatcher the Tk bindings call (no XTest / pointer-warp needed).
#    event: ButtonPress=4, MotionNotify=6, ButtonRelease=5; Button1Mask=0x100=256
#  Two xschem runs share one deterministic CAPBOX geometry:
#    Run A : cursor1 @ 100 MHz -> before.png (self-calibrate the plane C, R)
#    py1   : fit the unit circle; parse the raw; pick the sweep point nearest
#            TARGET_MHZ (F_t, a mid-radius s_2_1 point off the grid ring);
#            compute CAPBOX-px of cursor1's s_2_1 marker @100 MHz and @F_t,
#            convert to xschem units; write coords + data
#    Run B : re-setup cursor1 @100 MHz (unselected -> CAPBOX capture); read
#            live zoom/origin; capture beforeB; SELECT the graph (waves_selected
#            -> graph_master) so the drag routes; inject ButtonPress(on marker)
#            -> Motion(Button1Mask, on F_t point) -> ButtonRelease; read back
#            cursor1_x (before/after); UNselect; capture after.png
#    py2   : PRIMARY numeric -- the read-back cursor1_x moved toward / snapped
#            to F_t; SECONDARY pixel -- cursor1's s_2_1 marker dot physically
#            left P(100 MHz) and is present at P(read-back) (differential over
#            the trace, so it is independent of local trace density).
#  Usage: ./drag_check.sh [xschem-binary]  Needs: Xvfb, python3+numpy+PIL.
#  Overrides: SMITH_DISP, TARGET_MHZ.  Exit: 0=PASS 1=FAIL 2=setup.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
SCH="$REPO/tests/SC_Test.sch"
BASE_RAW="$REPO/tests/SC_Test.raw"
CAPBOX="2000 1600 -1000 -800 1000 800"
FC1="100e6"        # cursor1 start frequency (inside the 50k..600M sweep)
# grab target: a sweep point NEAR TARGET_MHZ where the s_2_1 marker is mid-radius
# (|Gamma| well inside the unit circle). s_2_1 hugs the ring above ~40 MHz
# (|Gamma|>0.9), where its marker dot is occluded by the grid ring; ~30 MHz puts
# the dot ~0.37 in radius -> a clean, detectable marker and a large inward move.
TARGET_MHZ="${TARGET_MHZ:-30}"

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$SCH" ]    || { echo "schematic not found: $SCH" >&2; exit 2; }
[ -f "$BASE_RAW" ] || { echo "base raw not found: $BASE_RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import numpy" >/dev/null 2>&1 || { echo "python3 numpy required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 PIL required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/drag_*.png "$RES"/drag_*.log "$RES"/drag_*.stderr \
      "$RES"/drag_*.stdout "$RES"/drag_capA.tcl "$RES"/drag_capB.tcl \
      "$RES"/drag_coords.txt "$RES"/drag_data.txt "$RES"/xauth.$$

# --- pointer-free X display: pick a free display; start Xvfb if the socket is
# --- missing (same logic as cursor_check.sh)
DISP="${SMITH_DISP:-}"
if [ -z "$DISP" ]; then
  for cand in 99 98 97 96 95 94; do
    if [ ! -e "/tmp/.X11-unix/X$cand" ]; then DISP=":$cand"; break; fi
  done
  [ -n "$DISP" ] || DISP=:99
fi
SOCK="/tmp/.X11-unix/X${DISP#:}"
XVFB_PID=""; OURED=0
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

# ================= Run A: cursor1 @ FC1, render before.png =================
# One fresh process (an imp() trace in a large raw can otherwise leave xschem
# unable to exit). Cursor state driven by the env: cursor1 enabled @ FC1.
cat > "$RES/drag_capA.tcl" <<'EOF'
set outch [open $env(OUTLOG) w]
set capbox [split $env(CAPBOX) " "]
xschem raw read $env(BASE_RAW)
xschem cursor 1 1
xschem set cursor1_x $env(FC1)
# UNSELECTED: `print png` uses the explicit CAPBOX only when lastsel==0
xschem select rect 2 0 clear nodraw
update; update; after 50; update
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# warm-up (sizes the canvas)
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# real capture (CAPBOX)
foreach c $tctx::colors { puts $outch "COL $c" }
puts $outch "CAP_DONE A"
close $outch
xschem exit closewindow force 0
EOF

run_capA() {
  local png="$RES/drag_before.png"
  for t in 1 2 3; do
    rm -f "$png"
    OUTPNG="$png" OUTLOG="$RES/drag_A.log" BASE_RAW="$BASE_RAW" \
    FC1="$FC1" CAPBOX="$CAPBOX" \
      timeout 90 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/drag_capA.tcl" </dev/null \
      >"$RES/drag_A.stdout" 2>"$RES/drag_A.stderr"
    [ -s "$png" ] && return 0
    echo "drag A attempt $t/3 failed"; sleep 2
  done
  echo "FAIL: drag A capture produced no png"
  tail -15 "$RES/drag_A.stderr" 2>/dev/null
  return 1
}
run_capA || exit 1

# no validation-error lines may appear (rejected/failed wave)
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES"/drag_A.stderr 2>/dev/null; then
  echo "FAIL: validation error in Run A stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" "$RES"/drag_A.stderr
  exit 1
fi

# ================= python1: calibrate C,R + pick F_t + write xschem coords ==
python3 - "$RES" "$BASE_RAW" "$TARGET_MHZ" "$FC1" <<'PY'
import sys
import struct
import numpy as np
from PIL import Image

RES, BASE_RAW, TARGET_MHZ, FC1 = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4])
def load(fn): return np.asarray(Image.open(fn).convert("RGB")).astype(int)

# color table (COL lines from Run A log)
colors = []
with open(RES + "/drag_A.log") as f:
    for line in f:
        if line.startswith("COL "):
            colors.append(line.strip()[4:].lstrip("#"))
if len(colors) < 13:
    print("FAIL: color table not found in drag_A.log"); sys.exit(1)
def rgb(i):
    c = colors[i]
    return np.array([int(c[0:2],16), int(c[2:4],16), int(c[4:6],16)], int)
COL_S11, COL_S21, COL_ZIN, COL_GRID = rgb(4), rgb(12), rgb(9), rgb(2)

before = load(RES + "/drag_before.png")
def near(img, c, tol=30): return (np.abs(img - c).max(axis=2) <= tol)
if int(near(before, COL_S21).sum()) < 200:
    print("FAIL: no s_2_1 trace in before.png (%d px)" % int(near(before, COL_S21).sum()))
    sys.exit(1)

# ---- self-calibrate C, R from the unit circle (GRIDLAYER) in before.png ----
# container 600,-440..1000,-40 -> CAPBOX px x[1600,2000] y[360,760] (scale 1)
X0, X1, Y0, Y1 = 1600, 2000, 360, 760
gm = near(before, COL_GRID, 14)
gm[:Y0+8, :]=0; gm[Y1-8:, :]=0; gm[:, :X0+8]=0; gm[:, X1-8:]=0
ys, xs = np.nonzero(gm)
if len(xs) < 200:
    print("FAIL: too few GRIDLAYER pixels to calibrate (%d)" % len(xs)); sys.exit(1)
C0 = np.array([(X0 + X1)/2.0, (Y0 + Y1)/2.0])
rad = np.hypot(xs - C0[0], ys - C0[1])
hist, edges = np.histogram(rad, bins=90, range=(0.0, 270.0))
strong = [i for i in range(len(hist)) if hist[i] >= 150]
if not strong:
    print("FAIL: no strong ring found in the grid"); sys.exit(1)
pk = max(strong); rp = (edges[pk] + edges[pk + 1]) / 2.0
sel = (rad >= rp - 6.0) & (rad <= rp + 6.0)
pts = np.stack([xs[sel], ys[sel]], axis=1).astype(float)
def fit_circle(p):
    x = p[:,0]; y = p[:,1]
    A = np.column_stack([2*x, 2*y, np.ones_like(x)]); b = x*x + y*y
    s, *_ = np.linalg.lstsq(A, b, rcond=None)
    cx, cy = s[0], s[1]
    return cx, cy, float(np.sqrt(s[2] + cx*cx + cy*cy))
Cx, Cy, R = fit_circle(pts)
for _ in range(6):
    d = np.abs(np.hypot(pts[:,0]-Cx, pts[:,1]-Cy) - R)
    keep = pts[d <= 2.0]
    if len(keep) < 50: break
    Cx, Cy, R = fit_circle(keep)
if not (80.0 <= R <= 600.0):
    print("FAIL: calibrated radius R=%.1f px implausible" % R); sys.exit(1)
print("CALIB  C=(%.2f, %.2f)  R=%.2f px" % (Cx, Cy, R))

# ---- expected Gamma: parse the SP raw, exactly like draw.c ----
def parse_raw(fn):
    data = open(fn, "rb").read()
    i = data.find(b"No. Variables:"); nv = int(data[i:data.find(b"\n", i)].split(b":")[1].strip())
    i = data.find(b"No. Points:"); npts = int(data[i:data.find(b"\n", i)].split(b":")[1].strip())
    vs = data.find(b"Variables:"); vb = data[vs:data.find(b"Binary:", vs)]
    names = []
    for ln in vb.split(b"\n")[1:]:
        parts = [p for p in ln.decode("latin1").split("\t") if p.strip() != ""]
        if len(parts) >= 2:
            names.append((int(parts[0]), parts[1]))
    off_b = data.find(b"Binary:", vs) + len(b"Binary:")
    while data[off_b:off_b + 1] in (b"\n", b"\r"):
        off_b += 1
    nper = 2 * nv
    freq = []; vals = {}
    for p in range(npts):
        row = struct.unpack_from("<%dd" % nper, data, off_b + p * nper * 8)
        freq.append(row[0])
        for k in range(1, len(names)):
            idx, name = names[k]
            vals.setdefault(name, []).append((row[2 * k], row[2 * k + 1]))
    return np.array(freq), vals

freq, vals = parse_raw(BASE_RAW)
if "s_1_1" not in vals or "s_2_1" not in vals:
    print("FAIL: raw parse failed (vars=%s)" % sorted(vals)); sys.exit(1)
if freq[0] <= 0 or freq[-1] <= freq[0]:
    print("FAIL: raw parse failed: frequency axis broken"); sys.exit(1)
print("RAW    %d points, freq %.3g..%.3g" % (len(freq), freq[0], freq[-1]))

# pick the ACTUAL sweep point nearest TARGET_MHZ (no lerp -> the snap target)
i_t = int(np.argmin(np.abs(freq - TARGET_MHZ * 1e6)))
ft = float(freq[i_t])
def gamma_lerp(name, fc):
    v = vals[name]
    for p in range(len(freq) - 1):
        if freq[p] <= fc <= freq[p + 1]:
            t = (fc - freq[p]) / (freq[p + 1] - freq[p]); a, b = v[p], v[p + 1]
            return (a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1]))
    raise SystemExit("FAIL: freq %.3e outside sweep span" % fc)
def gamma_idx(name, i):
    re_, im_ = vals[name][i]
    return (float(re_), float(im_))

g_s21_b = gamma_lerp("s_2_1", 100e6)   # start (cursor1 @ 100 MHz)
g_s11_b = gamma_lerp("s_1_1", 100e6)
g_s21_t = gamma_idx("s_2_1", i_t)      # target (the grabbed point)
g_s11_t = gamma_idx("s_1_1", i_t)
for tag, g in (("s_2_1@100M", g_s21_b), ("s_1_1@100M", g_s11_b),
               ("s_2_1@F_t", g_s21_t), ("s_1_1@F_t", g_s11_t)):
    if abs(complex(g[0], g[1])) >= 1.0:
        print("FAIL: %s has |Gamma|=%.3f outside the unit circle" % (tag, abs(complex(g[0], g[1]))))
        sys.exit(1)
print("TARGET F_t=%.6g Hz  (sweep idx %d)" % (ft, i_t))
dist = np.hypot(g_s21_t[0] - g_s21_b[0], g_s21_t[1] - g_s21_b[1])
if dist < 0.05:
    print("FAIL: s_2_1 target too close to start (%.3f Gamma units)" % dist); sys.exit(1)
# the target marker must be off the unit ring, else its dot is occluded by the
# grid ring and the pixel check below becomes unreliable
if abs(complex(g_s21_t[0], g_s21_t[1])) > 0.93:
    print("FAIL: s_2_1 target |Gamma|=%.3f too close to the unit ring (marker "
          "occluded). pick a lower TARGET_MHZ for a mid-radius target" %
          abs(complex(g_s21_t[0], g_s21_t[1]))); sys.exit(1)
print("s_2_1 @100M=(%.4f,%.4f)  @F_t=(%.4f,%.4f)  move=%.3f Gamma"
      % (g_s21_b[0], g_s21_b[1], g_s21_t[0], g_s21_t[1], dist))

# P(g): CAPBOX px of a plane point (identical formula to cursor_check.sh)
def P(g): return (Cx + g[0] * R, Cy - g[1] * R)
# CAPBOX px -> xschem units: CAPBOX spans xschem x[-1000,1000], y[-800,800] at
# scale 1, so  xs = px - 1000, ys = py - 800 (exact: zoom_box + origin)
def to_xschem(px, py): return (px - 1000.0, py - 800.0)
PB21, PT21 = P(g_s21_b), P(g_s21_t)
PB11, PT11 = P(g_s11_b), P(g_s11_t)
m_x, m_y = to_xschem(*PB21)   # marker (s_2_1 @ 100 MHz) to grab
t_x, t_y = to_xschem(*PT21)   # target (s_2_1 @ F_t) to drop on
with open(RES + "/drag_coords.txt", "w") as f:
    f.write("%.6f %.6f %.6f %.6f\n" % (m_x, m_y, t_x, t_y))
with open(RES + "/drag_data.txt", "w") as f:
    f.write("CALIB %.6f %.6f %.6f\n" % (Cx, Cy, R))
    f.write("FT %.6f\n" % ft)
    f.write("FC1 %.6f\n" % FC1)
    f.write("COL_S21 %s\n" % colors[12].lstrip("#"))
    f.write("COL_S11 %s\n" % colors[4].lstrip("#"))
    f.write("PB21 %.4f %.4f\n" % (PB21[0], PB21[1]))
    f.write("PT21 %.4f %.4f\n" % (PT21[0], PT21[1]))
    f.write("PB11 %.4f %.4f\n" % (PB11[0], PB11[1]))
    f.write("PT11 %.4f %.4f\n" % (PT11[0], PT11[1]))
print("coords: marker CAPBOX(%.1f,%.1f) target CAPBOX(%.1f,%.1f)" % (PB21[0], PB21[1], PT21[0], PT21[1]))
PY
py1=$?
echo "--- Run A log ---"; cat "$RES/drag_A.log" 2>/dev/null | grep -vE '^COL' | head -8; echo
if [ "$py1" -ne 0 ]; then echo "FAIL: python1 calibration (rc=$py1)"; exit 1; fi

# ================= Run B: inject the drag, render after.png =================
# Re-setup cursor1 @ FC1 (fresh process), read the LIVE zoom/origin, convert
# the xschem target to widget-px, then drive the C event dispatcher:
#   ButtonPress(4) on cursor1's s_2_1@FC1 marker  -> start move (within 10 px)
#   MotionNotify(6) w/ Button1Mask(256) on the s_2_1@F_t point -> snap
#   ButtonRelease(5)
cat > "$RES/drag_capB.tcl" <<'EOF'
set outch [open $env(OUTLOG) w]
proc say {ch s} { puts $ch $s; flush $ch }
set capbox [split $env(CAPBOX) " "]
xschem raw read $env(BASE_RAW)
xschem cursor 1 1
xschem set cursor1_x $env(FC1)
# UNSELECTED -> `print png` honours the explicit CAPBOX box (lastsel==0)
xschem select rect 2 0 clear nodraw
update; update; after 50; update
xschem draw_graph 0
# capture the "before" state (cursor1 @ FC1) IN THIS PROCESS for a clean diff
xschem print png $env(BEFOREB) {*}$capbox   ;# warm-up (sizes the canvas)
xschem draw_graph 0
xschem print png $env(BEFOREB) {*}$capbox   ;# real capture (before the drag)
say $outch "BEFOREB_DONE"
# live transform (normal view, after the before capture restored it)
set zoom [xschem get zoom]
set xorg [xschem get xorigin]
set yorg [xschem get yorigin]
set mooz [expr {1.0 / $zoom}]
say $outch "TRANSFORM zoom=$zoom xorg=$xorg yorg=$yorg mooz=$mooz"
set fh [open $env(COORDS) r]
set c [string trim [read $fh]]; close $fh
set c [split $c " "]
set mxs [lindex $c 0]; set mys [lindex $c 1]
set txs [lindex $c 2]; set tys [lindex $c 3]
set mpx [expr {int(($mxs + $xorg) * $mooz + 0.5)}]
set mpy [expr {int(($mys + $yorg) * $mooz + 0.5)}]
set tpx [expr {int(($txs + $xorg) * $mooz + 0.5)}]
set tpy [expr {int(($tys + $yorg) * $mooz + 0.5)}]
say $outch "WIDGET marker=($mpx,$mpy) target=($tpx,$tpy)"
# SELECT the graph so waves_selected() sets graph_master and the drag routes
# to waves_callback (a locked/selected graph is not skipped).
xschem select rect 2 0 nodraw
update
say $outch "CURSOR_BEFORE [xschem get cursor1_x]"
xschem callback .drw 4 $mpx $mpy 0 1 0 0   ;# ButtonPress: grab cursor1 marker
update
xschem callback .drw 6 $tpx $tpy 0 0 0 256 ;# Motion + Button1Mask: snap
update
xschem callback .drw 5 $tpx $tpy 0 1 0 0   ;# ButtonRelease
update; update
say $outch "CURSOR_AFTER [xschem get cursor1_x]"
say $outch "DRAG_DONE"
# UNselect so the CAPBOX capture uses the explicit box again
xschem select rect 2 0 clear nodraw
update
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# warm-up (sizes the canvas)
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# real capture (after the drag)
foreach cc $tctx::colors { puts $outch "COL $cc" }
puts $outch "CAP_DONE B"
close $outch
xschem exit closewindow force 0
EOF

run_capB() {
  local png="$RES/drag_after.png" beforeb="$RES/drag_beforeB.png"
  for t in 1 2 3; do
    rm -f "$png" "$beforeb"
    OUTPNG="$png" BEFOREB="$beforeb" OUTLOG="$RES/drag_B.log" \
    COORDS="$RES/drag_coords.txt" BASE_RAW="$BASE_RAW" FC1="$FC1" CAPBOX="$CAPBOX" \
      timeout 90 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/drag_capB.tcl" </dev/null \
      >"$RES/drag_B.stdout" 2>"$RES/drag_B.stderr"
    [ -s "$png" ] && [ -s "$beforeb" ] && return 0
    echo "drag B attempt $t/3 failed"; sleep 2
  done
  echo "FAIL: drag B capture produced no png"
  tail -15 "$RES/drag_B.stderr" 2>/dev/null
  return 1
}
run_capB || exit 1

# no validation-error lines may appear (rejected/failed wave)
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES"/drag_B.stderr 2>/dev/null; then
  echo "FAIL: validation error in Run B stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" "$RES"/drag_B.stderr
  exit 1
fi

# ================= python2: verify the cursor snapped (numeric + pixel) =====
# The C code snaps cursor1 to the sweep point NEAREST the mouse, so we read the
# result back (xschem get cursor1_x) and verify BOTH that it moved toward the
# aimed F_t (numeric, primary) and that the s_2_1 marker dot physically moved
# with it (pixel, secondary). The pixel check is differential (dot bump over
# the trace) so it is independent of the local trace density.
python3 - "$RES" "$BASE_RAW" "$RES/drag_B.log" <<'PY'
import sys
import struct
import numpy as np
from PIL import Image

RES, BASE_RAW, LOG = sys.argv[1], sys.argv[2], sys.argv[3]
def load(fn): return np.asarray(Image.open(fn).convert("RGB")).astype(int)

data = {}
with open(RES + "/drag_data.txt") as f:
    for line in f:
        p = line.split()
        if p: data[p[0]] = p[1:]
def hx(s): return np.array([int(s[0:2],16), int(s[2:4],16), int(s[4:6],16)], int)
COL_S21 = hx(data["COL_S21"][0])
Cx, Cy, R = (float(data["CALIB"][0]), float(data["CALIB"][1]), float(data["CALIB"][2]))
FT, FC1 = float(data["FT"][0]), float(data["FC1"][0])
def P(g): return (Cx + g[0]*R, Cy - g[1]*R)

# read back the actual cursor1_x before/after the drag (ground truth)
def read_cursor(key):
    with open(LOG) as f:
        for line in f:
            if line.startswith(key):
                return float(line.strip().split()[1])
    return None
cb = read_cursor("CURSOR_BEFORE")
ca = read_cursor("CURSOR_AFTER")
if cb is None or ca is None:
    print("FAIL: CURSOR_BEFORE/AFTER not found in drag_B.log"); sys.exit(1)
print("CURSOR  before=%.6g Hz  after=%.6g Hz   (aimed F_t=%.6g Hz)" % (cb, ca, FT))

# parse the SP raw (identical to python1 / draw.c) to get Gamma(freq)
def parse_raw(fn):
    d = open(fn, "rb").read()
    i = d.find(b"No. Variables:"); nv = int(d[i:d.find(b"\n", i)].split(b":")[1].strip())
    i = d.find(b"No. Points:"); npts = int(d[i:d.find(b"\n", i)].split(b":")[1].strip())
    vs = d.find(b"Variables:"); vb = d[vs:d.find(b"Binary:", vs)]
    names = []
    for ln in vb.split(b"\n")[1:]:
        parts = [p for p in ln.decode("latin1").split("\t") if p.strip() != ""]
        if len(parts) >= 2:
            names.append((int(parts[0]), parts[1]))
    off = d.find(b"Binary:", vs) + len(b"Binary:")
    while d[off:off+1] in (b"\n", b"\r"): off += 1
    nper = 2*nv; freq = []; vals = {}
    for p in range(npts):
        row = struct.unpack_from("<%dd" % nper, d, off + p*nper*8)
        freq.append(row[0])
        for k in range(1, len(names)):
            idx, nm = names[k]
            vals.setdefault(nm, []).append((row[2*k], row[2*k+1]))
    return np.array(freq), vals
freq, vals = parse_raw(BASE_RAW)
def gamma(name, fc):
    v = vals[name]
    for p in range(len(freq)-1):
        if freq[p] <= fc <= freq[p+1]:
            t = (fc - freq[p])/(freq[p+1]-freq[p]); a, b = v[p], v[p+1]
            return (a[0]+t*(b[0]-a[0]), a[1]+t*(b[1]-a[1]))
    raise SystemExit("FAIL: freq %.3e outside sweep span" % fc)

# CAPBOX px where the s_2_1 marker dot actually sat before / after the drag
PB = P(gamma("s_2_1", cb))   # start (cursor @ FC1)
PA = P(gamma("s_2_1", ca))   # snapped-to (the readback value)
print("marker CAPBOX  before=(%.1f,%.1f)  after=(%.1f,%.1f)" % (PB[0], PB[1], PA[0], PA[1]))

before = load(RES + "/drag_beforeB.png")   # both from Run B -> same process
after  = load(RES + "/drag_after.png")
def near(img, c, tol=30): return (np.abs(img - c).max(axis=2) <= tol)
def blob(img, c, px, py, half=6):
    x0, y0 = int(px-half), int(py-half); x1, y1 = int(px+half)+1, int(py+half)+1
    return int(near(img, c)[max(0,y0):y1, max(0,x0):x1].sum())

bad = 0
def check(label, ok, detail):
    global bad
    print("%s: %-46s %s" % ("PASS" if ok else "FAIL", label, detail))
    if not ok: bad += 1

# ---- numeric (primary): the snap moved cursor1 toward the aimed F_t ----
SPAN = abs(FT - FC1)
check("cursor1 moved at all", abs(ca - cb) > 1e6, "before=%.6g after=%.6g" % (cb, ca))
check("cursor1 moved toward F_t", abs(ca - FT) < abs(cb - FT),
      "|after-F_t|=%.6g < |before-F_t|=%.6g" % (abs(ca-FT), abs(cb-FT)))
check("cursor1 snapped near F_t", abs(ca - FT) < 0.2*SPAN,
      "after=%.6g, |after-F_t|=%.6g (< %.6g)" % (ca, abs(ca-FT), 0.2*SPAN))

# ---- pixel (secondary): the s_2_1 marker dot physically moved with it ----
# the trace passes through both points; the filled dot adds a fixed bump over
# the 2-3 px trace. The differential removes the (location-dependent) trace.
b_pb = blob(before, COL_S21, *PB)   # before @ start : dot + trace
b_pa = blob(before, COL_S21, *PA)   # before @ end   : trace only
a_pb = blob(after,  COL_S21, *PB)   # after  @ start : trace only
a_pa = blob(after,  COL_S21, *PA)   # after  @ end   : dot + trace
print("s_2_1 px   before@start=%3d  before@end=%3d  after@start=%3d  after@end=%3d"
      % (b_pb, b_pa, a_pb, a_pa))
DOT = 10   # observed dot bump is ~20 px; require at least half
check("s_2_1 dot at start in before / gone in after", b_pb - a_pb >= DOT,
      "bump=%.0f" % (b_pb - a_pb))
check("s_2_1 dot at end in after (absent in before)", a_pa - b_pa >= DOT,
      "bump=%.0f" % (a_pa - b_pa))

if bad:
    print("%d check(s) FAILED" % bad)
    sys.exit(1)
print("PASS: Smith cursor1 drag snapped %.6g Hz -> %.6g Hz under the mouse" % (cb, ca))
PY
py2=$?
echo "--- Run B log (transform + widget coords) ---"
grep -E "TRANSFORM|WIDGET|CURSOR|DRAG_DONE|CAP_DONE" "$RES/drag_B.log" 2>/dev/null
echo
# ================= summary ================
if [ "$py2" -eq 0 ]; then
  echo "ALL PASS (Smith cursor1 drag: press on marker, snap to grabbed point's frequency)"
  exit 0
else
  echo "FAIL (analysis rc=$py2)"
  exit 1
fi
