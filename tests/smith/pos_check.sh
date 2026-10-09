#!/bin/bash
#  File: pos_check.sh
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
#  Smith-chart button-3 "Pos:" prompt headless test (src/callback.c, mode-3 gated).
#  On SC_Test with cursor1 enabled, proves that a button-3 press within 10 px of a
#  cursor marker opens the input_line "Pos:" dialog and that the entered frequency
#  is applied to the cursor:
#    Run A : cursor1 @ 100 MHz -> before.png (self-calibrate the plane C, R)
#    py1   : fit the unit circle; parse the raw; compute CAPBOX-px of the shared
#            s_1_1/Zin marker @ FC1 (start, |Gamma|~0.42 -> a clean mid-radius
#            dot; Zin is drawn last, so the visible dot is ZIN color) and @ NEWF
#            (end, |Gamma|~0.21); convert start to xschem units; write coords + data
#    Run B : re-setup cursor1 @ FC1 (unselected -> CAPBOX capture); read
#            live zoom/origin; capture beforeB; schedule a Tcl after-handler;
#            inject ButtonPress button-3 + Button3Mask(512) on the start marker
#            with `xschem callback .drw 4 <px> <py> 0 3 0 512`. The C code opens
#            the modal .dialog (input_line) and blocks in tkwait; the after-
#            handler is pumped by that tkwait: it records the pre-filled preset,
#            types NEWF and clicks OK (exactly like a user would). Then read back
#            cursor1_x, the dialog-closed state, the hilight_wave prop and
#            capture after.png.
#    py2   : PRIMARY numeric -- cursor1_x == NEWF; the prompt was pre-filled with
#            the current cursor value; the dialog closed; no hilight_wave (a
#            marker click must not hilight a trace). SECONDARY pixel -- the Zin
#            marker dot physically left P(FC1) and is present at P(read-back)
#            (differential over the trace, so independent of local trace density).
#  Usage: ./pos_check.sh [xschem-binary]  Needs: Xvfb, python3+numpy+PIL.
#  Overrides: SMITH_DISP, NEWF.  Exit: 0=PASS 1=FAIL 2=setup.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
SCH="$REPO/tests/SC_Test.sch"
BASE_RAW="$REPO/tests/SC_Test.raw"
CAPBOX="2000 1600 -1000 -800 1000 800"
FC1="100e6"         # cursor1 start frequency (inside the 50k..600M sweep)
NEWF="${NEWF:-200e6}"  # the frequency typed into the Pos: prompt

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$SCH" ]    || { echo "schematic not found: $SCH" >&2; exit 2; }
[ -f "$BASE_RAW" ] || { echo "base raw not found: $BASE_RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import numpy" >/dev/null 2>&1 || { echo "python3 numpy required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 PIL required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/pos_*.png "$RES"/pos_*.log "$RES"/pos_*.stderr \
      "$RES"/pos_*.stdout "$RES"/pos_capA.tcl "$RES"/pos_capB.tcl \
      "$RES"/pos_coords.txt "$RES"/pos_data.txt "$RES"/xauth.$$

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
cat > "$RES/pos_capA.tcl" <<'EOF'
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
  local png="$RES/pos_before.png"
  for t in 1 2 3; do
    rm -f "$png"
    OUTPNG="$png" OUTLOG="$RES/pos_A.log" BASE_RAW="$BASE_RAW" \
    FC1="$FC1" CAPBOX="$CAPBOX" \
      timeout 90 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/pos_capA.tcl" </dev/null \
      >"$RES/pos_A.stdout" 2>"$RES/pos_A.stderr"
    [ -s "$png" ] && return 0
    echo "pos A attempt $t/3 failed"; sleep 2
  done
  echo "FAIL: pos A capture produced no png"
  tail -15 "$RES/pos_A.stderr" 2>/dev/null
  return 1
}
run_capA || exit 1

# no validation-error lines may appear (rejected/failed wave)
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES"/pos_A.stderr 2>/dev/null; then
  echo "FAIL: validation error in Run A stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" "$RES/pos_A.stderr"
  exit 1
fi
# ================= python1: calibrate C,R + write marker xschem coords ========
python3 - "$RES" "$BASE_RAW" "$NEWF" "$FC1" <<'PY'
import sys
import struct
import numpy as np
from PIL import Image

RES, BASE_RAW, NEWF, FC1 = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4])
def load(fn): return np.asarray(Image.open(fn).convert("RGB")).astype(int)

# color table (COL lines from Run A log)
colors = []
with open(RES + "/pos_A.log") as f:
    for line in f:
        if line.startswith("COL "):
            colors.append(line.strip()[4:].lstrip("#"))
if len(colors) < 13:
    print("FAIL: color table not found in pos_A.log"); sys.exit(1)
def rgb(i):
    c = colors[i]
    return np.array([int(c[0:2],16), int(c[2:4],16), int(c[4:6],16)], int)
COL_S11, COL_ZIN, COL_GRID = rgb(4), rgb(9), rgb(2)

before = load(RES + "/pos_before.png")
def near(img, c, tol=30): return (np.abs(img - c).max(axis=2) <= tol)
if int(near(before, COL_S11).sum()) < 200:
    print("FAIL: no s_1_1 trace in before.png (%d px)" % int(near(before, COL_S11).sum()))
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

# the shared s_1_1/Zin marker must sit mid-radius at both frequencies, else
# its dot is occluded by the unit ring and the pixel check is unreliable
def gamma_lerp(name, fc):
    v = vals[name]
    for p in range(len(freq) - 1):
        if freq[p] <= fc <= freq[p + 1]:
            t = (fc - freq[p]) / (freq[p + 1] - freq[p]); a, b = v[p], v[p + 1]
            return (a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1]))
    raise SystemExit("FAIL: freq %.3e outside sweep span" % fc)
g_s11_b = gamma_lerp("s_1_1", FC1)   # start marker position
g_s11_n = gamma_lerp("s_1_1", NEWF)  # end marker position
for tag, g in (("s_1_1@start", g_s11_b), ("s_1_1@end", g_s11_n)):
    mag = abs(complex(g[0], g[1]))
    if mag >= 1.0:
        print("FAIL: %s has |Gamma|=%.3f outside the unit circle" % (tag, mag)); sys.exit(1)
    if mag > 0.93:
        print("FAIL: %s |Gamma|=%.3f too close to the unit ring (marker "
              "occluded); pick a NEWF with a mid-radius s_1_1 point" % (tag, mag))
        sys.exit(1)
print("s_1_1 @start=(%.4f,%.4f)  @end=(%.4f,%.4f)" % (g_s11_b[0], g_s11_b[1], g_s11_n[0], g_s11_n[1]))

# P(g): CAPBOX px of a plane point (identical formula to cursor_check.sh)
def P(g): return (Cx + g[0] * R, Cy - g[1] * R)
# CAPBOX px -> xschem units: CAPBOX spans xschem x[-1000,1000], y[-800,800] at
# scale 1, so  xs = px - 1000, ys = py - 800 (exact: zoom_box + origin)
def to_xschem(px, py): return (px - 1000.0, py - 800.0)
PB11, PN11 = P(g_s11_b), P(g_s11_n)
m_x, m_y = to_xschem(*PB11)   # start marker to press button-3 on
with open(RES + "/pos_coords.txt", "w") as f:
    f.write("%.6f %.6f\n" % (m_x, m_y))
with open(RES + "/pos_data.txt", "w") as f:
    f.write("CALIB %.6f %.6f %.6f\n" % (Cx, Cy, R))
    f.write("NEWF %.6f\n" % NEWF)
    f.write("FC1 %.6f\n" % FC1)
    f.write("COL_ZIN %s\n" % colors[9].lstrip("#"))
    f.write("COL_S11 %s\n" % colors[4].lstrip("#"))
    f.write("PB11 %.4f %.4f\n" % (PB11[0], PB11[1]))
    f.write("PN11 %.4f %.4f\n" % (PN11[0], PN11[1]))
print("coords: start marker CAPBOX(%.1f,%.1f)  end CAPBOX(%.1f,%.1f)"
      % (PB11[0], PB11[1], PN11[0], PN11[1]))
PY
py1=$?
echo "--- Run A log ---"; cat "$RES/pos_A.log" 2>/dev/null | grep -vE '^COL' | head -8; echo
if [ "$py1" -ne 0 ]; then echo "FAIL: python1 calibration (rc=$py1)"; exit 1; fi
# ================= Run B: inject button-3, drive the Pos: dialog =============
# Re-setup cursor1 @ FC1 (fresh process), read the LIVE zoom/origin, convert the
# xschem start marker to widget-px, then drive the C event dispatcher:
#   ButtonPress(4) button-3 + Button3Mask(512) on cursor1's marker -> Pos: prompt
# The prompt is the modal input_line() dialog (.dialog); input_line blocks in
# `tkwait window .dialog`. The scheduled after-handler below is pumped by that
# tkwait: it records the pre-filled preset, types NEWF and clicks OK.
cat > "$RES/pos_capB.tcl" <<'EOF'
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
xschem print png $env(BEFOREB) {*}$capbox   ;# real capture (before the prompt)
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
set mpx [expr {int(($mxs + $xorg) * $mooz + 0.5)}]
set mpy [expr {int(($mys + $yorg) * $mooz + 0.5)}]
say $outch "WIDGET marker=($mpx,$mpy)"
say $outch "CURSOR_BEFORE [xschem get cursor1_x]"
# Drive the modal Pos: dialog from the event loop: while input_line() blocks
# in tkwait, this handler types NEWF into the entry and clicks OK, exactly like
# a user would. It retries until the dialog exists so it can never fire before
# the dialog is built (and must not destroy anything if the prompt never opens).
# IMPORTANT: read $env(NEWF) into a global HERE (top level). Accessing $env(...)
# inside the after-handler hangs the process: the handler runs while the C
# `xschem callback` command is still on the Tcl eval stack (input_line was
# entered via tclvareval), and $env lookups in that re-entrant context never
# return. Plain globals and widget commands are safe in the handler.
set ::newf $env(NEWF)
set ::dlg_preset "NO_DIALOG"
set ::dlg_retries 0
proc ::pos_drive_dialog {} {
  if {[winfo exists .dialog.f1.e]} {
    set ::dlg_preset [.dialog.f1.e get]
    .dialog.f1.e delete 0 end
    .dialog.f1.e insert 0 $::newf
    .dialog.f2.ok invoke
  } else {
    incr ::dlg_retries
    if {$::dlg_retries < 60} { after 50 ::pos_drive_dialog } ;# ~3 s of retries
  }
}
after 250 ::pos_drive_dialog
xschem callback .drw 4 $mpx $mpy 0 3 0 512   ;# ButtonPress: button-3 on marker
update
say $outch "CURSOR_AFTER [xschem get cursor1_x]"
say $outch "DIALOG_GONE [expr {1 - [winfo exists .dialog]}]"
say $outch "PRESET $::dlg_preset"
say $outch "HILIGHT_WAVE [xschem getprop rect 2 0 hilight_wave]"
say $outch "POS_DONE"
# UNselect-free capture: the cursor should now sit at NEWF
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# warm-up (sizes the canvas)
xschem draw_graph 0
xschem print png $env(OUTPNG) {*}$capbox   ;# real capture (after the prompt)
foreach cc $tctx::colors { puts $outch "COL $cc" }
puts $outch "CAP_DONE B"
close $outch
xschem exit closewindow force 0
EOF

run_capB() {
  local png="$RES/pos_after.png" beforeb="$RES/pos_beforeB.png"
  for t in 1 2 3; do
    rm -f "$png" "$beforeb"
    OUTPNG="$png" BEFOREB="$beforeb" OUTLOG="$RES/pos_B.log" \
    COORDS="$RES/pos_coords.txt" BASE_RAW="$BASE_RAW" FC1="$FC1" NEWF="$NEWF" CAPBOX="$CAPBOX" \
      timeout 90 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/pos_capB.tcl" </dev/null \
      >"$RES/pos_B.stdout" 2>"$RES/pos_B.stderr"
    [ -s "$png" ] && [ -s "$beforeb" ] && return 0
    echo "pos B attempt $t/3 failed"; sleep 2
  done
  echo "FAIL: pos B capture produced no png"
  tail -15 "$RES/pos_B.stderr" 2>/dev/null
  return 1
}
run_capB || exit 1

# no validation-error lines may appear (rejected/failed wave)
if grep -qiE "not supported|unbalanced expression|not a complex|no data found" \
      "$RES"/pos_B.stderr 2>/dev/null; then
  echo "FAIL: validation error in Run B stderr:"
  grep -iE "not supported|unbalanced expression|not a complex|no data found" "$RES/pos_B.stderr"
  exit 1
fi
# ================= python2: verify the prompt fired and applied the value ====
# The C code (mode-3, button-3 on a cursor marker) opens input_line "Pos:"
# pre-filled with the current cursor value; the OK click applies the typed
# frequency to cursor1. Ground truth is the read-back cursor1_x (primary);
# the marker-dot movement in the captures is secondary (differential over the
# trace, so independent of local trace density).
python3 - "$RES" "$BASE_RAW" "$RES/pos_B.log" <<'PY'
import sys
import struct
import numpy as np
from PIL import Image

RES, BASE_RAW, LOG = sys.argv[1], sys.argv[2], sys.argv[3]
def load(fn): return np.asarray(Image.open(fn).convert("RGB")).astype(int)

data = {}
with open(RES + "/pos_data.txt") as f:
    for line in f:
        p = line.split()
        if p: data[p[0]] = p[1:]
def hx(s): return np.array([int(s[0:2],16), int(s[2:4],16), int(s[4:6],16)], int)
COL_ZIN = hx(data["COL_ZIN"][0])
COL_S11 = hx(data["COL_S11"][0])
Cx, Cy, R = (float(data["CALIB"][0]), float(data["CALIB"][1]), float(data["CALIB"][2]))
NEWF, FC1 = float(data["NEWF"][0]), float(data["FC1"][0])

def read_field(key):
    with open(LOG) as f:
        for line in f:
            if line.startswith(key + " "):
                return line.rstrip("\n").split(" ", 1)[1]
            if line.rstrip("\n") == key:
                return ""
    return None

cb   = read_field("CURSOR_BEFORE")
ca   = read_field("CURSOR_AFTER")
pres = read_field("PRESET")
gone = read_field("DIALOG_GONE")
hil  = read_field("HILIGHT_WAVE")
if None in (cb, ca, pres, gone, hil):
    print("FAIL: CURSOR_BEFORE/AFTER, PRESET, DIALOG_GONE or HILIGHT_WAVE missing in pos_B.log")
    print(open(LOG).read()[:2000])
    sys.exit(1)
cb, ca = float(cb), float(ca)
print("CURSOR  before=%.6g Hz  after=%.6g Hz   (entered %.6g Hz)" % (cb, ca, NEWF))

bad = 0
def check(label, ok, detail):
    global bad
    print("%s: %-46s %s" % ("PASS" if ok else "FAIL", label, detail))
    if not ok: bad += 1

# ---- numeric (primary): the entered frequency was applied to cursor1 ----
check("cursor1 before == FC1", abs(cb - FC1) < 1e-6 * FC1, "before=%.6g" % cb)
check("cursor1 after == entered", abs(ca - NEWF) < 1e-9 * NEWF,
      "after=%.6g entered=%.6g" % (ca, NEWF))

# the Pos: prompt was pre-filled with the CURRENT cursor value. The preset is
# produced by dtoa_eng() (src/editprop.c), which appends an engineering suffix
# ("100MEG" = 100e6); parse it back the same way atof_eng() does.
import re
def parse_eng(s):
    s = s.strip()
    try:
        return float(s)                       # plain / scientific ("2e+08")
    except ValueError:
        pass
    m = re.match(r'^([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)([A-Za-z]*)$', s)
    if not m:
        return None
    mult = {'T':1e12,'G':1e9,'MEG':1e6,'M':1e6,'X':1e6,'k':1e3,'K':1e3,
            'm':1e-3,'u':1e-6,'n':1e-9,'p':1e-12,'f':1e-15,'a':1e-18}.get(m.group(2), 1.0)
    return float(m.group(1)) * mult
pval = parse_eng(pres)
check("prompt preset == current cursor value", pval is not None and abs(pval - FC1) < 1e-6 * FC1,
      "preset=%r (expected ~%.6g)" % (pres, FC1))

# the modal dialog was created and closed by the OK click
check("dialog closed after OK", gone == "1", "DIALOG_GONE=%s" % gone)

# a marker click must NOT hilight a trace (hilight_wave stays unset)
check("no hilight_wave toggled", hil == "", "hilight_wave=%r" % hil)

# ---- pixel (secondary): the Zin/s_1_1 marker dot moved with the cursor ----
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

# CAPBOX px where the shared s_1_1/Zin marker dot actually sat before / after
PB = (Cx + gamma("s_1_1", cb)[0]*R, Cy - gamma("s_1_1", cb)[1]*R)
PA = (Cx + gamma("s_1_1", ca)[0]*R, Cy - gamma("s_1_1", ca)[1]*R)
print("marker CAPBOX  before=(%.1f,%.1f)  after=(%.1f,%.1f)" % (PB[0], PB[1], PA[0], PA[1]))

before = load(RES + "/pos_beforeB.png")   # both from Run B -> same process
after  = load(RES + "/pos_after.png")
def near(img, c, tol=30): return (np.abs(img - c).max(axis=2) <= tol)
def blob(img, c, px, py, half=6):
    x0, y0 = int(px-half), int(py-half); x1, y1 = int(px+half)+1, int(py+half)+1
    return int(near(img, c)[max(0,y0):y1, max(0,x0):x1].sum())

# Zin is drawn last, so the visible dot at the shared s_1_1/Zin point is ZIN
# colored; the trace passes through both points, so the differential removes
# the (location-dependent) trace and leaves the fixed dot bump.
b_pb = blob(before, COL_ZIN, *PB)   # before @ start : dot + trace
b_pa = blob(before, COL_ZIN, *PA)   # before @ end   : trace only
a_pb = blob(after,  COL_ZIN, *PB)   # after  @ start : trace only
a_pa = blob(after,  COL_ZIN, *PA)   # after  @ end   : dot + trace
print("Zin px   before@start=%3d  before@end=%3d  after@start=%3d  after@end=%3d"
      % (b_pb, b_pa, a_pb, a_pa))
DOT = 8    # observed dot bump is ~14-21 px; keep well below that, above sub-pixel noise
check("Zin dot at start in before / gone in after", b_pb - a_pb >= DOT,
      "bump=%.0f" % (b_pb - a_pb))
check("Zin dot at end in after (absent in before)", a_pa - b_pa >= DOT,
      "bump=%.0f" % (a_pa - b_pa))

if bad:
    print("%d check(s) FAILED" % bad)
    sys.exit(1)
print("PASS: Smith button-3 Pos: prompt set cursor1 to %.6g Hz headlessly" % ca)
PY
py2=$?
echo "--- Run B log (transform + prompt result) ---"
grep -E "TRANSFORM|WIDGET|CURSOR|PRESET|DIALOG|HILIGHT|POS_DONE|CAP_DONE" "$RES/pos_B.log" 2>/dev/null
echo
# ================= summary ================
if [ "$py2" -eq 0 ]; then
  echo "ALL PASS (Smith button-3 on marker: Pos: prompt pre-filled, entered value applied, no hilight)"
  exit 0
else
  echo "FAIL (analysis rc=$py2)"
  exit 1
fi
