#
#  File: ngspice_backannotate.tcl
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
      set nodename [string tolower [lindex $line 1]]
      regsub -all {:} $nodename {.} nodename
      set var($nodename) {}
      set idx([lindex $line 0]) $nodename
    }
    if {[regexp {^Variables:} $line]} {
      set variables 1
    }
  }
  if {$variables} {
    if { $n_points == 1} {
      set bindata [read $fp [expr {8 * $n_vars * $n_points}]]
      binary scan $bindata d[expr {$n_vars * $n_points}] data
      for {set p 0} {$p < $n_points} { incr p} {
        for {set v 0} {$v < $n_vars} { incr v} {
          lappend var($idx($v)) [lindex $data [expr {$p * $n_vars + $v}]]
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
  return [format %.4g [expr {$var([string tolower $pp]) - $var([string tolower $mm])} ] ]
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
  global path graph_raw_level
  set path [string range [xschem get sch_path] 1 end]
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $graph_raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  set prefix [string range $n 0 0]
  #puts "ngspice::get_current: path=$path n=$n"
  set n $path$n
  if { ![sim_is_xyce] } {
    if {$path ne {} } {
      set n $prefix.$n
    }
    if { ![regexp $prefix {[ve]}] } {
      set n @$n
    }
  }
  set n i($n)
  #puts "ngspice::get_current --> $n"
  set err [catch {set ngspice::ngspice_data($n)} res]
  if { $err } {
    set res {?}
  } else {
    if { abs($res) <1e-3 && $res != 0.0} {
      set res [ format %.4e $res ]
    } else {
      set res [ format %.4g $res ]
    }
  }
  # puts "$n --> $res"
  return $res
}

proc ngspice::get_voltage {n} {
  global path graph_raw_level
  set path [string range [xschem get sch_path] 1 end]
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $graph_raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  # puts "ngspice::get_voltage: path=$path n=$n"
  set node $path$n
  set err [catch {set ngspice::ngspice_data($node)} res]
  if {$err} {
    set node v(${path}${n})
    # puts "ngspice::get_voltage: trying $node"
    set err [catch {set ngspice::ngspice_data($node)} res]
  }
  if { $err } {
    set res {?}
  } else {
    if { abs($res) <1e-5} {
      set res 0
    } elseif { abs($res) <1e-3 && $res != 0.0} {
      set res [ format %.4e $res ]
    } else {
      set res [ format %.4g $res ]
    }
  }
  return $res
}

proc ngspice::get_node {n} {
  global path graph_raw_level
  set path [string range [xschem get sch_path] 1 end]
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $graph_raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  # n may contain $path, so substitute its value
  set n [ subst -nocommand $n ]
  set err [catch {set ngspice::ngspice_data($n)} res]
  if { $err } { 
    set res {?}
  } else {
    if { abs($res) <1e-3 && $res != 0.0} {
      set res [ format %.4e $res ]
    } else {
      set res [ format %.4g $res ]
    }
  }
  return $res
}

proc ngspice::resetdata {} {
  array unset ngspice::ngspice_data
}

proc ngspice::annotate {{f {}} {read_file 1}} {
  upvar ngspice::ngspice_data arr


  if { $read_file == 1} {
    if { $f eq {}} {
      set rawfile "$::netlist_dir/[file rootname [file tail [xschem get schname 0]]].raw"
    } else {
      set rawfile $f
    }
    if { ![file exists $rawfile] } {
      puts "no raw file found: $rawfile"
      return
    }
    set fp [open $rawfile r]
    fconfigure $fp -translation binary
    set op_point_read 0 
    ## not needed: done in ngspice::read_ngspice_raw
    # array unset ngspice::ngspice_data
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
  } else {
    set op_point_read 1
  }


  if { $op_point_read } {
    ### disable screen redraw and undo when looping to speed up performance
    ### but save state on undo stack before doing backannotations.

    if {0} {
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
          if {[catch {xschem setprop instance $i voltage [ngspice::get_voltage_probe arr $net] fast} err]} {
            puts "Warning 1: ${err}, net: $net"
          }
        }
        if { $type eq {current_probe} } {
          if {[catch {xschem setprop instance $i current [ngspice::get_curr_probe arr $path$name] fast} err]} {
            puts "Warning 2: $err"
          }
        }
        if { $type eq {differential_probe} } {
          set netp $path[xschem instance_net $i p]
          set netm $path[xschem instance_net $i m]
          if {[catch {xschem setprop instance $i voltage [ngspice::get_diff_probe arr $netp $netm] fast} err]} {
            puts "Warning 3: $err"
          }
        }
        # puts "$i $name $type"
      }
    
      # re-enable undo and draw
      xschem set no_undo 0
      xschem set no_draw 0
    
      ### xschem setprop instructions have not altered circuit topology so 
      ### in this case a connectivity rebuild is not needed.
      # xschem rebuild_connectivity
      #
      #
    } ;# if {0}
    xschem redraw
  } else {
    puts "no operating point found!"
  }
}

# if { [info exists ::has_x] } {bind .drw <Alt-a> {puts {Annotating...}; ngspice::annotate} }
