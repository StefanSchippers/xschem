#
#  File: doc_screenshots.tcl
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
#  Reproducible screenshot harness for the Smith-plot documentation
#  (runner: doc_screenshots.sh in this directory).
#
#  A "scene" is a named capture: load a schematic whose graphs point at
#  $netlist_dir/*.raw with autoload=1, let xschem load the raws, then
#  `xschem print png` a fixed user-space window to a fixed-size image.
#  A discarded warm-up capture is taken before the deliverable one
#  (the first PNG capture of a freshly drawn canvas can be incomplete;
#  same settle pattern the sweep-expression label checks use).
#
#  Run by doc_screenshots.sh via:
#    DISPLAY=<display> xschem -r --script doc_screenshots.tcl
#  Environment:
#    SCENE     scene name (default smith01)
#    OUT       output PNG path (absolute, inside doc/xschem_man/)
#    SCH       schematic file to load (absolute)
#    SCRATCH   dir containing the *.raw files; set as netlist_dir so the
#              graphs' $netlist_dir/... rawfile refs resolve to it
#    DOC_LOG   log file for PASS/FAIL-style diagnostics
#    WARP      path to tests/smith/warp pointer-warp helper (optional)
#
#  Adding a scene: define `proc scene_<name> {}` returning
#  {img_w img_h x1 y1 x2 y2} and add <name> to doc_scene_list.
#
#  A running X server is required.
#  Requires Tcl 8.4+ (8.5-only features such as {*}-list expansion are avoided).

if {![info exists ::env(SCENE)]} { set ::env(SCENE) "smith01" }

foreach var {OUT SCH SCRATCH DOC_LOG} {
  if {![info exists ::env($var)]} {
    puts stderr "doc_screenshots.tcl: missing required env: $var"
    exit 1
  }
}

set SCENE   $::env(SCENE)
set OUT     $::env(OUT)
set SCH     $::env(SCH)
set SCRATCH $::env(SCRATCH)
set LOGF    $::env(DOC_LOG)

set outch [open $LOGF w]

proc diag {line} {
  global outch
  puts $outch $line
  flush $outch
}

# log + abort with exit status 1 (xschem exit does the cleanup; a plain
# tcl exit would skip it mid-interpreter)
proc die {line} {
  diag $line
  close $outch
  xschem exit closewindow force 1
}

## ------------------------------------------------------------------
## scene registry
##
## each scene proc returns {img_w img_h x1 y1 x2 y2}: the PNG size and
## the user-space window `xschem print png` renders. xschem maps the
## window to the image with zoom = max((x2-x1)/img_w, (y2-y1)/img_h)
## anchored at (x1, y1) (zoom_box in src/actions.c), so a non-square
## aspect is letter-boxed, not stretched.
## ------------------------------------------------------------------

set doc_scene_list [list smith01]

## smith01: hero shot -- the whole LNA_SP schematic with both Smith
## charts in frame (schematic user-space extent ~ x[-100..1100]
## y[-1100..0]):
##   Smith chart #1 (SP raw):  rect (550,-940)-(810,-700), colors 10/17
##   Smith chart #2 (AC raw):  rect (550,-700)-(810,-460), color 21
proc scene_smith01 {} {
  return [list 2000 1600 -100 -1100 1100 0]
}

## ------------------------------------------------------------------
## capture: warm-up print + final print of the scene window
## ------------------------------------------------------------------

proc doc_capture {scene imgw imgh x1 y1 x2 y2} {
  global outch OUT SCRATCH
  ## proven settle pattern (sweep_expression/check_label.tcl): point the
  ## X pointer over the canvas, pump events, redraw, print, redraw, print
  if {[info exists ::env(WARP)] && [file executable $::env(WARP)]} {
    catch {exec $::env(WARP)}
  }
  update
  update
  after 100
  update
  xschem redraw
  ## warm-up capture sizes/settles the canvas; it is discarded
  xschem print png [file join $SCRATCH "warm_${scene}.png"] $imgw $imgh $x1 $y1 $x2 $y2
  xschem redraw
  update
  after 100
  update
  xschem print png $OUT $imgw $imgh $x1 $y1 $x2 $y2
  diag "CAPTURE scene=$scene out=$OUT img=${imgw}x${imgh} box=[list $x1 $y1 $x2 $y2]"
}

## ------------------------------------------------------------------
## main
## ------------------------------------------------------------------

if {[llength [info procs scene_$SCENE]] == 0} {
  die "FATAL unknown scene: $SCENE (defined: $doc_scene_list)"
}
set params [uplevel #0 [list scene_$SCENE]]
if {[llength $params] != 6} {
  die "FATAL scene $SCENE: scene proc must return {img_w img_h x1 y1 x2 y2}"
}

## point the graphs' $netlist_dir at the scratch dir holding the raws
if {[set_netlist_dir 1 $SCRATCH] eq {}} {
  die "FATAL netlist_dir: could not set/verify $SCRATCH"
}
diag "PASS netlist_dir=$SCRATCH"

diag "LOAD sch=$SCH"
xschem load $SCH

## settle: pump X events so the initial draw (which triggers the graphs'
## autoload raw reads) runs; if no raw is loaded yet, force full redraws
## and poll (raw load is synchronous inside draw()).
update
update
after 300
update
set tries 0
while {[xschem raw loaded] < 0 && $tries < 40} {
  xschem redraw
  after 250
  update
  incr tries
}
set loaded [xschem raw loaded]
if {$loaded < 0} {
  die "FATAL autoload: no raw loaded after $tries redraws (netlist_dir=$SCRATCH)"
}
diag "PASS autoload: raw loaded (level $loaded, vars=[xschem raw vars], sim_type=[xschem raw sim_type])"

## explicit argument passing (Tcl 8.4: no {*}-list expansion available)
doc_capture $SCENE [lindex $params 0] [lindex $params 1] [lindex $params 2] \
                   [lindex $params 3] [lindex $params 4] [lindex $params 5]

diag "DONE scene=$SCENE"
close $outch
xschem exit closewindow force 0
