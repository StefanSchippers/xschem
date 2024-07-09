#
#  File: open_close.tcl
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
set testname "open_close"
set pathlist {}
set num_fatals 0

if {![file exists $testname]} {
  file mkdir $testname
}

file delete -force $testname/results
file mkdir $testname/results

set xschem_library_path "../xschem_library"

proc open_close {dir fn} {
  global xschem_library_path testname pathlist xschem_cmd
  set fpath "$dir/$fn"
  if { [regexp {\.(sym|sch)$} $fn ] } {
    puts "Testing (open_close) $fpath"  
    set output_dir $dir
    regsub -all $xschem_library_path $output_dir {} output_dir
    regsub {^/} $output_dir {} output_dir
    regsub {/} $output_dir {,} output_dir
    set fn_debug [join [list $output_dir , [regsub {\.} $fn {_}] "_debug.txt"] ""]
    set output [join [list $testname / results / $fn_debug] ""]
    puts "Output: $fn_debug"
    set cwd [pwd]
    cd $dir
    set catch_status [catch {eval exec {$xschem_cmd $fn -q -x -r -d 1 2> $cwd/$output}} msg]
    cd $cwd
    if {$catch_status} {
      puts "FATAL: $xschem_cmd $fn -q -x -r -d 1 2> $cwd/$output : $msg"
      incr num_fatals
    } else {
      lappend pathlist $fn_debug
      cleanup_debug_file $output
    }
  }
}

proc open_close_dir {dir} {
  set ff [lsort [glob -directory $dir -tails \{.*,*\}]]
  foreach f $ff {
    if {$f eq {..} || $f eq {.}} {
      continue
    }
    set fpath "$dir/$f"
    if {[file isdirectory $fpath]} {
      open_close_dir $fpath
    } else {
      open_close $dir $f
    }
  }
}

open_close_dir $xschem_library_path
print_results $testname $pathlist $num_fatals