#
#  File: tests/smith/smoke.tcl
#
#  Headless smoke test for Smith chart attributes (feature/smith-chart).
#  Verifies, without a display:
#    - graph "mode" attribute round-trips to "Smith"
#    - "smz0" reference-impedance attribute round-trips
#
#  Run:  xschem -q -x -r --script ../tests/smith/smoke.tcl </dev/null
#  Expect: exit 0 and lines "MODE=Smith" and "Z0=75".
#

# --- one graph, set to Smith mode, with a reference impedance ---
xschem add_graph
xschem setprop rect 2 0 mode Smith
xschem setprop rect 2 0 smz0 75

set mode [string trim [xschem getprop rect 2 0 mode]]
set z0   [string trim [xschem getprop rect 2 0 smz0]]

puts "MODE=$mode"
puts "Z0=$z0"

if {$mode eq "Smith" && $z0 eq "75"} {
  puts "SMOKE=OK"
} else {
  puts "SMOKE=MISMATCH"
}

xschem exit closewindow force 0
