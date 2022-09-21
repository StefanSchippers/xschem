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
  variable op_point_read
}

proc ngspice::read_raw_dataset {arr fp} {
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

proc ngspice::read_raw {{f {}}} {
  upvar ngspice::ngspice_data arr

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
  set ngspice::op_point_read 0 
  ## not needed: done in ngspice::read_ngspice_raw
  # array unset ngspice::ngspice_data
  while 1 {
    ngspice::read_raw_dataset arr $fp
    if { [info exists arr(n\ points)] } {
      if { $arr(n\ points) == 1 } {
        set ngspice::op_point_read 1; break
      }
    } else break;
  }
  close $fp
  puts {Raw file read ...} 
  if { !$ngspice::op_point_read } {
    puts "no operating point found!"
  }
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

proc ngspice::get_diff_voltage {n m} {
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
  set m [string tolower $m]
  set nn $path$n
  set mm $path$m
  set errn [catch {set ngspice::ngspice_data($nn)} resn]
  if {$errn} {
    set nn v(${path}${n})
    set errn [catch {set ngspice::ngspice_data($nn)} resn]
  }
  set errm [catch {set ngspice::ngspice_data($mm)} resm]
  if {$errm} {
    set mm v(${path}${m})
    set errm [catch {set ngspice::ngspice_data($mm)} resm]
  }
  if { $errn  || $errm} {
    set res {?}
  } else {
    set res [expr {$resn - $resm}]
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

# if { [info exists ::has_x] } {bind .drw <Alt-a> {puts {Annotating...}; ngspice::annotate} }
