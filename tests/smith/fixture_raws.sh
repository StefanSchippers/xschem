#!/bin/bash
#
#  File: fixture_raws.sh
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
#  Generates the six ASCII "complex" SP raw files used by trace_check.sh's
#  Phase-2 exact-geometry fixture test.
#
#  Each raw is a tiny SP sweep with two variables -- the sweep frequency
#  (index 0) and a single constant complex impedance "z" (index 1) -- across
#  five frequency points. "Flags: complex" + "Plotname: SP Analysis" make
#  xschem treat it as an ac/complex SP raw so the graph "node" attribute
#  "Zt; z imp()" evaluates z as an impedance and maps it to a Smith-chart
#  reflection coefficient Gamma = (Z - Z0)/(Z + Z0) with smz0 = 50:
#
#      c0    z = 50+0j   -> Gamma = (0, 0)      centre (calibration point C)
#      short z = 0+0j    -> Gamma = (-1, 0)     left edge (defines R = |short-C|)
#      third z = 100+0j  -> Gamma = (1/3, 0)    checked
#      bot   z = 0-50j   -> Gamma = (0, -1)     bottom of the unit circle, checked
#      top   z = 0+50j   -> Gamma = (0, +1)     top of the unit circle, checked
#      q1    z = 50+50j  -> Gamma = (0.2, 0.4)  first-quadrant interior, checked
#
#  All six mapping values were verified analytically against
#  Gamma = (Z-Z0)/(Z+Z0) (Z0 = 50). The raw files are regenerated on every
#  run so the test is fully self-contained and does not depend on any
#  hand-committed binary data.
#
#  Usage:  ./fixture_raws.sh [output-dir]
#          default output-dir: <this dir>/results
#  Exit:    0 = OK, non-zero on failure.
#
set -u
DIR=$(cd "$(dirname "$0")" && pwd)
OUT=${1:-$DIR/results}

mkdir -p "$OUT"

# gen_raw <name> <z_real,z_imag>
gen_raw() {
  {
    echo "Title: f"
    echo "Date: 2026"
    echo "Plotname: SP Analysis"
    echo "Flags: complex"
    echo "No. Variables: 2"
    echo "No. Points: 5"
    echo "Variables:"
    printf '\t0\tfrequency\tfrequency\n'
    printf '\t1\tz\timpedance\n'
    echo "Values:"
    local i f
    for i in 0 1 2 3 4; do
      f=$((1000 + i))
      echo "$i $f,0"
      echo " $2"
      echo
    done
  } > "$OUT/fx_$1.raw"
}

gen_raw c0    "50,0"
gen_raw short "0,0"
gen_raw third "100,0"
gen_raw q1    "50,50"
gen_raw bot   "0,-50"
gen_raw top   "0,50"

# sanity: all six must exist and be non-empty
ok=1
for n in c0 short third q1 bot top; do
  if [ ! -s "$OUT/fx_$n.raw" ]; then
    echo "FAIL: could not generate $OUT/fx_$n.raw" >&2
    ok=0
  fi
done
[ "$ok" -eq 1 ] || exit 1
echo "generated 6 fixture raws in $OUT"
