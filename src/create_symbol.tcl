#
#  File: create_symbol.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2024 Stefan Frederik Schippers
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
# Create an xschem symbol from a list of in, inout, out pins.
# Example:
#   create_symbol test.sym {CLK RST D} {Q QB} {VCC VSS}
#
proc create_symbol {name {in {}} {out {}} {inout {}}} {
  set symname [file rootname $name].sym
  set res [catch {open $symname {WRONLY CREAT EXCL}} fd]
  if {$res} {puts $fd; return 0} ;# Error. Print reason and exit.
  puts $fd {v {xschem version=3.4.6RC file_version=1.2}}
  puts $fd {K {type=subcircuit format="@name @pinlist @symname" template="name=X1"}}
  set x -150
  set y 0
  foreach pin $in { ;# create all input pins on the left
    puts $fd "B 5 [expr {$x - 2.5}] [expr {$y - 2.5}] [expr {$x + 2.5}] [expr {$y + 2.5}] {name=$pin dir=in}"
    puts $fd "T {$pin} [expr {$x + 25}] [expr {$y - 6}] 0 0 0.2 0.2 {}"
    puts $fd "L 4 $x $y [expr {$x + 20}] $y {}"
    incr y 20
  }
  set x 150
  set y 0
  foreach pin $out { ;# create all out pins on the top right
    puts $fd "B 5 [expr {$x - 2.5}] [expr {$y - 2.5}] [expr {$x + 2.5}] [expr {$y + 2.5}] {name=$pin dir=out}"
    puts $fd "T {$pin} [expr {$x - 25}] [expr {$y - 6}] 0 1 0.2 0.2 {}"
    puts $fd "L 4 [expr {$x - 20}] $y $x $y {}"
    incr y 20
  }
  foreach pin $inout { ;# create all inout pins on the bottom right
    puts $fd "B 5 [expr {$x - 2.5}] [expr {$y - 2.5}] [expr {$x + 2.5}] [expr {$y + 2.5}] {name=$pin dir=inout}"
    puts $fd "T {$pin} [expr {$x - 25}] [expr {$y - 6}] 0 1 0.2 0.2 {}"
    puts $fd "L 7 [expr {$x - 20}] $y $x $y {}"
    incr y 20
  }
  puts $fd "B 4 -130 -10 130 [expr {$y - 10}] {fill=0}" ;# symbol box
  puts $fd "T {@symname} 0 [expr {($y - 20) / 2}] 0 0 0.3 0.3 {hcenter=1 vcenter=1}" ;#symbol name
  puts $fd "T {@name} 130 -10 2 1 0.2 0.2 {}" ;# instance name
  close $fd
  puts "Created symbol ${symname}"
  return 1
}
