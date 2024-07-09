#
#  File: netlisting.tcl
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
set testname "netlisting"
set pathlist {}
set num_fatals 0

if {![file exists $testname]} {
  file mkdir $testname
}

file delete -force $testname/results
file mkdir $testname/results

set xschem_library_path "../xschem_library"

proc netlisting {dir fn} {
  global xschem_library_path testname pathlist xschem_cmd
  if { [regexp {\.sch$} $fn ] } {
    puts "Testing ($testname) $dir/$fn"  
    set output_dir $dir
    regsub -all $xschem_library_path $output_dir {} output_dir
    regsub {^/} $output_dir {} output_dir
    # Spice Netlist
    run_xschem_netlist vhdl $output_dir $dir $fn
    run_xschem_netlist v $output_dir $dir $fn
    run_xschem_netlist tdx $output_dir $dir $fn
    run_xschem_netlist spice $output_dir $dir $fn
  }
}

proc netlisting_dir {dir} {
  set ff [lsort [glob -directory $dir -tails \{.*,*\}]]
  foreach f $ff {
    if {$f eq {..} || $f eq {.}} {
      continue
    }
    set fpath "$dir/$f"
    if {[file isdirectory $fpath]} {
      netlisting_dir $fpath
    } else {
      netlisting $dir $f
    }
  }
}

proc run_xschem_netlist {type output_dir dir fn} {
  global testname pathlist xschem_cmd num_fatals
  set cwd [pwd]
  set fn_debug [join [list $output_dir , [regsub {\.} $fn {_}] "_${type}_debug.txt"] ""]
  regsub {./} $fn_debug {_} fn_debug
  set sch_name [regsub {\.sch} $fn {}]
  set fn_netlist [join [list $sch_name "." $type] ""]
  set output [join [list $cwd / $testname / results / $fn_debug] ""]
  set netlist_output_dir [join [list $cwd / $testname / results ] ""]
  puts "Output: $fn_debug"
  set opt s
  if {$type eq "vhdl"} {set opt V}
  if {$type eq "v"} {set opt w}
  if {$type eq "tdx"} {set opt t}
  set netlist_failed 0 ;# not used here but might be used in the future.
  set general_failure 0
  
  cd $dir
  set catch_status [catch {eval exec {$xschem_cmd $fn -q -x -r -$opt -o $netlist_output_dir -n 2> $output}} msg opt]
  cd $cwd
  if {$catch_status} {
    set error_code [dict get $opt -errorcode]
    # in case of child process error $error_code will be {CHILDSTATUS 11731 10}, second item is processID, 
    # 3rd item is child process exit code. In case of netlisting error xschem exit code is 10
    if {[regexp {^CHILDSTATUS.* 10$} $error_code]} {
      set netlist_failed 1
    } elseif {$error_code ne {NONE}} {
      set general_failure 1
    }
  }
  if {$general_failure} {
    puts "FATAL: $xschem_cmd $fn -q -x -r -$opt -o $netlist_output_dir -n 2> $output : $msg"
    incr num_fatals 
  } else {
    lappend pathlist $fn_debug
    lappend pathlist $fn_netlist
    cleanup_debug_file $output
  }
}

netlisting_dir $xschem_library_path
# Intermediate netlisting name (.*) will keep changing, so delete them for easier diff
set ff [glob -directory "$testname/results" \{.*\}]
foreach f $ff {
  if {$f eq "$testname/results/.." || $f eq "$testname/results/."} {
    continue
  }
  file delete $f
}

print_results $testname $pathlist $num_fatals
