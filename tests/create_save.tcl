#
#  File: create_save.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2022 Stefan Frederik Schippers
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

source test_utility.tcl
set testname "create_save"
set pathlist {}
set num_fatals 0

file delete -force $testname/results
file mkdir $testname/results

proc create_save {} {
  global testname pathlist xschem_cmd num_fatals
  set results_dir ${testname}/results
  if {[file exists ${testname}/tests]} {
    set ff [lsort [glob -directory ${testname}/tests -tails \{.*,*\}]]
    foreach f $ff {
      if {$f eq {..} || $f eq {.}} {continue}
      if {[regexp {\.(tcl)$} $f ]} {
        set fn_sch [regsub {tcl$} $f {sch}]
        set a [catch "open \"$results_dir/$fn_sch\" w" fd]
        if {!$a} {
          puts $fd "v {xschem version=2.9.5 file_version=1.1}"
          puts $fd "G {}"
          puts $fd "V {}"
          puts $fd "S {}"
          puts $fd "E {}"
          close $fd
          set filename [regsub {\.tcl$} $f {}]
          set output ${filename}_debug.txt
          if {[catch {eval exec {$xschem_cmd ${results_dir}/$fn_sch -d 1 --script ${testname}/tests/${f} 2> ${results_dir}/$output}} msg]} {
            puts "FATAL: $msg"
            incr num_fatals
          } else {
            lappend pathlist $output
            lappend pathlist "${filename}.sch"
            cleanup_debug_file ${results_dir}/$output
            cleanup_debug_file ${results_dir}/$fn_sch
          } 
        }
      }
    }
  }
}

create_save
print_results $testname $pathlist $num_fatals