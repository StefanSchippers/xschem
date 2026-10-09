#!/bin/bash
#
#  File: doc_screenshots.sh
#
#  This file is part of XSCHEM, a schematic capture and Spice/Vhdl/Verilog
#  netlisting tool for circuit simulation. Copyright (C) 1998-2026 S.F. Schippers
#  This program is free software; you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free Software
#  Foundation; either version 2 of the License, or (at your option) any later version.
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#  PARTICULAR PURPOSE. See the GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License along with
#  this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
#  Reproducible screenshot harness for the Smith-plot documentation
#  (scene definitions: doc_screenshots.tcl in this directory).
#
#  Pipeline, per run:
#    1. wipe + recreate a scratch dir OUTSIDE the repo
#       ($HOME/.xschem_smith_doc_scratch) -- all netlists and raws
#       live there, never in the repo tree
#    2. copy xschem_library/examples/LNA_SP.sch into the scratch dir
#    3. netlist it into the scratch dir (xschem -q -x -n -s -o)
#    4. run ngspice in the scratch dir (the .control writes the raws
#       to the CWD: LNA_SP2.raw, LNA_SP2_noise.raw, LNA_SP2_AC.raw)
#    5. verify the three raws exist
#    6. pick a display (Xvfb on a free number if none available) and
#       capture each requested scene with xschem print png, writing
#       doc/xschem_man/<scene>.png (the PNGs are doc assets, so they
#       belong in the repo)
#    7. gate-check each capture (python3 + PIL, same style as the
#       other smith tests):
#         smith01: 2000x1600 and non-blank;
#                  Smith chart #1 (SP raw) shows trace px of colors
#                  10 (s_1_1) and 17 (s_2_2);
#                  Smith chart #2 (AC raw) shows trace px of color 21
#                  -- the bare-name gate: the wave
#                  "Zin(vx1); vx1 i(vmes) / -1 * imp()" must plot even
#                  though the AC raw header name is "v(vx1)".
#
#  Usage: ./doc_screenshots.sh [scene ...]
#          no args: run all currently-defined scenes.
#  Env overrides: XSCHEM_BIN (default <repo>/src/xschem), NGSPICE_BIN,
#                 SMITH_DOC_SCRATCH, SMITH_DISP (like the other smith
#                 tests: use an existing X display instead of Xvfb).
#  Needs:   ngspice, Xvfb (or DISPLAY), python3 + PIL.
#  Exit:    0 all pass, 1 pipeline/gate failure, 2 setup error.

set -u
DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR/../.." && pwd)
XSCHEM=${XSCHEM_BIN:-$REPO/src/xschem}
NGSPICE=${NGSPICE_BIN:-ngspice}

ALL_SCENES=(smith01)

[ -x "$XSCHEM" ] || { echo "xschem not found or not executable: $XSCHEM" >&2; exit 2; }
command -v "$NGSPICE" >/dev/null 2>&1 || { echo "ngspice not found: $NGSPICE" >&2; exit 2; }
command -v python3 >/dev/null 2>&1 || { echo "python3 required" >&2; exit 2; }
python3 -c "import PIL" >/dev/null 2>&1 || { echo "python3 with PIL required" >&2; exit 2; }

if [ $# -gt 0 ]; then
  SCENES=("$@")
else
  SCENES=("${ALL_SCENES[@]}")
fi
for s in "${SCENES[@]}"; do
  case "$s" in
    smith*) : ;;
    *) echo "unknown scene: $s (defined: ${ALL_SCENES[*]})" >&2; exit 2 ;;
  esac
done

# --- scratch dir: wipe + recreate, always OUTSIDE the repo ---
SCRATCH=${SMITH_DOC_SCRATCH:-$HOME/.xschem_smith_doc_scratch}
case "$SCRATCH" in
  "$REPO"|"$REPO"/*) echo "scratch dir must be outside the repo: $SCRATCH" >&2; exit 2 ;;
esac
rm -rf "$SCRATCH"
mkdir -p "$SCRATCH" || { echo "cannot create scratch dir $SCRATCH" >&2; exit 2; }
cp "$REPO/xschem_library/examples/LNA_SP.sch" "$SCRATCH/" || exit 2

# --- netlist into the scratch dir ---
echo "== netlist LNA_SP.sch -> $SCRATCH/LNA_SP.spice =="
t0=$(date +%s)
timeout 120 "$XSCHEM" "$SCRATCH/LNA_SP.sch" -q -x -n -s -o "$SCRATCH" \
  </dev/null >"$SCRATCH/netlist.log" 2>&1
rc=$?
t1=$(date +%s)
if [ "$rc" -ne 0 ] || [ ! -s "$SCRATCH/LNA_SP.spice" ]; then
  echo "FAIL: netlist (rc=$rc, see $SCRATCH/netlist.log)"
  tail -15 "$SCRATCH/netlist.log" 2>/dev/null
  exit 1
fi
echo "netlist ok in $((t1 - t0))s"

# --- ngspice in the scratch dir: raws land in the CWD ---
echo "== ngspice: LNA_SP2.raw / LNA_SP2_noise.raw / LNA_SP2_AC.raw =="
t0=$(date +%s)
( cd "$SCRATCH" && timeout 600 "$NGSPICE" -b LNA_SP.spice >ngspice.log 2>&1 )
rc=$?
t1=$(date +%s)
[ "$rc" -eq 0 ] || echo "note: ngspice exit rc=$rc (see $SCRATCH/ngspice.log)"
missing=0
for r in LNA_SP2.raw LNA_SP2_noise.raw LNA_SP2_AC.raw; do
  if [ ! -s "$SCRATCH/$r" ]; then
    echo "FAIL: missing raw file: $SCRATCH/$r"
    tail -15 "$SCRATCH/ngspice.log" 2>/dev/null
    missing=1
  fi
done
if [ "$missing" -eq 1 ]; then exit 1; fi
echo "ngspice ok in $((t1 - t0))s"
ls -la "$SCRATCH"/*.raw "$SCRATCH"/*.s2p 2>/dev/null

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
XAUTH="$SCRATCH/xauth.$$"
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
    for i in $(seq 1 20); do [ -e "$SOCK" ] && break; sleep 0.25; done
  fi
fi
export DISPLAY="$DISP"
[ -n "$XVFB_PID" ] && export XAUTHORITY="$XAUTH"

if ! timeout 5 xset q >/dev/null 2>&1; then
  echo "SKIP: no usable X display on $DISP (install xvfb, or set SMITH_DISP / DISPLAY)"
  exit 2
fi
echo "== display: $DISP =="

# pointer-warp helper (tests/smith/warp.c, shared with the other smith tests)
WARP="$DIR/warp"
if [ ! -x "$WARP" ]; then
  if command -v gcc >/dev/null && gcc -o "$WARP" "$DIR/warp.c" -lX11 2>/dev/null; then
    :
  else
    echo "note: pointer-warp helper unavailable; continuing without it"
    WARP=""
  fi
fi

MAN="$REPO/doc/xschem_man"
mkdir -p "$MAN"

fail=0
for s in "${SCENES[@]}"; do
  echo "== scene $s -> $MAN/$s.png =="
  OUT="$MAN/$s.png"
  LOGF="$SCRATCH/$s.log"
  ok=no
  for try in 1 2 3; do
    rm -f "$OUT"
    SCENE="$s" OUT="$OUT" SCH="$SCRATCH/LNA_SP.sch" SCRATCH="$SCRATCH" \
      DOC_LOG="$LOGF" WARP="${WARP:-}" \
      timeout 180 "$XSCHEM" -r --script "$DIR/doc_screenshots.tcl" </dev/null \
      >"$SCRATCH/$s.stdout" 2>"$SCRATCH/$s.stderr"
    if [ $? -eq 0 ] && [ -s "$OUT" ]; then ok=yes; break; fi
    echo "capture $s attempt $try/3 failed (see $SCRATCH/$s.stderr)"
    sleep 2
  done
  if [ "$ok" != yes ]; then
    echo "FAIL: scene $s capture did not produce a PNG"
    tail -15 "$SCRATCH/$s.stderr" 2>/dev/null
    fail=1
    continue
  fi
  echo "--- $s.log ---"
  cat "$LOGF" 2>/dev/null
  echo "-----------------"
  ls -la "$OUT"

  # --- gate checks (python3 + PIL, same style as the other smith tests) ---
  python3 - "$OUT" "$s" <<'PY'
import sys

OUT = sys.argv[1]
SCENE = sys.argv[2]

# capture window + image size of the scene (doc_screenshots.tcl: scene_smith01).
# Only scenes using a known window are size-checked (smith01 for now); a future
# scene with a different window gets added to SCENE_WINDOWS instead of being
# rejected by the default 2000x1600 check.
SCENE_WINDOWS = {
    "smith01": (2000, 1600, -100.0, -1100.0, 1100.0, 0.0),
}
# user -> image px, per zoom_box() in src/actions.c:
#   zoom = max((x2-x1)/img_w, (y2-y1)/img_h), anchored at (x1, y1);
#   px = (user - win_min) / zoom
if SCENE in SCENE_WINDOWS:
    IMG_W, IMG_H, WX1, WY1, WX2, WY2 = SCENE_WINDOWS[SCENE]
    ZOOM = max((WX2 - WX1) / IMG_W, (WY2 - WY1) / IMG_H)
    INV = 1.0 / ZOOM
else:
    IMG_W = IMG_H = None

def user_to_px(ux, uy):
    return int(round((ux - WX1) * INV)), int(round((uy - WY1) * INV))

TOL = 40     # per-channel RGB tolerance when matching trace pixels
THRESH = 50  # minimum trace pixels expected inside a gate region

# default dark color table (src/xschem.tcl: dark_colors,
# dark_colorscheme=1): index -> rgb
COLORS = {
    10: (0xff, 0x00, 0xff),  # magenta   (s_1_1)
    17: (0x00, 0xff, 0xcc),  # turquoise (s_2_2, noise)
    21: (0xfd, 0xb2, 0x00),  # orange    (AC smith wave)
}

try:
    import numpy as np
    from PIL import Image
    _im = np.asarray(Image.open(OUT).convert("RGB")).astype(int)
    H, W = _im.shape[0], _im.shape[1]
    def count_color(x0, y0, bw, bh, rgb):
        r, g, b = rgb
        sub = _im[y0:y0 + bh, x0:x0 + bw]
        m = (np.abs(sub[:, :, 0] - r) <= TOL) & \
            (np.abs(sub[:, :, 1] - g) <= TOL) & \
            (np.abs(sub[:, :, 2] - b) <= TOL)
        return int(m.sum())
    bg = _im[0, 0]
    nonblank = int((np.abs(_im.astype(int) - bg.astype(int)).max(axis=2) > 30).sum())
except ImportError:
    from PIL import Image
    _im = Image.open(OUT).convert("RGB")
    W, H = _im.size
    px = _im.load()
    def count_color(x0, y0, bw, bh, rgb):
        r, g, b = rgb
        n = 0
        for y in range(max(0, y0), min(H, y0 + bh)):
            for x in range(max(0, x0), min(W, x0 + bw)):
                pr, pg, pb = px[x, y]
                if abs(pr - r) <= TOL and abs(pg - g) <= TOL and abs(pb - b) <= TOL:
                    n += 1
        return n
    bg = px[0, 0]
    nonblank = 0
    for y in range(H):
        for x in range(W):
            p = px[x, y]
            if max(abs(p[0] - bg[0]), abs(p[1] - bg[1]), abs(p[2] - bg[2])) > 30:
                nonblank += 1

bad = 0
if IMG_W is not None and (W, H) != (IMG_W, IMG_H):
    print("FAIL: %s is %dx%d, expected %dx%d" % (OUT, W, H, IMG_W, IMG_H))
    bad = 1
if nonblank < 1000:
    print("FAIL: %s looks blank (%d non-background px)" % (OUT, nonblank))
    bad = 1

if SCENE == "smith01":
    # Smith chart #1 (SP raw): user rect (550,-940)-(810,-700)
    ax0, ay0 = user_to_px(550, -940)
    ax1, ay1 = user_to_px(810, -700)
    n10 = count_color(ax0, ay0, ax1 - ax0, ay1 - ay0, COLORS[10])
    n17 = count_color(ax0, ay0, ax1 - ax0, ay1 - ay0, COLORS[17])
    # Smith chart #2 (AC raw): user rect (550,-700)-(810,-460)
    bx0, by0 = user_to_px(550, -700)
    bx1, by1 = user_to_px(810, -460)
    n21 = count_color(bx0, by0, bx1 - bx0, by1 - by0, COLORS[21])
    print("GATE smith01: size=%dx%d nonblank=%d zoom=%.6f px/unit(TOL=%d,THRESH=%d)"
          % (W, H, nonblank, ZOOM, TOL, THRESH))
    print("  smith#1 user=(550,-940)-(810,-700) px=(%d,%d)-(%d,%d): c10=%d c17=%d"
          % (ax0, ay0, ax1, ay1, n10, n17))
    print("  smith#2 user=(550,-700)-(810,-460) px=(%d,%d)-(%d,%d): c21=%d"
          % (bx0, by0, bx1, by1, n21))
    if n10 > THRESH:
        print("PASS: smith#1 color 10 (s_1_1) trace visible (%d px)" % n10)
    else:
        print("FAIL: smith#1 color 10 (s_1_1) trace not visible (%d px <= %d)" % (n10, THRESH))
        bad = 1
    if n17 > THRESH:
        print("PASS: smith#1 color 17 (s_2_2) trace visible (%d px)" % n17)
    else:
        print("FAIL: smith#1 color 17 (s_2_2) trace not visible (%d px <= %d)" % (n17, THRESH))
        bad = 1
    if n21 > THRESH:
        print("PASS BARE-NAME GATE: smith#2 color 21 trace visible (%d px)" % n21)
    else:
        print("FAIL BARE-NAME GATE: smith#2 color 21 trace not visible (%d px <= %d)"
              " -- wave 'vx1 i(vmes) / -1 * imp()' did not plot (raw header is v(vx1))"
              % (n21, THRESH))
        bad = 1
else:
    print("GATE %s: size=%dx%d nonblank=%d (no region checks defined for this scene)"
          % (SCENE, W, H, nonblank))

sys.exit(1 if bad else 0)
PY
if [ $? -ne 0 ]; then
  echo "FAIL: scene $s gate check(s) failed"
  fail=1
fi
done

echo
if [ "$fail" -eq 0 ]; then
  echo "ALL PASS (${#SCENES[@]} scene(s): ${SCENES[*]})"
  exit 0
fi
echo "FAIL: pipeline/gate check(s) failed"
exit 1
