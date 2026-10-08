#!/bin/bash
#
#  File: geometry_check.sh
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
#  Pixel-level sanity check for the Smith-chart plane change
#  (commit 9a0e69ca): the fixed plane window is +/-1.05 in Gamma units
#  (was +/-1.1), the inner plot-box frame (the solid GRIDLAYER rectangle
#  around the margin-inset plot box) is no longer drawn, and the square
#  mapping is now centered on the CONTAINER (not the asymmetric plot box).
#
#  Renders the Smith chart of tests/SC_Test.sch (its graph rect 2 0,
#  mode=Smith, 400x400 container in xschem units) on a virtual display
#  and captures the region at 1:1 (CAPBOX 2000x1600, like trace_check.sh
#  phase 1 / overlap_check.sh). All expectations are derived from the
#  captured image itself (no hardcoded pixel positions):
#
#    1. container boundary: the dashed SYMLAYER rectangle around the
#       graph (dark scheme #88dd00, the default color scheme) must be
#       present: its four edges are the only rows/columns carrying >= 150
#       px of that color (extent must be >= 300x300 px).
#    2. frame removal: the removed frame was a solid GRIDLAYER
#       (#4f4f4f) rectangle at the MARGIN-INSET plot box edges
#       (setup_graph_data(): marginx = 0.14*cw left, 0.35*marginx
#       right, marginy = 0.14*ch top and bottom). Count GRIDLAYER-color
#       pixels within 6 px of those four edges: the old frame would put
#       ~1200+ px in the band; without it the nearest grid element
#       (the unit circle) is >= 13 px away, so expect ~0. Threshold < 60.
#    3. +/-1.05 window: the unit circle (the outermost element of the
#       largest GRIDLAYER connected component, so that component's x/y
#       extent IS its diameter) must span >= 85% of the square plot
#       region: expected D = 2*min(plot_w, plot_h)/2.1
#       = 0.952 * min-side (a +/-1.1 window would give 2.1/2.2 = 0.955,
#       still in range; a +/-1.5 window gives 2.1/3.0 = 0.70 -> fails).
#       Upper guard 1.15 rejects a too-small window.
#    4. container centering: setup_graph_data() now sets
#       ssx0/ssy0 = X/Y_TO_SCREEN((rx1+rx2)/2, (ry1+ry2)/2), i.e. the
#       square mapping is centered on the CONTAINER. So the unit-circle
#       center (the largest-component bounding-box center, which is
#       symmetric about Gamma=0) must match the CONTAINER center (the
#       dashed SYMLAYER rectangle center) within a few px. The plot box
#       is x-asymmetric (left margin 0.14*cw, right 0.35*0.14*cw =
#       0.049*cw), so its center is ~0.0455*cw (~18 px at 400 px) to the
#       right of the container center: the old plot-box centering would
#       put the circle center ~18 px off and fail this check.
#
#  Usage: ./geometry_check.sh [xschem-binary]
#  Needs:  Xvfb, gcc + X11 dev libs (warp helper), python3 + PIL + numpy.
#  Overrides: SMITH_DISP (X display; default: first free of :99..)
#  Exit:   0 = PASS, 1 = FAIL, 2 = setup error.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${1:-$REPO/src/xschem}
SCH="$REPO/tests/SC_Test.sch"
RAW="$REPO/tests/SC_Test.raw"
CAPBOX="2000 1600 -1000 -800 1000 800"

[ -x "$XSCHEM" ] || { echo "xschem not found: $XSCHEM" >&2; exit 2; }
[ -f "$SCH" ]    || { echo "schematic not found: $SCH" >&2; exit 2; }
[ -f "$RAW" ]    || { echo "raw file not found: $RAW" >&2; exit 2; }
command -v python3 >/dev/null || { echo "python3 required" >&2; exit 2; }
python3 -c "import PIL, numpy" >/dev/null 2>&1 || { echo "python3 PIL+numpy required" >&2; exit 2; }

RES="$DIR/results"
mkdir -p "$RES"
rm -f "$RES"/geometry_check.* "$RES"/xauth.$$

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

# ================= capture (the SC_Test Smith chart as-is) =================
cat > "$RES/geometry_cap.tcl" <<EOF
set outch [open $RES/geometry_check.log w]
set warp $WARP
set capbox [split "$CAPBOX" " "]
xschem raw read $RAW
exec \$warp
update
update
after 50
update
xschem draw_graph 0
# warm-up: the first png capture of a fresh graph lands on the default
# 200x200 canvas; a discarded capture + re-draw sizes it to the CAPBOX
xschem print png $RES/geometry_check.png {*}\$capbox
xschem draw_graph 0
xschem print png $RES/geometry_check.png {*}\$capbox
puts \$outch "GEOMETRY_DONE"
close \$outch
xschem exit closewindow force 0
EOF

rc=1
for try in 1 2 3; do
  rm -f "$RES/geometry_check.log"
  timeout 90 "$XSCHEM" "$SCH" -o "$REPO/tests" -r --script "$RES/geometry_cap.tcl" </dev/null \
    >"$RES/geometry_check.stdout" 2>"$RES/geometry_check.stderr"
  rc=$?
  if [ "$rc" -eq 0 ] && [ -s "$RES/geometry_check.png" ] && \
     grep -q "GEOMETRY_DONE" "$RES/geometry_check.log" 2>/dev/null; then
    break
  fi
  echo "capture attempt $try/3 failed (rc=$rc)"
  sleep 2
done
if [ "$rc" -ne 0 ] || ! grep -q "GEOMETRY_DONE" "$RES/geometry_check.log" 2>/dev/null; then
  echo "FAIL: capture did not complete (rc=$rc)"
  tail -15 "$RES/geometry_check.stderr" 2>/dev/null
  exit 1
fi

# ================= pixel checks =================
python3 - "$RES/geometry_check.png" <<'PY'
import sys
import numpy as np
from PIL import Image

fn = sys.argv[1]
im = np.asarray(Image.open(fn).convert("RGB")).astype(int)

def near(mask_color, tol=3):
    return (np.abs(im - np.array(mask_color)).max(axis=2) <= tol)

bad = 0
def check(label, ok, detail):
    global bad
    print("%s: %s (%s)" % ("PASS" if ok else "FAIL", label, detail))
    if not ok: bad += 1

# --- dark scheme (default, dark_colorscheme=1): SYMLAYER #88dd00, GRIDLAYER #4f4f4f
# NOTE: #88dd00 is the SYMLAYER color, so it also colors the schematic's
# symbols/wires. The container boundary is the only large DASHED rectangle:
# each of its four edges carries ~half its length in this color on a single
# row/column (here ~200 px), far more than any schematic wire row.
container = near((136, 221, 0))
grid      = near((79, 79, 79))
EDGE = 150
rows = np.where(container.sum(axis=1) >= EDGE)[0]
cols = np.where(container.sum(axis=0) >= EDGE)[0]
if len(rows) < 2 or len(cols) < 2:
    print("FAIL: container boundary not found (rows=%d cols=%d)" % (len(rows), len(cols)))
    sys.exit(1)
cy1, cy2 = int(rows[0]), int(rows[-1])
cx1, cx2 = int(cols[0]), int(cols[-1])
cw, ch = cx2 - cx1 + 1, cy2 - cy1 + 1
check("container boundary renders", cw >= 300 and ch >= 300,
      "dashed SYMLAYER rectangle %dx%d px (expect ~400x400 at 1:1)" % (cw, ch))
if cw < 300 or ch < 300:
    sys.exit(1)

# --- the margin-inset plot box edges (setup_graph_data(): marginx=0.14*cw
# --- left, 0.35*marginx right; marginy=0.14*ch top/bottom); the removed
# --- frame ran exactly along these four edges
px1, px2 = cx1 + 0.14 * cw, cx2 - 0.35 * 0.14 * cw
py1, py2 = cy1 + 0.14 * ch, cy2 - 0.14 * ch
B = 6  # band half-width in px
band = np.zeros_like(grid)
band[max(0, int(py1 - B)):min(im.shape[0], int(py1 + B)), max(0, int(px1)):min(im.shape[1], int(px2 + 1))] = True
band[max(0, int(py2 - B)):min(im.shape[0], int(py2 + B)), max(0, int(px1)):min(im.shape[1], int(px2 + 1))] = True
band[max(0, int(py1)):min(im.shape[0], int(py2 + 1)), max(0, int(px1 - B)):min(im.shape[1], int(px1 + B))] = True
band[max(0, int(py1)):min(im.shape[0], int(py2 + 1)), max(0, int(px2 - B)):min(im.shape[1], int(px2 + B))] = True
n_frame = int((grid & band).sum())
check("no inner plot-box frame", n_frame < 60,
      "grid px within 6 px of the four plot-box edges = %d (old frame would be ~1200)" % n_frame)

# --- unit circle diameter vs the square plot region.
# --- The gray mask also catches antialiased label-text edges (TEXTLAYER
# --- #cccccc blended on black lands near #4f4f4f), so restrict to the plot
# --- box and keep only the LARGEST connected component: the grid strokes
# --- (unit circle + R circles + X arcs + real axis, all connected at the
# --- short-circuit point (-1,0)) form one component; label fragments (>=6 px
# --- outside the circle) are separate small components and are excluded.
# --- The 1 px stroke is 8-connectivity-broken on steep segments, so dilate
# --- the mask by 1 px before labeling (extent grows by +2 px, absorbed by
# --- the ratio tolerance).
X1, X2 = int(px1), int(px2 + 1)
Y1, Y2 = int(py1), int(py2 + 1)
gridbox = grid[Y1:Y2, X1:X2]
gy, gx = np.nonzero(gridbox)
if len(gx) == 0:
    print("FAIL: no GRIDLAYER pixels in the plot box (Smith grid not rendered?)")
    sys.exit(1)

def largest_component(m):
    try:
        from scipy import ndimage
        lab, n = ndimage.label(m)
        if n <= 1:
            return m
        sizes = ndimage.sum(m, lab, range(1, n + 1))
        return lab == (int(np.argmax(sizes)) + 1)
    except ImportError:
        pass
    # pure-Python 8-connected BFS fallback (mask has only ~10k px: fast)
    from collections import deque
    H, W = m.shape
    visited = np.zeros_like(m, bool)
    best = None
    for sy in range(H):
        for sx in range(W):
            if not m[sy, sx] or visited[sy, sx]:
                continue
            q = deque([(sy, sx)])
            visited[sy, sx] = True
            cur = []
            while q:
                y, x = q.popleft()
                cur.append((y, x))
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        ny, nx = y + dy, x + dx
                        if 0 <= ny < H and 0 <= nx < W and m[ny, nx] and not visited[ny, nx]:
                            visited[ny, nx] = True
                            q.append((ny, nx))
            if best is None or len(cur) > len(best):
                best = cur
    out = np.zeros_like(m, bool)
    for y, x in best:
        out[y, x] = True
    return out

def dilate1(m):
    d = m.copy()
    d[:-1, :] |= m[1:, :]
    d[1:, :] |= m[:-1, :]
    d[:, :-1] |= m[:, 1:]
    d[:, 1:] |= m[:, :-1]
    d[:-1, :-1] |= m[1:, 1:]
    d[1:, 1:] |= m[:-1, :-1]
    d[:-1, 1:] |= m[1:, :-1]
    d[1:, :-1] |= m[:-1, 1:]
    return d

comp = largest_component(dilate1(gridbox))
cy2b, cx2b = np.nonzero(comp)
dx = int(cx2b.max() - cx2b.min() + 1)
dy = int(cy2b.max() - cy2b.min() + 1)
plot_w = px2 - px1
plot_h = py2 - py1
D = 2.0 * min(plot_w, plot_h) / 2.1   # expected unit-circle diameter at +/-1.05
check("grid present", int(gridbox.sum()) > 500 and int(comp.sum()) > 500,
      "grid px=%d, largest-component px=%d" % (int(gridbox.sum()), int(comp.sum())))
if int(comp.sum()) <= 500:
    sys.exit(1)
for name, ext in (("x-extent", dx), ("y-extent", dy)):
    ratio = ext / D
    check("%s spans >85%% of the square plot region (+/-1.05)" % name,
          0.85 <= ratio <= 1.15,
          "extent=%d px, expected D=%.1f px, ratio=%.3f (a +/-1.5 window gave 0.700)" % (ext, D, ratio))

# --- container centering: the square mapping is centered on the CONTAINER
# --- (setup_graph_data(): ssx0/ssy0 = X/Y_TO_SCREEN of the container center),
# --- so the unit-circle center must equal the container center. The circle
# --- center is the bounding-box center of the largest grid component (which
# --- is symmetric about Gamma=0: unit circle + on-axis R circles + symmetric
# --- X-arc pair + the real axis all bound exactly the unit circle). The
# --- container center is the dashed SYMLAYER rectangle center measured above.
# --- The plot box is x-asymmetric, so a plot-box centering would be ~0.0455*cw
# --- (~18 px at 400 px) off; that is far outside the few-px tolerance.
ccx = (cx1 + cx2) / 2.0
ccy = (cy1 + cy2) / 2.0
circle_cx = X1 + (cx2b.min() + cx2b.max()) / 2.0
circle_cy = Y1 + (cy2b.min() + cy2b.max()) / 2.0
CTOL = 5.0  # a few px: the mapping places Gamma=0 exactly on the container center
dx_c = abs(circle_cx - ccx)
dy_c = abs(circle_cy - ccy)
check("circle center == container center (x)", dx_c <= CTOL,
      "dx=%.1f px (circle x=%.1f, container x=%.1f, tol %.0f)" % (dx_c, circle_cx, ccx, CTOL))
check("circle center == container center (y)", dy_c <= CTOL,
      "dy=%.1f px (circle y=%.1f, container y=%.1f, tol %.0f)" % (dy_c, circle_cy, ccy, CTOL))

if bad:
    print("%d geometry check(s) FAILED" % bad)
    sys.exit(1)
print("PASS: all geometry checks (frame removed, +/-1.05 plane, container-centered)")
sys.exit(0)
PY
rc=$?

echo "--- geometry_check.log ---"
cat "$RES/geometry_check.log" 2>/dev/null
echo

if [ "$rc" -eq 0 ]; then
  echo "ALL PASS (Smith chart geometry: no frame, +/-1.05 plane, container-centered)"
  exit 0
else
  echo "FAIL (geometry checks rc=$rc)"
  exit 1
fi
