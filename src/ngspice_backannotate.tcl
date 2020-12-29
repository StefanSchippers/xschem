#
#  File: ngspice_backannotate.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2020 Stefan Frederik Schippers
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


namespace eval ngspice {
  # Create a variable inside the namespace
  variable ngspice_data
}

proc ngspice::read_ngspice_raw {arr fp} {
  upvar $arr var
  unset -nocomplain var

  set variables 0
  while {[gets $fp line] >= 0} {
    if {$line eq "Binary:"} break
  
    if {[regexp {^No\. Variables:} $line]} {
      set n_vars [lindex $line end]
    }
    if {[regexp {^No\. Points:} $line]} {
      set n_points [lindex $line end]
    }
    if {$variables} {
      set var([lindex $line 1]) {}
      set idx([lindex $line 0]) [lindex $line 1]
    }
    if {[regexp {^Variables:} $line]} {
      set variables 1
    }
  }
  if {$variables} {
    set bindata [read $fp [expr 8 * $n_vars * $n_points]]
    if { $n_points == 1} {
      binary scan $bindata d[expr $n_vars * $n_points] data
      for {set p 0} {$p < $n_points} { incr p} {
        for {set v 0} {$v < $n_vars} { incr v} {
          lappend var($idx($v)) [lindex $data [expr $p * $n_vars + $v]]
          # puts "-->|$idx($v)|$var($idx($v))|"
        }
      }
    }
    set var(n\ vars) $n_vars
    set var(n\ points) $n_points
  }
}

proc ngspice::get_voltage_probe {arr n } {
  upvar $arr var
  set m "v($n)"
  if { ! [info exists var([string tolower $m])] } {
    set m $n
  }
  if { abs($var([string tolower $m])) < 1e-3 } {
    return [format %.4e $var([string tolower $m])]
  } else {
    return [format %.4g $var([string tolower $m])]
  }
  # return DELETE
}

proc ngspice::get_diff_probe {arr p m } {
  upvar $arr var
  set pp "v($p)"
  set mm "v($m)"
  if { ! [info exists var([string tolower $pp])] } {
    set pp $p
  }
  if { ! [info exists var([string tolower $mm])] } {
    set mm $m
  }
  return [format %.4g [expr $var([string tolower $pp]) - $var([string tolower $mm]) ] ]
  # return DELETE
}

proc ngspice::get_curr_probe {arr n } {
  upvar $arr var
  if { [xschem get currsch] > 0 } {
    set n "i(v.$n)"
  } else {
    set n "i($n)"
  }
  if {abs($var([string tolower $n])) <1e-3} {
    return [format %.4e $var([string tolower $n])]
  } else {
    return [format %.4g $var([string tolower $n])]
  }
  # return DELETE
}


proc ngspice::get_current {n} {
  set n [string tolower $n]
  set prefix [string range $n 0 0]
  set path [string range [xschem get sch_path] 1 end]
  set n $path$n
  if { $path ne {} } {
    set n $prefix.$n
   }
  if { ![regexp $prefix {[ve]}] } {
    set n @$n
  }
  set n i($n)
  # puts "ngspice::get_current --> $n"
  set err [catch {set ::ngspice::ngspice_data($n)} res]
  if { $err } {
    set res {?}
  } else {
    set res [ format %.4g $res ]
  }
  # puts "$n --> $res"
  return $res
}

proc ngspice::get_voltage {n} {
  set n [string tolower $n]
  set path [string range [xschem get sch_path] 1 end]
  set n v($path$n)
  set err [catch {set ::ngspice::ngspice_data($n)} res]
  if { $err } {
    # puts "get_ngspice_node: $res"
    set res {?}
  } else {
    set res [ format %.4g $res ]
  }
  return $res
}

proc ngspice::get_node {n} {
  set n [string tolower $n]
  set path [string range [xschem get sch_path] 1 end]
  # puts "ngspice::get_node  --> $n,  path=$path"
  set n [ subst -nocommand $n ]
  set err [catch {set ::ngspice::ngspice_data($n)} res]
  if { $err } { 
    set res {?}
  } else {
    set res [ format %.4g $res ]
  }
  return $res
}

proc ngspice::resetdata {} {
  array unset ::ngspice::ngspice_data
}

proc ngspice::annotate {} {
  upvar ::ngspice::ngspice_data arr
  set rawfile "[xschem get netlist_dir]/[file rootname [file tail [xschem get schname 0]]].raw"
  if { ![file exists $rawfile] } {
    puts "no raw file found: $rawfile"
    return
  }
  set fp [open $rawfile r]
  fconfigure $fp -translation binary
  set op_point_read 0 
  while 1 {
    ngspice::read_ngspice_raw arr $fp
    if { [info exists arr(n\ points)] } {
      if { $arr(n\ points) == 1 } {
        set op_point_read 1; break
      }
    } else break;
  }
  close $fp
  puts {Raw file read ...} 
  if { $op_point_read } {
    ### disable screen redraw and undo when looping to speed up performance
    ### but save state on undo stack before doing backannotations.
    xschem push_undo
    xschem set no_undo 1
    xschem set no_draw 1
    set lastinst [xschem get instances]
    set path [string range [xschem get sch_path] 1 end]
    for { set i 0 } { $i < $lastinst } {incr i } {
      set name [xschem getprop instance $i name]
      set type [xschem getprop instance $i cell::type]
      if { $type eq {probe} } {
        set net $path[xschem instance_net $i p]
        if {[catch {xschem setprop $i voltage [ngspice::get_voltage_probe arr $net] fast} err]} {
          puts "Warning 1: ${err}, net: $net"
        }
      }
      if { $type eq {current_probe} } {
        if {[catch {xschem setprop $i current [ngspice::get_curr_probe arr $path$name] fast} err]} {
          puts "Warning 2: $err"
        }
      }
      if { $type eq {differential_probe} } {
        set netp $path[xschem instance_net $i p]
        set netm $path[xschem instance_net $i m]
        if {[catch {xschem setprop $i voltage [ngspice::get_diff_probe arr $netp $netm] fast} err]} {
          puts "Warning 3: $err"
        }
      }
      # puts "$i $name $type"
    }
  
    # re-enable undo and draw
    xschem set no_undo 0
    xschem set no_draw 0
    xschem redraw
  
    ### xschem setprop instructions have not altered circuit topology so 
    ### in this case a connectivity rebuild is not needed.
    # xschem rebuild_connectivity
    #
    #
  } else {
    puts "no operating point found!"
  }
}

if { [info exists ::has_x] } {bind .drw <Alt-a> {puts {Annotating...}; ngspice::annotate} }
