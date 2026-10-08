#
#  File: tests/smith/expr_checks.tcl
#
#  Headless (Xvfb) miscasing checks for the Smith-chart imp() RPN operator
#  (feature/smith-chart, commits 1b43f044 + 9590914d). Driven by
#  tests/smith/expr_checks.sh which supplies SMITH_RAW / SMITH_WARP / SMITH_OUT
#  and captures xschem's stderr (where the info() lines land).
#
#  Each check sets the graph "node" attribute to a distinct value and forces
#  exactly ONE graph redraw (setprop -> auto-redraw, flushed with a single
#  'update'), so each bad expression emits its info() line exactly once.
#  The .sh then counts the resulting stderr lines and asserts:
#    a. "Zin; v(net1) i(v2) / imp()"   -> NO error line (valid Z->Gamma)
#    b. "Zin; nosuchvar imp()"         -> exactly ONE "not supported ... nosuchvar"
#    c. "Zin; v(net1)"                 -> NO error (depth ends at 1: V plotted as Gamma)
#    d. "Zin; v(net1) i(v2) /"         -> NO "unbalanced" (valid Z=v/i, depth ends at 1)
#    d2."Zin; v(net1) i(v2) / /"       -> exactly ONE "unbalanced" (depth reaches 0)
#
#  NOTE: the expression-validation path runs inside the graph node-draw loop,
#  which only executes when a (virtual) X display drives drawing. A pure
#  "-q -x -r" run without any display produces NO info() lines, so this test
#  requires the Xvfb display set up by expr_checks.sh.
#

set raw   $env(SMITH_RAW)
set out   $env(SMITH_OUT)
set warp  $env(SMITH_WARP)

set outch [open $out w]
xschem raw read $raw
puts $outch "SIM_TYPE=[xschem raw sim_type]"
puts $outch "IDX_S11=[xschem raw index s_1_1]"
puts $outch "IDX_Z11=[xschem raw index z_1_1]"

xschem add_graph
xschem setprop rect 2 0 mode Smith
xschem setprop rect 2 0 smz0 75
xschem setprop rect 2 0 color 7
xschem setprop rect 2 0 x1 0
xschem setprop rect 2 0 x2 100
xschem setprop rect 2 0 y1 0
xschem setprop rect 2 0 y2 10

exec $warp
update
update
after 50
update

# (a) valid Z -> Gamma expression; expect no error/warning lines
puts $outch "CHECK_A_BEGIN"
xschem setprop rect 2 0 node "Zin; v(net1) i(v2) / imp()"
update
puts $outch "CHECK_A_DONE"

# (b) unknown variable; expect exactly one "not supported" line naming it
puts $outch "CHECK_B_BEGIN"
xschem setprop rect 2 0 node "Zin; nosuchvar imp()"
update
puts $outch "CHECK_B_DONE"

# (c) bare voltage, no imp(); depth ends at 1 (valid) -> V plotted as Gamma, no error
puts $outch "CHECK_C_BEGIN"
xschem setprop rect 2 0 node "Zin; v(net1)"
update
puts $outch "CHECK_C_DONE"

# (d) spec-literal "v i /": actually a valid Z = v/i (depth ends at 1) -> no "unbalanced"
puts $outch "CHECK_D_BEGIN"
xschem setprop rect 2 0 node "Zin; v(net1) i(v2) /"
update
puts $outch "CHECK_D_DONE"

# (d2) genuinely unbalanced "v i / /": depth reaches 0 -> exactly one "unbalanced"
puts $outch "CHECK_D2_BEGIN"
xschem setprop rect 2 0 node "Zin; v(net1) i(v2) / /"
update
puts $outch "CHECK_D2_DONE"

puts $outch "ALL_CHECKS_DONE"
close $outch
xschem exit closewindow force 0
