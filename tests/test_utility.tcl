#
#  File: xschem_test_utility.tcl
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

set OS [lindex $tcl_platform(os) 0]
set xschem_cmd "xschem"

# From Glenn Jackman (Stack Overflow answer)
proc comp_file {file1 file2} {
    # optimization: check file size first
    set equal 0
    if {[file size $file1] == [file size $file2]} {
        set fh1 [open $file1 r]
        set fh2 [open $file2 r]
        set equal [string equal [read $fh1] [read $fh2]]
        close $fh1
        close $fh2
    }
    return $equal
}

proc print_results {testname pathlist num_fatals} {
    if {[file exists ${testname}/gold]} {
        set a [catch "open \"$testname.log\" w" fd]
        if {$a} {
            puts "Couldn't open $f"
        } else {
            set i 0
            set num_fail 0
            set num_gold 0
            foreach f $pathlist {
                incr i
                if {![file exists $testname/gold/$f]} {
                    puts $fd "$i. $f: GOLD?"
                    incr num_gold
                    continue
                }
                if {![file exists $testname/results/$f]} {
                    puts $fd "$i. $f: RESULT?"
                    continue
                }
                if ([comp_file $testname/gold/$f $testname/results/$f]) {
          puts $fd "$i. $f: PASS"
        } else {
          puts $fd "$i. $f: FAIL"
          incr num_fail
        }
            }
            puts $fd "Summary:"
            puts $fd "Num failed: $num_fail      Num missing gold: $num_gold      Num passed: [expr $i-$num_fail-$num_gold]"
            if {$num_fatals} {
                puts $fd "FATAL: $num_fatals.  Please search for FATAL in its output file for more detail"
            }
            close $fd
        }
    } else {
        puts "No gold folder.  Set results as gold please."
    }
}

# Edit lines that change each time regression is ran
proc cleanup_debug_file {output} {
    eval exec {awk -f cleanup_debug_file.awk $output}
}
