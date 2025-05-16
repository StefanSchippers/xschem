#
#  File: xschem.tcl
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

### INUTILE integration (spice stimuli generator from a higher level description language)
proc inutile_line {txtlabel} {
   global retval
   toplevel .inutile_line -class Dialog
   set X [expr [winfo pointerx .inutile_line] - 60]
   set Y [expr [winfo pointery .inutile_line] - 35]
   wm geometry .inutile_line "+$X+$Y"
   wm transient .inutile_line [xschem get topwindow]
   label .inutile_line.l1  -text $txtlabel
   entry .inutile_line.e1   -width 60
   entry_replace_selection .inutile_line.e1
   .inutile_line.e1 delete 0 end
   .inutile_line.e1 insert 0 $retval
   button .inutile_line.b1 -text "OK" -command  \
   {
     set retval [.inutile_line.e1 get ]
     destroy .inutile_line
   }
   bind .inutile_line <Return> {
     set retval [.inutile_line.e1 get ]
     destroy .inutile_line
   }
   pack .inutile_line.l1 -side top -fill x
   pack .inutile_line.e1  -side top -fill both -expand yes
   pack .inutile_line.b1 -side top -fill x
   grab set .inutile_line
   focus .inutile_line.e1
   tkwait window .inutile_line
   return $retval
}

proc inutile_write_data {w f} {
 set fid [open $f w]
 set t [$w get  0.0 {end - 1 chars}]
 puts  -nonewline $fid $t 
 close $fid
}
  
proc inutile_read_data {w f} {
 set fid [open $f r]
 set t [read $fid]
 $w delete 0.0 end
 $w insert 0.0 $t
 close $fid
}

proc inutile_template {w f} {
 set fid [open $f r]
 set t [read $fid]
 $w insert 0.0 $t
 close $fid
}

proc inutile_get_time {} {
 global netlist_dir
 regsub {/$} $netlist_dir {} netlist_dir
 set fileid [open "$netlist_dir/inutile.simulationtime"  "RDONLY"]
 .inutile.buttons.time delete 0 end
 .inutile.buttons.time insert 0 [read -nonewline $fileid]
 close $fileid
 file delete "$netlist_dir/inutile.simulationtime"
}
 
proc inutile_alias_window {w filename} {
 catch {destroy $w}
 toplevel $w -class Dialog
 wm title $w "(IN)UTILE ALIAS FILE: $filename"
 wm iconname $w "ALIAS"
 # wm transient $w [xschem get topwindow]

 set fileid [open $filename "RDONLY CREAT"]
 set testo [read $fileid]
 close $fileid
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 text $w.text -undo 1 -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30
 scrollbar $w.scroll -command "$w.text yview"
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "inutile_write_data $w.text \"$filename\""
 button $w.buttons.load -text Reload -command "inutile_read_data $w.text \"$filename\""
 pack $w.buttons.dismiss $w.buttons.save $w.buttons.load -side left -expand 1
 
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 $w.text insert 0.0 $testo
} 

proc inutile_help_window {w filename} {
 catch {destroy $w}
 toplevel $w -class Dialog
 wm title $w "(IN)UTILE ALIAS FILE"
 wm iconname $w "ALIAS"
 # wm transient $w [xschem get topwindow]
 
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "inutile_write_data $w.text \"$filename\""
 pack $w.buttons.dismiss  $w.buttons.save -side left -expand 1
 
 text $w.text -undo 1 -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30 -width 90
 scrollbar $w.scroll -command "$w.text yview"
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 set fileid [open $filename "RDONLY CREAT"]
 $w.text insert 0.0 [read $fileid]
 close $fileid
} 

proc inutile_translate {f} {
  global XSCHEM_SHAREDIR netlist_dir
  regsub {/$} $netlist_dir {} netlist_dir
  set p $XSCHEM_SHAREDIR/utile
  set savedir [pwd]
  cd $netlist_dir
  eval exec awk -f $p/preprocess.awk \"$f\" | awk -f $p/expand_alias.awk | awk -f $p/param.awk | awk -f $p/clock.awk | awk -f $p/stimuli.awk
  cd $savedir
}

proc inutile { {filename {}} {wait {}} } {
  global XSCHEM_SHAREDIR retval netlist_dir

  if {$wait ne {}} {
    xschem set semaphore [expr {[xschem get semaphore] +1}]
  }
  if { ![string compare $filename  ""]  } then {
   tk_messageBox -type ok -message "Please give a file name as argument"
   return
  }
  toplevel .inutile -class Dialog
  wm title .inutile "(IN)UTILE (Stefan Schippers, sschippe)"
  wm iconname .inutile "(IN)UTILE"
  # wm transient .inutile [xschem get topwindow]
  set utile_path $XSCHEM_SHAREDIR/utile
  set retval {}
  frame .inutile.buttons
  pack .inutile.buttons -side bottom -fill x -pady 2m
  button .inutile.buttons.translate -text Translate -command "
    inutile_write_data .inutile.text \"$filename\"
    inutile_translate \"$filename\"
    inutile_get_time"
  button .inutile.buttons.dismiss -text Dismiss -command "destroy .inutile"
  button .inutile.buttons.code -text "Help" -command "inutile_help_window .inutile.help $utile_path/utile.txt"
  text .inutile.text -undo 1 -relief sunken -bd 2 -yscrollcommand ".inutile.scroll set" -setgrid 1 -height 30
  scrollbar .inutile.scroll -command {.inutile.text yview}
  button .inutile.buttons.save -text Save -command "
    set retval \"$filename\"
    set filename \[inutile_line {Filename}\]
    inutile_write_data .inutile.text \"$filename\""
  button .inutile.buttons.load -text Reload -command "
    set retval \"$filename\"
    set filename \[inutile_line {Filename}\]
    inutile_read_data .inutile.text \"$filename\""
  button .inutile.buttons.send -text "Template" -command "
    if { !\[string compare \[.inutile.text get 0.0 {end - 1 chars}\]  {}\]} {
      inutile_template  .inutile.text  $utile_path/template.stimuli}"
  label .inutile.buttons.timelab -text "time:"
  entry .inutile.buttons.time  -width  11
  entry_replace_selection .inutile.buttons.time
  pack .inutile.buttons.dismiss .inutile.buttons.code \
       .inutile.buttons.load .inutile.buttons.save .inutile.buttons.translate \
       .inutile.buttons.send .inutile.buttons.timelab \
       .inutile.buttons.time  -side left -expand 1
  pack .inutile.scroll -side right -fill y
  pack .inutile.text -expand yes -fill both
  if { [file exists $filename] }  {  
    set fileid [open $filename "RDONLY"]
    .inutile.text insert 0.0 [read $fileid]
    close $fileid
  }
  set tmp [.inutile.text index end]
  regsub {\..*$} $tmp {} lines
  for {set i 1} {$i <= $lines} {incr i} {
   set tmp [.inutile.text get $i.0 "$i.0 lineend"]
   if [regexp {^(include)|(\.include)} $tmp  ] { 
    inutile_alias_window .inutile.tw$i [lindex $tmp 1] 
   }
  } 
  if {$wait ne {}} {
    tkwait window .inutile
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
}

### End INUTILE integration

### for tclreadline: disable customcompleters
proc completer { text start end line } { return {}}

# set 'var' with '$val' if 'var' not existing
proc set_ne { var val } {
    upvar #0 $var v
    if { ![info exists v ] } {
      set v $val
    }
}


# $execute(id) is an integer identifying the last running pipeline
#       set id $execute(id) to get the current subprocess id in variable 'id'
# $execute(status,$id) contains the status argument as given in proc execute
# $execute(pipe,$id) contains the channel descriptor to read or write as specified in status
# $execute(data,$id) contains the stdout of the pipeline (output data)
# $execute(cmd,$id) contains the pipeline command
# $execute(win_path,$id) contains the xctx->current_win_path that started the command
# $execute(exitcode,id) contains the exit code. This variable will not be deleted at the end
# when subprocess ends all execute(...,$id) data is cleared (except execute(exitcode,id)
#
# The following post-mortem data is available for last finished process:
#   execute(cmd,last)     : the command
#   execute(win_path,last): the xctx->current_win_path that started the last command
#   execute(data,last)    : the data
#   execute(error,last)   : the errors (stderr) 
#   execute(status,last)  : the status argument as was given when calling proc execute
#   execute(exitcode,last): exit code of last finished process
#
# execute service function
proc execute_fileevent {id} {
  global execute OS has_x errorCode

  append execute(data,$id) [read $execute(pipe,$id) 1024]
  if { $OS != {Windows} } {
    set eof [eof $execute(pipe,$id)]
    # handle processes that close stdout. Read pipe will go into eof condition
    # but process is still running. Doing a close operation in blocking mode
    # will block execution until process exits.
    # In this situation we avoid setting pipe to blocking mode and do an
    # asynchronous close. We lose exit status and stderr though, but
    # avoid the program to freeze waiting for process to exit.
    set lastproc [lindex [pid $execute(pipe,$id)] end]
    set ps_status [exec ps -o state= -p $lastproc]
    set finished [regexp Z $ps_status] ;# if zombie consider process to be finished.
    if { $eof && !$finished} {
      after 2000 "execute_fileevent $id"
      fileevent $execute(pipe,$id) readable ""
      # puts "rescheduling $id"
      return
    }
  } else {
    set eof [eof $execute(pipe,$id)]
    set finished 1
  }
  if {$eof} {
      set report [regexp {1} $execute(status,$id)]
      fileevent $execute(pipe,$id) readable ""
      # setting pipe to blocking before closing allows to get pipeline exit status
      # do not ask status for processes that close stdout/stderr, as eof might
      # occur before process ends and following close blocks until process terminates.
      if {$finished} {fconfigure $execute(pipe,$id) -blocking 1}
      set exit_status 0
      set catch_return [eval catch [list {close $execute(pipe,$id)} err] ]
      # puts "catch_return=$catch_return"
      if {$catch_return} {
        # puts "errorCode=$errorCode"
        if {"NONE" == [lindex $errorCode 0]} {
          set exit_status 0
        } elseif {"CHILDSTATUS" == [lindex $errorCode 0]} {
          set exit_status [lindex $errorCode 2]
          # puts "exit_status=$exit_status"
        } else {
          set exit_status  $catch_return
        }
        if {$exit_status != 0} {
          if {$report} {viewdata "Failed: $execute(cmd,$id)\nstderr:\n$err\ndata:\n$execute(data,$id)"}
        } else {
          if {$report} {viewdata "Completed: $execute(cmd,$id)\nstderr:\n$err\ndata:\n$execute(data,$id)"}
        }
      } else {
        if {$report} {viewdata "Completed: $execute(cmd,$id)\ndata:\n$execute(data,$id)"}
      }
      set execute(exitcode,last) $exit_status
      set execute(exitcode,$id) $exit_status
      set execute(cmd,last) $execute(cmd,$id)
      set execute(win_path,last) $execute(win_path,$id)
      set execute(data,last) $execute(data,$id)
      if { ![info exists err] } { set err {} }
      set execute(error,last) $err
      set execute(status,last) $execute(status,$id)

      if { [info exists tctx::$execute(win_path,$id)_simulate_id] } {
        if { [set tctx::$execute(win_path,$id)_simulate_id] eq $id } {
          unset tctx::$execute(win_path,$id)_simulate_id
        }
      }

      if {[info exists execute(callback,$id)] && $execute(callback,$id) ne {}} {
        # puts  $execute(callback,$id)
        # puts $execute(win_path,$id)
        eval uplevel #0 [list $execute(callback,$id)]
      }
      catch {unset execute(callback,$id)} 

      if { ![info exists exit_status] } { set exit_status 0 }
      unset execute(pipe,$id)
      unset execute(data,$id)
      unset execute(status,$id)
      unset execute(cmd,$id)
      unset execute(win_path,$id)
      # apply a delay, process does not disappear immediately.
      if {[info exists has_x] && [winfo exists .processlist]} { after 250 {insert_running_cmds .processlist.f2.lb}}
  }
}

proc execute_wait {status args} {
  global execute 
  set id [eval execute $status $args]
  if {$id == -1} {
    return -1
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  vwait execute(pipe,$id)
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $id
}

# equivalent to the 'exec' tcl function but keeps the event loop
# responding, so widgets get updated properly
# while waiting for process to end.
# status:
#   rw     open pipe in 'r+' (read write) mode instead of 'r'
#   line   set line buffering mode of channel
#   none   set no channel buffering. 
#   1      get status report at process end
#   0      no status report
#
# These options can be combined as in '1rwline' of '1rnone'
#
proc execute {status args} {
  global execute has_x
  if {![info exists execute(id)]} {
      set execute(id) 0
  } else {
      incr execute(id)
  }
  set id $execute(id)
  if { [info exists execute(callback)] } {
    set execute(callback,$id) $execute(callback)
    unset execute(callback)
  }
  set mode r
  if {[regexp {rw} $status]} {
    set mode r+
  }
  if { [catch {open "|$args" $mode} err] } {
    puts stderr "Proc execute error: $err"
    if { [info exists has_x]} {
        tk_messageBox -message  \
          "Can not execute '$args': ensure it is available on the system. Error: $err" \
           -icon error -parent [xschem get topwindow] -type ok
    }
    return -1
  } else {
    set pipe $err
  }
  set execute(status,$id) $status
  set execute(pipe,$id) $pipe
  set execute(cmd,$id) $args
  set execute(win_path,$id) [xschem get current_win_path]
  set execute(data,$id) ""

  # Apply a delay to catch the new process.
  if {[info exists has_x] && [winfo exists .processlist]} { after 250 {insert_running_cmds .processlist.f2.lb}} 
  fconfigure $pipe -blocking 0
  if {[regexp {line} $status]} {
    fconfigure $pipe -buffering line
  }
  if {[regexp {none} $status]} {
    fconfigure $pipe -buffering none
  }
  fileevent $pipe readable "execute_fileevent $id"
  return $id
}

# kill selected sub-processes by looking up their command strings
# into all running sub-processes, killing the matching ones
# with the supplied 'sig'.
proc kill_running_cmds {{lb {}} sig} {
  global execute
  if { [regexp {^[0-9]+$} $lb] && $lb >= 0 } {
    set id $lb
    if { [info exists execute(pipe,$id)] } {
      set pid [pid $execute(pipe,$id)]
      exec kill $sig $pid
    } else {
      return 0
    }
  } else {
    set selected [$lb curselection]
    foreach idx $selected {
      set cmd1 [$lb get $idx]
      foreach  {id pid cmd2} [get_running_cmds] {
        # puts "$cmd1\n$cmd2 \n$pid"
        if { $cmd1 eq $cmd2 } {
          exec kill $sig $pid
          break
        }
      }
    }
    after 250 insert_running_cmds $lb
  }
  # apply a delay, after a kill command process does not disappear
  # immediately.
  return 1
}

# refresh list of running commands in dialog box
proc insert_running_cmds {lb} {
  $lb delete 0 end
  foreach {id pid cmd} [get_running_cmds] {
    # puts "inserting $cmd"
    $lb insert end $cmd
  }
}

# periodically update status
proc update_process_status {lb} {
  global execute has_x
  set exists 0
  set selected [$lb curselection]
  if { ![info exists has_x]} {return}
  if { [winfo exists .pstat] } {
    set pos [lindex [.pstat.text yview] 1]
    if {$pos == 1} {
      if { $selected ne {} && [llength $selected] == 1} {
        .pstat.text delete 1.0 end
        set idx $selected
        set cmd1 [$lb get $idx]
        foreach  {id pid cmd2} [get_running_cmds] {
          if { $cmd1 eq $cmd2 } {
            if {[catch { set t $execute(data,$id) } err]} {
              set t $err
            }
            .pstat.text insert 1.0 $t
            .pstat.text yview moveto 1
            break
          }
        }
      }
    }
    after 1000 "update_process_status $lb"
  } else {
   after cancel "update_process_status $lb"
  }
}


# display stdout of selected sub-process
proc view_process_status {lb} {
  global execute has_x
  set exists 0
  if {![info exists has_x]} {return}
  if {[winfo exists .pstat] } {
    .pstat.text delete 1.0 end
    set exists 1
  }
  set selected [$lb curselection]
  if {$selected ne {} && [llength $selected] == 1} {
    set idx $selected
    set cmd1 [$lb get $idx]
    foreach  {id pid cmd2} [get_running_cmds] {
      if { $cmd1 eq $cmd2 } {
        if {[catch { set t $execute(data,$id) } err]} {
          set t $err
        }
        if {$exists == 0} {
          viewdata $t ro .pstat
        } else {
         .pstat.text insert 1.0 $t
        }
        .pstat.text yview moveto 1
        break
      }
    }
  }
  after 1000 "update_process_status $lb"
}

proc list_running_cmds_title {} {
  if {[winfo exists .processlist]} {
    wm title .processlist "List of running commands - [xschem get current_name]"
    after 1000 "list_running_cmds_title"
  }
}

# top level dialog displaying running sub-processes
proc list_running_cmds {} {
  global has_x
  set top .processlist
  if {![info exists has_x]} {return}
  if {[winfo exists $top]} {raise $top; return}
  toplevel $top -class Dialog
  list_running_cmds_title
  # wm transient $top [xschem get topwindow]
  set frame1 $top.f1
  set frame2 $top.f2
  set frame3 $top.f3
  frame $frame1
  frame $frame2
  frame $frame3
  set lb $frame2.lb
  listbox $lb -width 70 -height 8 -selectmode extended \
     -yscrollcommand "$frame2.yscroll set" \
     -xscrollcommand "$frame2.xscroll set"
  scrollbar $frame2.yscroll -command "$lb yview"
  scrollbar $frame2.xscroll -orient horiz -command "$lb xview"
  pack  $frame2.yscroll -side right -fill y
  pack  $frame2.xscroll -side bottom -fill x
  pack  $lb -side bottom  -fill both -expand true

  bind $lb <Double-Button-1> [list $frame3.b3 invoke]
  button $frame3.b1 -width 16 -text {Terminate selected} -command "kill_running_cmds $lb -15" \
    -fg black -background yellow
  button $frame3.b2 -width 16 -text {Kill selected} -command "kill_running_cmds $lb -9" \
    -fg black -background red
  button $frame3.b3 -width 16 -text {View status} -command "view_process_status $lb" \
    -fg black -background PaleGreen
  button $frame3.b4 -width 16 -text {Dismiss} \
    -fg black -background PaleGreen -command "
    if {\[winfo exists .pstat\]} {
      after cancel [list update_process_status $lb]
      destroy .pstat
    }
    destroy $top

  "

  pack $frame3.b1 $frame3.b2 $frame3.b3 $frame3.b4 -side left -fill x -expand 1
  pack $frame1 -fill x -expand 0
  pack $frame2 -fill both -expand 1
  pack $frame3 -fill x -expand 0

  insert_running_cmds $lb
}

# for each running sub-process return a list of three elements per process: 
# the integer id, the process PID, the command string.
proc get_running_cmds {} {
  global execute
  set ret {}
  foreach i [array names execute *pipe*] {
    set id [lindex [split $i ,] 1]
    lappend ret $id  [pid $execute($i)] $execute(cmd,$id)
  } 
  return $ret
}

# pause for $del_ms milliseconds, keep event loop responsive
proc delay {del_ms} {
  global delay_flag
  after $del_ms {set delay_flag 1}
  vwait delay_flag
  unset delay_flag
}  

#### Scrollable frame 
proc sframeyview {container args} {
  global ${container}_vpos ;# global to remember scrollbar position
  set_ne ${container}_vpos 0
  if {[lindex $args 0] eq {place}} {
    place ${container}.f.scrl -in $container.f -x 0 -y 0 -relwidth 1
    update ;# without this vpos of scrollbar will not be remembered when reopening toplevel
  }
  set ht [winfo height $container.f]
  set hs [winfo height $container.f.scrl]
  set frac [expr {double($ht)/$hs}]
  if { [lindex $args 0] eq {scroll}} { ;# mouse wheel
    set ${container}_vpos [expr {[set ${container}_vpos] + [lindex $args 1] *(1.0/$frac)/5}]
  } elseif { [lindex $args 0] eq {moveto}} { ;# scrollbar slider
    set ${container}_vpos [lindex $args 1]
  }
  if { [set ${container}_vpos] < 0.0 } {set ${container}_vpos 0.0}
  if { [set ${container}_vpos] > 1.0 - $frac } {set ${container}_vpos [expr {1.0 - $frac}]}
  $container.vs set [set ${container}_vpos] [expr {[set ${container}_vpos] + $frac}]
  place $container.f.scrl -in $container.f -x 0 -y [expr {-$hs * [set ${container}_vpos]}] -relwidth 1.0
}

# scrollable frame constructor
proc sframe {container} {
  frame $container.f
  scrollbar $container.vs -command "sframeyview $container" ;# sframeyview moveto commands
  frame $container.f.scrl
  pack $container.f -expand yes -fill both -side left
  pack $container.vs -expand yes -fill y
  return $container.f.scrl
}
#### /Scrollable frame

## convert engineering form to number
proc from_eng {i} {
  set str {}
  scan $i "%g%s" n str
  set str [string tolower $str]
  if { [regexp {^meg} $str] } { set str {meg} } else {
    set suffix [string index $str 0]
  }
  set mult [switch $suffix {
    a         { expr {1e-18}}
    f         { expr {1e-15}}
    p         { expr {1e-12}}
    n         { expr { 1e-9}}
    u         { expr {1e-6}}
    m         { expr {1e-3}}
    k         { expr {1e3}}
    x         { expr {1e6}} ;# Xyce extension
    meg       { expr {1e6}}
    g         { expr {1e9}}
    t         { expr {1e12}}
    default   { expr {1.0}}
  }]
  return [expr {$n * $mult}]
}

## convert number to engineering form
proc to_eng {args} {
  set suffix {}

  if {[ catch {uplevel #0 expr [join $args]} i]} {
    return [join $args]
  }
  set i [uplevel #0 expr [join $args]]
  set absi [expr {abs($i)}]

  if       {$absi == 0.0}  { set mult 1    ; set suffix {}
  } elseif {$absi >=1e12}  { set mult 1e-12; set suffix T
  } elseif {$absi >=1e9}   { set mult 1e-9 ; set suffix G
  } elseif {$absi >=1e6}   { set mult 1e-6 ; set suffix M
  } elseif {$absi >=1e3}   { set mult 1e-3 ; set suffix k
  } elseif {$absi >=0.1}   { set mult 1    ; set suffix {}
  } elseif {$absi >=1e-3}  { set mult 1e3  ; set suffix m
  } elseif {$absi >=1e-6}  { set mult 1e6  ; set suffix u
  } elseif {$absi >=1e-9}  { set mult 1e9  ; set suffix n
  } elseif {$absi >=1e-12} { set mult 1e12 ; set suffix p
  } elseif {$absi >=1e-15} { set mult 1e15 ; set suffix f
  } else                   { set mult 1e18 ; set suffix a}
  if {$suffix ne {}} {
    set i [expr {$i * $mult}]
    set s [format  {%.5g%s} $i $suffix]
  } else {
    set s [format  {%.5g} $i]
  }
  return $s
}

## evaluate expression with 7 significant digits.
## if expression has errors or does not evaluate return expression as is
proc ev7 {args} {
  set i [join $args]
  if {![catch {uplevel #0 expr $i} res]} {
    return [format %.7g $res]
  } else { 
    # puts stderr "proc ev: $res"
    return $args
  }
}   

## evaluate expression. if expression has errors or does not evaluate return expression as is
proc ev {args} {
  set i [join $args]
  if {![catch {uplevel #0 expr $i} res]} {
    return [format %.4g $res]
  } else {
    # puts stderr "proc ev: $res"
    return $args
  }
}

## evaluate expression. if expression has errors or does not evaluate return 0
proc ev0 {args} {
  set i [join $args]
  if {![catch {uplevel #0 expr $i} res]} {
    return [format %.4g $res]
  } else {
    # puts stderr "proc ev0: $res"
    return 0
  }
} 

# create the 'lassign' command if tcl does not have it (pre 8.5)
if {[info commands lassign] eq {}} {
  proc lassign { list args } {
    set n 0
    foreach i $args {
      upvar $i v
      set v [lindex $list $n]
      incr n
    }
    return [lrange $list $n end]
  }
}

# return "$n * $indent" spaces
proc spaces {n {indent 4}} {
  set n [expr {$n * $indent}]
  # return [format %${n}s {}]
  return [string repeat { } $n]
}

# complex number operators
# a + b
proc cadd {a b} {
  lassign $a ra ia
  lassign $b rb ib
  set c [list [expr {$ra + $rb}] [expr {$ia + $ib}]]
  return $c
}

# a - b
proc csub {a b} {
  lassign $a ra ia
  lassign $b rb ib
  set c [list [expr {$ra - $rb}] [expr {$ia - $ib}]]
  return $c
}

# a * b
proc cmul {a b} {
  lassign $a ra ia
  lassign $b rb ib
  set c [list [expr {$ra * $rb - $ia * $ib}] [expr {$ra * $ib + $rb * $ia}]]
  return $c
}

# a / b
proc cdiv {a b} {
  lassign $a ra ia
  lassign $b rb ib
  set ra [expr {double($ra)}]
  set ia [expr {double($ia)}]
  set rb [expr {double($rb)}]
  set ib [expr {double($ib)}]
  set m [expr {$rb * $rb + $ib * $ib}]
  set c [list [expr {($ra * $rb + $ia * $ib) / $m}] [expr {($rb * $ia - $ra * $ib) / $m}]]
  return $c
}

# 1/b
proc cinv {b} {

  lassign $b rb ib
  set rb [expr {double($rb)}]
  set ib [expr {double($ib)}]
  set m [expr {$rb * $rb + $ib * $ib}]
  set c [list [expr {$rb / $m}] [expr {-$ib / $m}]]
  return $c
}

# return real component
proc creal {a} {
  lassign $a ra ia
  return $ra
}

# return imaginary component
proc cimag {a} {
  lassign $a ra ia
  return $ia
}

# return resulting impedance of parallel connected impedances a and b
proc cparallel {a b} {
  return [cdiv [cmul $a $b] [cadd $a $b]]
}

# wraps provided table formatted text into a nice looking bordered table 
# sep is the list of characters used as separators, default are { }, {,}, {\t}
# if you want to tabulate data with spaces use only {,} as separator or any other character.
proc tabulate {text {sep ",\t "}} {
  # define table characters
  set top  {┌  ─  ┬ ┐}
  set row  {│ { } │ │}
  set mid  {├  ─  ┼ ┤}
  set head {╞  ═  ╪ ╡}
  set bot  {└  ─  ┴ ┘}

  # used_sep (first character of sep) will be used as separator after text cleanup
  set used_sep [string range $sep 0 0]
  set sep "\[$sep\]"
  set maxcols 0 ;# max number of columns
  set nlines 0  ;# number of data lines
  set chopped_text {}
  set found_data 0
  foreach line [split $text \n] {

    # skip completely empty lines
    if {[regexp {^$} $line]} { continue}
    # skip leading lines with no data (only separators) 
    if {!$found_data && [regexp ^${sep}*\$ $line]} {
      continue
    }
    set found_data 1

    # change separators to { }
    regsub -all $sep $line $used_sep line

    # transform resulting line into a proper list 
    set line [split $line $used_sep]
    incr nlines
    set ncols 0
    # calculate max field width and number of columns
    foreach field $line { 
      incr ncols
      if {![info exists maxlen($ncols)]} {set maxlen($ncols) 0}
      if {$ncols > $maxcols} {set maxcols $ncols} 
      set len [string length $field] 
      if {$len > $maxlen($ncols)} { set maxlen($ncols) $len}
    }
    if { $chopped_text ne {}} {append chopped_text \n}
    append chopped_text $line
  }

  set table {}
  set l 0
  foreach line [split $chopped_text \n] {
    incr l

    # top table border
    set rowsep {}
    if {$l == 1} {
      append rowsep [lindex $top 0] 
      set c 0
      for {set i 0} {$i < $maxcols} { incr i} {
        set field [lindex $line $i]
        incr c
        append rowsep [string repeat [lindex $top 1] $maxlen($c)]
        if { $c == $ncols} {
          append rowsep [lindex $top 3]
        } else {
          append rowsep [lindex $top 2]
        }
      }
      append table $rowsep \n
    } else {
      append table \n
    }
    set rowsep {}
    append table [lindex $row 0]
    if { $l == $nlines} {
      append rowsep [lindex $bot 0]
    } elseif {$l == 1} {
      append rowsep [lindex $head 0]
    } else {
      append rowsep [lindex $mid 0]
    }
    set c 0
    for {set i 0} {$i < $maxcols} { incr i} {
      set field [lindex $line $i]
      incr c
      set pad [expr {$maxlen($c) - [string length $field]}]
      append table $field [string repeat { } $pad]
      if { $l == $nlines} {
        append rowsep [string repeat [lindex $bot 1] $maxlen($c)]
      } elseif {$l == 1}  {
        append rowsep [string repeat [lindex $head 1] $maxlen($c)]
      } else {
        append rowsep [string repeat [lindex $mid 1] $maxlen($c)]
      }
      if { $c == $ncols} {
        append table [lindex $row 3]
        if { $l == $nlines} {
          append rowsep [lindex $bot 3]
        } elseif {$l == 1} {
          append rowsep [lindex $head 3]
        } else {
          append rowsep [lindex $mid 3]
        }
      } else {
        append table [lindex $row 2]
        if { $l == $nlines} {
          append rowsep [lindex $bot 2]
        } elseif { $l == 1}  {
          append rowsep [lindex $head 2]
        } else {
          append rowsep [lindex $mid 2]
        }
      }
    }
    append table \n $rowsep 
  }
  return $table
}

# get pin ordering from included subcircuit
# return empty string if not found.
proc has_included_subcircuit {symname spice_sym_def no_of_pins} {
  global has_x
  set include_found 0
  regsub -all {\n\+} $spice_sym_def { } spice_sym_def
  # puts "has_included_subcircuit: spice_sym_def=$spice_sym_def"
  set pinlist {}
  # .include? get the file ...
  if {[regexp -nocase {^[ \t\n]*\.include +} $spice_sym_def]} {
    regsub -nocase {^[ \t\n]*\.include +} $spice_sym_def {} filename 
    regsub -all {^"|"$} $filename {} filename ;# remove double quotes at beginning and end
    regsub -all {^\n*|\n*$} $filename {} filename ;# remove newlines at beginning and end
    set filename [abs_sym_path $filename]
    # puts "filename=$filename"
    set res [catch {open $filename r} fd]
    if { $res } {
      puts "has_included_subcircuit: error opening file $filename: $fd"
      if { [info exists has_x] } {alert_ "has_included_subcircuit: error opening file $filename: $fd"}
      return $pinlist
    }  
    set spice_sym_def [read -nonewline $fd]
    close $fd
    regsub -all {\n\+} $spice_sym_def { } spice_sym_def
  }
  # ... or use the attribute value as *the* subcircuit

  # split lines
  set spice_sym_def [split $spice_sym_def \n]
  foreach line $spice_sym_def {
    # get 1st line with a matching .subckt. This is our subcircuit definition, get the pin order
    if {[string tolower [lindex $line 0]] eq {.subckt}  && 
        [string tolower [lindex $line 1]] eq $symname} {
     set include_found 1
     regsub -all { *= *} $line {=} line
     # pin list ends where parameter assignment begins (param=value)
     set last [lsearch -regexp [string tolower $line] {=|param:}]
     if {$last == -1} {
       set last [expr {[llength $line] -1}]
     } else {
       set last [expr {$last -1}]
     }
     # if spice_sym_def has more ports than symbol (no_of_pins) assume these are extra ports 
     # assigned via attributes ('extra' symbol attribute, usually power rails), these extra
     # ports are not returned as @pinlist and correspondence with symbol format string
     # is not checked. user must ensure these additional ports are in the right order.
     if { $last >= $no_of_pins + 1 } {
       set last [expr {$no_of_pins + 1}]
     }
     set pinlist [lrange $line 2 $last]
     break
    }
  }
  # return pinlist as found in the .subckt line or empty string if not found
  if {!$include_found} {
    puts "has_included_subcircuit: $symname: no matching .subckt found in spice_sym_def. Ignore"
    if { [info exists has_x] } {
      alert_ "has_included_subcircuit: $symname: no matching .subckt found in spice_sym_def. Ignore"
    }
  }  
  # puts $pinlist
  return [string tolower [join $pinlist]]
} 

# should not be called directly by user 
# does netlist post processing, called from global_(spice|vhdl|verilog)_netlist()
proc netlist {source_file show netlist_file} {
 global XSCHEM_SHAREDIR flat_netlist netlist_dir simulate_bg
 global verilog_2001 debug_var OS has_x verilog_bitblast

 regsub {/$} $netlist_dir {} netlist_dir
 # if waves are loaded turn Waves button to orange to indicate old waves are shown
 set win_path [xschem get current_win_path]
 set top_path [xschem get top_path]
 set waves_var tctx::${win_path}_waves
 if {[info exists has_x] && $waves_var ne $simulate_bg} {
   set $waves_var orange
    $top_path.menubar entryconfigure Waves -background orange
 }
 
 set netlist_type [xschem get netlist_type]
 if {$debug_var <= -1} { puts "netlist: source_file=$source_file, netlist_type=$netlist_type" }
 set dest $netlist_dir/$netlist_file
 if {$netlist_type eq {spice}} {
   if { [sim_is_xyce] } {
     set xyce  {-xyce}
   } else {
     set xyce  {}
   }
   set cmd  ${XSCHEM_SHAREDIR}/spice.awk
   set brk ${XSCHEM_SHAREDIR}/break.awk
   set flatten ${XSCHEM_SHAREDIR}/flatten.awk
   if {$flat_netlist==0} {
     eval exec {awk -f $cmd -- $xyce $source_file | awk -f $brk > $dest}
   } else {
     eval exec {awk -f $cmd -- $xyce $source_file | awk -f $flatten | awk -f $brk > $dest}
   }
   if ![string compare $show "show"] {
      textwindow $dest
   }
 } 
 if {$netlist_type eq {vhdl}} {
   set cmd $XSCHEM_SHAREDIR/vhdl.awk
   eval exec {awk -f $cmd $source_file > $dest}
   if ![string compare $show "show"] {
     textwindow $dest
   }
 }
 if {$netlist_type eq {tedax}} {
    set cmd1  $XSCHEM_SHAREDIR/tedax.awk
    set cmd2 $XSCHEM_SHAREDIR/flatten_tedax.awk
    if {[catch {eval exec {awk -f $cmd1 $source_file | awk -f $cmd2 > $dest} } err] } {
     puts stderr "tEDAx errors: $err"
   }
   if ![string compare $show "show"] {
     textwindow $dest
   }
 }
 if {$netlist_type eq {verilog}} {
   set cmd  ${XSCHEM_SHAREDIR}/verilog.awk
   if { $verilog_bitblast == 1 } {
     eval exec {awk -f $cmd -- -bitblast $source_file > $dest}
   } else {
     eval exec {awk -f $cmd $source_file > $dest}
   }
   if { $verilog_2001==1 } { 
     set cmd ${XSCHEM_SHAREDIR}/convert_to_verilog2001.awk
     set interm ${dest}[pid]
     eval exec {awk -f $cmd $dest > $interm}
     file rename -force $interm $dest
   }
   if ![string compare $show "show"] {
     textwindow "$dest"
   }
 }
 return {}
}

# 20161121
proc convert_to_pdf {filename dest} {
  global to_pdf OS execute
  if { [regexp -nocase {\.pdf$} $dest] } {
    set pdffile [file rootname $filename].pdf
    # puts "--->  cmd=$to_pdf  filename=$filename  pdffile=$pdffile  dest=$dest"
    if {$OS == "Windows"} {
      set cmd "exec $to_pdf \$pdffile \$filename"
      if { ![catch {eval $cmd} msg] } {
        file rename -force $pdffile $dest
        # ps2pdf succeeded, so remove original .ps file
        if { ![xschem get debug_var] } {
          file delete $filename
        }
      } else {
        puts stderr "problems converting postscript to pdf: $msg"
      }
    } else { ;# OS == unix
      eval execute_wait 0 $to_pdf [list $filename $pdffile]
      if {$execute(status,last) == 0} {
        file rename -force $pdffile $dest
        if { ![xschem get debug_var] } {
          file delete $filename
        }
      }
    }
  } else {
    # puts "rename  $filename $dest"
    file rename -force $filename $dest
  }
}

# 20161121
proc convert_to_png {filename dest} {
  global to_png debug_var OS
  # puts "---> $to_png $filename $dest"
  set cmd "exec $to_png \$filename png:\$dest"
    if {$OS == "Windows"} {
      set cmd "exec $to_png \$dest \$filename"
    } 
  if { ![catch {eval $cmd} msg] } {
    # conversion succeeded, so remove original .xpm file
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  } else {
    puts stderr "problems converting xpm to png: $msg"
  }
}

# always specify Shift- modifier for capital letters
# see tk 'man keysyms' for key names
# example format for s, d: Control-Alt-Key-asterisk
#                          Control-Shift-Key-A
#                          Alt-Key-c
#                          ButtonPress-4
#
proc key_binding {  s  d { win_path {.drw} } } { 
  regsub {.*-} $d {} key


  switch $key {
     Insert { set keysym 65379 } 
     Escape { set keysym 65307 } 
     Return { set keysym 65293 } 
     Delete { set keysym 65535 } 
     F1 { set keysym  65470 } 
     F2 { set keysym  65471 } 
     F3 { set keysym  65472 } 
     F4 { set keysym  65473 } 
     F5 { set keysym  65474 } 
     F6 { set keysym  65475 } 
     F7 { set keysym  65476 } 
     F8 { set keysym  65477 } 
     F9 { set keysym  65478 } 
     F10 { set keysym  65479 } 
     F11 { set keysym  65480 } 
     F12 { set keysym  65481 } 
     BackSpace { set keysym 65288 } 
     default { set keysym [scan "$key" %c] }
  }
  set state 0
  # not found any portable way to get modifier constants ...
  if { [regexp {(Mod1|Alt)-} $d] } { set state [expr {$state +8}] }
  if { [regexp Control- $d] } { set state [expr {$state +4}] }
  if { [regexp Shift- $d] } { set state [expr {$state +1}] }
  if { [regexp ButtonPress-1 $d] } { set state [expr {$state +0x100}] }
  if { [regexp ButtonPress-2 $d] } { set state [expr {$state +0x200}] }
  if { [regexp ButtonPress-3 $d] } { set state [expr {$state +0x400}] }
  # puts "$state $key <${s}>"
  if {[regexp ButtonPress- $d]} {
    bind $win_path "<${s}>" "xschem callback %W %T %x %y 0 $key 0 $state"
  } else {
    if {![string compare $d {} ] } {
      # puts  "bind .drw  <${s}> {}"
      bind $win_path "<${s}>" {}
    } else {
      # puts  "bind .drw  <${s}> xschem callback %W %T %x %y $keysym 0 0 $state"
      bind $win_path  "<${s}>" "xschem callback %W %T %x %y $keysym 0 0 $state"
    }
  }

}

proc edit_file {filename} {
 
 global editor
 # since $editor can be an executable with options (gvim -f) I *need* to use eval
 eval execute 0  $editor  $filename
 return {}
}

# ============================================================
#      SIMULATION CONTROL
# ============================================================

# ============================================================
#      SIMCONF 
# ============================================================

## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
## $s : schematic name (opamp)
## $d : netlist directory
##
## Other global vars:
## netlist_dir
## terminal
## netlist_type can be obtained with [xschem get netlist_type]
proc save_sim_defaults {f} {
  global sim netlist_dir terminal
  
  regsub {/$} $netlist_dir {} netlist_dir
  set a [catch {open $f w} fd]
  if { $a } {
    puts "save_sim_defaults: error opening file $f: $fd"
    return
  }
  puts $fd {# set the list of tools known to xschem}
  puts $fd {# Note that no spaces are allowed around commas in array keys}
  puts $fd "set sim(tool_list) {$sim(tool_list)}"
  puts $fd {}
  foreach tool $sim(tool_list) {
    puts $fd "#Specify the number of configured $tool tools."
    puts $fd "set sim($tool,n) $sim($tool,n) ;# number of configured $tool tools"
    puts $fd "# Specify the default $tool tool to use (first=0)"
    puts $fd "set sim($tool,default) $sim($tool,default) ;# default $tool tool to launch"
    puts $fd {}
    for {set i 0} {$i < $sim($tool,n)} { incr i} {
      puts $fd "# specify tool command (cmd), name (name), if tool must run\
                in foreground and if exit status must be reported"
      puts $fd "set sim($tool,$i,cmd) {$sim($tool,$i,cmd)}"
      puts $fd "set sim($tool,$i,name) {$sim($tool,$i,name)}"
      puts $fd "set sim($tool,$i,fg) $sim($tool,$i,fg)"
      puts $fd "set sim($tool,$i,st) $sim($tool,$i,st)"
      puts $fd {}
    }
    puts $fd {}
  }
  close $fd
}

proc load_recent_file {} {
  global USER_CONF_DIR recentfile has_x
  # recent files
  set recentfile {}
  if { [file exists $USER_CONF_DIR/recent_files] } {
    if {[catch { source $USER_CONF_DIR/recent_files } err] } {
      puts "Problems opening recent_files: $err"
      if {[info exists has_x]} {
        tk_messageBox -message  "Problems opening recent_files: $err" \
            -icon warning -parent . -type ok
      }
    }
    foreach i [info vars c_toolbar::c_t_*] {
      if {[set ${i}(w)] != $c_toolbar::c_t(w) ||
          [set ${i}(n)] != $c_toolbar::c_t(n)} {
         array unset $i
      }
    }
    set hash $c_toolbar::c_t(hash)
    if { [info exists c_toolbar::c_t_$hash]} {
        array set c_toolbar::c_t [array get c_toolbar::c_t_$hash]
    }
  }
}

proc update_recent_file {f {topwin {} } } {
  global recentfile  has_x
  # puts "update recent file, f=$f, topwin=$topwin"
  set old $recentfile
  set recentfile {}
  lappend recentfile $f
  foreach i $old {
    if {[abs_sym_path $i] ne [abs_sym_path $f]} {
      lappend recentfile [abs_sym_path $i]
    }
  }
  # tcl8.4 errors if using lreplace past the last element
  if { [llength $recentfile] > 10  } {
    set recentfile [lreplace $recentfile 10 end]
  }
  write_recent_file
  if { [info exists has_x] } {setup_recent_menu $topwin}
}

proc write_recent_file {} {
  global recentfile USER_CONF_DIR

  # puts "write recent file recentfile=$recentfile"
  set a [catch {open $USER_CONF_DIR/recent_files w} fd]
  if { $a } {
    puts "write_recent_file: error opening file $f: $fd"
    return
  }
  puts $fd "set recentfile {$recentfile}"

  if {[info exists c_toolbar::c_t]} {
    set hash $c_toolbar::c_t(hash)
    set c_toolbar::c_t(time) [clock seconds]
    array set c_toolbar::c_t_$hash [array get c_toolbar::c_t]
    foreach i [info vars c_toolbar::c_t_*] {
      ## old (~3 months) recent components will expire
      if { [info exists [subst $i](time)]} {
        if { [clock seconds] -  [set [subst $i](time)] < 7500000} {
          puts $fd "array set $i {[array get $i]}"
        }
        # puts "Age: [expr {[clock seconds] -  [set [subst $i](time)]}]"
      }
    }
  }
  close $fd
}

proc setup_recent_menu { { topwin {} } } {
  global recentfile
  $topwin.menubar.file.recent delete 0 9
  set i 0
  if { [info exists recentfile] } {
    foreach i $recentfile {
      $topwin.menubar.file.recent add command \
        -command "xschem load -gui {$i}" \
        -label [file tail $i]
    }
  }
}



## ngspice:: raw file access functions
namespace eval ngspice {
  # Create a variable inside the namespace
  variable ngspice_data
  variable op_point_read
}

proc ngspice::get_current {n} {
  set raw_level [xschem get raw_level]
  set path [string range [xschem get sch_path] 1 end]
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  if { [xschem get currsch] < $raw_level} { return {}}
  set skip 0
  while { $skip < $raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  set prefix $n
  # if xm1.rd is given get prefix (r) by removing path components (xm1.)
  regsub {.*\.} $prefix {} prefix
  set prefix [string range $prefix 0 0]
  # puts "ngspice::get_current: path=$path n=$n prefix=$prefix"
  set n $path$n
  set currname i
  if { ![sim_is_xyce] } { ;# ngspice
    if {$path ne {} } {
      set n $prefix.$n
    }
    if { ![regexp $prefix {[ve]}] } {
      set n @$n
    }
    set n i($n)
  } else { ;# xyce
    if { [regexp {\[i[bcedgsb]\]$} $n] } {
      regexp {\[(i[bcesdgb])\]$} $n curr1 currname
      if { $prefix == {d} } {set currname i}
      regsub {\[(i[bcesdgb])\]$} $n {} n
    }
    regsub {\[i\]} $n {} n
    set n $currname\($n\)
  }
  # puts "ngspice::get_current --> $n"
  set err [catch {set ngspice::ngspice_data($n)} res]
  if { $err } {
    set res {?}
  }
  # puts "$n --> $res"
  return $res
}

proc ngspice::get_diff_voltage {n m} {
  set raw_level [xschem get raw_level]
  set path [string range [xschem get sch_path] 1 end]
  if { [xschem get currsch] < $raw_level} { return {}}
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $raw_level } {
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
  }
  return $res
}


proc ngspice::get_voltage {n} {
  set raw_level [xschem get raw_level]
  set path [string range [xschem get sch_path] 1 end]
  if { [xschem get currsch] < $raw_level} { return {}}
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  # puts "ngspice::get_voltage: path=$path n=$n"
  set node $path$n
  # puts "ngspice::get_voltage: trying $node"
  set err [catch {set ngspice::ngspice_data($node)} res]
  if {$err} {
    set node v(${path}${n})
    # puts "ngspice::get_voltage: trying $node"
    set err [catch {set ngspice::ngspice_data($node)} res]
  }
  if { $err } {
    set res {?}
  }
  return $res
}

proc update_schematic_header {} {
  global retval
  set retval [xschem get header_text]
  text_line {Header/License text:} 0
  if { $tctx::rcode ne {}} {
    xschem set header_text $retval
  }
}

proc ngspice::get_node {n} {
  set raw_level [xschem get raw_level]
  set path [string range [xschem get sch_path] 1 end]
  if { [xschem get currsch] < $raw_level} { return {}}
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $raw_level } { 
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  set n [string tolower $n]
  # n may contain $path, so substitute its value
  set n [ subst -nocommand $n ]
  set err [catch {set ngspice::ngspice_data($n)} res]
  if { $err } { 
    set res {?}
  }
  return $res
}

## end ngspice:: functions

# test if currently set simulator is ngspice
proc sim_is_ngspice {} {
  global sim

  set_sim_defaults

  if { [info exists sim(spice,default)] } {
    set idx $sim(spice,default)
    if { [regexp {ngspice} $sim(spice,$idx,cmd)] } {
      return 1
    }
  }
  return 0
}

# test if currently set simulator is Xyce
proc sim_is_Xyce {} {
  return [sim_is_xyce]
}

proc sim_is_xyce {} {
  global sim

  set_sim_defaults

  if { [info exists sim(spice,default)] } {
    set idx $sim(spice,default)
    if { [regexp {[xX]yce} $sim(spice,$idx,cmd)] } {
      return 1
    }
  }
  return 0
}

# tests if file f exists. One level of global scope 'subst' is done on f
# to expand global variables / commands catching errors.
# example:
#    % set b {$env(HOME)/.bashrc}
#    $env(HOME)/.bashrc
#    % file_exists $b
#    1
#    % 
#    %
#    % set b {$env(HOMExx)/.bashrc}
#    $env(HOMExx)/.bashrc
#    % file_exists $b
#    0
#    % 
proc file_exists {f} {
  set ret 0
  set r [catch "uplevel #0 {subst $f}" res]
  if {$r == 0} {
    if {[file exists $res]} { set ret 1}
  }
  return $ret
}

# wrapper to "xschem list_tokens" comand to handle non list results
# usually as a result of malformed input strings
proc list_tokens {s} {
  set res [xschem list_tokens $s 0]


  if { [info tclversion] > 8.4 } {
    if { [string is list $res]} {
      return $res
    } else {
      return [split $res]
    }
  } else {
    if {![catch {llength $res}]} {
      return $res
    } else {
      return [split $res]
    }
  }
}

# generates a proper list, trimming multiple separators
proc tolist {s} {
  set s [string trim $s]
  regsub -all {[\t\n ]+} $s { } s


  if { [info tclversion] > 8.4 } {
    if { [string is list $s]} {
      return $s
    } else {
      return [split $s]
    }
  } else {
    if {![catch {llength $s}]} {
      return $s
    } else {
      return [split $s]
    }
  }
}

# Initialize the tcl sim array variable (if not already set) 
# setting up simulator / wave viewer commands
proc set_sim_defaults {{reset {}}} {
  global sim terminal USER_CONF_DIR has_x bespice_listen_port env OS
  if {$reset eq {reset} } { file delete ${USER_CONF_DIR}/simrc }
  if { $reset eq {} } {
    set failure 0
    if { [info exists has_x] && [winfo exists .sim] } {
      foreach tool $sim(tool_list) {
        for {set i 0} {$i < $sim($tool,n)} { incr i} {
          set sim($tool,$i,cmd) [.sim.topf.f.scrl.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
        }
      }
    } 
    if { ![info exists sim] } {
      if { [file exists ${USER_CONF_DIR}/simrc] } {
        # get conf from simrc
        if { [catch {source ${USER_CONF_DIR}/simrc} err]} {
          puts "Problems opening simrc file: $err"
          if {[info exists has_x]} {
            tk_messageBox -message  "Problems opening simrc file: $err" -icon warning \
               -parent [xschem get topwindow] -type ok
          }
          set failure 1
        }
      }
    } 
  }
  if {( $reset eq {reset} ) || ![info exists sim] || $failure} {
    if {[info exists sim]} {unset sim}
    # no simrc, set a reasonable default
    set sim(tool_list) {spice spicewave verilog verilogwave vhdl vhdlwave}
    if {$OS == "Windows"} {
      set_ne sim(spice,0,cmd) {ngspice -i "$N" -a}
    } else {
      set_ne sim(spice,0,cmd) {$terminal -e {ngspice -i "$N" -a || sh}}
    }
    # can not use set_ne as variables bound to entry widgets always exist if widget exists
    set sim(spice,0,name) {Ngspice interactive}
    set_ne sim(spice,0,fg) 0
    set_ne sim(spice,0,st) 0


    set_ne sim(spice,1,cmd) {ngspice "$N" -a}
    set sim(spice,1,name) {Ngspice Control mode}
    set_ne sim(spice,1,fg) 0
    set_ne sim(spice,1,st) 1

    
    set_ne sim(spice,2,cmd) {ngspice -b -r "$n.raw" "$N"}
    set sim(spice,2,name) {Ngspice batch}
    set_ne sim(spice,2,fg) 0
    set_ne sim(spice,2,st) 1
    
    set_ne sim(spice,3,cmd) {Xyce "$N"}
    set sim(spice,3,name) {Xyce batch}
    set_ne sim(spice,3,fg) 0
    set_ne sim(spice,3,st) 1
    
    set_ne sim(spice,4,cmd) {mpirun /path/to/parallel/Xyce "$N"}
    set sim(spice,4,name) {Xyce parallel batch}
    set_ne sim(spice,4,fg) 0
    set_ne sim(spice,4,st) 1
    
    # number of configured spice simulators, and default one
    set_ne sim(spice,n) 5
    set_ne sim(spice,default) 0
    
    ### spice wave view
    set_ne sim(spicewave,0,cmd) {gaw "$n.raw" } 
    set sim(spicewave,0,name) {Gaw viewer}
    set_ne sim(spicewave,0,fg) 0
    set_ne sim(spicewave,0,st) 0
   
    set_ne sim(spicewave,1,cmd) {$terminal -e ngspice}
    set sim(spicewave,1,name) {Ngspice Viewer}
    set_ne sim(spicewave,1,fg) 0
    set_ne sim(spicewave,1,st) 0

    set_ne sim(spicewave,2,cmd) {rawtovcd -v 1.5 "$n.raw" > "$n.vcd" && gtkwave "$n.vcd" "$n.sav" 2>/dev/null} 
    set sim(spicewave,2,name) {Rawtovcd}
    set_ne sim(spicewave,2,fg) 0
    set_ne sim(spicewave,2,st) 0

    # A server communicating with bespice wave was set up in the function setup_tcp_bespice().
    # This server is listening on port $bespice_listen_port. 
    set_ne sim(spicewave,3,cmd) {$env(HOME)/analog_flavor_eval/bin/bspwave --socket localhost $bespice_listen_port "$n.raw" } 
    set sim(spicewave,3,name) {Bespice wave}
    set_ne sim(spicewave,3,fg) 0
    set_ne sim(spicewave,3,st) 0
    # number of configured spice wave viewers, and default one
    set_ne sim(spicewave,n) 4
    set_ne sim(spicewave,default) 0
    
    ### icarus verilog
    set_ne sim(verilog,0,cmd) {sh -c "iverilog -o .verilog_object -g2012 '$N' && vvp .verilog_object"}
    set sim(verilog,0,name) {Icarus verilog}
    set_ne sim(verilog,0,fg) 0
    set_ne sim(verilog,0,st) 1

    ### verilator
    set_ne sim(verilog,1,cmd) {sh -c "verilator -Wno-fatal --binary --timing --cc --Mdir obj_$s -o $s '$N' --trace \
                                     && ./obj_$s/$s"}
    set sim(verilog,1,name) {Verilator}
    set_ne sim(verilog,1,fg) 0
    set_ne sim(verilog,1,st) 1

    # number of configured verilog simulators, and default one
    set_ne sim(verilog,n) 2
    set_ne sim(verilog,default) 0
    
    ### verilog wave view
    set_ne sim(verilogwave,0,cmd) {gtkwave dumpfile.vcd "$N.sav" 2>/dev/null}
    set sim(verilogwave,0,name) {Gtkwave}
    set_ne sim(verilogwave,0,fg) 0
    set_ne sim(verilogwave,0,st) 0
    # number of configured verilog wave viewers, and default one
    set_ne sim(verilogwave,n) 1
    set_ne sim(verilogwave,default) 0
    
    ### vhdl
    set_ne sim(vhdl,0,cmd) {ghdl -c --ieee=synopsys -fexplicit "$N" -r "$s" "--wave=$n.ghw"}
    set sim(vhdl,0,name) {Ghdl}
    set_ne sim(vhdl,0,fg) 0
    set_ne sim(vhdl,0,st) 1
    # number of configured vhdl simulators, and default one
    set_ne sim(vhdl,n) 1
    set_ne sim(vhdl,default) 0
    
    ### vhdl wave view
    set_ne sim(vhdlwave,0,cmd) {gtkwave "$n.ghw" "$N.sav" 2>/dev/null}
    set sim(vhdlwave,0,name) {Gtkwave}
    set_ne sim(vhdlwave,0,fg) 0
    set_ne sim(vhdlwave,0,st) 0
    # number of configured vhdl wave viewers, and default one
    set_ne sim(vhdlwave,n) 1
    set_ne sim(vhdlwave,default) 0
  }
} 

proc simconf_reset {} {
  global sim

  set answer [tk_messageBox -message  "Warning: delete simulation configuration file and reset to default?" \
            -icon warning -parent .sim  -type okcancel]
  if { $answer eq {ok}} {
    set_sim_defaults reset
    foreach tool $sim(tool_list) {
      for {set i 0} { $i < $sim($tool,n)} {incr i} {
        .sim.topf.f.scrl.center.$tool.r.$i.cmd delete 1.0 end
        .sim.topf.f.scrl.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      }
    }
  }
}

proc simconf_saveconf {scrollframe} {
  global sim USER_CONF_DIR
  foreach tool $sim(tool_list) {
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      set sim($tool,$i,cmd) [${scrollframe}.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
    }
  }
  # destroy .sim
  # xschem set semaphore [expr {[xschem get semaphore] -1}]
  save_sim_defaults ${USER_CONF_DIR}/simrc
  # puts "saving simrc"
}
 
proc simconf {} {
  global sim USER_CONF_DIR simconf_default_geometry

  if {[winfo exists .sim]} {
    destroy .sim 
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  set_sim_defaults
  toplevel .sim -class Dialog
  wm title .sim {Simulation Configuration}
  wm geometry .sim 790x370
  # wm transient .sim [xschem get topwindow]
  frame .sim.topf
  set scrollframe [sframe .sim.topf]
  frame ${scrollframe}.center
  frame .sim.bottom
  pack ${scrollframe}.center -fill both -expand yes
  set bg(0) {#dddddd}
  set bg(1) {#aaaaaa}
  set toggle 0
  foreach tool $sim(tool_list) {
    frame ${scrollframe}.center.$tool
    label ${scrollframe}.center.$tool.l -width 12 -text $tool  -background $bg($toggle) -fg black 
    frame ${scrollframe}.center.$tool.r
    pack ${scrollframe}.center.$tool -fill both -expand yes
    pack ${scrollframe}.center.$tool.l -fill y -side left
    pack ${scrollframe}.center.$tool.r -fill both -expand yes
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      frame ${scrollframe}.center.$tool.r.$i
      pack ${scrollframe}.center.$tool.r.$i -fill x -expand yes
      entry ${scrollframe}.center.$tool.r.$i.lab -textvariable sim($tool,$i,name) -width 18 \
         -background $bg($toggle) -fg black
      entry_replace_selection  ${scrollframe}.center.$tool.r.$i.lab
      radiobutton ${scrollframe}.center.$tool.r.$i.radio -background $bg($toggle) -fg black \
         -selectcolor white -variable sim($tool,default) -value $i
      text ${scrollframe}.center.$tool.r.$i.cmd -undo 1 -width 20 -height 3 -wrap none -background $bg($toggle) -fg black
      ${scrollframe}.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      checkbutton ${scrollframe}.center.$tool.r.$i.fg -text Fg -variable sim($tool,$i,fg) \
        -selectcolor white -background $bg($toggle) -fg black
      checkbutton ${scrollframe}.center.$tool.r.$i.st -text Status -variable sim($tool,$i,st) \
        -selectcolor white -background $bg($toggle) -fg black
      pack ${scrollframe}.center.$tool.r.$i.lab -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.radio -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.cmd -side left -fill x -expand yes
      pack ${scrollframe}.center.$tool.r.$i.fg -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.st -side left -fill y 
    }
    incr toggle
    set toggle [expr {$toggle %2}]
  }
  button .sim.bottom.cancel  -text Cancel -command {
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  button .sim.bottom.help  -text Help -command {
    viewdata {In this dialog box you set the commands xschem uses to launch the 
various external tools. The commands go through a tcl 'subst' round
 to do TCL variable and command substitution, so you may use the TCL
'{' and '}' characters for grouping arguments with spaces.
Xschem has 3 main netlisting modes (spice, verilog, vhdl) and for each
netlisting mode some simulators and some viewers can be defined.
The following variables are defined and will get substituted by
XSCHEM before sending commands to the shell:

 - N: complete filename of netlist for current netlisting mode
   (example: /home/schippes/.xschem/simulations/opamp.spice for spice)
   (example: /home/schippes/.xschem/simulations/opamp.v for verilog)
 - n: complete filename of netlist as above but without extension
   (example: /home/schippes/.xschem/simulations/opamp)
 - S: full pathname of schematic being used (example: 
      /home/schippes/.xschem/xschem_library/opamp.sch)
 - s: name of schematic being used (example: opamp)
 - d: simulation directory (example: /home/schippes/.xschem/simulations)
 - terminal: terminal to be used for applications that need to be
   executed in terminal (example: $terminal -e ngspice -i "$N" -a)

If for a given tool there are multiple rows then the radiobutton
tells which one will be called by xschem.
Variables should be used with the usual substitution character $: $n, $N, etc.
Foreground (Fg) checkbutton tells xschem to wait for child process to finish.
Status checkbutton tells xschem to report a status dialog (stdout, stderr,
exit status) when process finishes.

If 'Accept, Save and Close' is pressed then changes made in the dialog box
entries will be saved in ~/.xschem/simrc, changes will be persistent.

If 'Accept, no Save and Close' is pressed then the changes are kept in
memory and dialog is closed without writing to a file,
if xschem is restarted changes will be lost.

If no ~/.xschem/simrc is present then a minimal default setup is presented.
To reset to default use the corresponding button or just delete the
~/.xschem/simrc file manually.
    } ro
  }
  button .sim.bottom.ok  -text {Accept, Save and Close} -command "
    set_sim_defaults
    simconf_saveconf $scrollframe
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  "
  button .sim.bottom.reset -text {Reset to default} -command {
    simconf_reset
  }
  button .sim.bottom.close -text {Accept, no Save and Close} -command {
    set_sim_defaults
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  wm protocol .sim WM_DELETE_WINDOW {
    set_sim_defaults 
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  pack .sim.bottom.cancel -side left -anchor w
  pack .sim.bottom.help -side left
  #foreach tool $sim(tool_list) {
  #  button .sim.bottom.add${tool} -text +${tool} -command "
  #    simconf_add $tool
  #    destroy .sim
  #    xschem set semaphore [expr {[xschem get semaphore] -1}]
  #    save_sim_defaults ${USER_CONF_DIR}/simrc
  ##    simconf
  #  "
  #  pack .sim.bottom.add${tool} -side left
  #}
  pack .sim.bottom.ok -side right -anchor e
  pack .sim.bottom.close -side right
  pack .sim.bottom.reset -side right
  pack .sim.topf -fill both -expand yes
  pack .sim.bottom -fill x
  if { [info exists simconf_default_geometry]} {
     wm geometry .sim "${simconf_default_geometry}"
  }
 
  bind .sim.topf.f <Configure> {sframeyview .sim.topf}
  bind .sim <Configure> {
    set simconf_default_geometry [wm geometry .sim]
  }
  bind .sim <ButtonPress-4> { sframeyview .sim.topf scroll -0.2}
  bind .sim <ButtonPress-5> { sframeyview .sim.topf scroll 0.2}
  sframeyview .sim.topf place
  set maxsize [expr {[winfo height ${scrollframe}] + [winfo height .sim.bottom]}]
  wm maxsize .sim 9999 $maxsize
  # tkwait window .sim
}

proc simconf_add {tool} {
  global sim
  set n $sim($tool,n)
  set sim($tool,$n,cmd) {}
  set sim($tool,$n,name) {}
  set sim($tool,$n,fg) 0
  set sim($tool,$n,st) 0
  incr sim($tool,n)
}


############ cellview
# proc cellview prints symbol bindings (default binding or "schematic" attr in symbol)
# of all symbols used in current and sub schematics.
# the derived_symbols parameter of cellview should be either empty or 'derived_symbols'
proc cellview_setlabels {w symbol derived_symbol} {
  global dark_gui_colorscheme netlist_type
  if {$dark_gui_colorscheme} {
    set instfg orange1
    set symfg SeaGreen1
    set symbg SeaGreen4
    set missingbg IndianRed4
  } else { 
    set instfg orange4
    set symfg SeaGreen4
    set symbg SeaGreen1
    set missingbg IndianRed1 
  }     
  set save_netlist_type [xschem get netlist_type]
  set current [xschem get current_name]
  set sym_spice_sym_def  [xschem getprop symbol $symbol spice_sym_def 2]
  set abs_sch [xschem get_sch_from_sym -1 $symbol]
  set sym_sch [rel_sym_path $abs_sch]
  set default_sch [add_ext $symbol .sch]
  set new_sch [$w get]
  $w configure -fg [option get . foreground {}]
  $w configure -bg [option get . background {}]
  if { $derived_symbol} {
    $w configure -fg $instfg
  } elseif {$sym_spice_sym_def ne {} } {
    $w configure -fg $symfg
  } 
  if { $sym_spice_sym_def eq {}} {
    if { ![file exists [abs_sym_path [$w get]]] } {
      $w configure -bg $missingbg
    } elseif {$new_sch ne $default_sch } {
      $w configure -bg $symbg
    }
  }
  puts ===============
  if {$sym_sch ne $new_sch && $sym_spice_sym_def eq {}} {
    puts "Changing schematic attribute in symbol"
    xschem load -keep_symbols -nodraw -noundoreset $symbol
    set oldprop [xschem get schsymbolprop]
    if { $new_sch eq $default_sch } {
      set newprop [xschem subst_tok $oldprop schematic {}] ;# delete schematic attr in symbol
    } else {
      set newprop [xschem subst_tok $oldprop schematic $new_sch]
    }
    xschem set schsymbolprop $newprop
    xschem set_modify 3 ;# set only modified flag to force a save, do not update window/tab titles
    xschem save fast
    # no! cellview_setlabels called from cellview inside a "foreach {i symbol} $syms {...}"
    # xschem remove_symbols ;# purge all symbols to force a reload from disk 
    xschem load -keep_symbols -nodraw -noundoreset $current
    set netlist_type $save_netlist_type 
    xschem set netlist_type $netlist_type
    xschem netlist -keep_symbols -noalert;# traverse the hierarchy and retain all encountered symbols
    puts "get netlist"
  }
  puts sym_sch=$sym_sch
  puts default_sch=$default_sch
  puts new_sch=$new_sch
  puts symbol=$symbol
}

proc cellview_edit_item {symbol w} {
  global netlist_type
  set save_netlist_type [xschem get netlist_type]
  set sym_spice_sym_def  [xschem getprop symbol $symbol spice_sym_def 2]
  if {[xschem is_generator [$w get]]} {
    set f [$w get]
    regsub {\(.*} $f {} f
    textwindow [abs_sym_path $f]
  } elseif { $sym_spice_sym_def eq {}} {
    xschem load_new_window [$w get]
  } else {
    set current [xschem get current_name]
    set old_sym_def [xschem getprop symbol $symbol spice_sym_def 2]
    set new_sym_def [editdata $sym_spice_sym_def {Symbol spice_sym_def attribute}]
    if {$new_sym_def ne $old_sym_def} {
      xschem load -keep_symbols -nodraw -noundoreset $symbol
      set oldprop [xschem get schsymbolprop]
      set newprop [xschem subst_tok $oldprop spice_sym_def $new_sym_def]
      xschem set schsymbolprop $newprop
      xschem set_modify 3 ;# set only modified flag to force a save, do not update window/tab titles
      xschem save fast
      puts "$symbol: updated spice_sym_def attribute"
      xschem load -keep_symbols -nodraw -noundoreset $current
      set netlist_type $save_netlist_type 
      xschem set netlist_type $netlist_type
      xschem reload_symbols ;# update in-memory symbol data
    }
  }
}

proc cellview_edit_sym {w} {
  set sym [$w cget -text]
  set res [catch {xschem symbol_base_name $sym} base_name]
  if {$res == 0} {
    if {$base_name ne {}} {
      set sym $base_name
    }
  }
  xschem load_new_window $sym
}

# derived_symbols: empty or 'derived_symbols'
# upd: never set by caller (used iinternally to update)
proc cellview { {derived_symbols {}} {upd 0}} {
  global nolist_libs dark_gui_colorscheme netlist_type

  set save_netlist_type [xschem get netlist_type]

  if {$dark_gui_colorscheme} { 
    set instfg orange1
    set symfg SeaGreen1
    set symbg SeaGreen4
    set missingbg IndianRed4
  } else { 
    set instfg orange4
    set symfg SeaGreen4
    set symbg SeaGreen1
    set missingbg IndianRed1 
  }     
  if {[info tclversion] >= 8.5} {
    set font {TkDefaultFont 10 bold} ;# Monospace
  } else {
    set font fixed
  }

  set netlist_type $save_netlist_type 
  xschem set netlist_type $netlist_type
  xschem reload_symbols ;# purge unused symbols
  xschem netlist -keep_symbols -noalert;# traverse the hierarchy and retain all encountered symbols
  if {!$upd} {
    catch {destroy .cv}
    toplevel .cv
    wm geometry .cv 800x200
    update
    raise .cv
    frame .cv.top
    label .cv.top.sym -text {   SYMBOL} -width 30 -bg grey60 -anchor w -padx 4 -font $font
    label .cv.top.sch -text SCHEMATIC -width 45 -bg grey60 -anchor w -padx 4 -font $font
    label .cv.top.pad -text {      } -width 4 -bg grey60 -font $font
    pack .cv.top.sym .cv.top.sch -side left -fill x -expand 1
    pack .cv.top.pad -side left -fill x
    frame .cv.center
    set sf [sframe .cv.center]
  } else {
    set sf .cv.center.f.scrl
  }
  
  set syms [join [lsort -index 1 [xschem symbols $derived_symbols]]]
  # puts "syms=$syms"
  foreach {i symbol} $syms {
    if { [catch {set base_name [xschem symbol_base_name $symbol]}] } {
      set base_name $symbol
    }
    # puts "i=$i, symbol=$symbol"
    set derived_symbol 0
    if {$base_name ne {}} {
      set derived_symbol 1
    }
    if { [catch {xschem get_sch_from_sym -1 $symbol} abs_sch ]} {
      set abs_sch [abs_sym_path [add_ext $symbol .sch]]
    }
    if {$derived_symbol} {
      set abs_sym [abs_sym_path $base_name]
    } else {
      set abs_sym [abs_sym_path $symbol]
    }
    set skip 0
    foreach j $nolist_libs {
      if {[regexp $j $abs_sym]} { 
        set skip 1
        break
      }
    }
    if {$skip} { continue }
    set sym_sch [rel_sym_path $abs_sch]
    if {[catch {xschem getprop symbol $symbol type} type]} {
      puts "error: $symbol not found: $type"
    }
    set type [xschem getprop symbol $symbol type]
    set sym_spice_sym_def [xschem getprop symbol $symbol spice_sym_def 2]
    if {$type eq {subcircuit}} {

      if {!$upd} {
        frame $sf.f$i
        pack $sf.f$i -side top -fill x
        label  $sf.f$i.l -text $symbol -width 30 -anchor w -padx 4 -borderwidth 1 \
          -relief sunken -pady 1 -font $font
        if {$derived_symbol} { 
          $sf.f$i.l configure -fg $instfg
        }
        # puts $sf.f$i.s
        entry $sf.f$i.s -width 45 -borderwidth 1 -relief sunken -font $font
        button $sf.f$i.sym -text Sym -padx 4 -borderwidth 1 -pady 0 -font $font \
               -command "cellview_edit_sym $sf.f$i.l"
        button $sf.f$i.sch -text Sch -padx 4 -borderwidth 1 -pady 0 -font $font \
               -command "cellview_edit_item $symbol $sf.f$i.s"
        if {$sym_spice_sym_def eq {}} {
          $sf.f$i.s insert 0 $sym_sch
        } else {
          if {$derived_symbol} {
            $sf.f$i.s insert 0 {defined in instance spice_sym_def}
          } else {
            $sf.f$i.s insert 0 {defined in symbol spice_sym_def}
          }
        }
      }

      if {[xschem is_generator [ $sf.f$i.s get]]} {
        set f [ $sf.f$i.s get]
        regsub {\(.*} $f {} f
      } elseif { $sym_spice_sym_def eq {}} {
        set f [abs_sym_path [$sf.f$i.s get]]
      } else {
        set ff [split $sym_spice_sym_def \n]
        if {[llength $ff] > 5} {
          set ff [lrange $ff 0 4]
          lappend ff ...
        }   
        set f [join $ff \n]
      }
      balloon $sf.f$i.s $f

      cellview_setlabels $sf.f$i.s $symbol $derived_symbol
      if {!$upd} {
        pack $sf.f$i.l $sf.f$i.s -side left -fill x -expand 1
        pack $sf.f$i.sch $sf.f$i.sym -side left
      }
    }
  }

  if {$upd} {return}

  frame .cv.bottom
  button .cv.bottom.update -text Update -command "cellview [list $derived_symbols] 1; xschem reload_symbols"
  pack .cv.bottom.update -side left
  label .cv.bottom.status -text {STATUS LINE}
  pack .cv.bottom.status -fill x -expand yes
  pack .cv.top -side top -fill x -expand no
  pack .cv.center -side top -fill both -expand yes
  pack .cv.bottom -side top -fill x -expand no
  sframeyview .cv.center place
  set maxsize [expr {[winfo height ${sf}] + [winfo height .cv.top] + [winfo height .cv.bottom]}]
  wm maxsize .cv 9999 $maxsize
  bind .cv.center.f <Configure> {sframeyview .cv.center}
  bind .cv <ButtonPress-4> { sframeyview .cv.center scroll -0.1}
  bind .cv <ButtonPress-5> { sframeyview .cv.center scroll 0.1}
  bind .cv <Escape> {destroy .cv}
}
############ /cellview

############ traversal
proc traversal_setlabels {w parent_sch instname inst_sch sym_sch default_sch
                          inst_spice_sym_def sym_spice_sym_def} {
  global traversal dark_gui_colorscheme netlist_type
  set sf .trav.center.f.scrl

  set save_netlist_type [xschem get netlist_type]
  # puts "traversal_setlabels: $w parent: |$parent_sch| inst: $instname def: $sym_sch $inst_sch --> [$w get]"
  # update schematic
  if {$parent_sch ne {} && $sym_spice_sym_def eq {} &&  $inst_spice_sym_def eq {} } {
    set current [xschem get current_name]
    if { $inst_sch ne [$w get] } {
      xschem load -undoreset -nodraw $parent_sch 
      if { [$w get] eq  $sym_sch} {
        xschem setprop -fast instance $instname schematic  ;# remove schematic attr on instance
      } else {
        xschem setprop -fast instance $instname schematic [$w get]  ;# set schematic attr on instance
      } 
      xschem set_modify 3 ;# set only modified flag to force a save, do not update window/tab titles
      xschem save fast
      set inst_sch [$w get]
      # puts "inst_sch set to: $inst_sch"
      xschem load -undoreset -nodraw $current
      set netlist_type $save_netlist_type 
      xschem set netlist_type $netlist_type
    }
  }
  # /update schematic
  if {$dark_gui_colorscheme} {
    set instfg orange1
    set symfg SeaGreen1
    set instbg orange4
    set symbg SeaGreen4
    set missingbg IndianRed4
  } else {
    set instfg orange4
    set symfg SeaGreen4
    set instbg Orange1
    set symbg SeaGreen1
    set missingbg IndianRed1
  }
  $w configure -fg [option get . foreground {}]
  $w configure -bg [option get . background {}]
  if { $sym_spice_sym_def ne {}} {
    $w configure -fg $symfg
  } elseif {$inst_spice_sym_def ne {}} {
    $w configure -fg $instfg
  } elseif { ![file exists [abs_sym_path [$w get]]] } {
    $w configure -bg $missingbg
  } else {
    if {[$w get] eq $default_sch} {
      # ....
    } elseif {[$w get] eq $sym_sch} {
      $w configure -bg $symbg
    } elseif {[$w get] eq $inst_sch} {
      $w configure -bg $instbg
    }
  }
}

# This proc traverses the hierarchy and prints all instances in design.
proc traversal {{only_subckts 1} {all_hierarchy 1}} {
  global traversal keep_symbols
  set traversal(only_subckts) $only_subckts
  set traversal(all_hierarchy) $all_hierarchy
  set traversal(cnt) 0
  set save_keep $keep_symbols
  set keep_symbols 1
  xschem unselect_all
  xschem set no_draw 1 ;# disable screen update
  xschem set no_undo 1 ;# disable undo 

  if {[info tclversion] >= 8.5} {
    set font {TkDefaultFont 10 bold} ;# Monospace
  } else {
    set font fixed
  }

  toplevel .trav
  frame .trav.top
  label .trav.top.inst -text {INSTANCE} -width 25 -bg grey60 -anchor w -padx 4 -font $font
  label .trav.top.sym  -text {SYMBOL} -width 30 -bg grey60 -anchor w -padx 4 -font $font
  label .trav.top.sch  -text SCHEMATIC -width 45 -bg grey60 -anchor w -padx 4 -font $font
  label .trav.top.pad  -text {        } -bg grey60 -font $font
  pack .trav.top.inst -side left -fill x -expand 1
  pack .trav.top.sym .trav.top.sch -side left -fill x
  pack .trav.top.pad -side left -fill x
  frame .trav.center
  set sf [sframe .trav.center]
  hier_traversal 0 $only_subckts $all_hierarchy
  xschem set no_draw 0
  xschem set no_undo 0
  set keep_symbols $save_keep

  frame .trav.bottom
  label .trav.bottom.status -text {STATUS LINE}
  pack .trav.bottom.status -fill x -expand yes
  pack .trav.top -side top -fill x -expand no
  pack .trav.center -side top -fill both -expand yes
  pack .trav.bottom -side top -fill x -expand no
  sframeyview .trav.center place
  set maxsize [expr {[winfo height ${sf}] + [winfo height .trav.top] + [winfo height .trav.bottom]}]
  wm maxsize .trav 9999 $maxsize
  bind .trav.center.f <Configure> {sframeyview .trav.center}
  bind .trav <ButtonPress-4> { sframeyview .trav.center scroll -0.1}
  bind .trav <ButtonPress-5> { sframeyview .trav.center scroll 0.1}
  bind .trav <Escape> "
    set traversal(geom) \[winfo geometry .trav\]
    destroy .trav
  "
  if {[info exists traversal(geom)]} {
    wm geometry .trav $traversal(geom)
  } else {
    wm geometry .trav 1200x400
  }
  update
  set traversal(geom) [winfo geometry .trav]
  return {}
}

# recursive procedure
proc hier_traversal {{level 0} {only_subckts 0} {all_hierarchy 1}} {
  global nolist_libs traversal retval

  if {[info tclversion] >= 8.5} {
    set font {TkDefaultFont 10 bold} ;# Monospace
  } else { 
    set font fixed
  } 
  set parent_sch [xschem get current_name]
  # puts $parent_sch
  set sf .trav.center.f.scrl
  set done_print 0
  set schpath [xschem get sch_path]
  set instances  [xschem get instances]
  set current_level [xschem get currsch]
  for {set i 0} { $i < $instances} { incr i} {
    # puts "hier_traversal: i=$i, current_level=$current_level, parent_sch=$parent_sch"
    set instname [xschem getprop instance $i name]
    set symbol [xschem getprop instance $i cell::name]
    set default_sch [add_ext $symbol .sch]
    set sym_sch [rel_sym_path [xschem get_sch_from_sym -1 $symbol]]
    set abs_symbol [abs_sym_path $symbol]
    set type [xschem getprop symbol $symbol type]
    set schematic [xschem get_sch_from_sym $i]
    set sch_exists [expr {[file exists $schematic] ? {} : {**missing**}}]
    set inst_sch [rel_sym_path $schematic]
    set sch_rootname [file tail [file rootname $inst_sch]]
    set inst_spice_sym_def [xschem getprop instance $i spice_sym_def]
    set sym_spice_sym_def [xschem getprop instance $i cell::spice_sym_def]
    if {$only_subckts && ($type ne {subcircuit})} { continue }

    set skip 0
    foreach j $nolist_libs {
      if {[regexp $j $abs_symbol]} {
        set skip 1
        break
      }
    }
    if {$skip} { continue }
    incr traversal(cnt)
    set cnt $traversal(cnt)

    # puts "building frame $sf.f$cnt"
    frame $sf.f$cnt
    pack $sf.f$cnt -side top -fill x
    label  $sf.f$cnt.i -text "[spaces $level 2]$instname" \
        -width 25 -anchor w -padx 4 -borderwidth 1 \
        -relief sunken -pady 1 -font $font
    label  $sf.f$cnt.l -text $symbol -width 30 -anchor w -padx 4 -borderwidth 1 \
        -relief sunken -pady 1 -font $font
    entry $sf.f$cnt.s -width 45 -borderwidth 1 -relief sunken -font $font
    if {$type eq {subcircuit}} {
      if {$inst_spice_sym_def ne {}} {
        $sf.f$cnt.s insert 0 "$sch_rootname defined in instance spice_sym_def"
      } elseif {$sym_spice_sym_def ne {}} {
        $sf.f$cnt.s insert 0 "$sch_rootname defined in symbol spice_sym_def"
      } else {
        $sf.f$cnt.s insert 0 "$inst_sch"
      }
    }
    button $sf.f$cnt.bsym -text {Sym} -padx 4 -borderwidth 1 -pady 0 -font $font \
           -command "
              xschem load_new_window \[$sf.f$cnt.l cget -text\]
            "
    button $sf.f$cnt.bsch -text {Sch} -padx 4 -borderwidth 1 -pady 0 -font $font \
         -command "
            if { [list $sym_spice_sym_def] eq {} && [list $inst_spice_sym_def] eq {}} {
              xschem load_new_window \[$sf.f$cnt.s get\]
            } elseif {[list $sym_spice_sym_def] ne {}} {
              editdata [list $sym_spice_sym_def] {Symbol spice_sym_def attribute}
            } else {
              editdata [list $inst_spice_sym_def] {Instance spice_sym_def attribute}
            }"
    button $sf.f$cnt.upd -text Upd  -padx 4 -borderwidth 1 -pady 0 -font $font \
      -command "
        traversal_setlabels $sf.f$cnt.s [list $parent_sch] [list $instname] [list $inst_sch] \
        [list $sym_sch] [list $default_sch] [list $inst_spice_sym_def] [list $sym_spice_sym_def]
        set traversal(geom) \[winfo geometry .trav\]
        destroy .trav
        traversal $traversal(only_subckts) $traversal(all_hierarchy)
      "

    traversal_setlabels $sf.f$cnt.s $parent_sch $instname $inst_sch $sym_sch \
                        $default_sch $inst_spice_sym_def $sym_spice_sym_def
    pack $sf.f$cnt.i -side left -fill x -expand 1
    pack $sf.f$cnt.l $sf.f$cnt.s -side left -fill x
    pack $sf.f$cnt.bsym $sf.f$cnt.bsch $sf.f$cnt.upd -side left
    set done_print 1
    if {$type eq {subcircuit} && $all_hierarchy} {
      xschem select instance $i fast nodraw
      set descended [xschem descend 1 6]
      if {$descended} {
        incr level
        set dp [hier_traversal $level $only_subckts 1]
        xschem go_back 2
        incr level -1
      } else { ;# descended into a blank schematic. Go back.
        xschem go_back 2
      }
    }
  }
  return $done_print
}
############ /traversal

proc bespice_getdata {sock} {
  global bespice_server_getdata
  if {[eof $sock] || [catch {gets $sock bespice_server_getdata(line,$sock)}]} {
    puts "Closing connection $sock to bespice : $bespice_server_getdata(addr,$sock)"
    unset bespice_server_getdata(addr,$sock)
    unset bespice_server_getdata(line,$sock)
    # we remove the closing socket from the list of clients
    set search_index [ lsearch $bespice_server_getdata(clients) $sock]   
    # puts " search_index for $sock in $bespice_server_getdata(clients) ==> $search_index"
    if { $search_index >= 0 } { 
      set bespice_server_getdata(clients) [ lreplace $bespice_server_getdata(clients) $search_index $search_index ]
    }
    set nb [ llength $bespice_server_getdata(clients) ]
    if { $nb == 0 } {
      # no more clients left => communication closed
      unset bespice_server_getdata(sock)
    } else {
       # we have another client connection => use this one
       set new_sock [lindex $bespice_server_getdata(clients) 0]
       puts "Communicating to bespice over socket $new_sock"
       set bespice_server_getdata(sock) [list $new_sock ]
    }
    close $sock

    # puts "Clients = $bespice_server_getdata(clients)"
    # set nb [ llength $bespice_server_getdata(clients) ]
    # puts "number of Clients = $nb"

  } else {
    puts "bespice --> $bespice_server_getdata(line,$sock)"
    set bespice_server_getdata(last) $bespice_server_getdata(line,$sock)
  }
}

proc xschem_getdata {sock} {
  global xschem_server_getdata tclcmd_puts debug_var

  while {1} {
    if {[gets $sock line] < 0} {
      break
    } else {
      append xschem_server_getdata(line,$sock) $line \n
    }
  }
  if {$debug_var<=-1} {puts "tcp<-- $xschem_server_getdata(line,$sock)"}
  # xschem command must be executed at global scope...
  redef_puts
  uplevel #0 [list catch $xschem_server_getdata(line,$sock) tclcmd_puts]
  rename puts {}
  rename ::tcl::puts puts
  if {$debug_var<=-1} {puts "tcp--> $tclcmd_puts"}
  set xschem_server_getdata(res,$sock) "$tclcmd_puts"
  puts -nonewline $sock "$xschem_server_getdata(res,$sock)"
  flush $sock
  close $sock ;# server closes
  if {$debug_var<=-1} {puts "Close $xschem_server_getdata(addr,$sock)"}
  unset xschem_server_getdata(addr,$sock)
  unset xschem_server_getdata(line,$sock)
  unset xschem_server_getdata(res,$sock)
} 

# this function is called as soon as bespice wave connects to the communication server listening on $bespice_listen_port
# it makes sure the communication over the socket connection is possible
proc bespice_server {sock addr port} {
  global bespice_server_getdata
  if { ![info exists bespice_server_getdata(sock)] } {
    puts "Accepting bespice connection $sock from $addr port $port"
    set bespice_server_getdata(clients) [list $sock]
  } else {
    # we can' t handle more tha one socket at once 
    #	however we put this socket to a list in case the 1st socket is closed. 
    #	we can then use the 2nd socket
    puts "Can't handle bespice connection $sock from $addr port $port"
    lappend bespice_server_getdata(clients) [list $sock]
  }

  fconfigure $sock -buffering line
  set bespice_server_getdata(addr,$sock) [list $addr $port]
  set bespice_server_getdata(sock) [list $sock]
  fileevent $sock readable [list bespice_getdata $sock]
  # this informs bespice wave that it receives it's instructions from xschem. Some features will be adjusted for that.
  puts $bespice_server_getdata(sock) "set_customer_specialization xschem"

  # puts "Clients = $bespice_server_getdata(clients)"
  # set nb [ llength $bespice_server_getdata(clients) ]
  # puts "number of Clients = $nb"
}

proc xschem_server {sock addr port} {
  global xschem_server_getdata debug_var
  if {$debug_var<=-1} {puts "Accept $sock from $addr port $port"}
  fconfigure $sock -buffering line -blocking 0
  set xschem_server_getdata(addr,$sock) [list $addr $port]
  set xschem_server_getdata(line,$sock) {}
  fileevent $sock readable [list xschem_getdata $sock]
}

proc list_hierarchy {} {
  set s [xschem list_hierarchy]
  set r {}
  foreach {a b} [lsort -decreasing -dictionary -index 0 -stride 2 $s] {
    append r  $a {  } $b \n
  }
  return $r
}

## given a path (x1.x2.m4) descend into x1.x2 and return m4 whether m4 found or not 
proc descend_hierarchy {path {redraw 1}} {
  xschem set no_draw 1
  # return to top level if not already there
  while { [xschem get currsch] } { xschem go_back } 
  # recursively descend into sub-schematics
  while { [regexp {\.} $path] } {
    xschem unselect_all
    set inst $path
    regsub {\..*} $inst {} inst    ;# take 1st path component: xlev1[3].xlev2.m3 -> xlev1[3]
    regsub {[^.]+\.} $path {} path ;# take remaining path: xlev1[3].xlev2.m3 -> xlev2.m3
    xschem search exact 1 name $inst 1
    # handle vector instances: xlev1[3:0] -> xlev1[3],xlev1[2],xlev1[1],xlev1[0]
    # descend into the right one
    set inst_list [split [lindex [xschem expandlabel [lindex [xschem selected_set] 0 ] ] 0] {,}]
    set instnum [expr {[lsearch -exact  $inst_list $inst] + 1}]
    xschem descend $instnum
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  return $path
}

## given a hierarchical instname name (x1.xamp.m1) go down in the hierarchy and 
## select the specified instance (m1).
## this search assumes it is given from the top of hierarchy
proc select_inst {fullinst {redraw 1 } } {
  xschem set no_draw 1
  set inst [descend_hierarchy $fullinst 0] 
  set res [xschem select instance $inst]
  # if nothing found return to top
  if {!$res} {
    while { [xschem get currsch] } { xschem go_back } 
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  if {$res} {return $inst} else { return {} } 
}

proc pin_label {} {
  return [rel_sym_path [find_file_first lab_pin.sym]]
}

## given a hierarchical net name x1.xamp.netname go down in the hierarchy and 
## highlight the specified net.
## this search assumes it is given from the top of hierarchy
proc probe_net {fullnet {redraw 1} } {
  xschem set no_draw 1
  set net [descend_hierarchy $fullnet 0]
  set res [xschem hilight_netname $net]
  if {$res==0  && [regexp {^net[0-9]+$} $net]} {
    set net \#$net
    set res [xschem hilight_netname $net]
  }
  if {!$res} {
    while { [xschem get currsch] } { xschem go_back } 
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  if {$res} {return $net} else { return {} } 
}

# backannotate newnet to be connected to specified hierarchical instance name and pin.
# places a label close to the instance pin to be re-routed.
# actual reconnect is human assisted! 
proc reroute_inst {fullinst pinattr pinval newnet} {
  if { [regexp {\.} $fullinst] } { set hier 1 } else { set hier 0 } 
  set res [descend_hierarchy $fullinst 0]
  if {$res ne {} } {
    set coord [xschem instance_pin_coord $res $pinattr $pinval]
    if { $coord eq {} } {
      while { [xschem get currsch] } { xschem go_back } 
      return 0
    }
    set pinname [lindex $coord 0]
    set x [expr {[lindex $coord 1] - 10} ]
    set y [expr {[lindex $coord 2] - 10} ]
    set oldnet [xschem instance_net $res $pinname]

    regsub {.*\.} $newnet {} newnet
    if { $oldnet eq $newnet } {
      while { [xschem get currsch] } { xschem go_back } 
      puts "Warning: netlist patch already done? "
      return 0
    }
 
    xschem instance [pin_label] $x $y 0 0 [list name=l1 lab=$newnet]
    xschem hilight_netname $newnet
    xschem select instance $res
    xschem hilight_netname $oldnet
    if {$hier} { xschem save} ;# save so we can process other reroute_inst without beink asked to save.
    xschem redraw
    return 1
  }
  return 0
}

## put $new net labels close to pins on all elements connected to $old
proc reroute_net {old new} {
  xschem push_undo
  xschem set no_undo 1
  xschem unhilight
  probe_net $old
  set old_nopath [regsub {.*\.} $old {}]
  set new_nopath [regsub {.*\.} $new {}]
  set devlist [xschem instances_to_net $old_nopath]
  foreach i $devlist {
    set instname [lindex $i 0]
    set x  [expr {[lindex $i 2] - 10}]
    set y  [expr {[lindex $i 3] - 10}]
    xschem instance [pin_label] $x $y 0 0 [list name=l1 lab=$new_nopath]
    xschem select instance $instname
  }
  xschem hilight_netname $new_nopath
  xschem set no_undo 0
}

## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
## $s : schematic name (opamp) or netlist_name if given
## $S : schematic name full path (/home/schippes/.xschem/xschem_library/opamp.sch)
## $d : netlist directory
proc sim_cmd {cmd} {
  global  netlist_dir terminal
  regsub {/$} $netlist_dir {} netlist_dir
  set tool [xschem get netlist_type]
  set d ${netlist_dir}
  set S [xschem get schname]

  set custom_netlist_file [xschem get netlist_name]
  if {$custom_netlist_file ne {}} {
    set s [file rootname $custom_netlist_file]
  } else {
    set s [file tail [file rootname $S]]
  }
  set n ${netlist_dir}/${s}
  if {$tool eq {verilog}} {
    set N ${n}.v
  } else {
    set N ${n}.${tool}
  }
  # puts "N=$N\nn=$n\ns=$s\nS=$S\nd=$d"
  return [subst $cmd]
}

# wrapper to proc simulate, if called from button.
proc simulate_from_button {{callback {}}} {
   global simulate_bg
   set simvar tctx::[xschem get current_win_path]_simulate
   if {![info exists $simvar] || [set $simvar] ne {orange}} {
     simulate $callback
   } elseif {[info exists $simvar] && [set $simvar] eq {orange}} {
     set simulate_id tctx::[xschem get current_win_path]_simulate_id
     if { [info exists $simulate_id] } {
       set id [set $simulate_id]
       set res [kill_running_cmds $id -15]
       if { $res == 0} {
         # something went wrong. Forget about the process
         unset tctx::[xschem get current_win_path]_simulate_id
         set tctx::[xschem get current_win_path]_simulate $simulate_bg
         [xschem get top_path].menubar entryconfigure Simulate -background $simulate_bg
       }
     }
   }
}
 
## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
## $s : schematic name (opamp) or netlist_name if given
## $S : schematic name full path (/home/schippes/.xschem/xschem_library/opamp.sch)
## $d : netlist directory
proc simulate {{callback {}}} { 

  global netlist_dir terminal sim env
  global execute XSCHEM_SHAREDIR has_x OS

  regsub {/$} $netlist_dir {} netlist_dir
  set_sim_defaults
  set netlist_type [xschem get netlist_type]
  if { [set_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool $netlist_type
    set S [xschem get schname]
    set custom_netlist_file [xschem get netlist_name]
    if {$custom_netlist_file ne {}} {
      set s [file rootname $custom_netlist_file]
    } else {
      set s [file tail [file rootname $S]]
    }
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    if { ![info exists  sim($tool,default)] } {
      if { [info exists has_x] } {alert_ "Warning: simulator for $tool is not configured"}
      puts "Warning: simulator for $tool is not configured"
      return -1
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nobackslashes $sim($tool,$def,cmd)]
    
    # window interface       tabbed interface
    # -----------------------------------------
    # top_path   win_path    top_path   win_path
    #  {}        .drw         {}        .drw
    #  .x1       .x1.drw      {}        .x1.drw
    #  .x2       .x2.drw      {}        .x2.drw
    set execute(callback) "
       set_simulate_button [list [xschem get top_path] [xschem get current_win_path]]
       $callback
    "
    if {$fg eq {execute_wait}} {xschem set semaphore [expr {[xschem get semaphore] +1}]}

    set save [pwd]
    cd $netlist_dir
    # Note: Windows $cmd cannot be surrounded by {} as exec will change forward slash to backward slash
    set id [eval execute $st $cmd]   ;# Start simulation process
    cd $save

    if {[info exists has_x] && $id >= 0 } {
      set tctx::[xschem get current_win_path]_simulate_id $id
      set button_path [xschem get top_path].menubar
      $button_path entryconfigure Simulate -background orange
      set tctx::[xschem get current_win_path]_simulate orange
    }

    puts "Simulation started: execution ID: $id"

    if {$fg eq {execute_wait}} {
      if {$id >= 0} {
        vwait execute(pipe,$id)
      }
      xschem set semaphore [expr {[xschem get semaphore] -1}]
    }
    return $id
  } else {
    return -1
  }
}

proc gaw_echoline {} {
  global gaw_fd
  gets $gaw_fd line
  if {[eof $gaw_fd]} {
     puts "finishing connection from gaw"
     close $gaw_fd
     unset gaw_fd
  } else {
    # generate a variable event we can vwait for
    set gaw_fd $gaw_fd
  }
  puts "gaw -> $line"
}

proc setup_tcp_gaw {} {
  global gaw_fd gaw_tcp_address netlist_dir has_x
 
  regsub {/$} $netlist_dir {} netlist_dir
  if { [info exists gaw_fd] } { return 1; } 
  set custom_netlist_file [xschem get netlist_name]
  if {$custom_netlist_file ne {}} {
    set s [file rootname $custom_netlist_file]
  } else {
    set s [file tail [file rootname [xschem get schname 0]]]
  }
  if { ![info exists gaw_fd] && [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {[info exists has_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "Ensure the following line is present uncommented in ~/.gaw/gawrc: up_listenPort = 2020." \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return 0
  }
  fconfigure $gaw_fd -blocking 1 -buffering line -encoding binary -translation binary
  fileevent $gaw_fd readable gaw_echoline
  puts $gaw_fd "table_set $s.raw"
  return 1
}

proc gaw_cmd {cmd} {
  global gaw_fd gaw_tcp_address netlist_dir has_x

  regsub {/$} $netlist_dir {} netlist_dir
  if { ![info exists gaw_fd] && [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {[info exists has_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return
  }
  fconfigure $gaw_fd -blocking 0 -buffering line -encoding binary -translation binary
  puts $gaw_fd "$cmd"
  set n [regexp -all \n $cmd]
  incr n
  puts "gaw command lines: $n"
  fileevent $gaw_fd readable gaw_echoline
  while { $n} { 
    #timeout for abnormal deadlocks 
    set wd [after 10000 set gaw_fd stalled]
    vwait gaw_fd
    if { $gaw_fd ne {stalled} } {
      after cancel $wd
    } else { 
      puts "timeout waiting for gaw response.."
      break
    }
    incr n -1
  }
  close $gaw_fd
  unset gaw_fd
}

proc waves {{type {}}} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp) or netlist_name if given
  ## $S : schematic name full path (/home/schippes/.xschem/xschem_library/opamp.sch)
  ## $d : netlist directory

  global netlist_dir terminal sim XSCHEM_SHAREDIR has_x 
  global bespice_listen_port env simulate_bg execute

  regsub {/$} $netlist_dir {} netlist_dir
  if {$type ne {external} } {
    load_raw $type
    return
  }
  set netlist_type [xschem get netlist_type]
  set_sim_defaults
  if { [set_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool ${netlist_type}
    set S [xschem get schname]

    set custom_netlist_file [xschem get netlist_name]
    if {$custom_netlist_file ne {}} {
      set s [file rootname $custom_netlist_file]
    } else {
      set s [file tail [file rootname $S]]
    }
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    set tool ${tool}wave
    if {![info exists  sim($tool,default)] } {
      if { [info exists has_x] } {alert_ "Warning: viewer for $tool is not configured"}
      puts "Warning: viewer for $tool is not configured"
      return
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nobackslashes $sim($tool,$def,cmd)]

    set save [pwd]
    cd $netlist_dir
    set id [eval execute $st $cmd]
    cd $save

    if {$fg eq {execute_wait}} {
      if {$id >= 0} {
        xschem set semaphore [expr {[xschem get semaphore] +1}]
        vwait execute(pipe,$id)
        xschem set semaphore [expr {[xschem get semaphore] -1}]
      }
    }
  }
}
# ============================================================


proc graph_push_undo {} {
  global graph_change_done

  if {$graph_change_done == 0} {
    xschem push_undo
    set graph_change_done 1
  }
}

# allow change color (via graph_change_wave_color) of double clicked wave
proc graph_edit_wave {n n_wave} {
  global graph_sel_color graph_selected graph_sel_wave
  global graph_schname cadlayers
  if {[winfo exists .graphdialog]} {return}
  set graph_schname [xschem get schname]
  set_ne graph_sel_color 4
  set graph_selected $n
  set graph_sel_wave $n_wave
  set col  [xschem getprop rect 2 $graph_selected color]
  set node [xschem my_strtok_r [xschem getprop rect 2 $graph_selected node] \n \" 0]
  # add default colors if unspecified in col
  set i 0
  foreach graph_node $node {
    if {[lindex $col $i] eq {}} { lappend col $graph_sel_color}
    incr i
  }
  # remove excess colors
  set col [lrange $col 0 [expr {$i - 1}]]
  set graph_sel_color [lindex $col $graph_sel_wave]
  xschem setprop -fast rect 2 $graph_selected color $col 
  xschem draw_graph  $graph_selected
  toplevel .graphdialog -class Dialog
  wm transient .graphdialog [xschem get topwindow]
  frame .graphdialog.f
  button .graphdialog.ok -text OK -command {
    destroy .graphdialog
  }
  button .graphdialog.cancel -text Cancel -command {destroy .graphdialog}
  for {set i 4} {$i < $cadlayers} {incr i} {
    radiobutton .graphdialog.f.r$i -value $i -background [lindex $tctx::colors $i] \
         -variable graph_sel_color -command {graph_change_wave_color $graph_sel_wave } \
         -selectcolor white -foreground black
    pack .graphdialog.f.r$i -side left -fill both -expand yes
  }
  grid .graphdialog.f  - -sticky nsew
  grid .graphdialog.ok .graphdialog.cancel -sticky ew
  grid rowconfig .graphdialog 0 -weight 1
  grid column .graphdialog 0 -weight 1
  grid column .graphdialog 1 -weight 1
  tkwait window .graphdialog
  set graph_schname {}
}


# get selected text from a text widget:
#
# .graphdialog.center.right.text1 get sel.first sel.last
#
# see if a selection is present:
# .graphdialog.center.right.text1 tag ranges sel
#
# replace selected text: 
# .graphdialog.center.right.text1 replace sel.first sel.last BUS
#
# programmatically select text:
# .graphdialog.center.right.text1 tag add sel 1.0 {end - 1 chars}
# clear selection
# .graphdialog.center.right.text1 tag remove sel 1.0 end
# get position of cursor:
# .graphdialog.center.right.text1  index insert
# set cursor position: 
# .graphdialog.center.right.text1 mark set insert 2.18


# add nodes from provided list of {node color} .... 
# used in hilight_net()
proc graph_add_nodes_from_list {nodelist} {
  global graph_bus graph_selected graph_schname has_x
  if {$graph_bus} {
    set sep ,
  } else {
    set sep \n
  }
  if { [info exists has_x] && [winfo exists .graphdialog] } {
    set sel {}
    set current_node_list [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
    set col  [xschem getprop rect 2 $graph_selected color]
    if {[string length $current_node_list] > 0 && ![regexp "\n$" $current_node_list]} {
      .graphdialog.center.right.text1 insert end \n
    }
    set change_done 0
    set first 0
    foreach {i c} $nodelist {
      if {$sel ne {}} {append sel $sep}
      if {!$first  || !$graph_bus } {
        regsub {\[.*} $i {} busname
        lappend col $c
      }
      append sel $i
      set change_done 1
      set first 1
    }
    if {$change_done && $graph_bus} {
      set sel "[string toupper $busname];${sel}\n"
    } else {
      set sel "${sel}\n"
    }
    if {$change_done} {
      .graphdialog.center.right.text1 insert end $sel
      if { [xschem get schname] eq $graph_schname } {
        set node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
        xschem setprop -fastundo rect 2 $graph_selected color $col
        graph_update_nodelist
        regsub -all {[\\"]} $node {\\&} node_quoted
        xschem setprop -fast rect 2 $graph_selected node $node_quoted
        xschem draw_graph $graph_selected
      }
    }
  } else {
    set sel {}
    set change_done 0
    set first 0
    set col  [xschem getprop rect 2 [xschem get graph_lastsel] color]
    set nnn [xschem getprop rect 2 [xschem get graph_lastsel] node]
    foreach {i c} $nodelist {
      if {$sel ne {}} {append sel $sep}
      if {!$first  || !$graph_bus } {
        regsub {\[.*} $i {} busname
        lappend col $c
      }
      append sel $i
      set change_done 1
      set first 1
    }
    if {$change_done && $graph_bus} {
      set sel "[string toupper $busname];${sel}\n"
    } else {
      set sel "${sel}\n"
    }

    if {$change_done} {
      xschem setprop -fastundo rect 2 [xschem get graph_lastsel] color $col
      if {[string length $nnn] > 0 && ![regexp "\n$" $nnn]} {
        append nnn "\n"
      } 
      append nnn $sel
      regsub -all {[\\"]} $nnn {\\&} node_quoted
      xschem setprop -fast rect 2 [xschem get graph_lastsel] node $node_quoted
      xschem draw_graph [xschem get graph_lastsel]
    }
  }
}

# add nodes from left listbox
proc graph_add_nodes {} {
  global graph_bus
  set sel_idx [.graphdialog.center.left.list1 curselection]
  set sel {}
  if {$graph_bus} {
    set sep ,
  } else {
    set sep \n
  }
  set current_node_list [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
  if {[string length $current_node_list] > 0 && ![regexp "\n$" $current_node_list]} {
    .graphdialog.center.right.text1 insert end \n
  }
  set change_done 0
  foreach i $sel_idx {
    set c [.graphdialog.center.left.list1 get $i]
    # escape [ and ] characters.
    set c [regsub -all {([][])}  $c {\\\1}]
    if { ![regexp "(^|\[ \t\n\])${c}($|\[ \t\n\])" $current_node_list]} {
      if {$sel ne {}} {append sel $sep}
      append sel [.graphdialog.center.left.list1 get $i]
      set change_done 1
    }
  }
  if {$change_done && $graph_bus} {
    set sel "BUS_NAME;${sel}\n"
  } else {
    set sel "${sel}\n"
  }
  if {$change_done} {
    .graphdialog.center.right.text1 insert {insert lineend + 1 char} $sel
  }
}

proc graph_get_signal_list {siglist pattern } {
  global graph_sort
  set direction {-decreasing}
  if {$graph_sort} {set direction {-increasing}}
  set result {}
  set siglist [join [lsort $direction -dictionary $siglist] \n]
  # just check if pattern is a valid regexp
  set err [catch {regexp $pattern {12345}} res]
  if {$err} {set pattern {}}
  foreach i $siglist {
    regsub {^v\((.*)\)$} $i {\1} i
    if {[regexp $pattern $i] } {
       lappend result $i
    }
  }
  return $result
}

proc touches {sel tag} {
  set res 0
  scan $sel {%d.%d %d.%d} sel_linestart sel_charstart sel_lineend sel_charend
  scan $tag {%d.%d %d.%d} tag_linestart tag_charstart tag_lineend tag_charend
  set selstart [expr {$sel_linestart * 1000000 + $sel_charstart}]
  set selend [expr {$sel_lineend * 1000000 + $sel_charend}]
  set tagstart [expr {$tag_linestart * 1000000 + $tag_charstart}]
  set tagend [expr {$tag_lineend * 1000000 + $tag_charend}]
  # puts "selstart: $selstart"
  # puts "selend: $selend"
  # puts "tagstart: $tagstart"
  # puts "tagend: $tagend"
  if { ($tagstart >= $selstart && $tagstart <= $selend) ||
       ($tagend >= $selstart && $tagend <= $selend) ||
       ($selstart >= $tagstart && $selstart <= $tagend) ||
       ($selend >= $tagstart && $selend <= $tagend)
     } {
    set res 1
  }
  # puts "touch: returning $res"
  return $res
}

proc set_graph_default_colors {} {
  global graph_selected graph_schname
  if { [xschem get schname] ne $graph_schname } return
  xschem setprop -fast rect 2 $graph_selected color "4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21"
  graph_update_nodelist
  xschem draw_graph $graph_selected
}

# change color of selected wave in text widget and redraw graph
# OR
# change color attribute of wave given as parameter, redraw graph
proc graph_change_wave_color {{wave {}}} {
  global graph_sel_color graph_selected graph_schname

  if { [xschem get schname] ne $graph_schname } return
  #  get tag the cursor is on:
  if { $wave eq {}} {
    set sel_range [.graphdialog.center.right.text1 tag ranges sel]
    if {$sel_range ne {}} {
      # puts "sel_range --> $sel_range"
      foreach tag [.graphdialog.center.right.text1 tag names] {
        if {$tag eq {sel}} {continue}
        set tag_range [.graphdialog.center.right.text1 tag ranges $tag]
        # puts "$tag --> $tag_range"

        if { [touches $sel_range $tag_range]} {
          set index [string range $tag 1 end]
          set col  [xschem getprop rect 2 $graph_selected color]
          set col [lreplace $col $index $index  $graph_sel_color]
          xschem setprop -fast rect 2 $graph_selected color $col 
        }
      }
      graph_update_nodelist
      xschem draw_graph $graph_selected
    } else {
      set tag [.graphdialog.center.right.text1 tag names insert]
      if { $tag eq {}} {set tag [.graphdialog.center.right.text1 tag names {insert - 1 char}]}
      if { [regexp {^t} $tag]} {
        set index [string range $tag 1 end]
        set col  [xschem getprop rect 2 $graph_selected color]
        set col [lreplace $col $index $index  $graph_sel_color]
        xschem setprop -fast rect 2 $graph_selected color $col 
        graph_update_nodelist
        xschem draw_graph $graph_selected
      }
    }
  # wave to change provided as parameter
  } else {
    set col  [xschem getprop rect 2 $graph_selected color]
    set col [lreplace $col $wave $wave  $graph_sel_color]
    xschem setprop -fast rect 2 $graph_selected color $col 
    xschem draw_graph $graph_selected
  }
}

# set txt [xschem getprop rect 2 $n node 2]
# set txt [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
# return first and last indexes of nodes in txt
proc graph_tag_nodes {txt} {
  global graph_selected graph_sel_color
  # delete old tags
  if { [winfo exists .graphdialog.center.right.text1] } {
    eval .graphdialog.center.right.text1 tag delete [ .graphdialog.center.right.text1 tag names] 
    set col  [xschem getprop rect 2 $graph_selected color]
    set col [string trim $col " \n"]
  }


  set start 0
  if {[regexp {^tcleval\(} $txt]} {
    set start 8
    regsub {\)[ \n]*$} $txt {} txt
  }
  set regx {("[^"]+")|([^\n]+)}
  
  set tt {}
  set cc {}
  while {[regexp -indices -start $start $regx $txt idxall]} {
    lappend tt [lindex $idxall 0]
    set ccc [lindex $idxall 1]
    lappend cc $ccc
    set start [expr {$ccc + 1}]
  }
  if { [winfo exists .graphdialog.center.right.text1] } {
    set n 0
    if { $tt ne {} } {
      foreach t $tt c $cc {
        set col_idx [lindex $col $n]
        # add missing colors
        if {$col_idx eq {}} {
          set col_idx $graph_sel_color
          lappend col $graph_sel_color
        }
        set b [lindex $tctx::colors $col_idx]
        .graphdialog.center.right.text1 tag add t$n "1.0 + $t chars" "1.1 + $c chars"
        if { [info tclversion] > 8.4} {
          .graphdialog.center.right.text1 tag configure t$n -background $b -selectbackground grey40
        } else {
          .graphdialog.center.right.text1 tag configure t$n -background $b
        }
        incr n
      }
      # remove excess colors
      set col [lrange $col 0 [expr {$n - 1}]]
    } else {
      set col {}
    }
    xschem setprop -fast rect 2 $graph_selected color $col 
  }
  return [list $tt $cc] 
}

# tag nodes in text widget with assigned colors 
proc graph_update_nodelist {} {
  global graph_selected graph_sel_color graph_schname
  if { [xschem get schname] ne $graph_schname } return
  # tagging nodes in text widget:
  set txt [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
  graph_tag_nodes $txt
}

proc graph_fill_listbox {} {
  global graph_selected
  set pattern [.graphdialog.center.left.search get]
  set retval {}
  set autoload [uplevel #0 {subst [xschem getprop rect 2 $graph_selected autoload 2]}]
  set rawfile [xschem getprop rect 2 $graph_selected rawfile]
  if {$rawfile ne {}} {
    if {![catch {eval uplevel #0 {subst $rawfile}} res]} {
      set rawfile $res
    }
  }
  set sim_type [uplevel #0 {subst [xschem getprop rect 2 $graph_selected sim_type 2]}]
  if {$autoload ne {} && $autoload } { set autoload read} else {set autoload switch}
  # puts "graph_fill_listbox: $rawfile $sim_type"
  if {$rawfile ne {}} {
    if {$sim_type eq {table}} {
      set res [xschem raw table_read $rawfile $sim_type]
    } else {
      set res [xschem raw $autoload $rawfile $sim_type]
    }
    if {$res} {
      set retval [graph_get_signal_list [xschem raw_query list] $pattern]
      xschem raw switch_back
    }
    # puts "switch back"
  } elseif {[xschem raw loaded] != -1} {
    set retval [graph_get_signal_list [xschem raw_query list] $pattern]
  }
  .graphdialog.center.left.list1 delete 0 end
  eval .graphdialog.center.left.list1 insert 0 $retval

}

# called from event handlers (OK, KeyRelease, DoubleClick) in graph_edit_properties
proc graph_update_node {node} {
  global graph_selected
  graph_update_nodelist
  # add a backslash before " and \ characters
  # note the double escaping for regsub replace string
  regsub -all {[\\"]} $node {\\&} node_quoted
  graph_push_undo
  xschem setprop -fast rect 2 $graph_selected node $node_quoted 
  xschem draw_graph $graph_selected
}

proc graph_update_div {graph_selected div} {
  set divis [.graphdialog.top2.$div get]
  if {[regexp {^[0-9]+$} $divis] && $divis < 1} {
    set divis 1
    .graphdialog.top2.$div delete 0 end
    .graphdialog.top2.$div insert 0 $divis
  }
  graph_push_undo
  xschem setprop rect 2 $graph_selected $div $divis
  xschem draw_graph $graph_selected
}

proc graph_set_linewidth {graph_sel} {
  global graph_linewidth_mult
  set custom_lw [.graphdialog.top.lwe get]
  if {[regexp {^[ \t]*$} $custom_lw]} {
    set custom_lw  $graph_linewidth_mult
  }
  graph_push_undo
  if {$custom_lw != $graph_linewidth_mult} {
    xschem setprop rect 2 $graph_sel linewidth_mult $custom_lw
  } else {
    xschem setprop rect 2 $graph_sel linewidth_mult ;# delete attribute since it is default
  }
}

proc raw_is_loaded {rawfile type} {
  set loaded 0

  set r [catch "uplevel #0 {subst $rawfile}" res]
  if {$r == 0} {
    set rawfile $res
  } else {
    return $loaded
  }
  set rawlist [lrange [xschem raw info] 2 end]
  foreach {n f t} $rawlist {
    if {$rawfile eq $f && $type eq $t} {
       set loaded 1
       break
    }
  }
  return $loaded
}
proc set_rect_flags {graph_selected} {
      global graph_private_cursor graph_unlocked
      
      if {$graph_private_cursor} {
        set private_cursor {,private_cursor}
      } else {
        set private_cursor {}
      }
      if {$graph_unlocked} {
        set unlocked {,unlocked}
      } else {
        set unlocked {}
      }
      xschem setprop -fast rect 2 $graph_selected flags "graph$unlocked$private_cursor" 
  }

proc graphdialog_set_raw_props {} {
  global graph_selected
  xschem setprop -fast rect 2 $graph_selected rawfile [.graphdialog.center.right.rawentry get] 
  xschem setprop -fast rect 2 $graph_selected sim_type [.graphdialog.center.right.list get] 
  graph_fill_listbox
}

proc graph_edit_properties {n} {
  global graph_bus graph_sort graph_digital graph_selected graph_sel_color graph_legend
  global graph_unlocked graph_schname graph_logx graph_logy cadlayers graph_rainbow 
  global graph_linewidth_mult graph_change_done has_x graph_dialog_default_geometry
  global graph_autoload graph_private_cursor

  if { ![info exists has_x]} {return} 
  set graph_change_done 0
  if { [winfo exists .graphdialog]} {
    set graph_dialog_default_geometry [winfo geometry .graphdialog]
  } 
  catch {destroy .graphdialog}
  toplevel .graphdialog -class Dialog ;# -width 1 -height 1
  wm withdraw .graphdialog
  wm transient .graphdialog [xschem get topwindow]
  update idletasks

  set graph_selected $n
  set graph_schname [xschem get schname]
  set_ne graph_sel_color 4
  set_ne graph_sort 0
  set graph_rainbow 0
  if {[xschem getprop rect 2 $n rainbow] == 1} {set graph_rainbow 1}
  set graph_logx 0
  if {[xschem getprop rect 2 $n logx] == 1} {set graph_logx 1}
  set graph_logy 0
  if {[xschem getprop rect 2 $n logy] == 1} {set graph_logy 1}
  set graph_legend 1
  if {[xschem getprop rect 2 $n legend] == 0} {set graph_legend 0}
  set graph_digital 0
  if {[xschem getprop rect 2 $n digital] == 1} {set graph_digital 1}

  if {[regexp {private_cursor} [xschem getprop rect 2 $n flags]]} {
    set graph_private_cursor 1
  } else {
    set graph_private_cursor 0
  }

  if {[regexp {unlocked} [xschem getprop rect 2 $n flags]]} {
    set graph_unlocked 1
  } else {
    set graph_unlocked 0
  }

  set autoload [xschem getprop rect 2 $n autoload]
  if {$autoload ne {} && $autoload} {
    set graph_autoload 1
  } else {
    set graph_autoload 0
  }
  
  frame .graphdialog.top
  # another row of buttons
  frame .graphdialog.top2 
  frame .graphdialog.top3 
  panedwindow .graphdialog.center -orient horiz
  frame .graphdialog.bottom
  frame .graphdialog.center.left
  frame .graphdialog.center.right
  .graphdialog.center add .graphdialog.center.left .graphdialog.center.right
  pack .graphdialog.top -side top -fill x 
  pack .graphdialog.top2 -side top -fill x 
  pack .graphdialog.top3 -side top -fill x 
  pack .graphdialog.center -side top -fill both -expand yes
  pack .graphdialog.bottom -side top -fill x 
  # center-left frame
  label .graphdialog.center.left.labsearch -text Search:
  entry .graphdialog.center.left.search -width 10 
  entry_replace_selection .graphdialog.center.left.search
  button .graphdialog.center.left.add -text Add -command {
    graph_add_nodes; graph_update_nodelist
  }
  listbox .graphdialog.center.left.list1 -width 20 -height 5 -selectmode extended \
     -yscrollcommand {.graphdialog.center.left.yscroll set} \
     -xscrollcommand {.graphdialog.center.left.xscroll set}
  scrollbar .graphdialog.center.left.yscroll -command {.graphdialog.center.left.list1 yview}
  scrollbar .graphdialog.center.left.xscroll -orient horiz -command {.graphdialog.center.left.list1 xview}

  grid .graphdialog.center.left.labsearch .graphdialog.center.left.search .graphdialog.center.left.add
  grid .graphdialog.center.left.list1 - - .graphdialog.center.left.yscroll -sticky nsew
  grid .graphdialog.center.left.xscroll - - -sticky nsew
  grid rowconfig .graphdialog.center.left 0 -weight 0
  grid rowconfig .graphdialog.center.left 1 -weight 1 -minsize 2c
  grid columnconfig .graphdialog.center.left 0 -weight 1
  grid columnconfig .graphdialog.center.left 1 -weight 1

  # center right frame
  checkbutton .graphdialog.center.right.autoload -text {Auto load}  -variable graph_autoload \
    -command {
      if {$graph_autoload} {
        xschem setprop -fast rect 2 $graph_selected autoload 1 
      } else {
        xschem setprop -fast rect 2 $graph_selected autoload 0 
      } 
    } 
  label .graphdialog.center.right.lab2 -text {    Sim type:}
  if { [info tclversion] > 8.4} {
    ttk::combobox .graphdialog.center.right.list \
      -values {dc ac tran op sp spectrum noise constants table}  -width 9
  } else {
    entry .graphdialog.center.right.list -width 4
    entry_replace_selection .graphdialog.center.right.list
  }
  if { [info tclversion] > 8.4} {
    bind .graphdialog.center.right.list <<ComboboxSelected>> {
      xschem setprop -fast rect 2 $graph_selected sim_type [.graphdialog.center.right.list get] 
      graph_fill_listbox
    }
    if { [xschem getprop rect 2 $graph_selected sim_type 2] ne {}} {
      .graphdialog.center.right.list set [xschem getprop rect 2 $graph_selected sim_type 2]
    } else {
      .graphdialog.center.right.list set {}
    }
  } else {
    .graphdialog.center.right.list delete 0 end
    if { [xschem getprop rect 2 $graph_selected sim_type 2] ne {}} {
      .graphdialog.center.right.list insert 0 [xschem getprop rect 2 $graph_selected sim_type 2]
    } else {
      .graphdialog.center.right.list insert 0 {}
    }
  }

  bind .graphdialog.center.right.list <KeyRelease> {
    xschem setprop -fast rect 2 $graph_selected sim_type [.graphdialog.center.right.list get] 
    graph_fill_listbox
  }


  entry .graphdialog.center.right.rawentry -width 20
  entry_replace_selection .graphdialog.center.right.rawentry
  button .graphdialog.center.right.rawbut -text {Raw file:} -command {
    regsub {/$} $netlist_dir {} netlist_dir
   .graphdialog.center.right.rawentry delete 0 end
   .graphdialog.center.right.rawentry insert 0 [string map [list $netlist_dir {$netlist_dir}] [select_raw]]
    graphdialog_set_raw_props
  }

  bind .graphdialog.center.right.rawentry <KeyRelease> graphdialog_set_raw_props
  bind .graphdialog.center.right.rawentry <Leave> graphdialog_set_raw_props

  .graphdialog.center.right.rawentry insert 0 [xschem getprop rect 2 $graph_selected rawfile 2]
  .graphdialog.center.right.rawentry xview moveto 1
  text .graphdialog.center.right.text1 -undo 1 -wrap none -width 50 -height 5 -background grey90 -fg black \
     -insertbackground grey40 -exportselection 1 \
     -yscrollcommand {.graphdialog.center.right.yscroll set} \
     -xscrollcommand {.graphdialog.center.right.xscroll set}
  scrollbar .graphdialog.center.right.yscroll -command {.graphdialog.center.right.text1 yview}
  scrollbar .graphdialog.center.right.xscroll -orient horiz -command {.graphdialog.center.right.text1 xview}

  grid .graphdialog.center.right.autoload \
       .graphdialog.center.right.lab2 .graphdialog.center.right.list \
       .graphdialog.center.right.rawbut  .graphdialog.center.right.rawentry -
  grid configure .graphdialog.center.right.rawentry -sticky ew
  grid .graphdialog.center.right.text1 - - - - .graphdialog.center.right.yscroll -sticky nsew
  grid .graphdialog.center.right.xscroll - - - - - -sticky ew
  grid rowconfig .graphdialog.center.right 0 -weight 0
  grid rowconfig .graphdialog.center.right 1 -weight 1 -minsize 3c
  grid rowconfig .graphdialog.center.right 2 -weight 0
  grid columnconfig .graphdialog.center.right 0 -weight 0
  grid columnconfig .graphdialog.center.right 1 -weight 0
  grid columnconfig .graphdialog.center.right 2 -weight 0
  grid columnconfig .graphdialog.center.right 3 -weight 0
  grid columnconfig .graphdialog.center.right 4 -weight 1
  grid columnconfig .graphdialog.center.right 5 -weight 0

  # bottom frame
  button .graphdialog.bottom.cancel -padx 1 -borderwidth 1 -pady 0 -text Cancel -command {
    set graph_dialog_default_geometry [winfo geometry .graphdialog]
    destroy .graphdialog
    set graph_selected {}
    set graph_schname {}
  }
  button .graphdialog.bottom.ok -padx 1 -borderwidth 1 -pady 0 -text OK -command {
    if { [xschem get schname] eq $graph_schname } {
      graph_push_undo
      graph_update_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
      xschem setprop -fast rect 2 $graph_selected x1 [.graphdialog.top3.xmin get] 
      xschem setprop -fast rect 2 $graph_selected x2 [.graphdialog.top3.xmax get] 
      xschem setprop -fast rect 2 $graph_selected y1 [.graphdialog.top3.ymin get] 
      xschem setprop -fast rect 2 $graph_selected y2 [.graphdialog.top3.ymax get] 
      set_rect_flags $graph_selected
    }
    set graph_dialog_default_geometry [winfo geometry .graphdialog]
    destroy .graphdialog
    set graph_selected {}
    set graph_schname {}
  }
  button .graphdialog.bottom.apply -padx 1 -borderwidth 1 -pady 0 -text Apply -command {
    if { [xschem get schname] eq $graph_schname } {
      graph_push_undo
      graph_update_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
      xschem setprop -fast rect 2 $graph_selected x1 [.graphdialog.top3.xmin get] 
      xschem setprop -fast rect 2 $graph_selected x2 [.graphdialog.top3.xmax get] 
      xschem setprop -fast rect 2 $graph_selected y1 [.graphdialog.top3.ymin get] 
      xschem setprop -fast rect 2 $graph_selected y2 [.graphdialog.top3.ymax get] 
      set_rect_flags $graph_selected
    }
  }

  # top packs
  pack .graphdialog.bottom.ok -side left
  pack .graphdialog.bottom.apply -side left
  pack .graphdialog.bottom.cancel -side left

  for {set i 4} {$i <= $cadlayers} {incr i} {
    if {$i == $cadlayers } {
      button .graphdialog.bottom.r$i -padx 1 -borderwidth 1 -pady 0 \
        -command "set_graph_default_colors" \
        -text {AUTO SET}
    } else {
      radiobutton .graphdialog.bottom.r$i -value $i -background [lindex $tctx::colors $i] \
        -variable graph_sel_color -command graph_change_wave_color \
        -selectcolor white -foreground black
    }
    pack .graphdialog.bottom.r$i -side left
  }

  # top2 frame
  checkbutton .graphdialog.top.legend -text {Legend} -variable graph_legend -indicatoron 1 \
    -command {
       if { [xschem get schname] eq $graph_schname } {
         graph_push_undo
         xschem setprop -fast rect 2 $graph_selected legend $graph_legend 
         xschem draw_graph $graph_selected
       }
     }
  label .graphdialog.top2.labunitx -text {X units}
  spinbox .graphdialog.top2.unitx -values {f p n u m 1 k M G T} -width 2 \
   -command {
      graph_push_undo
      xschem setprop rect 2 $graph_selected unitx [.graphdialog.top2.unitx get]
      xschem draw_graph $graph_selected
    }

  label .graphdialog.top2.labunity -text {  Y units}
  spinbox .graphdialog.top2.unity -values {f p n u m 1 k M G T} -width 2 \
   -command {
      graph_push_undo
      xschem setprop rect 2 $graph_selected unity [.graphdialog.top2.unity get]
      xschem draw_graph $graph_selected
    }

  label .graphdialog.top2.labdivx -text {  X div.}
  entry .graphdialog.top2.divx -width 2
  entry_replace_selection .graphdialog.top2.divx
  bind .graphdialog.top2.divx <KeyRelease> {
    graph_update_div $graph_selected divx
  }

  label .graphdialog.top2.labdivy -text {  Y div.}
  entry .graphdialog.top2.divy -width 2
  entry_replace_selection .graphdialog.top2.divy
  bind .graphdialog.top2.divy <KeyRelease> {
    graph_update_div $graph_selected divy
  }

  label .graphdialog.top2.labsubdivx -text {  X subdiv.}
  entry .graphdialog.top2.subdivx  -width 2
  entry_replace_selection .graphdialog.top2.subdivx
  bind .graphdialog.top2.subdivx <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected subdivx [.graphdialog.top2.subdivx get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top2.labsubdivy -text {  Y subdiv.}
  entry .graphdialog.top2.subdivy -width 2
  entry_replace_selection .graphdialog.top2.subdivy
  bind .graphdialog.top2.subdivy <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected subdivy [.graphdialog.top2.subdivy get]
    xschem draw_graph $graph_selected
  }
  
  label .graphdialog.top2.labsweep -text {  Sweep}
  entry .graphdialog.top2.sweep -width 10 
  entry_replace_selection .graphdialog.top2.sweep

  label .graphdialog.top2.labmode -text {Mode}
  if  { [info tclversion] > 8.4} {
    ttk::combobox .graphdialog.top2.mode -values {Line HistoV HistoH} -width 6
    bind .graphdialog.top2.mode <<ComboboxSelected>> {
      graph_push_undo
      xschem setprop rect 2 $graph_selected mode [.graphdialog.top2.mode get]
      xschem draw_graph $graph_selected
    }
  } else {
    spinbox .graphdialog.top2.mode -values {Line HistoV HistoH} -width 6 \
     -command {
        graph_push_undo
        xschem setprop rect 2 $graph_selected mode [.graphdialog.top2.mode get]
        xschem draw_graph $graph_selected
      }
  }

  # bind .graphdialog.top2.sweep <KeyRelease> {
  #   graph_push_undo
  #   xschem setprop rect 2 $graph_selected sweep [.graphdialog.top2.sweep get]
  #   xschem draw_graph $graph_selected
  # }

  bind .graphdialog.top2.sweep <Leave> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected sweep [.graphdialog.top2.sweep get]
    xschem draw_graph $graph_selected
  }   

  .graphdialog.top2.sweep insert 0 [xschem getprop rect 2 $graph_selected sweep]
  
  set graph_divx [xschem getprop rect 2 $graph_selected divx]
  if {$graph_divx eq {}} { set graph_divx 5}
  .graphdialog.top2.divx insert 0 $graph_divx

  set graph_divy [xschem getprop rect 2 $graph_selected divy]
  if {$graph_divy eq {}} { set graph_divy 5}
  .graphdialog.top2.divy insert 0 $graph_divy

  set graph_subdivx [xschem getprop rect 2 $graph_selected subdivx]
  if {$graph_subdivx eq {}} { set graph_subdivx 1}
  .graphdialog.top2.subdivx insert 0 $graph_subdivx

  set graph_subdivy [xschem getprop rect 2 $graph_selected subdivy]
  if {$graph_subdivy eq {}} { set graph_subdivy 1}
  .graphdialog.top2.subdivy insert 0 $graph_subdivy

  set graph_unitx [xschem getprop rect 2 $graph_selected unitx]
  if {$graph_unitx eq {}} { set graph_unitx 1}
  .graphdialog.top2.unitx set $graph_unitx

  set graph_unity [xschem getprop rect 2 $graph_selected unity]
  if {$graph_unity eq {}} { set graph_unity 1}
  .graphdialog.top2.unity set $graph_unity

  set graph_mode [xschem getprop rect 2 $graph_selected mode]
  if {$graph_mode eq {}} { set graph_mode Line}
  .graphdialog.top2.mode set $graph_mode

  pack .graphdialog.top2.labunitx .graphdialog.top2.unitx \
       .graphdialog.top2.labunity .graphdialog.top2.unity -side left

  pack .graphdialog.top2.labdivx .graphdialog.top2.divx \
       .graphdialog.top2.labdivy .graphdialog.top2.divy \
       .graphdialog.top2.labsubdivx .graphdialog.top2.subdivx \
       .graphdialog.top2.labsubdivy .graphdialog.top2.subdivy -side left 
  pack .graphdialog.top2.labmode .graphdialog.top2.mode .graphdialog.top2.labsweep -side left 
  pack .graphdialog.top2.sweep -side left -fill x -expand yes

  # top frame
  checkbutton .graphdialog.top.bus -text Bus -padx 2 -variable graph_bus 
  checkbutton .graphdialog.top.incr -text {Incr. sort} -variable graph_sort -indicatoron 1 \
    -command graph_fill_listbox
  checkbutton .graphdialog.top.rainbow -text {Rainbow col.} -variable graph_rainbow \
    -command {
       if { [xschem get schname] eq $graph_schname } {
         graph_push_undo
         xschem setprop -fast rect 2 $graph_selected rainbow $graph_rainbow 
         xschem draw_graph $graph_selected
       }
     }
  label .graphdialog.top.lw -text "  Line width:"
  entry .graphdialog.top.lwe -width 4 
  entry_replace_selection .graphdialog.top.lwe
  bind .graphdialog.top.lwe <KeyRelease> {
    graph_set_linewidth $graph_selected
    xschem draw_graph $graph_selected
  }
  set custom_lw [xschem getprop rect 2 $n linewidth_mult]
  if {[regexp {^[ \t]*$} $custom_lw]} {
    .graphdialog.top.lwe insert 0 $graph_linewidth_mult 
  } else {
    .graphdialog.top.lwe insert 0 $custom_lw
  }

  label .graphdialog.top.labdset -text {  Dataset}
  entry .graphdialog.top.dset -width 4
  entry_replace_selection .graphdialog.top.dset
  bind .graphdialog.top.dset <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected dataset [.graphdialog.top.dset get]
    xschem draw_graph $graph_selected
  }
  .graphdialog.top.dset insert 0 [xschem getprop rect 2 $graph_selected dataset]
  
  checkbutton .graphdialog.top.priv_curs -text {Priv. Cursor} -variable graph_private_cursor \
  -command {set_rect_flags $graph_selected }

  checkbutton .graphdialog.top.unlocked -text {Unlock. X axis} -variable graph_unlocked \
  -command {set_rect_flags $graph_selected }

  checkbutton .graphdialog.top.dig -text {Digital} -variable graph_digital -indicatoron 1 \
    -command {
       if { [xschem get schname] eq $graph_schname } {
         graph_push_undo
         xschem setprop -fast rect 2 $graph_selected digital $graph_digital 
         xschem draw_graph $graph_selected
       }
     }
  label .graphdialog.top3.xlabmin -text { X min:}
  entry .graphdialog.top3.xmin -width 7
  entry_replace_selection .graphdialog.top3.xmin
  bind .graphdialog.top3.xmin <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected x1 [.graphdialog.top3.xmin get]
    xschem draw_graph $graph_selected
  }
  label .graphdialog.top3.xlabmax -text { X max:}
  entry .graphdialog.top3.xmax -width 7
  entry_replace_selection .graphdialog.top3.xmax
  bind .graphdialog.top3.xmax <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected x2 [.graphdialog.top3.xmax get]
    xschem draw_graph $graph_selected
  }


  label .graphdialog.top3.ylabmin -text { Y min:}
  entry .graphdialog.top3.ymin -width 7
  entry_replace_selection .graphdialog.top3.ymin
  bind .graphdialog.top3.ymin <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected y1 [.graphdialog.top3.ymin get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top3.ylabmax -text { Y max:}
  entry .graphdialog.top3.ymax -width 7
  entry_replace_selection .graphdialog.top3.ymax
  bind .graphdialog.top3.ymax <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected y2 [.graphdialog.top3.ymax get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top3.xlabmag -text { X/Y lab mag:}
  entry .graphdialog.top3.xmag -width 4
  entry_replace_selection .graphdialog.top3.xmag
  bind .graphdialog.top3.xmag <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected xlabmag [.graphdialog.top3.xmag get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top3.ylabmag -text { }
  entry .graphdialog.top3.ymag -width 4
  entry_replace_selection .graphdialog.top3.ymag
  bind .graphdialog.top3.ymag <KeyRelease> {
    graph_push_undo
    xschem setprop rect 2 $graph_selected ylabmag [.graphdialog.top3.ymag get]
    xschem draw_graph $graph_selected
  }


  pack .graphdialog.top.legend -side left
  pack .graphdialog.top.incr -side left
  pack .graphdialog.top.bus -side left
  pack .graphdialog.top.priv_curs -side left
  pack .graphdialog.top.dig -side left
  pack .graphdialog.top.unlocked -side left
  pack .graphdialog.top.rainbow -side left
  pack .graphdialog.top.lw -side left
  pack .graphdialog.top.lwe -side left
  pack .graphdialog.top.labdset -side left
  pack .graphdialog.top.dset -side left
  .graphdialog.top3.ymin insert 0 [xschem getprop rect 2 $graph_selected y1]
  .graphdialog.top3.ymax insert 0 [xschem getprop rect 2 $graph_selected y2]
  .graphdialog.top3.xmin insert 0 [xschem getprop rect 2 $graph_selected x1]
  .graphdialog.top3.xmax insert 0 [xschem getprop rect 2 $graph_selected x2]
  .graphdialog.top3.xmag insert 0 [xschem getprop rect 2 $graph_selected xlabmag]
  .graphdialog.top3.ymag insert 0 [xschem getprop rect 2 $graph_selected ylabmag]

  # top3 frame
  set graph_rainbow [xschem getprop rect 2 $graph_selected rainbow]
  set graph_logx [xschem getprop rect 2 $graph_selected logx]
  set graph_logy [xschem getprop rect 2 $graph_selected logy]
  if { $graph_rainbow eq {} } { set graph_rainbow 0 }
  if { $graph_logx eq {} } { set graph_logx 0 }
  if { $graph_logy eq {} } { set graph_logy 0 }
  checkbutton .graphdialog.top3.logx -padx 2 -text {Log X} -variable graph_logx \
     -command {
       if { [xschem get schname] eq $graph_schname } {
         graph_push_undo
         xschem setprop -fast rect 2 $graph_selected logx $graph_logx 
         if { $graph_logx eq 1} {
           graph_push_undo
           xschem setprop -fast rect 2 $graph_selected subdivx 8 
           .graphdialog.top2.subdivx delete 0 end
           .graphdialog.top2.subdivx insert 0 8
           xschem setprop rect 2 $graph_selected fullxzoom
           xschem setprop rect 2 $graph_selected fullyzoom
         } else {
           graph_push_undo
           xschem setprop -fast rect 2 $graph_selected subdivx 4 
           .graphdialog.top2.subdivx delete 0 end
           .graphdialog.top2.subdivx insert 0 4
           xschem setprop rect 2 $graph_selected fullxzoom
           xschem setprop rect 2 $graph_selected fullyzoom
         }
         xschem draw_graph $graph_selected
       }
     }

  checkbutton .graphdialog.top3.logy -text {Log Y} -variable graph_logy \
     -command {
       if { [xschem get schname] eq $graph_schname } {
         graph_push_undo
         xschem setprop -fast rect 2 $graph_selected logy $graph_logy 
         if { $graph_logy eq 1} {
           graph_push_undo
           xschem setprop -fast rect 2 $graph_selected subdivy 8 
           .graphdialog.top2.subdivy delete 0 end
           .graphdialog.top2.subdivy insert 0 8
           xschem setprop rect 2 $graph_selected fullyzoom
         } else {
           graph_push_undo
           xschem setprop -fast rect 2 $graph_selected subdivy 4 
           .graphdialog.top2.subdivy delete 0 end
           .graphdialog.top2.subdivy insert 0 4
           xschem setprop rect 2 $graph_selected fullyzoom
         }
         xschem draw_graph $graph_selected
       }
     }
  pack .graphdialog.top3.logx .graphdialog.top3.logy \
   .graphdialog.top3.xlabmin .graphdialog.top3.xmin .graphdialog.top3.xlabmax .graphdialog.top3.xmax \
   .graphdialog.top3.ylabmin .graphdialog.top3.ymin .graphdialog.top3.ylabmax .graphdialog.top3.ymax \
   .graphdialog.top3.xlabmag .graphdialog.top3.xmag .graphdialog.top3.ylabmag .graphdialog.top3.ymag \
   -fill x -expand yes -side left
  # binding
  bind .graphdialog.center.left.search <KeyRelease> {
    graph_fill_listbox
  }
  bind .graphdialog.center.left.list1 <Double-Button-1> {
    graph_add_nodes
    if { [xschem get schname] eq $graph_schname } {
      graph_update_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
    }
  }

  bind .graphdialog.center.right.text1 <KeyRelease> {
    if { [xschem get schname] eq $graph_schname } {
      graph_update_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
    }
  }
  bind .graphdialog <Control-Return> {
    .graphdialog.bottom.ok invoke
  }
  bind .graphdialog <Escape> {
    .graphdialog.bottom.cancel invoke
  }
  wm protocol .graphdialog  WM_DELETE_WINDOW {
    .graphdialog.bottom.cancel invoke
  }
  # fill data in left listbox
  graph_fill_listbox

  # fill data in right textbox
  set plotted_nodes [xschem getprop rect 2 $n node 2]
  if {[string length $plotted_nodes] > 0 && [string index $plotted_nodes end] ne "\n"} {append plotted_nodes \n}
  .graphdialog.center.right.text1 insert 1.0 $plotted_nodes
  graph_update_nodelist
  # add stuff in textbox at end of line + 1 char (after newline) 
  # .graphdialog.center.right.text1 insert {insert lineend + 1 char} foo\n
  # tkwait window .graphdialog
  wm deiconify .graphdialog
  if {$graph_dialog_default_geometry ne {}} { wm geometry .graphdialog $graph_dialog_default_geometry}
}

proc graph_show_measure {{action show}} {
  global measure_id measure_text has_x
 
  if {![info exists has_x]} return;
  set_ne measure_text "y=\nx="
  if { [info exists measure_id] } {
    after cancel $measure_id
    unset measure_id
  }
  destroy .measure
  if {$action eq {stop}} { return }
  set measure_id [after 400 {
    unset measure_id
    toplevel .measure -background {}
    label .measure.lab -text $measure_text -background black -fg yellow -justify left
    pack .measure.lab
    wm overrideredirect .measure 1
    wm geometry .measure +[expr {[winfo pointerx .measure] +10}]+[expr {[winfo pointery .measure] -8}]
  }]
}

# launch a terminal shell, if 'curpath' is given set path to 'curpath'
proc get_shell { {curpath {}} } {
 global netlist_dir debug_var
 global  terminal

 regsub {/$} $netlist_dir {} netlist_dir
 set save [pwd]
 if { $curpath ne {} } {
   cd $curpath
   eval execute 0 $terminal
 } else {
   eval execute 0 $terminal
 }
 cd $save
}

proc edit_netlist {netlist } {
 global netlist_dir debug_var
 global editor terminal OS

 regsub {/$} $netlist_dir {} netlist_dir
 set netlist_type [xschem get netlist_type]

 if { [regexp vim $editor] } { set ftype "-c \":set filetype=$netlist_type\"" } else { set ftype {} }
 if { [set_netlist_dir 0] ne "" } {
   set save [pwd]
   cd $netlist_dir
   if {$OS == "Windows"} {
     set cmd "$editor \"${netlist}\""
     eval exec $cmd &
   } else {
     eval execute 0 $editor $ftype  \"${netlist}\"
   }
   cd $save
 }
 return {}
}


# 20180926
# global_initdir should be set to:
#   INITIALLOADDIR  for load
#   INITIALINSTDIR  for instance placement
# ext:  .sch or .sym or .sch.sym or .sym.sch
#
proc save_file_dialog { msg ext global_initdir {initialf {}} {overwrt 1} } {
  upvar #0 $global_initdir initdir
  set temp $initdir
  if { $initialf ne {}} {
    set initialdir [file dirname $initialf]
    set initialf [file tail $initialf]
  } else {
    set initialdir $initdir
    set initialf {}
  }
  set initdir $initialdir
  set r [load_file_dialog $msg $ext $global_initdir 0 $overwrt $initialf] 
  set initdir $temp
  return $r
}

# opens indicated instance (or selected one) into a separate tab/window
# keeping the hierarchy path, as it was descended into (as with 'e' key).
proc open_sub_schematic {{inst {}} {inst_number 0}} {
  global search_schematic
  set rawfile {}
  set n_sel [xschem get lastsel]
 
  if { $inst eq {} && $n_sel == 0} {
    if {$search_schematic == 1} {
      set f [abs_sym_path [xschem get current_name] {.sch}]
    } else {
      set f [file rootname [xschem get schname]].sch
    }
    xschem new_schematic create {} $f
    return 1
  } elseif { $inst eq {} && $n_sel == 1} {
    set inst [lindex [xschem selected_set] 0]
    xschem unselect_all
  } else {
    set instlist {}
    # get list of instances (instance names only) 
    foreach {i s t} [xschem instance_list] {lappend instlist $i}
    # if provided $inst is not in the list return 0
    if {[lsearch -exact $instlist $inst] == -1} {return 0}
  }
  # open a new top level in another window / tab
  if {[xschem raw loaded] >= 0} {
    set rawfile [xschem raw_query rawfile]
    set sim_type [xschem raw_query sim_type]
  }
  set res [xschem schematic_in_new_window force]
  # if successfull descend into indicated sub-schematic
  if {$res} {
    xschem new_schematic switch [xschem get last_created_window]
    if { $rawfile ne {}} {
      if {$sim_type eq {op}} {
        xschem annotate_op $rawfile
      } else {
        xschem raw_read $rawfile $sim_type
      }
    }
    xschem select instance $inst fast
    xschem descend
    return 1
  }
  return 0
}


proc is_xschem_file {f} {
  # puts "is_xschem_file $f"
  regsub {\(.*} $f {} f ;# remove trailing generator args (gen.tcl(....))  if any
  if { ![file exists $f] } { return 0 
  } elseif { [file isdirectory $f] } { return 0 }
  set a [catch {open "$f" r} fd]
  set ret 0
  set score 0
  set instances 0
  set nline 0
  set generator 0
  if {$a} {
    puts stderr "Can not open file $f"
  } else {
    fconfigure $fd -translation binary
    while { [gets $fd line] >=0 } {
 
      #### Can not use this. schematics may containg 8 bit extended characters
      # if {[regexp {[^[:print:][:space:]]} $line]} { ;# line contains non ascii chars 
      #   close $fd
      #   return 0
      #  }

      # this is a script. not an xschem file
      if { $nline == 0 && [regexp {^#!} $line] } { 
        #### too dangerous executing an arbitrary script...
        # close $fd
        # set fd [open "|$f"]
        set generator 1
        # continue
      }
      if { [regexp {^[TKGVSE] \{} $line] } { incr score }
      if { [regexp {^[BL] +[0-9]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } {incr score}
      if { [regexp {^N +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } {incr score}
      if { [regexp {^C +\{[^{}]+\} +[-0-9.eE]+ +[-0-9.eE]+ +[0-3]+ +[0-3]+ +\{} $line] } {incr instances; incr score}
      if { [regexp "^v\[ \t\]+\{xschem\[ \t\]+version\[ \t\]*=.*\[ \t\]+file_version\[ \t\]*=" $line] } {
        set ret 1
      }
      incr nline
    } 
    if { $score > 4 }  { set ret 1} ;# Heuristic decision :-)
    if {$generator eq {1}} {
      set ret GENERATOR
    } elseif { $ret ne {0}} {
      if { $instances} {
        set ret SCHEMATIC
      } else { 
        set ret SYMBOL
      }
    }
    close $fd
  }
  # puts "ret=$ret score=$score"
  return $ret
}

# "xschem hash_string" in scheduler.c is faster
proc hash_string {s} {
  set hash 5381
  set len [string length $s]
  for {set i 0} { $i < $len} { incr i} {
    set c [string index $s $i]
    set ascii [scan $c %c]
    set hash [expr {($hash + ($hash << 5) + $ascii)%4294967296} ]
  }
  return $hash
}

## Recent component toolbar
namespace eval c_toolbar {
  # Create a variable inside the namespace
  variable c_t
  variable i
  set c_t(w) .load.l.recent
  set c_t(hash) [xschem hash_string $::XSCHEM_LIBRARY_PATH]
  set c_t(n) 25
  set c_t(top) 0
  for {set i 0} {$i < $c_t(n)} {incr i} {
    set c_t($i,text) {}
    set c_t($i,command) {}
    set c_t($i,file) {}
  }
  
  proc cleanup {} {
      variable c_t 
      if {![info exists c_t(n)]} return
      set j 0
      set n $c_t(n)
      set top $c_t(top)
      set i $top
      while {1} {
        set f [abs_sym_path $c_t($i,file)]
        if { $c_t($i,text) eq {} } {break}
        if { $j } {
          set k [expr {$i - $j}]
          if {$k < 0 } { set k [expr {$k + $n}]}
          set c_t($k,text) $c_t($i,text)
          set c_t($k,command) $c_t($i,command)
          set c_t($k,file) $c_t($i,file)
          set c_t($i,text) {}
          set c_t($i,command) {}
          set c_t($i,file) {}
        }
        if {$f ne {} && ![file exists $f]} {
          incr j
        } elseif {[array names files -exact $f] ne {}} {
          incr j
        }
        set files($f) 1
        set i [expr {($i + 1) % $n} ]
        if {$i == $top} break
      }
  }

  proc display {} {
    global has_x
    variable c_t
    
    if {![info exists has_x]} {return}
  
    if { [winfo exists $c_t(w)]} {
      set w $c_t(w)
      set n $c_t(n)
      cleanup
      destroy $w.title
      for {set i 0} {$i < $n} {incr i} {
        destroy $w.b$i
      }
      set i $c_t(top)
      button $w.title -text Recent -pady 0 -padx 0 -state disabled -disabledforeground black \
        -background grey60 -borderwidth 0 -font {TkDefaultFont 12 bold}
      pack $w.title -side top -fill x
      while {1} {
        button $w.b$i -text $c_t($i,text)  -pady 0 -padx 0 -command $c_t($i,command) -takefocus 0
        pack $w.b$i -side top -fill x
        set i [expr {($i + 1) % $n}]
        if { $i == $c_t(top) } break
      }
    }
  }

  proc add {f} {
    variable c_t
    for {set i 0} {$i < $c_t(n)} {incr i} {
      if {  $c_t($i,file) eq $f } { return 0}
    }
    set ret 0
    set ret 1
    set i [expr { ($c_t(top)-1) % $c_t(n) } ];# last element
    set c_t($i,file) $f
    set c_t($i,command) "
      set file_dialog_retval {}
      if { \[xschem get ui_state\] & 8192 } { xschem abort_operation }
      file_dialog_display_preview {$f}
      xschem place_symbol {$f} "
    set c_t($i,text)  [file tail [file rootname $f]]
    set c_t(top) $i
    if {$ret} {write_recent_file}
    return $ret
  }
}
## end c_toolbar namespace

proc file_dialog_set_colors1 {} {
  global file_dialog_files1 dircolor
  for {set i 0} { $i< [.load.l.paneleft.list index end] } { incr i} {
    set maxlen 0
    set name "[lindex $file_dialog_files1 $i]"
    .load.l.paneleft.list itemconfigure $i -foreground black -selectforeground black
    foreach j [array names dircolor] {
      set pattern $j
      set color $dircolor($j)
      set len [string length [regexp -inline $pattern $name]]
      if { $len > $maxlen } {
        .load.l.paneleft.list itemconfigure $i -foreground $color -selectforeground $color
        set maxlen $len
      }
    }
  }
}

proc file_dialog_set_colors2 {} {
  global file_dialog_index1 file_dialog_files2 dircolor file_dialog_files1
  set dir1 [abs_sym_path [lindex $file_dialog_files1 $file_dialog_index1]]
  for {set i 0} { $i< [.load.l.paneright.f.list index end] } { incr i} {
    set maxlen 0
    set name "$dir1/[lindex $file_dialog_files2 $i]"
    if {[ file isdirectory $name]} {
      .load.l.paneright.f.list itemconfigure $i -foreground blue
      foreach j [array names dircolor] {
        set pattern $j 
        set color $dircolor($j)
        set len [string length [regexp -inline $pattern $dir1]]
        # puts "len=$len\npattern=$pattern\nname=$name\n\n\n"
        if { $len > $maxlen } {
          .load.l.paneright.f.list itemconfigure $i -foreground $color -selectforeground $color
          set maxlen $len
        }
      }

    } else {
      .load.l.paneright.f.list itemconfigure $i -foreground black
    }
  }
}

proc file_dialog_set_names1 {} {
  global file_dialog_names1 file_dialog_files1 load_file_dialog_fullpath
  
  set file_dialog_names1 {}
  foreach i $file_dialog_files1 {
    if { $load_file_dialog_fullpath == 1} {
      set item $i
    } else {
      set item [get_cell $i 0]
    }
    lappend file_dialog_names1 $item
  }
}
  
proc file_dialog_set_home {dir} {
  global pathlist  file_dialog_files1 file_dialog_index1 file_dialog_names1

  set curr_dirname [xschem get current_dirname]
  .load.l.paneleft.list selection clear 0 end
  if { $dir eq {.}} { set dir $curr_dirname}
  # puts "set home: dir=$dir, pathlist=$pathlist"
  set pl {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]} {
      set path_elem $curr_dirname
    }
    lappend pl $path_elem
  }
  set i [lsearch -exact $pl $dir]
  if { $i>=0 } {
    set file_dialog_files1 $pathlist
    file_dialog_set_names1
    update
    file_dialog_set_colors1
    .load.l.paneleft.list xview moveto 1
    set file_dialog_index1 $i
    .load.l.paneleft.list selection set $file_dialog_index1
  } else {
    set file_dialog_files1 [list $dir]
    file_dialog_set_names1
    update
    file_dialog_set_colors1
    .load.l.paneleft.list xview moveto 1
    set file_dialog_index1 0
    .load.l.paneleft.list selection set 0
  }
}

proc setglob {dir} {
      global file_dialog_globfilter file_dialog_files2 OS
      # puts "setglob: $dir, filter=$file_dialog_globfilter"
      set file_dialog_files2 [lsort [glob -nocomplain -directory $dir -tails -type d .* *]]
      if { $file_dialog_globfilter eq {*}} {
        set file_dialog_files2 ${file_dialog_files2}\ [lsort [
           glob -nocomplain -directory $dir -tails -type {f} .* $file_dialog_globfilter]]
      } else {
        if {$OS == "Windows"} {
          regsub {:} $file_dialog_globfilter {\:} file_dialog_globfilter
        }

        if {![catch {glob -nocomplain -directory $dir -tails -type {f} $file_dialog_globfilter} res]} {
          set flist $res
        } else {
          set flist [glob -nocomplain -directory $dir -tails -type {f} {*}]
        }
        set file_dialog_files2 ${file_dialog_files2}\ [lsort $flist]
      }
}

proc load_file_dialog_mkdir {dir} {
  global file_dialog_dir1 has_x
  if { $dir ne {} } {
    if {[catch {file mkdir "${file_dialog_dir1}/$dir"} err]} {
      puts $err
      if {[info exists has_x]} {
        tk_messageBox -message "$err" -icon error -parent [xschem get topwindow] -type ok
      }
    }
    setglob ${file_dialog_dir1}
    file_dialog_set_colors2
  }
}

proc load_file_dialog_up {dir} {
  global file_dialog_dir1
  bind .load.l.paneright.draw <Expose> {}
  bind .load.l.paneright.draw <Configure> {}
  .load.l.paneright.draw configure -background white
  set d [file dirname $dir]
  if { [file isdirectory $d]} {
    file_dialog_set_home $d
    setglob $d
    file_dialog_set_colors2
    set file_dialog_dir1 $d
  }
}


proc file_dialog_getresult {loadfile confirm_overwrt} {
  global file_dialog_dir1 file_dialog_retval file_dialog_ext has_x
  if { $file_dialog_retval ne {}} {
    if { [regexp {^https?://} $file_dialog_retval] } {
      set fname $file_dialog_retval
    } elseif { [regexp {^/} $file_dialog_retval]} {
      set fname $file_dialog_retval
    } else {
      regsub {/*$} $file_dialog_dir1 {/} file_dialog_dir1
      set fname "${file_dialog_dir1}${file_dialog_retval}"
    }
    if {![file exists "$fname"] } {
      return "$fname"
    }
    if { $loadfile == 0 } {
      if {[file exists "$fname"]} {
        if {$confirm_overwrt == 1 } {
          set answer [alert_ "Overwrite $fname?" {} 0 1]
        } else {
          set answer 1
        }
        if {$answer eq {1}} {
          return "$fname"
        } else { 
          set file_dialog_retval {}
          return {}
        }
      }
    }
    set type [is_xschem_file "$fname"]
    if { $type eq {0}  || $type eq {GENERATOR} } {
      if { $type eq {0} } {
        set answer [alert_ "$fname does not seem to be an xschem file...\nContinue?" {} 0 1]
      } else { ;# $type == GENERATOR
        set answer 1
      }
      if { $answer eq {0}} {
        set file_dialog_retval {}
        return {}
      } else { ;# $answer == 1
        if { $type eq {GENERATOR} } {
          return "${fname}"
        }
        # $type == 0 but $answer==1 so return selected filename
        return "$fname"
      }
    # $type == SYMBOL or SCHEMATIC
    } elseif { $type ne {SYMBOL} && ($file_dialog_ext eq {*.sym}) } { ;# SCHEMATIC
      set answer [
        alert_ "$fname does not seem to be a SYMBOL file...\nContinue?" {} 0 1]
      if { $answer eq {0}} {
        set file_dialog_retval {}
        return {}
      } else {
        return "$fname"
      }
    } else { ;# SYMBOL
      return "$fname"
    }
  } else {
    return {}
  }
}

proc file_dialog_place_symbol {} {
  global file_dialog_retval

  set entry [.load.buttons_bot.entry get]
  # puts "entry=$entry"
  set file_dialog_retval  $entry
  set sym [file_dialog_getresult 2 0]
  # puts "sym=$sym"
  if { [xschem get ui_state] & 8192 } {
    xschem abort_operation
  }
  if {$sym ne {}} {
    xschem place_symbol "$sym"
  }
}

proc file_dialog_display_preview {f} {
  set type [is_xschem_file $f]
  if { $type ne {0} } {
    if { [winfo exists .load] } {
      .load.l.paneright.draw configure -background {}
      xschem preview_window draw .load.l.paneright.draw "$f"
      bind .load.l.paneright.draw <Expose> [subst {xschem preview_window draw .load.l.paneright.draw "$f"}]
      bind .load.l.paneright.draw <Configure> [subst {xschem preview_window draw .load.l.paneright.draw "$f"}]
    }
  } else {
    bind .load.l.paneright.draw <Expose> {}
    bind .load.l.paneright.draw <Configure> {}
    .load.l.paneright.draw configure -background white
  }
}

proc file_dialog_right_listboxselect {dirselect} {
    global file_dialog_yview file_dialog_dir1 file_dialog_dir2  file_dialog_retval file_dialog_sel
    global OS file_dialog_loadfile file_dialog_index1 file_dialog_files1 file_dialog_globfilter
    global file_dialog_others
    set file_dialog_yview [.load.l.paneright.f.list yview] 
    set file_dialog_sel [lindex [.load.l.paneright.f.list curselection] 0]
    
    if { $file_dialog_sel ne {} } {
      set curr_dir [abs_sym_path [lindex $file_dialog_files1 $file_dialog_index1]]
      set curr_item [.load.l.paneright.f.list get $file_dialog_sel]

      if {$curr_item eq {..}} {
        set file_dialog_d [file dirname $curr_dir]
      } elseif {$curr_item eq {.} } {
        set file_dialog_d  $curr_dir
      } else {
        if {$OS == "Windows"} {
          if {[regexp {^[A-Za-z]\:/$} $curr_dir]} {
            set file_dialog_d "$curr_dir$curr_item"
          } else {
            set file_dialog_d "$curr_dir/$curr_item"
          }
        } else {
          if {$curr_dir eq "/"} {
            set file_dialog_d "$curr_dir$curr_item"
          } else {
            set file_dialog_d "$curr_dir/$curr_item"
          }
        }
      }

      if { !$dirselect && [file isdirectory $file_dialog_d] } {
        .load.buttons_bot.entry delete 0 end
        set file_dialog_retval  {   }
        return
      }

      set file_dialog_dir1 $curr_dir
      set file_dialog_dir2 $curr_item

      set file_dialog_others {}
      if {$file_dialog_loadfile == 1} {
        foreach i [lrange [.load.l.paneright.f.list curselection] 1 end] {
          set file_dialog_retval [.load.l.paneright.f.list get $i]
          lappend file_dialog_others [file_dialog_getresult 1 0]
        }
      }
      set file_dialog_retval {} ;# we used this variable above to communicate with file_dialog_getresult


      if { [file isdirectory $file_dialog_d]} {
        bind .load.l.paneright.draw <Expose> {}
        bind .load.l.paneright.draw <Configure> {}
        .load.l.paneright.draw configure -background white
        file_dialog_set_home $file_dialog_d
        setglob $file_dialog_d
        file_dialog_set_colors2
        set file_dialog_dir1 $file_dialog_d
      } else {
        .load.buttons_bot.entry delete 0 end
        .load.buttons_bot.entry insert 0 $file_dialog_dir2

        file_dialog_display_preview  $file_dialog_d
        # puts "xschem preview_window draw .load.l.paneright.draw \"$file_dialog_dir1/$file_dialog_dir2\""
      }
    }
    if {$file_dialog_loadfile == 2} {
      # set to something different to any file to force a new placement in file_dialog_place_symbol
      set file_dialog_retval  {   }
    }
}

proc load_additional_files {} {
  global file_dialog_others

  if {$file_dialog_others ne {} } {
    foreach i $file_dialog_others {
      xschem load_new_window $i
    }
  }
}

# global_initdir: name of global variable containing the initial directory
# loadfile: set to 0 if calling for saving instead of loading a file
#           set to 2 for non blocking operation (symbol insertion)
# confirm_overwrt: ask before overwriting an existing file
# initialf: fill the file entry box with this name (used when saving)
#
proc load_file_dialog {{msg {}} {ext {}} {global_initdir {INITIALINSTDIR}} 
     {loadfile {1}} {confirm_overwrt {1}} {initialf {}}} {
  global file_dialog_index1 file_dialog_files2 file_dialog_files1 file_dialog_retval file_dialog_dir1 pathlist OS
  global file_dialog_default_geometry file_dialog_sp0 file_dialog_sp1 file_dialog_v_sp0 file_dialog_yview 
  global file_dialog_names1 tcl_version file_dialog_globfilter file_dialog_dir2
  global file_dialog_save_initialfile file_dialog_loadfile file_dialog_ext

  if { [winfo exists .load] } {
    .load.buttons_bot.cancel invoke
  }
  set file_dialog_loadfile $loadfile
  if {$ext ne {}} {set file_dialog_ext $ext}
  set file_dialog_globfilter $file_dialog_ext

  set file_dialog_save_initialfile $initialf
  set file_dialog_retval {} 
  upvar #0 $global_initdir initdir
  if { $loadfile != 2} {xschem set semaphore [expr {[xschem get semaphore] +1}]}
  toplevel .load -class Dialog
  wm title .load $msg
  # wm transient .load [xschem get topwindow]
  set_ne file_dialog_index1 0
  if { ![info exists file_dialog_files1]} {
    set file_dialog_files1 $pathlist
    file_dialog_set_names1
    update
    set file_dialog_index1 0
  }
  set_ne file_dialog_files2 {}
  panedwindow  .load.l -orient horizontal -height 8c
  if { $loadfile == 2} {frame .load.l.recent -takefocus 0}
  frame .load.l.paneleft -takefocus 0 -highlightcolor red -highlightthickness 2 -bg {grey90} \
    -highlightbackground [option get . background {}]
  eval [subst {listbox .load.l.paneleft.list -listvariable file_dialog_names1 -width 40 -height 12 \
    -fg black -background {grey90} -highlightthickness 0 -relief flat -borderwidth 0 \
    -yscrollcommand ".load.l.paneleft.yscroll set" -selectmode browse \
    -xscrollcommand ".load.l.paneleft.xscroll set" -exportselection 0}]
  if { ![catch {.load.l.paneleft.list cget -justify}]} {
    .load.l.paneleft.list configure -justify right
  }
  file_dialog_set_colors1
  scrollbar .load.l.paneleft.yscroll -command ".load.l.paneleft.list yview" -takefocus 0
  scrollbar .load.l.paneleft.xscroll -command ".load.l.paneleft.list xview" -orient horiz -takefocus 0
  bind .load.l.paneleft.list <<ListboxSelect>> { 
    set file_dialog_sel [.load.l.paneleft.list curselection]
    if { $file_dialog_sel ne {} } {
      set file_dialog_dir1 [abs_sym_path [lindex $file_dialog_files1 $file_dialog_sel]]
      set file_dialog_index1 $file_dialog_sel

      set file_dialog_globfilter  *[.load.buttons_bot.src get]*
      if { $file_dialog_globfilter eq {**} } { set file_dialog_globfilter * }

      setglob $file_dialog_dir1
      file_dialog_set_colors2
    }
  }

  panedwindow .load.l.paneright -orient vertical 
  frame .load.l.paneright.f -takefocus 0
  frame .load.l.paneright.draw -background white -height 3.8c -takefocus 0
  .load.l.paneright add .load.l.paneright.f
  .load.l.paneright add .load.l.paneright.draw -minsize 150

  if { ![catch {.load.l.paneright  panecget .load.l.paneright.f -stretch}]} {
    set optnever {-stretch never}
    set optalways {-stretch always}
  } else {
    set optnever {}
    set optalways {}
  }


  eval .load.l.paneright paneconfigure .load.l.paneright.f $optnever
  eval .load.l.paneright paneconfigure .load.l.paneright.draw $optalways

  if {$global_initdir eq {INITIALINSTDIR}} {
    set selmode browse
  } else {
    set selmode extended
  }

  listbox .load.l.paneright.f.list  -background {grey90} -listvariable file_dialog_files2 -width 20 -height 12\
    -fg black -highlightcolor red -highlightthickness 2 \
    -highlightbackground [option get . background {}] \
    -yscrollcommand ".load.l.paneright.f.yscroll set" -selectmode $selmode \
    -xscrollcommand ".load.l.paneright.f.xscroll set" -exportselection 0
  scrollbar .load.l.paneright.f.yscroll -command ".load.l.paneright.f.list yview" -takefocus 0
  scrollbar .load.l.paneright.f.xscroll -command ".load.l.paneright.f.list xview" -orient horiz -takefocus 0

  if { $loadfile == 2} {
    .load.l  add .load.l.recent
    c_toolbar::display
  }
  .load.l  add .load.l.paneleft -minsize 40
  .load.l  add .load.l.paneright -minsize 150
  eval .load.l paneconfigure .load.l.paneleft $optnever
  eval .load.l paneconfigure .load.l.paneright $optalways
  frame .load.buttons -takefocus 0
  frame .load.buttons_bot -takefocus 0
  button .load.buttons_bot.ok -width 5 -text OK -takefocus 0 -command "
    set file_dialog_retval \[.load.buttons_bot.entry get\]
    xschem preview_window destroy .load.l.paneright.draw {}
    destroy .load
    set $global_initdir \"\$file_dialog_dir1\"
  "
  button .load.buttons_bot.cancel -width 5 -text Cancel -takefocus 0 -command "
    set file_dialog_retval {}
    if {\$file_dialog_loadfile == 2} {xschem abort_operation}
    xschem preview_window destroy .load.l.paneright.draw {}
    destroy .load
    set $global_initdir \"\$file_dialog_dir1\"
  "
  wm protocol .load WM_DELETE_WINDOW {.load.buttons_bot.cancel invoke}
  button .load.buttons.home -width 5 -text {Home} -takefocus 0 -command {
    bind .load.l.paneright.draw <Expose> {}
    bind .load.l.paneright.draw <Configure> {}
    .load.l.paneright.draw configure -background white
    set file_dialog_files1 $pathlist
    file_dialog_set_names1
    update
    file_dialog_set_colors1
    .load.l.paneleft.list xview moveto 1
    set file_dialog_index1 0
    set file_dialog_dir1 [abs_sym_path [lindex $file_dialog_files1 $file_dialog_index1]]
    setglob $file_dialog_dir1
    file_dialog_set_colors2
    .load.l.paneleft.list selection clear 0 end
    .load.l.paneright.f.list selection clear 0 end
    .load.l.paneleft.list selection set $file_dialog_index1
  }
  label .load.buttons_bot.label  -text { File:}
  entry .load.buttons_bot.entry -highlightcolor red -highlightthickness 2 \
    -highlightbackground [option get . background {}] -takefocus 0
  entry_replace_selection .load.buttons_bot.entry
  label .load.buttons_bot.srclab  -text { Search:}
  entry .load.buttons_bot.src -width 18 -highlightcolor red -highlightthickness 2 \
    -highlightbackground [option get . background {}]
  entry_replace_selection .load.buttons_bot.src
  .load.buttons_bot.src delete 0 end
  .load.buttons_bot.src insert 0 $file_dialog_globfilter
  if { $file_dialog_save_initialfile ne {} } { 
    .load.buttons_bot.entry insert 0 $file_dialog_save_initialfile
  }
  bind .load.buttons_bot.src <KeyRelease> {
    if {$file_dialog_save_initialfile eq {} } {
      set file_dialog_globfilter  *[.load.buttons_bot.src get]*
      if { $file_dialog_globfilter eq {**} } { set file_dialog_globfilter * }
      setglob $file_dialog_dir1
      if {[.load.buttons_bot.entry get] ne {}} {
        .load.l.paneright.f.list yview moveto 1.0
      } else {
        if { [info exists file_dialog_yview]} {
         .load.l.paneright.f.list yview moveto  [lindex $file_dialog_yview 0]
        }
      }
    }
    # set to something different to any file to force a new placement in file_dialog_place_symbol
    set file_dialog_retval {   }
  }
  bind .load.buttons_bot.entry <ButtonPress> {
    # set to something different to any file to force a new placement in file_dialog_place_symbol
    set file_dialog_retval {   }
  }

  button .load.buttons.up -width 5 -text Up -command {load_file_dialog_up  $file_dialog_dir1} -takefocus 0
  label .load.buttons.mkdirlab -text { New dir: } 
  entry .load.buttons.newdir -width 16 -takefocus 0
  entry_replace_selection .load.buttons.newdir
  button .load.buttons.mkdir -width 5 -text Create -takefocus 0 -command { 
    load_file_dialog_mkdir [.load.buttons.newdir get]
  }
  button .load.buttons.rmdir -width 5 -text Delete -takefocus 0 -command { 
    if { [.load.buttons.newdir get] ne {} } {
      file delete "${file_dialog_dir1}/[.load.buttons.newdir get]"
      setglob ${file_dialog_dir1}
      file_dialog_set_colors2
    }
  }
  button .load.buttons.pwd -text {Current dir} -takefocus 0 -command {load_file_dialog_up  [xschem get schname]}
  checkbutton .load.buttons.path -text {Library paths} -variable load_file_dialog_fullpath -takefocus 0 \
    -command {
      file_dialog_set_names1
      update
      file_dialog_set_colors1
      .load.l.paneleft.list xview moveto 1
    }

  pack .load.l -expand true -fill both
  pack  .load.l.paneleft.yscroll -side right -fill y
  pack  .load.l.paneleft.xscroll -side bottom -fill x
  pack  .load.l.paneleft.list -fill both -expand true -padx 12

  pack .load.buttons.home .load.buttons.up .load.buttons.pwd .load.buttons.path -side left
  pack .load.buttons.mkdirlab -side left
  pack .load.buttons.newdir -expand true -fill x -side left
  pack .load.buttons.rmdir .load.buttons.mkdir -side right
  pack .load.buttons_bot.srclab -side left
  pack .load.buttons_bot.src -side left 
  pack .load.buttons_bot.label -side left
  pack .load.buttons_bot.entry -side left -fill x -expand true

  pack  .load.l.paneright.f.yscroll -side right -fill y
  pack  .load.l.paneright.f.xscroll -side bottom -fill x
  pack  .load.l.paneright.f.list -side bottom  -fill both -expand true

  pack .load.buttons_bot.cancel .load.buttons_bot.ok -side left
  pack .load.buttons_bot -side bottom -fill x
  pack .load.buttons -side bottom -fill x
  if { [info exists file_dialog_default_geometry]} {
     wm geometry .load "${file_dialog_default_geometry}"
  }
  file_dialog_set_home $initdir
  if { $loadfile != 2} {
    bind .load <Return> " 
      set file_dialog_retval \[.load.buttons_bot.entry get\]
      if {\$file_dialog_retval ne {} && !\[file isdirectory \$file_dialog_retval\]} {
        bind .load.l.paneright.draw <Expose> {}
        bind .load.l.paneright.draw <Configure> {}
        xschem preview_window destroy .load.l.paneright.draw {}
        destroy .load
        set $global_initdir \"\$file_dialog_dir1\"
      }
    "
    bind .load.l.paneright.f.list <Double-Button-1> "
      set file_dialog_retval \[.load.buttons_bot.entry get\]
      if {\$file_dialog_retval ne {} && !\[file isdirectory \$file_dialog_retval\]} {
        bind .load.l.paneright.draw <Expose> {}
        bind .load.l.paneright.draw <Configure> {}
        xschem preview_window destroy .load.l.paneright.draw {}
        destroy .load
        set $global_initdir \"\$file_dialog_dir1\"
      }
    "
  }
  bind .load <Escape> "
    set file_dialog_retval {}
    if {\$file_dialog_loadfile == 2} {xschem abort_operation}
    xschem preview_window destroy .load.l.paneright.draw {}
    destroy .load
    set $global_initdir \"\$file_dialog_dir1\"
  "
  bind .load <KeyPress-H> {.load.buttons.home invoke }
  bind .load <KeyPress-U> {.load.buttons.up invoke }
  ### update

  if { [info exists file_dialog_v_sp0] } {
    eval .load.l.paneright sash mark 0 [.load.l.paneright sash coord 0]
    eval .load.l.paneright sash dragto 0 [subst {1 $file_dialog_v_sp0}]
  }

  if { [info exists file_dialog_sp0] } {
    if { $file_dialog_loadfile == 2} {
      eval .load.l sash mark 1 [.load.l sash coord 1]
      eval .load.l sash dragto 1 [subst {$file_dialog_sp0 1}]
    } else {
      eval .load.l sash mark 0 [.load.l sash coord 0]
      eval .load.l sash dragto 0 [subst {$file_dialog_sp0 1}]
    }
  }
  if { $file_dialog_loadfile == 2 && [info exists file_dialog_sp1] } {
    eval .load.l sash mark 0 [.load.l sash coord 0]
    eval .load.l sash dragto 0 [subst {$file_dialog_sp1 1}]
  }
  ### update
  .load.l.paneleft.list xview moveto 1

  bind .load <Configure> {
    set file_dialog_v_sp0 [lindex [.load.l.paneright sash coord 0] 1]
    if { $file_dialog_loadfile == 2} {
      set file_dialog_sp0 [lindex [.load.l sash coord 1] 0]
    } else {
      set file_dialog_sp0 [lindex [.load.l sash coord 0] 0]
    }
    if {$file_dialog_loadfile == 2} {
      set file_dialog_sp1 [lindex [.load.l sash coord 0] 0]
    }
    set file_dialog_default_geometry [wm geometry .load]
    .load.l.paneleft.list xview moveto 1
    # regsub {\+.*} $file_dialog_default_geometry {} file_dialog_default_geometry
  }

  bind .load.l.paneright.f.yscroll <Motion> {
    set file_dialog_yview [.load.l.paneright.f.list yview]
  }

  xschem preview_window create .load.l.paneright.draw {}
  set file_dialog_dir1 [abs_sym_path [lindex $file_dialog_files1 $file_dialog_index1]]
  setglob $file_dialog_dir1
  file_dialog_set_colors2


  if {$file_dialog_loadfile == 2} { 
    bind .load <Leave> {
      if { {%W} eq {.load} && $file_dialog_retval ne {} && [.load.buttons_bot.entry get] ne $file_dialog_retval} {
        file_dialog_place_symbol
      }
    }
  }

  bind .load.l.paneright.f.list <KeyRelease-Return> { 
    file_dialog_right_listboxselect 1
  }

  bind .load.l.paneright.f.list <ButtonRelease-1> { 
    file_dialog_right_listboxselect 1
  }

  bind .load.l.paneright.f.list <KeyRelease-Up> { 
    file_dialog_right_listboxselect 0
  }

  bind .load.l.paneright.f.list <KeyRelease-Down> { 
    file_dialog_right_listboxselect 0
  }

  # bind .load.l.paneright.f.list <<ListboxSelect>> {
  #   file_dialog_right_listboxselect
  # }

  if { [info exists file_dialog_yview]} {
   .load.l.paneright.f.list yview moveto  [lindex $file_dialog_yview 0]
  }
  focus .load.buttons_bot.src
  .load.buttons_bot.src selection range 0 end
  if {$loadfile != 2} {
    tkwait window .load
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  return [file_dialog_getresult $loadfile $confirm_overwrt]
}

# recursive procedure. Returns list of directories containing
# files by extension 'ext' from a list of supplied 'paths'
# if paths empty use XSCHEM_LIBRARY_PATH list.
# 'levels' is set to the number of levels to descend into.
# 'level' is used internally by the function and should not be set.
proc get_list_of_dirs_with_files {{paths {}} {levels -1} {ext {\.(sch|sym)$}}   {level -1}} {
  # puts "get_list_of_dirs_with_files paths=$paths"
  global pathlist
  set dir_with_symbols {}
  if {$level == -1} { set level 0}
  if {$paths eq {}} {set paths $pathlist}
  foreach i $paths {
    set filelist [glob -nocomplain -directory $i -type f *]
    set there_are_symbols 0
    foreach f $filelist {
      if {[regexp $ext $f]} {
      # if {[is_xschem_file $f] ne {0}} {  }
        set there_are_symbols 1
        break
      }
    }
    if {$there_are_symbols} {
      lappend dir_with_symbols $i
    }
    set dirlist [glob -nocomplain -directory $i -type d *]
    if {$levels >=0 && $level + 1 > $levels} {return}
    foreach d $dirlist {
      set dirs [get_list_of_dirs_with_files $d $levels $ext [expr {$level + 1} ]]
      if { $dirs ne {}} {set dir_with_symbols [concat $dir_with_symbols $dirs]}
    }
  }
  return $dir_with_symbols
}

#######################################################################
##### new alternate insert_symbol browser
#######################################################################

#### Display preview of selected symbol and start sym placement
proc insert_symbol_draw_preview {f} {
  global insert_symbol
  # puts "insert_symbol_draw_preview"
  if {[winfo exists .ins]} {
    .ins.center.right configure -bg {}
    xschem preview_window create .ins.center.right {}
    xschem preview_window draw .ins.center.right [list $f]
    bind .ins.center.right <Expose> "xschem preview_window draw .ins.center.right [list $f]"
    bind .ins.center.right <Configure> "xschem preview_window draw .ins.center.right [list $f]"
    if {$insert_symbol(action) eq {symbol}} {
      insert_symbol_place symbol
    }
  }
}

proc insert_symbol_select_preview {} {
  # puts "insert_symbol_select_preview"
  global insert_symbol
  if {[info exists insert_symbol(f)]} {
    after cancel ".ins.center.right configure -bg white"
    after cancel "insert_symbol_draw_preview $insert_symbol(f)"
    unset insert_symbol(f)
  }
  xschem preview_window close .ins.center.right {}
  bind .ins.center.right <Expose> {}
  bind .ins.center.right <Configure> {}
  set sel [.ins.center.left.l curselection]
  if {$sel eq {}} {
    set sel [.ins.center.left.l index active]
    .ins.center.left.l selection set active
  }
  if {$sel ne {} && [info exists insert_symbol(fullpathlist)]} {
    set insert_symbol(fileindex) $sel
    # puts "set fileindex=$sel"
    .ins.center.left.l see $sel
    set f [lindex $insert_symbol(fullpathlist) $sel]
    if {$f ne {}} {
      set insert_symbol(f) $f
      set type [is_xschem_file $f]
      if {$type ne {0}} {
        set dir [rel_sym_path $f]
        .ins.top.dir_e configure -state normal
        .ins.top.dir_e delete 0 end
        .ins.top.dir_e  insert 0 $dir
        .ins.top.dir_e configure -state readonly

        .ins.top2.dir_e configure -state normal
        .ins.top2.dir_e delete 0 end
        .ins.top2.dir_e insert 0 $f
        .ins.top2.dir_e configure -state readonly
        # global used to cancel delayed script
        after 200 "insert_symbol_draw_preview $f"
      } else {
        after 200 {.ins.center.right configure -bg white}
      }
    }
  }
}

proc insert_symbol_update_dirs {} {
  # puts insert_symbol_update_dirs
  global insert_symbol
  # regenerate list of dirs
  set insert_symbol(dirs) [
    get_list_of_dirs_with_files $insert_symbol(paths) $insert_symbol(maxdepth) $insert_symbol(ext)
  ]
  set insert_symbol(dirtails) {}
  foreach i $insert_symbol(dirs) {
    lappend insert_symbol(dirtails) [file tail $i]
  }
  # sort dirs using dirtails as key
  set files {}
  foreach f $insert_symbol(dirtails) ff $insert_symbol(dirs) {
    lappend files [list $f $ff]
  }
  set files [lsort -dictionary -index 0 $files]
  set insert_symbol(dirtails) {}
  set insert_symbol(dirs) {}
  
  foreach f $files {
    lassign $f ff fff
    lappend insert_symbol(dirtails) $ff
    lappend insert_symbol(dirs) $fff
  } 
}

#### fill list of files matching pattern
proc insert_symbol_filelist {} {
  global insert_symbol

  set sel [.ins.center.leftdir.l curselection]
  if {![info exists insert_symbol(dirs)]} {return}
  if {$sel eq {}} {
    set sel [.ins.center.leftdir.l index active]
    .ins.center.leftdir.l selection set active
  }
  set insert_symbol(dirindex) $sel
  set path [lindex $insert_symbol(dirs) $sel]
  .ins.top2.dir_e configure -state normal
  .ins.top2.dir_e delete 0 end
  .ins.top2.dir_e insert 0 $path
  .ins.top2.dir_e configure -state readonly
  # check if regex is valid
  set err [catch {regexp $insert_symbol(regex) {12345}} res]
  if {$err} {return}
  set f {}
  if {$path ne {} } {
    set f [match_file $insert_symbol(regex) $path 0]
  }
  set filelist {}
  set insert_symbol(fullpathlist) {}
  set sel [.ins.center.left.l curselection]
  if {$sel ne {}} {
    set insert_symbol(fileindex) $sel
    # puts "set fileindex=$sel"
  }
  if {$sel eq {}} { set sel 0}
  .ins.center.left.l activate $sel
  foreach i $f {
    set err [catch {regexp $insert_symbol(ext) $i} type]
    if {!$err && $type} {
      set fname [file tail $i]
      lappend filelist $fname
      lappend insert_symbol(fullpathlist) $i
    }
  }
  # sort lists using filelist as key
  set files {}
  foreach f $filelist ff $insert_symbol(fullpathlist) {
    lappend files [list $f $ff]
  }
  set files [lsort -dictionary -index 0 $files]
  set filelist {}
  set insert_symbol(fullpathlist) {}
  foreach f $files {
    lassign $f ff fff
    lappend filelist $ff
    lappend insert_symbol(fullpathlist) $fff
  }
  set insert_symbol(nitems) [llength $filelist]
  # assign listbox variable all at the end, it is faster...
  set insert_symbol(list) $filelist
}

proc insert_symbol_place {action} {
  # puts insert_symbol_place
  global insert_symbol
  set sel [.ins.center.left.l curselection]
  if {$sel eq {}} {
    set sel [.ins.center.left.l index active]
    .ins.center.left.l selection set active
  }
  if {$sel ne {}} {
    set f [lindex $insert_symbol(fullpathlist) $sel 0]
    if {$f ne {}} {
      set type [is_xschem_file $f]
      if {$type ne {0}} {
        if { [xschem get ui_state] & 8192 } {
          xschem abort_operation
        }
        if {$action eq {symbol}} {
          xschem place_symbol $f
        } elseif {$action eq {load}} {
          xschem load -gui $f
        }
      }
    }
  }
}

#### maxdepth: how many levels to descend for each $paths directory (-1: no limit)
proc insert_symbol {{paths {}} {maxdepth -1} {ext {.*}} {action {symbol}}} {
  global insert_symbol
  set insert_symbol(action) $action
  
  set insert_symbol(maxdepth) $maxdepth
  set insert_symbol(paths) [cleanup_paths $paths] ;# remove ~ and other strange path combinations
  # xschem set semaphore [expr {[xschem get semaphore] +1}]
  set insert_symbol(ext) $ext
  if {[winfo exists .ins]} {
    raise .ins
    return
  }
  if {![info exists insert_symbol(regex)]} { set insert_symbol(regex) {}}
  toplevel .ins
  frame .ins.top -takefocus 0
  frame .ins.top2 -takefocus 0
  panedwindow  .ins.center -orient horizontal
  frame .ins.center.leftdir  -takefocus 0  -height 40
  frame .ins.center.left  -takefocus 0  -height 40
  frame .ins.center.right -width 50 -height 40 -bg white -takefocus 0
  .ins.center add .ins.center.leftdir
  .ins.center add .ins.center.left
  .ins.center add .ins.center.right
  frame .ins.bottom  -takefocus 0
  pack .ins.top -side top -fill x
  pack .ins.top2 -side top -fill x
  pack .ins.center -side top -expand 1 -fill both
  pack .ins.bottom -side top -fill x

  listbox .ins.center.leftdir.l -listvariable insert_symbol(dirtails) -width 20 -height 4 \
    -yscrollcommand ".ins.center.leftdir.s set" -highlightcolor red -highlightthickness 2 \
    -activestyle underline -highlightbackground [option get . background {}] \
    -exportselection 0

  listbox .ins.center.left.l -listvariable insert_symbol(list) -width 20 -height 4 \
    -yscrollcommand ".ins.center.left.s set" -highlightcolor red -highlightthickness 2 \
    -activestyle underline -highlightbackground [option get . background {}] \
    -exportselection 0

  scrollbar .ins.center.leftdir.s -command ".ins.center.leftdir.l yview" -takefocus 0
  scrollbar .ins.center.left.s -command ".ins.center.left.l yview" -takefocus 0

  pack .ins.center.left.l -expand 1 -fill both -side left
  pack .ins.center.left.s -fill y -side left

  pack .ins.center.leftdir.l -expand 1 -fill both -side left
  pack .ins.center.leftdir.s -fill y -side left

  label .ins.top2.dir_l -text {Full path:}
  entry .ins.top2.dir_e -width 20 -state readonly \
    -readonlybackground [option get . background {}] -takefocus 0
  label .ins.top.pat_l -text Pattern:
  entry .ins.top.pat_e -width 15 -highlightcolor red -highlightthickness 2 \
     -textvariable insert_symbol(regex) -highlightbackground [option get . background {}]
  label .ins.top.dir_l -text { Symbol ref: }
  entry .ins.top.dir_e -width 20 -state readonly \
    -readonlybackground [option get . background {}] -takefocus 0
  label .ins.top.ext_l -text Ext:
  entry .ins.top.ext_e -width 15 -takefocus 0  -state normal -textvariable insert_symbol(ext)
  button .ins.top.upd -takefocus 0 -text Update -command {
    insert_symbol_update_dirs
    insert_symbol_filelist
  }
  bind .ins <KeyPress-Escape> {.ins.bottom.dismiss invoke}
  bind .ins <KeyRelease> "
    if {{%K} eq {Tab} && {%W} eq {.ins.center.left.l}} {
      insert_symbol_filelist
      insert_symbol_select_preview
    } elseif {{%K} eq {Tab} && {%W} eq {.ins.center.leftdir.l}} {
      insert_symbol_filelist
    }
  "
  bind .ins.center.leftdir.l <<ListboxSelect>> "insert_symbol_filelist"
  bind .ins.center.left.l <<ListboxSelect>> "insert_symbol_select_preview"
  bind .ins.center.left.l <KeyPress-Return> {
    if {$insert_symbol(action) eq {load}} {
      .ins.bottom.load invoke
    }
    xschem preview_window close .ins.center.right {}
    destroy .ins
  }
  bind .ins.center.left.l <Enter> "
    if { \[xschem get ui_state\] & 8192 } {
      xschem abort_operation
    }
  "
  label .ins.bottom.n -text { N. of items:}
  label .ins.bottom.nitems -textvariable insert_symbol(nitems)
  button .ins.bottom.dismiss -takefocus 0 -text Dismiss -command {
    if { [xschem get ui_state] & 8192 } {
      xschem abort_operation
    } else {
      xschem preview_window close .ins.center.right {}
      destroy .ins
    }
  }
  button .ins.bottom.load -text {Load file} -command {
    insert_symbol_place load
  }
 
  checkbutton .ins.bottom.sym -text {Place symbol} -onvalue symbol -offvalue load -variable insert_symbol(action)

  pack .ins.bottom.dismiss -side left
  pack .ins.bottom.load -side left
  pack .ins.bottom.sym -side left
  pack .ins.bottom.n -side left
  pack .ins.bottom.nitems -side left
  pack .ins.top2.dir_l -side left
  pack .ins.top2.dir_e -side left -fill x -expand 1
  pack .ins.top.pat_l -side left
  pack .ins.top.pat_e -side left
  pack .ins.top.dir_l -side left
  pack .ins.top.dir_e -side left -fill x -expand 1
  pack .ins.top.upd -side left
  pack .ins.top.ext_l -side left
  pack .ins.top.ext_e -side left
  if { [info exists insert_symbol(geometry)]} {
    wm geometry .ins "${insert_symbol(geometry)}"
  } else {
    wm geometry .ins 800x300
  }
  update
  if { [info exists insert_symbol(sp0)]} {
    .ins.center sash place 0 $insert_symbol(sp0) 1
  }
  if { [info exists insert_symbol(sp1)]} {
    .ins.center sash place 1 $insert_symbol(sp1) 1
  }
  bind .ins <Configure> {
    set insert_symbol(geometry) [wm geometry .ins]
    set insert_symbol(sp0) [lindex [.ins.center sash coord 0] 0]
    set insert_symbol(sp1) [lindex [.ins.center sash coord 1] 0]
  }
  insert_symbol_update_dirs
  if {[info exists insert_symbol(dirindex)]} {.ins.center.leftdir.l selection set $insert_symbol(dirindex)}
  if {[info exists insert_symbol(fileindex)]} {
    .ins.center.left.l selection set $insert_symbol(fileindex)
    .ins.center.left.l see $insert_symbol(fileindex)
  }
  insert_symbol_filelist
  return {}
}
#######################################################################
##### /new alternate insert_symbol browser
#######################################################################

# get last n path components: example , n=1 --> /aaa/bbb/ccc/ddd.sch -> ccc/ddd.sch
proc get_cell {s n } {
  set slist [file split $s]
  set l [llength $slist]
  if { $n >= $l } {set n [expr {$l - 1}]}
  set p {}
  for {set i [expr {$l-1-$n}]} {$i < $l} { incr i } {
    append p [lindex $slist $i]
    if {$i < $l - 1} {
      append p {/}
    }
  }
  return $p
}

# chop last n path components from s
proc path_head {s n } {
  set slist [file split $s]
  set l [llength $slist]
  if { $n < 0 } { set n 0 }
  set p {}
  for {set i 0} {$i < [expr {$l - $n}]} { incr i } {
    append p [lindex $slist $i]
    if {$i < $l -$n- 1 && ([lindex $slist $i] ne {/})} {
      append p {/}
    }
  }
  return $p
}


proc delete_files { dir } { 
 if  { [info tclversion]  >=8.4} {
   set x [tk_getOpenFile -title "DELETE FILES" -multiple 1 -initialdir [file dirname $dir] ]
 } else {
   set x [tk_getOpenFile -title "DELETE FILES" -initialdir [file dirname $dir] ]
 }
 foreach i  $x {
   file delete $i
 }
}

proc create_user_xschemrc {} {
  global USER_CONF_DIR XSCHEM_SHAREDIR

  if {![file exists $USER_CONF_DIR/xschemrc]} {
    file copy $XSCHEM_SHAREDIR/xschemrc $USER_CONF_DIR/xschemrc
    puts stderr "copied system $XSCHEM_SHAREDIR/xschemrc to $USER_CONF_DIR/xschemrc"
    puts stderr "Please review the file and make your changes, then restart xschem"
  } else {
    puts stderr "$USER_CONF_DIR/xschemrc already exists, will not overwrite."
  }
}

proc create_pins {} {
  global env retval USER_CONF_DIR
  global filetmp

  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval 
  regsub -all {>} $retval {]} retval 
  set lines [split $retval \n]
  set dirprefix [file dirname [rel_sym_path [find_file_first ipin.sym]]]
  if {$dirprefix == {.}} { set dirprefix {}} else {append dirprefix {/}}
  
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch w]
  foreach i $lines { 
    puts $fd "C \{${dirprefix}[lindex $i 1].sym\} 0 [set y [expr {$y-20}]] \
                0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

proc rectorder {x1 y1 x2 y2} {
  if {$x2 < $x1} {set tmp $x1; set x1 $x2; set x2 $tmp}
  if {$y2 < $y1} {set tmp $y1; set y1 $y2; set y2 $tmp}
  return [list $x1 $y1 $x2 $y2]
}

proc order {x1 y1 x2 y2} {
  if {$x2 < $x1} {set tmp $x1; set x1 $x2; set x2 $tmp; set tmp $y1; set y1 $y2; set y2 $tmp
  } elseif {$x2==$x1 && $y2<$y1} {set tmp $y1; set y1 $y2; set y2 $tmp}
  return [list $x1 $y1 $x2 $y2]
}

proc rotation {x0 y0 x y rot flip} {
  set tmp [expr {$flip? 2*$x0-$x : $x}]
  if {$rot==0} {set rx $tmp; set ry $y }
  if {$rot==1} {set rx [expr {$x0 - $y +$y0}]; set ry [expr {$y0+$tmp-$x0}]}
  if {$rot==2} {set rx [expr {2*$x0-$tmp}]; set ry [expr {2*$y0-$y}]}
  if {$rot==3} {set rx [expr {$x0+$y-$y0}]; set ry [expr {$y0-$tmp+$x0}]}
  return [list $rx $ry]
}

proc schpins_to_sympins {} {
  global env USER_CONF_DIR
  set pinhsize 2.5
  set first 1
  xschem copy
  set clipboard [read_data_nonewline $USER_CONF_DIR/.clipboard.sch]
  set lines [split $clipboard \n]
  set fd [open $USER_CONF_DIR/.clipboard.sch w]
  foreach i $lines {
    set ii [split [regexp -all -inline {\S+} $i]]
    if {[regexp {^C \{.*(i|o|io)pin} $i ]} {
      if {[regexp {ipin} [lindex $ii 1]]} { set dir in }
      if {[regexp {opin} [lindex $ii 1]]} { set dir out }
      if {[regexp {iopin} [lindex $ii 1]]} { set dir inout }
      set rot [lindex $ii 4]
      set flip [lindex $ii 5]
      while {1} {
        if { [regexp {lab=} $i] } {
          regsub {^.*lab=} $i {} lab
          regsub {[\} ].*} $lab {} lab
        } 
        if { [regexp {\}} $i]} { break} 
      }
      set x0 [lindex $ii 2]
      set y0 [lindex $ii 3]
      if {$first} {
        puts $fd "G {$x0 $y0 }"
        set first 0
      }
      set pinx1 [expr {$x0-$pinhsize}]
      set pinx2 [expr {$x0+$pinhsize}]
      set piny1 [expr {$y0-$pinhsize}]
      set piny2 [expr {$y0+$pinhsize}]
      if {![string compare $dir  "out"] || ![string compare $dir "inout"] } {
        set linex1 [expr {$x0-20}]
        set liney1 $y0
        set linex2 $x0
        set liney2 $y0
        set textx0 [expr {$x0-25}] 
        set texty0 [expr {$y0-4}]
        set textflip [expr {!$flip}]
      } else {
        set linex1 [expr {$x0+20}]
        set liney1 $y0
        set linex2 $x0
        set liney2 $y0
        set textx0 [expr {$x0+25}]
        set texty0 [expr {$y0-4}]
        set textflip [expr {$flip}]
      }
      ## lassign not available pre-tck8.5
      lassign [rotation $x0 $y0 $linex1 $liney1 $rot $flip] linex1 liney1
      lassign [rotation $x0 $y0 $linex2 $liney2 $rot $flip] linex2 liney2
      lassign [order $linex1 $liney1 $linex2 $liney2] linex1 liney1 linex2 liney2
      lassign [rotation $x0 $y0 $textx0 $texty0 $rot $flip] textx0 texty0
      foreach {textx0 texty0} [rotation $x0 $y0 $textx0 $texty0 $rot $flip] break
      puts $fd "B 5 $pinx1 $piny1 $pinx2 $piny2 \{name=$lab dir=$dir\}"
      puts $fd "L 4 $linex1 $liney1 $linex2 $liney2 \{\}"
      puts $fd "T \{$lab\} $textx0 $texty0 $rot $textflip 0.2 0.2 \{\}"
    }
  }
  close $fd
  xschem paste
}


proc add_lab_no_prefix {} { 
  global env retval USER_CONF_DIR
  global filetmp

  set dirprefix [file dirname [rel_sym_path [find_file_first ipin.sym]]]
  if {$dirprefix == {.}} { set dirprefix {}} else {append dirprefix {/}}
  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch w]
  foreach i $lines {
    puts $fd "C \{${dirprefix}lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

proc add_lab_prefix {} {
  global env retval USER_CONF_DIR
  global filetmp

  set dirprefix [file dirname [rel_sym_path [find_file_first ipin.sym]]]
  if {$dirprefix == {.}} { set dirprefix {}} else {append dirprefix {/}}
  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch w]
  foreach i $lines {
    puts $fd "C \{${dirprefix}lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

# Create an xschem symbol from a list of in, out, inout pins.
# Example:
#   create_symbol test.sym {CLK RST D} {Q QB} {VCC VSS}
#
proc create_symbol {name {in {}} {out {}} {inout {}}} {
  set symname [file rootname $name].sym
  set res [catch {open $symname {WRONLY CREAT EXCL}} fd]
  if {$res} {puts $fd; return 0} ;# Error. Print reason and exit.
  puts $fd {v {xschem version=3.4.8RC file_version=1.2}}
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

proc make_symbol {name {ask {no}} } {
  global XSCHEM_SHAREDIR symbol_width
  set name [abs_sym_path $name ]
  set symname [abs_sym_path $name .sym]
  if { $ask eq {no} && [file exists $symname] } {
    set answer [tk_messageBox -message "Warning: symbol $symname already exists. Overwrite?" \
        -icon warning -parent [xschem get topwindow] -type okcancel]
    if {$answer ne {ok}} { return {}}

  }
  # puts "make_symbol{}, executing: ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width ${name}"
  eval exec {awk -f ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width $name}
  return {}
}

proc make_symbol_lcc {name} {
  global XSCHEM_SHAREDIR
  set name [abs_sym_path $name]
  # puts "make_symbol{}, executing: ${XSCHEM_SHAREDIR}/make_sym_lcc.awk ${name}"
  eval exec {awk -f ${XSCHEM_SHAREDIR}/make_sym_lcc.awk $name}
  return {}
}

# point netlist_dir to  simulation dir 'simulation/' under current schematic directory
proc simuldir {} {
  global netlist_dir local_netlist_dir has_x
  if { $local_netlist_dir == 1 } {
    set simdir [xschem get current_dirname]/simulation
    set netlist_dir $simdir
    regsub {/$} $netlist_dir {} netlist_dir
    return $netlist_dir
  }
  if { $local_netlist_dir == 2 } {
    set simdir [xschem get current_dirname]/simulation/[get_cell [xschem get current_name] 0]
    set netlist_dir $simdir
    regsub {/$} $netlist_dir {} netlist_dir
    return $netlist_dir
  }
  return {}
}

#
# what==0: force creation of $netlist_dir (if netlist_dir variable not empty)
#           and return current setting.
#
# what==1: if no dir given prompt user
#           else set netlist_dir to dir  
#
# what==2: just set netlist_dir according to local_netlist_dir setting
#
# Return current netlist directory
#
proc set_netlist_dir { what {dir {} }} {
  global netlist_dir env OS has_x local_netlist_dir USER_CONF_DIR

  #### set local-to-schematic-dir if local_netlist_dir tcl var is set
  simuldir
  regsub {/$} $netlist_dir {} netlist_dir
  if {$what == 2} { return $netlist_dir }

  #### what == 0
  if {$what == 0} {
    if {$netlist_dir ne {}} {
      if {![file exist $netlist_dir]} {
        if {[catch {file mkdir "$netlist_dir"} err]} {
          puts stderr $err
          if {[info exists has_x]} {
            tk_messageBox -message "$err" -icon error -parent [xschem get topwindow] -type ok
          } 
        }
      }
    } 
  #### what == 1
  } else {
    # if local_netlist_dir is set can not provide a dir, set dir to netlist_dir as set by proc simuldir
    if {$local_netlist_dir != 0} {
      set dir $netlist_dir
    }
    if { $dir eq {} } {
      # set to default simulations/ directory.
      if {$local_netlist_dir == 0  && $netlist_dir eq {}} {
        set initdir "$USER_CONF_DIR/simulations"
      } elseif {$local_netlist_dir == 0 } {
        set initdir $netlist_dir
      } elseif { $netlist_dir ne {} }  { 
        set initdir $netlist_dir
      } else {
        if {$OS == "Windows"} {
          set initdir  $env(windir)
        } else {
          set initdir  [pwd]
        }
      }
      # prompt user for a dir
      set new_dir [tk_chooseDirectory -initialdir $initdir \
         -parent [xschem get topwindow] -title {Select netlist DIR} -mustexist false]
    } else {
      # use provided dir argument
      set new_dir $dir
    }
  
    # create new dir if not already existing
    if {$new_dir ne {} } {
      if {![file exist $new_dir]} {
        if {[catch {file mkdir "$new_dir"} err]} {
          puts stderr $err
          if {[info exists has_x]} {
            tk_messageBox -message "$err" -icon error -parent [xschem get topwindow] -type ok
          } 
        }
  
      }
      set netlist_dir $new_dir  
    }
  } ;# what == 1
  regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
  regsub {/$} $netlist_dir {} netlist_dir
  # return $netlist_dir if valid and existing, else return empty string
  if {$netlist_dir ne {} && [file exists $netlist_dir]} {
    return $netlist_dir
  }
  return {}
}


proc enter_text {textlabel {preserve_disabled disabled}} {
  global retval has_cairo preserve_unchanged_attrs wm_fix props enter_text_default_geometry
  global text_tabs_setting tabstop
  set tctx::rcode {}
  toplevel .dialog -class Dialog
  wm title .dialog {Enter text}
  wm transient .dialog [xschem get topwindow]

  set X [expr {[winfo pointerx .dialog] - 30}]
  set Y [expr {[winfo pointery .dialog] - 25}]

  bind .dialog <Configure> {
    set enter_text_default_geometry [wm geometry .dialog]
    regsub {\+.*} $enter_text_default_geometry {} enter_text_default_geometry
  }

  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "${enter_text_default_geometry}+$X+$Y"
  frame .dialog.f1
  label .dialog.f1.txtlab -text $textlabel
  eval text .dialog.txt -undo 1 -width 120 -height 9 $text_tabs_setting
  .dialog.txt delete 1.0 end
  .dialog.txt insert 1.0 $retval
  checkbutton .dialog.f1.l1 -text "preserve unchanged props" -variable preserve_unchanged_attrs \
     -state $preserve_disabled
  pack .dialog.f1 -side top -fill x ;# -expand yes
  pack .dialog.f1.l1 -side left
  pack .dialog.f1.txtlab -side left -expand yes -fill x

  pack .dialog.txt -side top -fill  both -expand yes
  frame .dialog.edit
  frame .dialog.edit.hsize
  frame .dialog.edit.vsize
  frame .dialog.edit.props
  pack  .dialog.edit.hsize -side bottom -expand yes -fill x 
  pack  .dialog.edit.vsize -side bottom -expand yes -fill x 
  pack  .dialog.edit.props -side bottom -expand yes -fill x 
  pack .dialog.edit  -side top  -fill x 
  if {$has_cairo } {
    entry .dialog.edit.hsize.hsize -relief sunken -textvariable tctx::vsize -width 20
    entry_replace_selection .dialog.edit.hsize.hsize
  } else {
    entry .dialog.edit.hsize.hsize -relief sunken -textvariable tctx::hsize -width 20
    entry_replace_selection .dialog.edit.hsize.hsize
  }
  entry .dialog.edit.vsize.vsize -relief sunken -textvariable tctx::vsize -width 20
  entry_replace_selection .dialog.edit.vsize.vsize
  text .dialog.edit.props.props -undo 1 -width 70 -height 3
  .dialog.edit.props.props insert 1.0 $props
  label .dialog.edit.hsize.hlab -text "hsize:"
  label .dialog.edit.vsize.vlab -text "vsize:"
  label .dialog.edit.props.proplab -text "props:"
  pack .dialog.edit.hsize.hlab -side left
  pack .dialog.edit.hsize.hsize -side left -fill x -expand yes
  pack .dialog.edit.vsize.vlab  -side left
  pack .dialog.edit.vsize.vsize -side left -fill x -expand yes
  pack .dialog.edit.props.proplab -side left
  pack .dialog.edit.props.props -side left -fill x -expand yes
  frame .dialog.buttons
  button .dialog.buttons.ok -text "OK" -command  \
  {
   set props [.dialog.edit.props.props get 1.0 {end - 1 chars}]
   set retval [.dialog.txt get 1.0 {end - 1 chars}]
   if {$has_cairo} { 
     set tctx::hsize $tctx::vsize
   }
   set tctx::rcode {ok}
   destroy .dialog 
  }
  button .dialog.buttons.cancel -text "Cancel" -command  \
  {
   set retval {}
   set tctx::rcode {}
   destroy .dialog 
  }
  button .dialog.buttons.b3 -text "Load" -command \
  {
    global INITIALTEXTDIR
    if { ![info exists INITIALTEXTDIR] } { set INITIALTEXTDIR [xschem get current_dirname] }
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALTEXTDIR ]
    if [string compare $a ""] {
     set INITIALTEXTDIR [file dirname $a]
     read_data_window  .dialog.txt  $a
    }
  }
  button .dialog.buttons.b4 -text "Del" -command \
  {
    .dialog.txt delete 1.0 end
  }
  pack .dialog.buttons.ok  -side left -fill x -expand yes
  pack .dialog.buttons.cancel  -side left -fill x -expand yes
  pack .dialog.buttons.b3  -side left -fill x -expand yes
  pack .dialog.buttons.b4  -side left -fill x -expand yes
  pack .dialog.buttons -side bottom -fill x
  bind .dialog <Escape> {
    if ![string compare $retval [.dialog.txt get 1.0 {end - 1 chars}]] {
      .dialog.buttons.cancel invoke
    }
  }
  bind .dialog.txt <Shift-KeyRelease-Return> {return_release %W; .dialog.buttons.ok invoke}
  #grab set .dialog
  tkwait window .dialog
  return $retval
}

# will redefine puts to output into tclcmd_puts
proc redef_puts {} {
  global tclcmd_puts
  if ![llength [info command ::tcl::puts]] {
    rename puts ::tcl::puts
    proc puts args {
      set la [llength $args]
      if {$la<1 || $la>3} {
        error "usage: puts ?-nonewline? ?channel? string"
      }
      set nl \n
      if {[lindex $args 0]=="-nonewline"} {
        set nl ""
        set args [lrange $args 1 end]
      }
      if {[llength $args]==1} {
        set args [list stdout [join $args]] ;# (2)
      }
      foreach {channel s} $args break
      #set s [join $s] ;# (1) prevent braces at leading/tailing spaces
      if {$channel=="stdout" || $channel=="stderr"} {
        append tclcmd_puts $s$nl
      } else {
        set cmd ::tcl::puts
        if {$nl==""} {lappend cmd -nonewline}
        lappend cmd $channel $s
        eval $cmd
      }
    };# puts
  }
}

# return key release, used to remove last entered character
# when binding close text-widget window to Shift-return or Control-return.
proc return_release {window} {
  set curs [$window index insert]
  $window delete "$curs - 1 chars" $curs
}


proc tclcmd_ok_button {} {
  global tclcmd_txt tclcmd_puts

  set tclcmd_txt [.tclcmd.t get 1.0 end]
  redef_puts 
  catch {uplevel #0 $tclcmd_txt} tclcmd_puts
  rename puts {}
  rename ::tcl::puts puts
  if {$tclcmd_puts != {} && [string index $tclcmd_puts end] != "\n"} {
    append tclcmd_puts "\n"
  }
  .tclcmd.r.r insert end $tclcmd_puts
  .tclcmd.r.r yview moveto 1
}

# evaluate a tcl command from GUI
proc tclcmd {} {
  global tclcmd_txt
  if {[winfo exists .tclcmd]} {
    destroy .tclcmd
  }
  toplevel .tclcmd -class Dialog
  # wm transient .tclcmd [xschem get topwindow]
  label .tclcmd.txtlab -text {Enter TCL expression. Shift-Return will evaluate}
  panedwindow .tclcmd.p -orient vert
  text .tclcmd.t -undo 1 -width 100 -height 3
  frame .tclcmd.r
  text .tclcmd.r.r -undo 1 -width 100 -height 8 -yscrollcommand ".tclcmd.r.yscroll set"
  scrollbar .tclcmd.r.yscroll -command  ".tclcmd.r.r yview"
  .tclcmd.p add .tclcmd.t .tclcmd.r
  .tclcmd.t insert 1.0 $tclcmd_txt

  frame .tclcmd.b
  button .tclcmd.b.clear -text Clear -command {
    .tclcmd.r.r delete 1.0 end
  }
  button .tclcmd.b.close -text Close -command {
    set tclcmd_txt [.tclcmd.t get 1.0 {end - 1 chars}]
    destroy .tclcmd
  }
  button .tclcmd.b.ok -text Evaluate -command {tclcmd_ok_button}
  # bind .tclcmd.t <Shift-KeyPress-Return> { .tclcmd.b.ok invoke }
  bind .tclcmd.t <Shift-KeyRelease-Return> {return_release %W; .tclcmd.b.ok invoke }
  pack .tclcmd.txtlab -side top -fill x
  pack .tclcmd.b -side bottom -fill x
  pack .tclcmd.p -side top -fill  both -expand yes
  pack .tclcmd.r.yscroll -side right -fill y
  pack .tclcmd.r.r -side top -fill  both -expand yes
  pack .tclcmd.b.ok -side left -expand yes -fill x
  pack .tclcmd.b.close -side left -expand yes -fill x
  pack .tclcmd.b.clear -side left -expand yes -fill x
}

proc select_layers {} {
  global dark_colorscheme enable_layer
  # xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .sl -class Dialog
  wm transient .sl [xschem get topwindow]
  if { $dark_colorscheme == 1 } {
    set txt_color black
  } else {
    set txt_color white
  }
  set j 0
  set f 0
  frame .sl.f0
  frame .sl.f1
  pack .sl.f0 .sl.f1 -side top -fill x
  button .sl.f1.ok -text OK -command { destroy .sl}
  pack .sl.f1.ok -side left -expand yes -fill x
  frame .sl.f0.f$f 
  pack .sl.f0.f$f -side left -fill y
  foreach i $tctx::colors {
    if { $dark_colorscheme == 1 } {
      set ind_bg white
    } else {
      set ind_bg black
    }
    if {  $j == [xschem get pinlayer] } {
      set laylab [format %2d $j]-PIN
      set layfg $txt_color
    } elseif { $j == [xschem get wirelayer] } {
      set laylab [format %2d $j]-WIRE
      set layfg $txt_color
    } elseif { $j == [xschem get textlayer] } {
      set laylab [format %2d $j]-TEXT
      set layfg $txt_color
    } elseif { $j == [xschem get backlayer] } {
      set laylab [format %2d $j]-BG
      if { $dark_colorscheme == 1 } {
        set layfg white
        set ind_bg black
      } else {
        set layfg black
        set ind_bg white
      }
    } elseif { $j == [xschem get gridlayer] } {
      set laylab [format %2d $j]-GRID
      set layfg $txt_color
    } else {
      set laylab "[format %2d $j]        "
      set layfg $txt_color
    }

    checkbutton .sl.f0.f$f.cb$j -text $laylab  -variable enable_layer($j) -activeforeground $layfg \
       -selectcolor $ind_bg -anchor w -foreground $layfg -background $i -activebackground $i \
       -command { 
           xschem enable_layers
           xschem redraw
        }
    pack .sl.f0.f$f.cb$j -side top -fill x
    incr j
    if { [expr {$j%10}] == 0 } {
      incr f
      frame .sl.f0.f$f
      pack .sl.f0.f$f -side left  -fill y
    }
  }
  # tkwait window .sl
  # xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc color_dim {} {
  global dim_bg dim_value enable_dim_bg
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dim -class Dialog
  wm title .dim {Dim colors}
  wm transient .dim [xschem get topwindow]
  checkbutton .dim.bg -text {Dim background} -variable enable_dim_bg
  # xschem color_dim <scale value> sets also dim_value variable
  scale .dim.scale -digits 2 -label {Dim factor} -length 256 \
     -showvalue 1 -command {xschem color_dim} -orient horizontal \
     -from -5 -to 5 -resolution 0.1
  button .dim.ok -text OK -command {destroy .dim}
  .dim.scale set $dim_value
  pack .dim.scale
  pack .dim.bg -side left
  pack .dim.ok -side right -anchor e
  tkwait window .dim
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

# show xschem about dialog
proc about {} {
  global OS running_in_src_dir
  if [winfo exists .about] { 
    bind .about.link <Button-1> {}
    bind .about.link2 <Button-1> {}
    destroy .about
  }
  toplevel .about -class Dialog
  wm title .about {About XSCHEM}
  wm transient .about [xschem get topwindow]
  label .about.xschem -text "XSCHEM V[xschem get version]" -font {Sans 24 bold}
  label .about.descr -text "Schematic editor / netlister for VHDL, Verilog, SPICE, tEDAx"
  button .about.link -text {http://repo.hu/projects/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link2 -text {https://github.com/StefanSchippers/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link3 -text {Online XSCHEM Manual} -font Underline-Font -fg blue -relief flat
  button .about.link4 -text {Local XSCHEM Manual} -font Underline-Font -fg blue -relief flat
  label .about.copyright -text "\n Copyright (C) 1998-2024 Stefan Schippers (stefan.schippers@gmail.com) \n
 This is free software; see the source for copying conditions.  There is NO warranty;
 not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\n"
  button .about.close -text Close -command {destroy .about} -font {Sans 18}
  pack .about.xschem
  pack .about.link
  pack .about.link2
  pack .about.link3
  pack .about.link4
  pack .about.descr
  pack .about.copyright
  pack .about.close
  if {$OS == "Windows"} {
    bind .about.link <Button-1> {eval start http://repo.hu/projects/xschem}
    bind .about.link2 <Button-1> {eval start https://github.com/StefanSchippers/xschem}
    bind .about.link3 <Button-1> {eval start http://repo.hu/projects/xschem/index.html}
    bind .about.link4 <Button-1> {
      if {$running_in_src_dir} {
        eval start [pwd]/../doc/xschem_man/xschem_man.html
      } else {
        eval start $XSCHEM_SHAREDIR/../doc/xschem/xschem_man/xschem_man.html
      }
    }
  } else {
    bind .about.link <Button-1> {execute 0 xdg-open http://repo.hu/projects/xschem}
    bind .about.link2 <Button-1> {execute 0 xdg-open https://github.com/StefanSchippers/xschem}
    bind .about.link3 <Button-1> {execute 0 xdg-open http://repo.hu/projects/xschem/index.html}
    bind .about.link4 <Button-1> {
      if {$running_in_src_dir} {
        execute 0 xdg-open file://$XSCHEM_SHAREDIR/../doc/xschem_man/xschem_man.html
      } else {
        execute 0 xdg-open file://$XSCHEM_SHAREDIR/../doc/xschem/xschem_man/xschem_man.html
      }
    }
  }
}

proc property_search {} {
  global search_value search_found
  global search_exact search_case
  global search_select
  global custom_token OS

  set search_found 0
  while { !$search_found} {
    if { [winfo exists .dialog] } return
    xschem set semaphore [expr {[xschem get semaphore] +1}]
    toplevel .dialog -class Dialog
    wm title .dialog {Search}
    wm transient .dialog [xschem get topwindow]
    if { ![info exists X] } {
      set X [expr {[winfo pointerx .dialog] - 60}]
      set Y [expr {[winfo pointery .dialog] - 35}]
    }
    wm geometry .dialog "+$X+$Y"
    frame .dialog.custom 
    label .dialog.custom.l -text "Token"
    entry .dialog.custom.e -width 32
    entry_replace_selection .dialog.custom.e
    .dialog.custom.e insert 0 $custom_token
    pack .dialog.custom.e .dialog.custom.l -side right
    frame .dialog.val 
    label .dialog.val.l -text "Value"
    entry .dialog.val.e -width 32
    entry_replace_selection .dialog.val.e
    .dialog.val.e insert 0 $search_value
    pack .dialog.val.e .dialog.val.l -side right
    frame .dialog.but
    button .dialog.but.ok -text OK -command {
          set search_value [.dialog.val.e get]
          set custom_token [.dialog.custom.e get]
          if {$debug_var<=-1} { puts stderr "|$custom_token|" }
          if { $search_exact==1 } {
            set search_found [xschem searchmenu exact $search_select $custom_token $search_value $search_case]
          } else {
            set search_found [xschem searchmenu regex $search_select $custom_token $search_value $search_case]
          }
          destroy .dialog 
    }
    button .dialog.but.cancel -text Cancel -command { set search_found 1; destroy .dialog }
    
    # Window doesn't support regular expression, has to be exact match for now
    checkbutton .dialog.but.sub -text {Exact search} -variable search_exact 
    checkbutton .dialog.but.case -text {Match case} -variable search_case 
    radiobutton .dialog.but.nosel -text {Highlight} -variable search_select -value 0
    radiobutton .dialog.but.sel -text {Select} -variable search_select -value 1
    # 20171211 added unselect
    radiobutton .dialog.but.unsel -text {Unselect} -variable search_select -value -1
    pack .dialog.but.ok  -anchor w -side left
    pack .dialog.but.sub  -side left
    pack .dialog.but.case  -side left
    pack .dialog.but.nosel  -side left
    pack .dialog.but.sel  -side left
    pack .dialog.but.unsel  -side left
    pack .dialog.but.cancel -anchor e
    pack .dialog.custom  -anchor e
    pack .dialog.val  -anchor e
    pack .dialog.but -expand yes -fill x
    focus  .dialog
    bind .dialog <Escape> {.dialog.but.cancel invoke}
    bind .dialog <Return> {.dialog.but.ok invoke}
    wm protocol .dialog WM_DELETE_WINDOW {.dialog.but.cancel invoke} 
    # grab set .dialog
    tkwait window .dialog
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  } 
  return {}
}

#20171029
# allows to call TCL hooks from 'format' strings during netlisting
# example of symbol spice format definition:
# format="@name @pinlist @symname @tcleval(<script>) m=@m"
# NOTE: spaces and quotes in <script> must be escaped
# symname and instname are predefined variables in the <script> context
# they can be used together with TCL xschem command to query instance or symbol 
# attributes.
#
proc tclpropeval {s instname symname} {
  # puts "tclpropeval: $s $instname $symname"
  global env debug_var
  regsub {^@tcleval\(} $s {} s
  regsub {\)([ \t\n]*)$} $s {\1} s
  # puts "tclpropeval: $s $instname $symname"
  if { [catch {subst $s} res] } {
    # puts stderr $res
    set res ?\n
  }
  return $res
}

# this hook is called in translate() if whole string is contained in a tcleval(...) construct
proc tclpropeval2 {s} {
  global debug_tcleval env path debug_var sch_basename

  set raw_level [xschem get raw_level]
  set netlist_type [xschem get netlist_type]
  # puts "tclpropeval2: s=|$s|"
  if {$debug_var <=-1} {puts "tclpropeval2: $s"}
  set path [string range [xschem get sch_path] 1 end]
  set sch_basename [file tail [file rootname [xschem get current_name]]]
  # skip hierarchy components above the level where raw file has been loaded. 
  # node path names to look up in raw file begin from there.
  set skip 0
  while { $skip < $raw_level } {
    regsub {^[^.]*\.} $path {} path
    incr skip
  }
  
  if { $netlist_type eq {spice} } {
    # this is necessary if spiceprefix is being used in netlists
    regsub {^([^xX])} $path {x\1} path
    while { [regsub {\.([^xX])} $path {.x\1} path] } {}
  }
  
  ## no more necessary, ':' are converted to '.' when reading raw file */
  #  if { [sim_is_xyce]} {
  #    regsub -all {\.} [string toupper $path] {:} path
  #  }

  if { $debug_var<=-1 } { puts "---> path=$path" }
  regsub {^tcleval\(} $s {} s
  regsub {\)([ \n\t]*)$} $s {\1} s
  # puts "tclpropeval2: s=|$s|"
  # puts "tclpropeval2: subst $s=|[subst $s]|"
  if { [catch {uplevel #0 "subst \{$s\}"} res] } {
    if { $debug_tcleval > 0} { puts "tclpropeval2 warning: $s --> $res"}
    set res ?\n
  }
  # puts "tclpropeval2: res=|$res|"
  return $res
}

proc attach_labels_to_inst {} {
  global use_lab_wire use_label_prefix custom_label_prefix do_all_inst rotated_text

  set tctx::rcode {}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Add labels to instances}
  wm transient .dialog [xschem get topwindow]

  # 20100408
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  wm geometry .dialog "+$X+$Y"

  frame .dialog.custom 
  label .dialog.custom.l -text "Prefix"
  entry .dialog.custom.e -width 32
  entry_replace_selection .dialog.custom.e
  .dialog.custom.e insert 0 $custom_label_prefix
  pack .dialog.custom.e .dialog.custom.l -side right


  frame .dialog.but
  button .dialog.but.ok -text OK -command {
        set custom_label_prefix [.dialog.custom.e get]
        #### put command here
        set tctx::rcode yes
        destroy .dialog 
  }
  button .dialog.but.cancel -text Cancel -command { set tctx::rcode {}; destroy .dialog }
  checkbutton .dialog.but.wire -text {use wire labels} -variable use_lab_wire
  checkbutton .dialog.but.do_all -text {Do all} -variable do_all_inst
  label .dialog.but.rot -text {Rotated Text}
  entry .dialog.but.rotated -textvariable rotated_text -width 2
  entry_replace_selection .dialog.but.rotated
  checkbutton .dialog.but.prefix -text {use prefix} -variable use_label_prefix
  pack .dialog.but.ok  -anchor w -side left
  pack .dialog.but.prefix  -side left
  pack .dialog.but.wire  -side left
  pack .dialog.but.do_all  -side left
  pack .dialog.but.rotated  -side left
  pack .dialog.but.rot  -side left
  pack .dialog.but.cancel -anchor e

  pack .dialog.custom  -anchor e
  pack .dialog.but -expand yes -fill x

  focus  .dialog
  bind .dialog <Escape> {.dialog.but.cancel invoke}
  bind .dialog <Return> {.dialog.but.ok invoke}
  grab set .dialog
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return {}
}

# if no_ask_save is set to 1 just return yes (always save)
proc ask_save_optional { {ask {save file?}} {cancel 1}} {
  global no_ask_save

  if { $no_ask_save } {
    return yes
  } else {
    return [ask_save $ask $cancel]
  }
}

proc ask_save { {ask {save file?}} {cancel 1}} {
  global wm_fix
  set tctx::rcode {}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Ask Save}
  wm transient .dialog [xschem get topwindow]

  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  if { $wm_fix } { tkwait visibility .dialog }

  wm geometry .dialog "+$X+$Y"
  label .dialog.l1  -text $ask
  frame .dialog.f1
  button .dialog.f1.b1 -text {Yes} -command\
  {
   set tctx::rcode {yes}
   destroy .dialog
  }
  if {$cancel} {
    button .dialog.f1.b2 -text {Cancel} -command\
    {
     set tctx::rcode {}
     destroy .dialog
    }
  }
  button .dialog.f1.b3 -text {No} -command\
  {
   set tctx::rcode {no}
   destroy .dialog
  }
  pack .dialog.l1 .dialog.f1 -side top -fill x
  pack .dialog.f1.b1 -side left -fill x -expand yes
  if { $cancel} {
    pack .dialog.f1.b2 -side left -fill x -expand yes
  }
  pack .dialog.f1.b3 -side left -fill x -expand yes
  if {$cancel} {
    bind .dialog <Escape> {.dialog.f1.b2 invoke}
  }
  # needed, otherwise problems when descending with double clixk 23012004
  tkwait visibility .dialog
  grab set .dialog
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $tctx::rcode
}


proc edit_vi_prop {txtlabel} {
  global XSCHEM_TMP_DIR retval symbol prev_symbol debug_var editor
  global user_wants_copy_cell
 
  set netlist_type [xschem get netlist_type]
  set user_wants_copy_cell 0
  set tctx::rcode {}
  set filename .xschem_edit_file.[pid]
  if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
  set filename $filename.$suffix
  write_data $retval $XSCHEM_TMP_DIR/$filename
  # since $editor can be an executable with options (gvim -f) I *need* to use eval
  eval execute_wait 0 $editor $XSCHEM_TMP_DIR/$filename ;# 20161119
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\nretval=$retval\n---------\n"}
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\nsymbol=$symbol\n---------\n"}
  set tmp [read_data $XSCHEM_TMP_DIR/$filename]
  file delete $XSCHEM_TMP_DIR/$filename
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
  if [string compare $tmp $retval] {
         set retval $tmp
         if {$debug_var<=-1} {puts "modified"}
         set tctx::rcode ok
         return  $tctx::rcode
  } else {
         set tctx::rcode {}
         return $tctx::rcode
  }
}

proc edit_vi_netlist_prop {txtlabel} {
  global XSCHEM_TMP_DIR retval debug_var editor
  global user_wants_copy_cell
 
  set netlist_type [xschem get netlist_type]
  set user_wants_copy_cell 0
  set filename .xschem_edit_file.[pid]
  if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
  set filename $filename.$suffix
  regsub -all {\\?"} $retval {"} retval
  regsub -all {\\?\\} $retval {\\} retval
  write_data $retval $XSCHEM_TMP_DIR/$filename
  if { [regexp vim $editor] } { set ftype "\{-c :set filetype=$netlist_type\}" } else { set ftype {} }
  # since $editor can be an executable with options (gvim -f) I *need* to use eval
  eval execute_wait 0 $editor  $ftype $XSCHEM_TMP_DIR/$filename
  if {$debug_var <= -1}  {puts "edit_vi_prop{}:\n--------\n$retval\n---------\n"}
  set tmp [read_data $XSCHEM_TMP_DIR/$filename]
  file delete $XSCHEM_TMP_DIR/$filename
  if {$debug_var <= -1}  {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
  if [string compare $tmp $retval] {
         set retval $tmp
         regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
         set retval \"${retval}\" 
         if {$debug_var <= -1}  {puts "modified"}
         set tctx::rcode ok
         return  $tctx::rcode
  } else {
         set tctx::rcode {}
         return $tctx::rcode
  }
}
proc reset_colors {ask} {
  global dark_colors light_colors dark_colorscheme USER_CONF_DIR svg_colors ps_colors
  global light_colors_save dark_colors_save

  if {$ask} {
    set answer [tk_messageBox -message  "Warning: delete 'colors' configuration file?" \
        -icon warning -parent [xschem get topwindow] -type okcancel]
    if {$answer ne {ok}} { return }
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  set light_colors $light_colors_save
  set dark_colors $dark_colors_save
  if { $dark_colorscheme == 1 } {
    set tctx::colors $dark_colors
  } else {
    set tctx::colors $light_colors
  }
  regsub -all {"} $light_colors  {} ps_colors
  regsub -all {#} $ps_colors  {0x} ps_colors
  regsub -all {"} $tctx::colors {} svg_colors
  regsub -all {#} $svg_colors {0x} svg_colors
  file delete ${USER_CONF_DIR}/colors
  xschem build_colors
  xschem redraw
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc change_color {} {
  global dark_colors light_colors dark_colorscheme cadlayers USER_CONF_DIR svg_colors ps_colors

  set n [xschem get rectcolor]
  if { $n < 0 || $n >=$cadlayers} return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  if { $dark_colorscheme == 1 } {
    set c $dark_colors
  } else {
    set c $light_colors
  }
  set initial_color [lindex $c $n]
  set value [tk_chooseColor -initialcolor $initial_color]
  if {[string compare $value {}] } {
    set cc [lreplace $c $n $n $value]
    set tctx::colors $cc
    if { $dark_colorscheme == 1 } {
      set dark_colors $cc
    } else {
      set light_colors $cc
      regsub -all {"} $cc  {} ps_colors
      regsub -all {#} $ps_colors  {0x} ps_colors
    }
    regsub -all {"} $tctx::colors {} svg_colors
    regsub -all {#} $svg_colors {0x} svg_colors

    xschem build_colors
    xschem redraw
    set    savedata "#### THIS FILE IS AUTOMATICALLY GENERATED BY XSCHEM, DO NOT EDIT.\n"
    append savedata "set cadlayers $cadlayers\n"
    append savedata "set light_colors {$light_colors}\n"
    append savedata "set dark_colors {$dark_colors}\n"
    write_data $savedata ${USER_CONF_DIR}/colors
  }
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc edit_prop {txtlabel} {
  global edit_prop_size infowindow_text selected_tok edit_symbol_prop_new_sel edit_prop_pos
  global prev_symbol retval symbol no_change_attrs preserve_unchanged_attrs copy_cell debug_var
  global user_wants_copy_cell editprop_sympath retval_orig old_selected_tok text_tabs_setting
  global tabstop
  set user_wants_copy_cell 0
  set tctx::rcode {}
  set retval_orig $retval
  if {$debug_var <= -1}  {puts " edit_prop{}: retval=$retval"}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog  -class Dialog 
  wm title .dialog {Edit Properties}
  wm transient .dialog [xschem get topwindow]
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]

  # 20160325 change and remember widget size
  bind .dialog <Configure> { 
    # puts [wm geometry .dialog]
    if { $edit_symbol_prop_new_sel != 1 } {
      set edit_prop_size [wm geometry .dialog]
      set edit_prop_pos $edit_prop_size
      regsub {\+.*} $edit_prop_size {} edit_prop_size
      regsub {[^+]*\+} $edit_prop_pos {+} edit_prop_pos
    }
  }
  wm geometry .dialog "${edit_prop_size}+$X+$Y"
  set prev_symbol $symbol
  set editprop_sympath [get_directory [abs_sym_path [tclpropeval2 $symbol]]]
  frame .dialog.f4
  label .dialog.f4.l1  -text $txtlabel
  label .dialog.f4.path  -text "Path:"
  entry .dialog.f4.symprop  -textvariable editprop_sympath -width 0 -state readonly
  entry_replace_selection .dialog.f4.symprop
  eval text .dialog.symprop -undo 1 -yscrollcommand \".dialog.yscroll set\" -setgrid 1 \
                  -xscrollcommand \".dialog.xscroll set\" -wrap none $text_tabs_setting
    .dialog.symprop delete 1.0 end
    .dialog.symprop insert 1.0 $retval
  scrollbar .dialog.yscroll -command  ".dialog.symprop yview"
  scrollbar .dialog.xscroll -command ".dialog.symprop xview" -orient horiz
  frame .dialog.f1
  frame .dialog.f2
  label .dialog.f1.l2 -text "Symbol"
  entry .dialog.f1.e2 -width 30
  entry_replace_selection .dialog.f1.e2
  .dialog.f1.e2 insert 0 $symbol
  button .dialog.f1.b5 -text "Browse" -command {
    set r [tk_getOpenFile -parent .dialog -initialdir $INITIALINSTDIR ]
    if {$r ne {} } {
      .dialog.f1.e2 delete 0 end
      .dialog.f1.e2 insert 0 $r
    }
    raise .dialog .drw
  }
  button .dialog.f1.b1 -text "OK" -command   {
    set retval [.dialog.symprop get 1.0 {end - 1 chars}]
    if { $selected_tok ne {<ALL>} } {
      regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
      set retval \"${retval}\"
      set retval [xschem subst_tok $retval_orig $selected_tok $retval]
      set selected_tok {<ALL>}
    }
    set symbol [.dialog.f1.e2 get]
    set abssymbol [abs_sym_path $symbol]
    set tctx::rcode {ok}
    set user_wants_copy_cell $copy_cell
    set prev_symbol [abs_sym_path $prev_symbol]
    if { ($abssymbol ne $prev_symbol) && $copy_cell } {
      if { ![regexp {^/} $symbol] && ![regexp {^[a-zA-Z]:} $symbol] } {
        set symlist [file split $symbol]
        set symlen [llength $symlist]
        set abssymbol "[path_head $prev_symbol $symlen]/$symbol"
      }
      # puts "$abssymbol   $prev_symbol"
      if { [file exists "[file rootname $prev_symbol].sch"] } {
        if { ! [file exists "[file rootname ${abssymbol}].sch"] } {
          file copy "[file rootname $prev_symbol].sch" "[file rootname $abssymbol].sch"
          # puts "file copy [file rootname $prev_symbol].sch [file rootname $abssymbol].sch"
        }
      }
      if { [file exists "$prev_symbol"] } {
        if { ! [file exists "$abssymbol"] } {
          file copy "$prev_symbol" "$abssymbol"
          # puts "file copy [file rootname $prev_symbol].sym [file rootname $abssymbol].sym"
        }
      }
    }
    #puts "symbol: $symbol , prev_symbol: $prev_symbol"
    set copy_cell 0 ;# 20120919
    destroy .dialog
  }
  button .dialog.f1.b2 -text "Cancel" -command  {
    set tctx::rcode {}
    set edit_symbol_prop_new_sel {}
    destroy .dialog
  }
  wm protocol .dialog  WM_DELETE_WINDOW {
    set tctx::rcode {}
    set edit_symbol_prop_new_sel {}
    destroy .dialog
  }
  button .dialog.f1.b3 -text "Load" -command {
    global INITIALPROPDIR
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
    if [string compare $a ""] {
     set INITIALPROPDIR [file dirname $a]
     read_data_window  .dialog.symprop  $a
    }
  }
  button .dialog.f1.b4 -text "Del" -command {
    .dialog.symprop delete 1.0 end
  }
  checkbutton .dialog.f2.r1 -text "No change properties" -variable no_change_attrs -state normal
  checkbutton .dialog.f2.r2 -text "Preserve unchanged props" -variable preserve_unchanged_attrs -state normal
  checkbutton .dialog.f2.r3 -text "Copy cell" -variable copy_cell -state normal
  set tok_list "<ALL> [list_tokens $retval]"
  set selected_tok {<ALL>}
  set old_selected_tok {<ALL>}
  label .dialog.f2.r4 -text {   Edit Attr:}
  if  { [info tclversion] > 8.4} {
    ttk::combobox .dialog.f2.r5 -values $tok_list -textvariable selected_tok -width 14
  }
  pack .dialog.f1.l2 -side left
  pack .dialog.f1.e2 -side left -fill x -expand yes
  pack .dialog.f1.b1 -side left
  pack .dialog.f1.b2 -side left
  pack .dialog.f1.b3 -side left
  pack .dialog.f1.b4 .dialog.f1.b5 -side left
  pack .dialog.f4 -side top  -anchor nw
  #pack .dialog.f4.path .dialog.f4.symprop .dialog.f4.l1 -side left -fill x 
  pack .dialog.f4.path -side left
  pack .dialog.f4.symprop -side left 
  pack .dialog.f1 .dialog.f2 -side top -fill x 
  pack .dialog.f2.r1 -side left
  pack .dialog.f2.r2 -side left
  pack .dialog.f2.r3 -side left
  pack .dialog.f2.r4 -side left
  if { [info tclversion] > 8.4 } { pack .dialog.f2.r5 -side left }
  pack .dialog.yscroll -side right -fill y 
  pack .dialog.xscroll -side bottom -fill x
  pack .dialog.symprop  -fill both -expand yes
  bind .dialog.symprop <Shift-KeyRelease-Return> {return_release %W;.dialog.f1.b1 invoke}
  bind .dialog <Escape> {
    if { ![string compare $retval [.dialog.symprop get 1.0 {end - 1 chars}]] && \
         ![string compare $symbol [ .dialog.f1.e2 get]] } {
      .dialog.f1.b2 invoke
    }
  }
  if  { [info tclversion] > 8.4} {
    bind .dialog.f2.r5 <<ComboboxSelected>> {
      if {$old_selected_tok ne $selected_tok} {
        if { $old_selected_tok eq {<ALL>} } {
          set retval_orig [.dialog.symprop get 1.0 {end - 1 chars}]
        } else {
          set retval [.dialog.symprop get 1.0 {end - 1 chars}]
          regsub -all {(["\\])} $retval {\\\1} retval ;# vim syntax fix "
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } { 
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.symprop delete 1.0 end
      .dialog.symprop insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
 
    bind .dialog.f2.r5 <KeyRelease> {
      set selected_tok [.dialog.f2.r5 get]
      if { $old_selected_tok eq {<ALL>}} {
        set retval_orig [.dialog.symprop get 1.0 {end - 1 chars}]
      } else {
        set retval [.dialog.symprop get 1.0 {end - 1 chars}]
        if {$retval ne {}} {
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.symprop delete 1.0 end
      .dialog.symprop insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
  }
  if {$edit_symbol_prop_new_sel == 1} { 
    wm geometry .dialog $edit_prop_pos
  }
  set edit_symbol_prop_new_sel 0

  tkwait visibility .dialog
  # select text after value= or lab= and place cursor just before selection
  set regx {value *= *("([^"]|(\\"))+"|[^ \t\n"]+)} ;# vim syntax fix "
  set regx1 {value *= *[^ \n]}
  set idx [.dialog.symprop search -regexp -nolinestop -count nchars $regx 1.0]
  .dialog.symprop search -regexp -nolinestop -count len $regx1 1.0
  incr len -1
  if {$idx eq {} } { 
    set regx {lab *= *("([^"]|(\\"))+"|[^ \t\n"]+)} ;# vim syntax fix "
    set regx1 {lab *= *[^ \n]}
    set idx [.dialog.symprop search -regexp -nolinestop -count nchars $regx 1.0]
    .dialog.symprop search -regexp -nolinestop -count len $regx1 1.0
    incr len -1
  }
  if { $idx ne {} } {
    .dialog.symprop tag add sel "$idx + $len chars" "$idx + $nchars chars"
    .dialog.symprop mark set insert "$idx + $len chars"
  }
  focus .dialog.symprop
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $tctx::rcode
}

# reads metadata tokens from header component of the symbol inside `v {data}` element
# and returns dictionary in name of the token as key and value of the token as value in dictionary
# Courtesy @georgtree
proc symbolParse {file} {
    try {
        set file [open $file r]
    } on error {errmsg erropts} {
        puts stderr "Error while reading symbol file '$file': $errmsg"
        return
    }
    set data [read $file]
    close $file
    # pattern to find v {} and extract content inside brackets
    set pattern {(?:^|\n)v\s+\{((?:[^{}]|\\[{}])*)\}}
    if {![regexp $pattern $data -> content]} {
        return
    }
    # outer braces {...} are not part of content, so remove escaping of inner {, } and \ .
    set content [string map { \\\{ \{ \\\} \} \\\\ \\ } $content]
    # pattern to find: name="value" or name=value or name = "value" or name = value
    set pattern {(\w+)\s*=\s*(?:"((?:[^"]|\\["])*)"|(\S+))}
    set start 0
    # 'match' variable holds the complete matched pattern, used for searching next one
    while {[regexp -start $start $pattern $content -> name quotedValue unquotedValue]} {
        # Determine the value (quoted or unquoted)
        if {[string length $quotedValue] > 0} {
            set value $quotedValue
            # outer quotes are removed from value,
            # remove escaping of internal backslashes and doublequotes
            dict append tokens $name [string map {\\\\ \\ \\\" \" } $value] 
        } else {
            set value $unquotedValue
            dict append tokens $name $value
        }
        # Update the start index to continue searching after this match
        set start [expr {[string first $value $content $start] + [string length $value]}]
        # Safety check to prevent infinite loops
        if {$start <= 0} {
            break
        }
    }
    return $tokens
}

proc read_data_nonewline {f} {
  set fid [open $f r]
  set data [read -nonewline $fid]
  close $fid
  return $data
}

proc read_data {f} {
  set fid [open $f r]
  set data [read $fid]
  close $fid
  return $data
}

proc read_data_window {w f} {
  set fid [open $f r]
  set t [read $fid]
  #  $w delete 0.0 end
  ## 20171103 insert text at cursor position instead of at beginning (insert index tag)
  $w insert insert $t
  close $fid
}

proc write_data {data f} {
  set fid [open $f w]
  puts  -nonewline $fid $data
  close $fid
  return {}
}

proc text_line {txtlabel clear {preserve_disabled disabled} } {
  global text_line_default_geometry preserve_unchanged_attrs wm_fix tabstop
  global retval debug_var selected_tok retval_orig old_selected_tok text_tabs_setting
  set retval_orig $retval
  if $clear==1 then {set retval ""}
  if {$debug_var <= -1}  {puts " text_line{}: clear=$clear"}
  if {$debug_var <= -1}  {puts " text_line{}: retval=$retval"}
  set tctx::rcode {}
  if { [winfo exists .dialog] } return
  toplevel .dialog  -class Dialog
  wm title .dialog {Text input}
  wm transient .dialog [xschem get topwindow]
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]

  set tok_list "<ALL> [list_tokens $retval]"
  set selected_tok {<ALL>}
  set old_selected_tok {<ALL>}
  bind .dialog <Configure> {
    # puts [wm geometry .dialog]
    set text_line_default_geometry [wm geometry .dialog]
    regsub {\+.*} $text_line_default_geometry {} text_line_default_geometry
  }

  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "${text_line_default_geometry}+$X+$Y"

  frame .dialog.f0
  frame .dialog.f1
  label .dialog.f0.l1  -text $txtlabel
  eval text .dialog.textinput -undo 1 -relief sunken -bd 2 -yscrollcommand \".dialog.yscroll set\" -setgrid 1 \
       -xscrollcommand \".dialog.xscroll set\" -wrap none -width 90 -height 40 $text_tabs_setting
  scrollbar .dialog.yscroll -command  ".dialog.textinput yview"
  scrollbar .dialog.xscroll -command ".dialog.textinput xview" -orient horiz
  .dialog.textinput delete 1.0 end
  .dialog.textinput insert 1.0 $retval
  button .dialog.f1.b1 -text "OK" -command  \
  {
    set retval [.dialog.textinput get 1.0 {end - 1 chars}]
    if { $selected_tok ne {<ALL>} } {
      regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
      set retval \"${retval}\"
      set retval [xschem subst_tok $retval_orig $selected_tok $retval]
      set selected_tok {<ALL>}
    }
    destroy .dialog
    set tctx::rcode {ok}
  }
  button .dialog.f1.b2 -text "Cancel" -command  \
  {
    set retval [.dialog.textinput get 1.0 {end - 1 chars}]
    set tctx::rcode {}
    destroy .dialog
  }
  button .dialog.f1.b3 -text "Load" -command \
  {
    global INITIALPROPDIR
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
    if [string compare $a ""] {
     set INITIALPROPDIR [file dirname $a]
     read_data_window  .dialog.textinput  $a
    }
  }
  button .dialog.f1.b4 -text "Del" -command \
  {
    .dialog.textinput delete 1.0 end
  }
  label .dialog.f1.r4 -text {   Edit Attr:}
  if  { [info tclversion] > 8.4} {
    ttk::combobox .dialog.f1.r5 -values $tok_list -textvariable selected_tok -width 14
  }
  checkbutton .dialog.f0.l2 -text "preserve unchanged props" -variable preserve_unchanged_attrs \
     -state $preserve_disabled
  pack .dialog.f0 -fill x
  pack .dialog.f0.l2 -side left
  pack .dialog.f0.l1 -side left -expand yes
  pack .dialog.f1  -fill x
  pack .dialog.f1.b1 -side left -fill x -expand yes
  pack .dialog.f1.b2 -side left -fill x -expand yes
  pack .dialog.f1.b3 -side left -fill x -expand yes
  pack .dialog.f1.b4 -side left -fill x -expand yes
  pack .dialog.f1.r4 -side left
  if  { [info tclversion] > 8.4} {pack .dialog.f1.r5 -side left}


  pack .dialog.yscroll -side right -fill y 
  pack .dialog.xscroll -side bottom -fill x
  pack .dialog.textinput   -expand yes -fill both
  bind .dialog <Escape> {
    if ![string compare $retval [.dialog.textinput get 1.0 {end - 1 chars}]] {
      .dialog.f1.b2 invoke
    }
  }

  if  { [info tclversion] > 8.4} {
    bind .dialog.f1.r5 <<ComboboxSelected>> {
      if {$old_selected_tok ne $selected_tok} {
        if { $old_selected_tok eq {<ALL>} } {
          set retval_orig [.dialog.textinput get 1.0 {end - 1 chars}]
        } else {
          set retval [.dialog.textinput get 1.0 {end - 1 chars}]
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.textinput delete 1.0 end
      .dialog.textinput insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
 
    bind .dialog.f1.r5 <KeyRelease> {
      set selected_tok [.dialog.f1.r5 get]
      if { $old_selected_tok eq {<ALL>}} {
        set retval_orig [.dialog.textinput get 1.0 {end - 1 chars}]
      } else {
        set retval [.dialog.textinput get 1.0 {end - 1 chars}]
        if {$retval ne {}} {
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.textinput delete 1.0 end
      .dialog.textinput insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
  }
  bind .dialog.textinput <Shift-KeyRelease-Return> {return_release %W; .dialog.f1.b1 invoke}
  #tkwait visibility .dialog
  #grab set .dialog
  #focus .dialog.textinput
  set tctx::rcode {}   
  tkwait window .dialog
  return $tctx::rcode
}

# alert_ text [position] [nowait] [yesno]
# show an alert dialog box and display 'text'.
# if 'position' is empty (example: alert_ {hello, world} {}) show at mouse coordinates
# otherwise use specified coordinates example: alert_ {hello, world} +300+400
# if nowait is 1 do not wait for user to close dialog box
# if yesnow is 1 show yes and no buttons and return user choice (1 / 0).
# (this works only if nowait is unset).
proc alert_ {txtlabel {position +200+300} {nowait {0}} {yesno 0}} {
  global has_x
  set tctx::rcode 1
  if {![info exists has_x] } {return}
  toplevel .alert -class Dialog
  wm title .alert {Alert}
  wm transient .alert [xschem get topwindow]
  set X [expr {[winfo pointerx .alert] - 70}]
  set Y [expr {[winfo pointery .alert] - 60}]
  if { [string compare $position ""] != 0 } {
    wm geometry .alert $position
  } else {
    wm geometry .alert "+$X+$Y"
  }
  label .alert.l1 -font {Sans 10 bold} -justify left \
       -text "                              \n${txtlabel}  \n" -wraplength 750 
  if { $yesno} {
    set oktxt Yes
  } else {
    set oktxt OK
  }
  button .alert.b1 -text $oktxt -command  \
  {
    set tctx::rcode 1
    destroy .alert
  } 
  if {$yesno} {
    button .alert.b2 -text "No" -command  \
    {  
      set tctx::rcode 0
      destroy .alert
    }  
  }

  pack .alert.l1 -side top -fill both -expand yes
  pack .alert.b1 -side left -fill x -expand yes
  if {$yesno} {pack .alert.b2 -side left -fill x -expand yes}
  tkwait visibility .alert
  # grab set .alert
  focus .alert.b1
  bind .alert <Return> { destroy .alert }
  bind .alert <Escape> { destroy .alert }
  bind .alert <Visibility> {
    if { [winfo exists .alert] && [winfo ismapped .alert] && [winfo ismapped .] && [wm stackorder .alert isbelow . ]} {
      if { [winfo exists .drw] } {
        raise .alert .drw
      } else {
        raise .alert
      }
      
    }
  }
  if {!$nowait} {tkwait window .alert}
  return $tctx::rcode
}

proc show_infotext {{err 0}} {
  global has_x infowindow_text show_infowindow_after_netlist
  set s $show_infowindow_after_netlist
  if {$s == 1} { set s always}
  if {$s == 0} { set s onerror}
  if {[info exists has_x]} {
    infowindow
  }
  if {($s eq {always}) || ($err != 0 && $s eq {onerror})} {
    if {[info exists has_x]} {
      wm deiconify .infotext
    } else {
      xschem log_write  [xschem get infowindow_text]
    }
  }
}

proc infowindow {} {
  global infowindow_text has_x
  set infowindow_text [xschem get infowindow_text]
  if {![info exists has_x]} { return }
  set z {.infotext}
  if ![string compare $infowindow_text ""] { 
    if [winfo exists $z] {
      $z.f1.text delete 1.0 end
    }
  }
  if ![winfo exists $z] {
    toplevel $z
    wm title $z {Info window}
    # wm transient $z [xschem get topwindow]
    wm geometry $z 90x24+0+400 ;# gridded window: grid units, not pixels
    wm iconname $z {Info window}
    wm withdraw $z
    wm protocol .infotext WM_DELETE_WINDOW "wm withdraw $z; set show_infowindow 0"
    #  button $z.dismiss -text Dismiss -command  "destroy $z"
    frame $z.f1
    frame $z.f2
    text $z.f1.text -undo 1 -relief sunken -bd 2 -yscrollcommand "$z.f1.yscroll set" -setgrid 1 \
         -height 6 -width 50  -xscrollcommand "$z.f1.xscroll set" -wrap none
    scrollbar $z.f1.yscroll -command "$z.f1.text yview" -orient v 
    scrollbar $z.f1.xscroll -command "$z.f1.text xview" -orient h 
    grid $z.f1.text - $z.f1.yscroll -sticky nsew
    grid $z.f1.xscroll - -sticky ew
    # grid $z.dismiss - -
    grid rowconfig $z.f1 0 -weight 1
    grid columnconfig $z.f1 0 -weight 1
    pack $z.f1 -fill both -expand yes
    button $z.f2.dismiss -text Dismiss -command "wm withdraw $z; set show_infowindow 0"
    pack $z.f2.dismiss
    pack $z.f2 -fill x
    bind $z <Escape> "wm withdraw $z; set show_infowindow 0"
  }
  $z.f1.text delete 1.0 end
  $z.f1.text insert 1.0 $infowindow_text
  ## does not work in tcl/tk8.4
  # set lines [$z.f1.text count -displaylines 1.0 end]
  set lines [regexp -all \n [$z.f1.text get 1.0 end]]
  $z.f1.text see ${lines}.0
  return {}
}

proc editdata {{data {}} {title {Edit data}} } {
  global text_tabs_setting tabstop retval
  set window .editdata
  set retval $data
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel $window
  wm title $window $title
  wm iconname $window $title
  # wm transient $window [xschem get topwindow]
  frame $window.buttons
  pack $window.buttons -side bottom -fill x -pady 2m
  button $window.buttons.copy -text Copy -command "
     clipboard clear
     clipboard append \[$window.text get 1.0 {end - 1 chars}\]
  "
  button $window.buttons.ok -text OK -command "
     set retval \[$window.text get 1.0 {end - 1 chars}\]; destroy $window
  "
  button $window.buttons.cancel -text Cancel -command "destroy $window"
  pack $window.buttons.ok -side left -expand 1
  pack $window.buttons.cancel -side left -expand 1
  pack $window.buttons.copy -side left -expand 1
  
  eval text $window.text -undo 1 -relief sunken -bd 2 -yscrollcommand \"$window.yscroll set\" -setgrid 1 \
       -xscrollcommand \"$window.xscroll set\" -wrap none -height 30 $text_tabs_setting
  scrollbar $window.yscroll -command  "$window.text yview"
  scrollbar $window.xscroll -command "$window.text xview" -orient horiz
  pack $window.yscroll -side right -fill y
  pack $window.text -expand yes -fill both
  pack $window.xscroll -side bottom -fill x
  bind $window <Escape> "$window.buttons.cancel invoke"

  # 20171103 insert at insertion cursor(insert tag) instead of 0.0
  $window.text insert insert $data
  tkwait window $window
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $retval
}

proc textwindow {filename {ro {}}} {
  global textwindow_wcounter
  global textwindow_w
  # set textwindow_w .win$textwindow_wcounter
  # catch {destroy $textwindow_w}
  set textwindow_wcounter [expr {$textwindow_wcounter+1}]
  set textwindow_w .win$textwindow_wcounter


  global textwindow_filename text_tabs_setting tabstop
  global textwindow_fileid
  set textwindow_filename $filename
  toplevel $textwindow_w
  wm title $textwindow_w $filename
  wm iconname $textwindow_w $filename
  # wm transient $textwindow_w [xschem get topwindow]
 frame $textwindow_w.buttons
  pack $textwindow_w.buttons -side bottom -fill x -pady 2m
  button $textwindow_w.buttons.dismiss -text Dismiss -command "destroy $textwindow_w"
  pack $textwindow_w.buttons.dismiss -side left -expand 1
  if { $ro eq {} } {
    button $textwindow_w.buttons.save -text "Save" -command \
     { 
      set textwindow_fileid [open $textwindow_filename w]
      puts -nonewline $textwindow_fileid [$textwindow_w.text get 1.0 {end - 1 chars}]
      close $textwindow_fileid 
      destroy $textwindow_w
     }
    pack $textwindow_w.buttons.save  -side left -expand 1
  }

  eval text $textwindow_w.text -undo 1 -relief sunken -bd 2 -yscrollcommand \"$textwindow_w.yscroll set\" -setgrid 1 \
       -xscrollcommand \"$textwindow_w.xscroll set\" -wrap none -height 30 $text_tabs_setting
  scrollbar $textwindow_w.yscroll -command  "$textwindow_w.text yview" 
  scrollbar $textwindow_w.xscroll -command "$textwindow_w.text xview" -orient horiz
  pack $textwindow_w.yscroll -side right -fill y
  pack $textwindow_w.text -expand yes -fill both
  pack $textwindow_w.xscroll -side bottom -fill x
  bind $textwindow_w <Escape> "$textwindow_w.buttons.dismiss invoke"
  set textwindow_fileid [open $filename r]

  # 20171103 insert at insertion cursor(insert tag) instead of 0.0
  $textwindow_w.text insert insert [read $textwindow_fileid]
  close $textwindow_fileid
  return {}
}

proc viewdata {data {ro {}} {win .view}} {
  global viewdata_wcounter viewdata_filename
  global viewdata_w OS viewdata_fileid env has_x text_tabs_setting tabstop
  if {![info exists has_x]} {return}
  # set viewdata_w .view$viewdata_wcounter
  # catch {destroy $viewdata_w}
  if {$win eq {.view}} {
    set viewdata_wcounter [expr {$viewdata_wcounter+1}]
    set viewdata_w $win$viewdata_wcounter
  } else {
    set viewdata_w $win
  }
  set tctx::rcode {}
  toplevel $viewdata_w
  wm title $viewdata_w {View data}
  # wm transient $viewdata_w [xschem get topwindow]
  frame $viewdata_w.buttons
  pack $viewdata_w.buttons -side bottom -fill x -pady 2m

  button $viewdata_w.buttons.dismiss -text Dismiss -command  "destroy $viewdata_w" 
  pack $viewdata_w.buttons.dismiss -side left -expand 1

  if { $ro eq {} } {
    button $viewdata_w.buttons.saveas -text {Save As} -command  {
      if {$OS == "Windows"} {
        set viewdata_filename [tk_getSaveFile -initialdir $env(windir) ]
      } else {
        set viewdata_filename [tk_getSaveFile -initialdir [pwd] ]
      }
      if { $viewdata_filename != "" } {
        set viewdata_fileid [open $viewdata_filename w]
        puts -nonewline $viewdata_fileid [$viewdata_w.text get 1.0 {end - 1 chars}]
        close $viewdata_fileid
      }
    } 
    pack $viewdata_w.buttons.saveas  -side left -expand 1
  }

  eval text $viewdata_w.text -undo 1 -relief sunken -bd 2 -yscrollcommand \"$viewdata_w.yscroll set\" -setgrid 1 \
       -xscrollcommand \"$viewdata_w.xscroll set\" -wrap none -height 30 $text_tabs_setting
  scrollbar $viewdata_w.yscroll -command  "$viewdata_w.text yview" 
  scrollbar $viewdata_w.xscroll -command "$viewdata_w.text xview" -orient horiz
  pack $viewdata_w.yscroll -side right -fill y
  pack $viewdata_w.text -expand yes -fill both
  pack $viewdata_w.xscroll -side bottom -fill x
  # 20171103 insert at insertion cursor(insert tag) instead of 0.0
  $viewdata_w.text insert insert $data
  return $tctx::rcode
}

proc sub_match_file { f {paths {}} {maxdepth -1} } {
  global pathlist match_file_dir_arr match_file_level nolist_libs
  set res {}
  if {$paths eq {}} {set paths $pathlist}
  foreach i $paths {
    foreach j [glob -nocomplain -directory $i *] {
      set skip 0
      foreach k $nolist_libs {
        if {[regexp $k $j]} {
          set skip 1
          break
        }
      }
      if {$skip} { continue }
  
      if {[file isdirectory $j] && [file readable $j]} {
        if { $maxdepth == -1 || $match_file_level < $maxdepth} {
          set jj [regsub {/ $} [file normalize ${j}/\ ] {}]
          # if {[array names match_file_dir_arr -exact $jj] == {}} { ... }
          if {![info exists match_file_dir_arr($jj)]} {
            set match_file_dir_arr($jj) 1
            incr match_file_level
            set sub_res [sub_match_file $f $j $maxdepth] ;# recursive call
            incr match_file_level -1
            if {$sub_res != {} } {set res [concat $res $sub_res]}
          }
        }
      } else {
        if {![info exists match_file_dir_arr($j)]} {
         set match_file_dir_arr($j) 1
          if {[regexp $f $j]} {
            lappend res $j
          }
        }
      }
    }
  }
  return $res
}

# find files into $paths directories matching $f
# use $pathlist global search path if $paths empty
# recursively descend directories
proc match_file  { f {paths {}} {maxdepth -1}  } {
  global match_file_dir_arr match_file_level
  set err [catch {regexp $f {12345}} res]
  if {$err} {return {}}
  set match_file_level 0
  catch {unset match_file_dir_arr}
  set res  [sub_match_file $f $paths $maxdepth]
  catch {unset match_file_dir_arr}
  return $res
}

proc sub_find_file { f {paths {}} {first 0} {maxdepth -1}} {
  global pathlist match_file_dir_arr match_file_level nolist_libs
  set res {}
  if {$paths eq {}} {set paths $pathlist}
  foreach i $paths {
    foreach j [glob -nocomplain -directory $i *] {

      set skip 0
      foreach k $nolist_libs {
        if {[regexp $k $j]} {
          set skip 1
          break
        }
      }
      if {$skip} { continue }

      if {[file isdirectory $j]  && [file readable $j]} {
        if { $maxdepth == -1 || $match_file_level < $maxdepth} {
          set jj [regsub {/ $} [file normalize ${j}/\ ] {}]
          # if {[array names match_file_dir_arr -exact $jj] == {}} { ... }
          if {![info exists match_file_dir_arr($jj)]} {
            set match_file_dir_arr($jj) 1
            incr match_file_level
            set sub_res [sub_find_file $f $j $first $maxdepth] ;# recursive call
            incr match_file_level -1
            if {$sub_res != {} } {
              set res [concat $res $sub_res]
              if {$first} {return $res}
            }
          }
        }
      } else {
        set fname [file tail $j]
        if {![info exists match_file_dir_arr($j)]} {
          set match_file_dir_arr($j) 1
          if {$fname == $f} {
            lappend res $j
            if {$first} {return $res}
          }
        }
      }
    }
  }
  return $res
}

# find given file $f into $paths directories 
# use $pathlist global search path if $paths empty
# recursively descend directories
proc find_file { f {paths {}} {maxdepth -1}} {
  global match_file_dir_arr match_file_level
  set match_file_level 0
  catch {unset match_file_dir_arr}
  set res  [sub_find_file $f $paths 0 $maxdepth]
  catch {unset match_file_dir_arr}
  return $res
}

# find given file $f into $paths directories 
# use $pathlist global search path if $paths empty
# recursively descend directories
# only return FIRST FOUND
proc find_file_first  { f {paths {}} {maxdepth -1}} {
  global match_file_dir_arr match_file_level
  set match_file_level 0
  catch {unset match_file_dir_arr}
  set res  [sub_find_file $f $paths 1 $maxdepth] 
  catch {unset match_file_dir_arr}
  return $res
}        


# alternative implementation of "file dirname ... "
# that does not mess with http:// (file dirname removes double slashes)
proc get_directory {f} {
  if {![regexp {/} $f]} {
    set r .
  } else {
    set r [regsub {/[^/]*$} $f {}]
  }
  regsub {/\.$} $r {} r
  return $r
}

# set 'n' last directory components to every symbol
proc fix_symbols {n {pattern {}}} {
  xschem push_undo
  foreach {i s t} [xschem instance_list] {
    if {$pattern ne {} && ![regexp $pattern $s]} {continue}
    if {![regexp {\(.*\)$} $s param]} { set param {}}
    regsub {\([^)]*\)$} $s {} s
    set sympath  [find_file_first [file tail $s]]
    if { $sympath  ne {}} {
      set new_sym_ref [get_cell $sympath $n]$param
      puts "Remapping $i:  $s$param --> $new_sym_ref"
      xschem reset_symbol $i $new_sym_ref
    }
  }
  xschem reload_symbols
  xschem set_modify 1
  xschem redraw
}

# fetch a remote url into ${XSCHEM_TMP_DIR}/xschem_web
proc download_url {url} {
  global XSCHEM_TMP_DIR download_url_helper OS has_x
  # puts "download_url: $url"
  # puts "xschem_web_dirname: $xschem_web_dirname"
  set xschem_web_dirname [xschem get xschem_web_dirname]
  if {![file exists $xschem_web_dirname]} { 
    set msg {xschem_web_xxxxx directory was not created. Remote url download will not work}
    puts stderr $msg
    if {[info exists has_x]} {
      alert_ $msg {} 0 0
    }  
    return 1 ;# fail
  }
  if {$OS eq "Windows"} {
    set cmd "cmd /c \"cd ${xschem_web_dirname} & $download_url_helper $url\""
    set r [catch {eval exec $cmd } res]
  } else {
    set r [catch {exec sh -c "cd ${xschem_web_dirname}; $download_url_helper $url"} res]
  }
  # puts "download_url: url=$url, exit code=$r, res=$res"
  return $r
}

# use some heuristic to find a sub sch/sym reference in the web repository.
proc try_download_url {dirname sch_or_sym} {
  set url $dirname/$sch_or_sym
  # puts "try_download_url: dirname=$dirname, sch_or_sym=$sch_or_sym"
  set r [download_url $url]
  if { $r!=0} {
    # count # of directories in sch/sym reference
    set nitems [regexp -all {/+} $sch_or_sym]
    # puts "try_download_url: dirname=$dirname, sch_or_sym=$sch_or_sym, nitems=$nitems"
    while { $nitems > 0} {
      # remove one path component from dirname and try to download URL
      set dirname [get_directory $dirname]
      incr nitems -1
      set url $dirname/$sch_or_sym
      set r [download_url $url]
      # done if url found
      if { $r == 0 } { break } 
    }
  }
}

# Given an absolute path 'symbol' of a symbol/schematic remove the path prefix
# if file is in a library directory (a $pathlist dir)
# Example: rel_sym_path /home/schippes/share/xschem/xschem_library/devices/iopin.sym
#          devices/iopin.sym
proc rel_sym_path {symbol {paths {}} } {
  global OS pathlist env
  # puts "rel_sym_path: $symbol    $paths"

  if { $paths eq {}} {set paths $pathlist}
  regsub {^~/} $symbol ${env(HOME)}/ symbol
  # if {$OS eq "Windows"} {
  #   if {![regexp {^[A-Za-z]\:/} $symbol]} {
  #     set symbol [pwd]/$symbol
  #   }
  # } else {
  #   if {![regexp {^/} $symbol]} {
  #     set symbol [pwd]/$symbol
  #   }
  # }
  set curr_dirname [pwd]
  set name {}
  foreach path_elem $paths {
    if { ![string compare $path_elem .]  && [info exist curr_dirname]} {
      set path_elem $curr_dirname
    }
    regsub {/*$} $path_elem {/} path_elem
    set pl [string length $path_elem]
    if { [string equal -length $pl $path_elem $symbol] } {
      set name [string range $symbol [expr {$pl}] end]
      # When $path_elem is "C:/", string equal will match, 
      # but $path_elem should not be removed from $name if $name 
      # has more / for more directory levels.
      if {$OS eq "Windows" && [regexp {^[A-Za-z]\:/$} $path_elem ] && [regexp {/} $name]} {
           set name {}
           continue
      }
      break
    }
  }
  if {$name eq {} } {
    # no known lib, so return full path
    set name ${symbol}
    if {[file dirname $name] eq $curr_dirname} {
      # remove path if file is in pwd
      set name [file tail $name]
    }

  }

  return $name
}

# given a symbol reference 'sym' return its absolute path
# Example: % abs_sym_path devices/iopin.sch
#          /home/schippes/share/xschem/xschem_library/devices/iopin.sym
proc abs_sym_path {fname {ext {} } {paths {}}} {
  global pathlist OS

  if { $paths eq {}} {set paths $pathlist}
  set curr_dirname [pwd]
  ## empty: do nothing
  if {$fname eq {} } return {}
  ## add extension for 1.0 file format compatibility
  if { $ext ne {} } { 
    set fname [file rootname $fname]$ext
  }
  # web url: return as is
  if { [regexp {^https?://} $fname]} { 
     return "$fname"
  }
  if {$OS eq "Windows"} {
    ## absolute path: return as is
    if { [regexp {^[A-Za-z]\:/} $fname ] } {
      return "$fname"
    }
    # network drive, return as is
    if { [regexp {^/} $fname] } {
      return "$fname"
    }
  } else {
    ## absolute path: return as is
    if { [regexp {^/} $fname] } {
      return "$fname"
    }
  }
  ## replace all runs of multiple / with single / in fname
  regsub -all {/+} $fname {/} fname
  ## replace all '/./' with '/'
  while {[regsub {/\./} $fname {/} fname]} {}
  ## transform  a/b/../c to a/c or a/b/c/.. to a/b
  while {[regsub {([^/]*\.*[^./]+[^/]*)/\.\./?} $fname {} fname] } {}  
  ## remove trailing '/'  or '/.'
  while {[regsub {/\.?$} $fname {} fname]} {}
  ## 'found' set to 1 if fname begins with './' or  '../'
  set found 0 
  ## remove any leading './'
  while {[regsub {^\./} $fname {} fname]} {set found 1}
  ## if previous operation left fname empty set to '.'
  if { $fname eq {} } { set fname . }
  ## if fname is just "." return $curr_dirname
  if {[regexp {^\.$} $fname] } { return "$curr_dirname" }
  set tmpdirname $curr_dirname
  set tmpfname $fname
  ## if tmpfname begins with '../' remove this prefix and remove one path component from tmpdirname
  while { [regsub {^\.\./} $tmpfname {} tmpfname] } {
    set found 1
    set tmpdirname [file dirname $tmpdirname]
  }
  ## if tmpfname reduced to '..' return dirname of tmpdirname
  if { $tmpfname eq {..}} { return "[file dirname $tmpdirname]" }
  ## if given file begins with './' or '../' and dir or file exists relative to curr_dirname 
  ## just return it.
  if {$found } {
    set tmpfname "${tmpdirname}/$tmpfname"
    if { [file exists "$tmpfname"] } { return "$tmpfname" }
    ## if file does not exists but directory does return anyway
    if { [file exists [file dirname "$tmpfname"]] } { return "$tmpfname" }
  }
  ## if fname is present in one of the paths paths get the absolute path
  set name {}
  foreach path_elem $paths {
    if { ![string compare $path_elem .]  && [info exist curr_dirname]} {
      set path_elem $curr_dirname
    }
    regsub {/*$} $path_elem {/} path_elem
    set fullpath "${path_elem}${fname}"
    if { [file exists $fullpath] } {
      set name $fullpath
      break
    }
  }
  ## nothing found -> use current schematic directory
  if {$name eq {} } {
    set name "$curr_dirname/$fname"
  }
  return "$name"
}

proc add_ext {fname ext} {
  return [file rootname $fname]$ext
}

proc swap_compare_schematics {} {
  global compare_sch XSCHEM_TMP_DIR
  set sch1 [xschem get schname]
  set sch2 [xschem get sch_to_compare]
  if { $sch1 eq $sch2} {
    set msg "Swapping with same schematic name on disk. Suggest to cancel to avoid losing/overwriting data"
    set answer [tk_messageBox -message  $msg \
       -icon warning -parent [xschem get topwindow]  -type okcancel]
    if { $answer ne {ok} } {return} 
  }
  puts "swap_compare_schematics:\n  sch1=$sch1\n  sch2=$sch2"
  if {$sch2 ne {}} {
    xschem load -nofullzoom -gui $sch2
    set current  [xschem get schname]
    # Use "file tail" to handle equality of
    # https://raw.githubusercon...tb_reram.sch and /tmp/xschem_web/tb_reram.sch
    if {( [regexp "^${XSCHEM_TMP_DIR}/xschem_web_" $current] && [file tail $current] eq [file tail $sch2] ) || 
        ( $current eq $sch2 )} { ;# user did not cancel loading
      puts "swapping..."
      if {$compare_sch} {
        xschem compare_schematics $sch1
      } else {
        xschem set sch_to_compare $sch1
      }
    }
  }
}
proc input_line {txt {cmd {}} {preset {}}  {w 12}} {
  global wm_fix retval
  set retval $preset
  if { [winfo exists .dialog] } {return}
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Input number}
  wm transient .dialog [xschem get topwindow]
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "+$X+$Y"
  frame .dialog.f1
  label .dialog.f1.l -text $txt
  entry .dialog.f1.e -width $w
  entry_replace_selection .dialog.f1.e
  .dialog.f1.e insert 0 $preset
  .dialog.f1.e selection range 0 end
  
  pack .dialog.f1.l .dialog.f1.e -side left
  frame .dialog.f2

  button .dialog.f2.ok -text OK  -command  "
    if { {$cmd} ne {} } {
      eval $cmd \[.dialog.f1.e get\]
    }
    set retval \[.dialog.f1.e get\]
    destroy .dialog
  "
  button .dialog.f2.cancel -text Cancel -command { destroy .dialog }
  pack .dialog.f2.ok  -anchor w -side left
  pack .dialog.f2.cancel -anchor e
  pack .dialog.f1
  pack .dialog.f2 -expand yes -fill x
  bind .dialog <Escape> {.dialog.f2.cancel invoke}
  bind .dialog <Return> {.dialog.f2.ok invoke}
  tkwait visibility .dialog
  grab set .dialog
  focus .dialog.f1.e
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $retval
}

proc launcher {launcher_var {launcher_program {} } } {
  # env, XSCHEM_SHAREDIR and netlist_dir not used directly but useful in paths passed thru launcher_var
  global launcher_default_program env XSCHEM_SHAREDIR netlist_dir OS
  
  regsub {/$} $netlist_dir {} netlist_dir
  if { ![string compare $launcher_program {}] } { set launcher_program $launcher_default_program}
  if {$OS == "Windows"} {
    eval exec $launcher_program \"\" {[subst $launcher_var]}
  } else {
    eval exec  [subst $launcher_program] {[subst $launcher_var]} &
  }
}

proc reconfigure_layers_button { { topwin {} } } {
   global dark_colorscheme
   set c [xschem get rectcolor]
   $topwin.menubar entryconfigure Layers -background [lindex $tctx::colors $c]
   if { $dark_colorscheme == 1 && $c == 0} {
     $topwin.menubar entryconfigure Layers -foreground white
   } else {
     $topwin.menubar entryconfigure Layers -foreground black
   }
}

proc reconfigure_layers_menu { {topwin {} } } {
   global dark_colorscheme
   set j 0
   foreach i $tctx::colors {
     set ind_bg white
     if {  $j == [xschem get backlayer] } {
        if { $dark_colorscheme == 1 } { 
          set layfg white
        } else {
          set layfg black
        }
     } else {
        if { $dark_colorscheme == 1 } { 
          set layfg black
        } else {
          set layfg white
        }
     }
     $topwin.menubar.layers entryconfigure $j -activebackground $i \
        -background $i -foreground $layfg -activeforeground $layfg
     incr j
   }
   reconfigure_layers_button $topwin
}

proc get_file_path {ff} {
  global env OS
  # Absolute path ? return as is.
  #        Linux                Windows
  if { [regexp {^/} $ff] || [regexp {^[a-zA-Z]:} $ff] } { return $ff }
  if {$OS == "Windows"} {
    set mylist [split $env(PATH) \;]
  } else {
    set mylist [split $env(PATH) :]
  }
  foreach i $mylist {
    set ii $i/$ff
    if { [file exists $ii]} {return $ii}
  }
  # nothing found, return $ff as is and hope for the best :-)
  return $ff
}

#
# Balloon help system, from https://wiki.tcl-lang.org/page/balloon+help
#
proc balloon {w help {pos 1}} {
    bind $w <Any-Enter> "after 1000   [list balloon_show %W [list $help] $pos]"
    bind $w <Any-Leave> "after cancel [list balloon_show %W [list $help] $pos]; destroy %W.balloon"
    bind $w <FocusOut>  "after cancel [list balloon_show %W [list $help] $pos]; destroy %W.balloon"
}

### pos: 
## 0: set balloon close to mouse
## 1: set balloon below related widget
proc balloon_show {w arg pos} {
    if {[eval winfo containing  [winfo pointerxy .]]!=$w} {return}
    set top $w.balloon
    catch {destroy $top}
    toplevel $top -bd 1 -background black
    wm overrideredirect $top 1
    if {[string equal [tk windowingsystem] aqua]}  {
        ::tk::unsupported::MacWindowStyle style $top help none
    }   
    pack [message $top.txt -aspect 10000 -fg black -background lightyellow \
        -font fixed -text $arg]
    if { $pos } {
      set wmx [winfo rootx $w]
      set wmy [expr {[winfo rooty $w]+[winfo height $w]}]
    } else {
      set wmx [expr [winfo pointerx $w] + 20]
      set wmy [winfo pointery $w]
    }
    wm geometry $top [winfo reqwidth $top.txt]x[winfo reqheight $top.txt]+$wmx+$wmy
    raise $top
}

proc context_menu { } {
  global retval

  set retval 0
  if {[info tclversion] >= 8.5} {
    set font TkDefaultFont
    # set font {Sans 8 bold}
  } else {
    set font fixed
  }
  set selection  [expr {[xschem get lastsel] > 0}]
  toplevel .ctxmenu
  wm overrideredirect .ctxmenu 1
  set x [expr {[winfo pointerx .ctxmenu] - 10}]
  set y [expr {[winfo pointery .ctxmenu] - 10}]
  if { !$selection} {
    button .ctxmenu.b9 -text {Open most recent} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuRecent -compound left \
      -font [subst $font] -command {set retval 9; destroy .ctxmenu} 
  }
  button .ctxmenu.b10 -text {Edit attributes} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuEdit -compound left \
    -font [subst $font] -command {set retval 10; destroy .ctxmenu}
  button .ctxmenu.b11 -text {Edit attr in editor} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuEdit -compound left \
    -font [subst $font] -command {set retval 11; destroy .ctxmenu}
  if {$selection} {
    button .ctxmenu.b12 -text {Descend schematic} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuDown -compound left \
      -font [subst $font] -command {set retval 12; destroy .ctxmenu}
    button .ctxmenu.b13 -text {Descend symbol} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuDownSym -compound left \
      -font [subst $font] -command {set retval 13; destroy .ctxmenu}
    button .ctxmenu.b18 -text {Delete selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuDelete -compound left \
      -font [subst $font] -command {set retval 18; destroy .ctxmenu}
    button .ctxmenu.b7 -text {Cut selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuCut -compound left \
      -font [subst $font] -command {set retval 7; destroy .ctxmenu}
    button .ctxmenu.b15 -text {Copy selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuCopy -compound left \
      -font [subst $font] -command {set retval 15; destroy .ctxmenu}
    button .ctxmenu.b16 -text {Move selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuMove -compound left \
      -font [subst $font] -command {set retval 16; destroy .ctxmenu}
    button .ctxmenu.b17 -text {Duplicate selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuDuplicate -compound left \
      -font [subst $font] -command {set retval 17; destroy .ctxmenu}
    button .ctxmenu.b22 -text {Rotate selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuRotate -compound left \
      -font [subst $font] -command {xschem rotate; destroy .ctxmenu}
    button .ctxmenu.b23 -text {Flip selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuFlip -compound left \
      -font [subst $font] -command {xschem flip; destroy .ctxmenu}
    button .ctxmenu.b24 -text {Rotate in-place sel.} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuRotate -compound left \
      -font [subst $font] -command {xschem rotate_in_place; destroy .ctxmenu}
    button .ctxmenu.b25 -text {Flip in-place sel.} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuFlip -compound left \
      -font [subst $font] -command {xschem flip_in_place; destroy .ctxmenu}
  }
  if {!$selection} {
    button .ctxmenu.b14 -text {Go to upper level} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuUp -compound left \
      -font [subst $font] -command {set retval 14; destroy .ctxmenu}
    button .ctxmenu.b1 -text {Insert symbol} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuSymbol -compound left \
      -font [subst $font] -command {set retval 1; destroy .ctxmenu}
    button .ctxmenu.b2 -text {Insert wire} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuWire -compound left \
      -font [subst $font] -command {set retval 2; destroy .ctxmenu}
    button .ctxmenu.b3 -text {Insert line} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuLine -compound left \
      -font [subst $font] -command {set retval 3; destroy .ctxmenu}
    button .ctxmenu.b4 -text {Insert box} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuBox -compound left \
      -font [subst $font] -command {set retval 4; destroy .ctxmenu}
    button .ctxmenu.b5 -text {Insert polygon} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuPoly -compound left \
      -font [subst $font] -command {set retval 5; destroy .ctxmenu}
    button .ctxmenu.b19 -text {Insert arc} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuArc -compound left \
      -font [subst $font] -command {set retval 19; destroy .ctxmenu}
    button .ctxmenu.b20 -text {Insert circle} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuCircle -compound left \
      -font [subst $font] -command {set retval 20; destroy .ctxmenu}
    button .ctxmenu.b6 -text {Insert text} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuText -compound left \
      -font [subst $font] -command {set retval 6; destroy .ctxmenu}
    button .ctxmenu.b8 -text {Paste clipboard} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuPaste -compound left \
      -font [subst $font] -command {set retval 8; destroy .ctxmenu}
  }
  button .ctxmenu.b21 -text {Abort command} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuAbort -compound left \
    -font [subst $font] -command {set retval 21; destroy .ctxmenu}

  pack .ctxmenu.b21 -fill x -expand true
  if {!$selection} {
    pack .ctxmenu.b9  -fill x -expand true
  }
  pack .ctxmenu.b10 .ctxmenu.b11 -fill x -expand true
  if {$selection} {
    pack .ctxmenu.b12 .ctxmenu.b13 .ctxmenu.b18 -fill x -expand true
    pack .ctxmenu.b22 .ctxmenu.b23 -fill x -expand true
    pack .ctxmenu.b24 .ctxmenu.b25 -fill x -expand true
  }
  if {!$selection} {
    pack .ctxmenu.b14 -fill x -expand true
    pack .ctxmenu.b1 .ctxmenu.b2 .ctxmenu.b3 .ctxmenu.b4 .ctxmenu.b5 -fill x -expand true
    pack .ctxmenu.b19 .ctxmenu.b20 .ctxmenu.b6  -fill x -expand true
  }
  if {$selection} {
    pack .ctxmenu.b7 -fill x -expand true
    pack .ctxmenu.b15 .ctxmenu.b16 .ctxmenu.b17 -fill x -expand true
  }
  if {!$selection} {
    pack .ctxmenu.b8 -fill x -expand true
  }
  wm geometry .ctxmenu "+$x+$y"
  update
  # if window has been destroyed (by mouse pointer exiting) do nothing
  if { ![winfo exists .ctxmenu] } { return 0 }

  set wx [winfo width .ctxmenu]
  set wy [winfo height .ctxmenu]
  set sx [winfo screenwidth .]
  set sy [winfo screenheight .]
  if { $y + $wy > $sy } {
    set y [expr {$y - ( $y + $wy - $sy )} ]
  }
  if { $x + $wx > $sx } {
    set x [expr {$x - ( $x + $wx - $sx )} ]
  }
  wm geometry .ctxmenu "+$x+$y";# move away from screen edges
  bind .ctxmenu <Leave> {if { {%W} eq {.ctxmenu} } {destroy .ctxmenu}}
  tkwait window .ctxmenu
  # when context menu is destroyed an EnterNotify event is generated in the
  # main xschem window. We want to process this event before taking 
  # actions based on $retval.
  update
  return $retval
}

proc tab_ctx_cmd {tab_but what} {
  global terminal editor netlist_dir OS has_x
  regsub {/$} $netlist_dir {} netlist_dir
  # get win_path from tab name
  set win_path [lindex [$tab_but cget -command] 3] ;# xschem new_schematic switch .x1.drw
  set tablist [xschem tab_list]
  set found 0
  foreach {tabname filename} $tablist {
    if {$win_path eq $tabname} {
      set found 1
      break
    }
  }
  if {!$found} { set filename {}}
  if { $filename ne {} } {
    if {$what eq {dir}} {
      if {$OS == "Windows" } {
        set dir [file dirname $filename]
        set command [list {*}[auto_execok start] {}]
        exec {*}$command $dir &
      } else {
        execute 0 xdg-open [file dirname $filename]
      }
    } elseif {$what eq {copy}} {
      clipboard clear
      clipboard append $filename
    } elseif {$what eq {term}} {
      if {$OS == "Windows" } {
        set save [pwd]
        set dir [file dirname $filename]
        cd $dir
        set command [list {*}[auto_execok start] {}]
        exec {*}$command &
        cd $save
      } else {
        set save [pwd]
        cd [file dirname $filename]
        execute 0 $terminal
        cd $save
      }
    } elseif {$what eq {simterm}} {
      if {$OS == "Windows" } {
        set save [pwd]
        cd $netlist_dir
        set command [list {*}[auto_execok start] {}]
        exec {*}$command &
        cd $save
      } else {
        set save [pwd]
        cd $netlist_dir
        execute 0 $terminal
        cd $save
      }
    } elseif {$what eq {edit}} {
      eval execute 0 $editor $filename
    } elseif {$what eq {netlist}} {
      set old [xschem get current_win_path]
      set save [pwd]
      xschem new_schematic switch $win_path {} 0 ;# no draw
      if {[file exists $netlist_dir] && [file exists "$netlist_dir/[xschem get netlist_name fallback]"]} {
        cd $netlist_dir
        eval execute 0 $editor \"[xschem get netlist_name fallback]\"
        cd $save
      } else {
        if {[info exists has_x]} {
          alert_ {Netlist not existing, not yet generated} {} 0 0
        } else {
          puts {Netlist not existing, not yet generated}
        }
      }
      xschem new_schematic switch $old {} 0 ;# no draw
    } elseif {$what eq {save}} {
      set old [xschem get current_win_path]
      xschem new_schematic switch $win_path {} 0 ;# no draw
      xschem save
      xschem new_schematic switch $old {} 0 ;# no draw
    } elseif {$what eq {close}} {
      set old [xschem get current_win_path]
      set ntabs [xschem get ntabs]
      xschem new_schematic switch $win_path {} 0 ;# no draw
      set remaining_tabs [xschem exit]
      if { $old != $win_path || $remaining_tabs == $ntabs} {
        # don't switch if we were on the tab that has been closed.
        xschem new_schematic switch $old {} 1 ;# draw
      }
    } elseif {[regexp {^open } $what]} {
      set counterpart [lindex $what 1]
      set filetype  [lindex $what 2]

      # see if $counterpart already open, in this case switch to it
      set found 0
      foreach {tabname filename} $tablist {
        if {$filename eq $counterpart} {
          set found 1
          break
        }
      }
      if {$found} {
        xschem new_schematic switch $tabname {} 1 ;# draw
      } else {
        xschem new_schematic create {1} $counterpart
        if {$filetype ne {symbol} } { xschem set netlist_type symbol}
      }
    }
  }
  # puts $filename
}

proc tab_context_menu {tab_but} {
  global retval search_schematic

  # find filename associated with tab button
  set win_path [lindex [$tab_but cget -command] 3] ;# xschem new_schematic switch .x1.drw
  set tablist [xschem tab_list] ;# .drw filename0 .x1.drw filename1 ...
  set found 0
  foreach {tabname filename} $tablist {
    if {$win_path eq $tabname} {
      set found 1
      break
    }
  }
  if {!$found} { set filename {}}
  set old [xschem get current_win_path]
  xschem new_schematic switch $win_path {} 0 ;# no draw
  set filetype [xschem get netlist_type] ;# symbol or spice or vhdl or tedax or verilog
  xschem new_schematic switch $old {} 0 ;# no draw

  if {$filetype ne {symbol}} {
    if {$search_schematic == 1} {
      set counterpart [abs_sym_path [xschem get current_name] {.sym}]
    } else {
      set counterpart [abs_sym_path $filename .sym]
    }
    set msg {Open symbol}
    set img CtxmenuSymbol
  } else {
    if {$search_schematic == 1} {
      set counterpart [abs_sym_path [xschem get current_name] {.sch}]
    } else {
      set counterpart [abs_sym_path $filename .sch]
    }
    set msg {Open schematic}
    set img CtxmenuSchematic
  }
  # puts $counterpart

  # puts $tab_but
  set retval 0
  if {[info tclversion] >= 8.5} {
    set font {Sans 8 bold}
  } else {
    set font fixed
  }
  toplevel .ctxmenu
  wm overrideredirect .ctxmenu 1
  set x [expr {[winfo pointerx .ctxmenu] - 10}]
  set y [expr {[winfo pointery .ctxmenu] - 10}]
  button .ctxmenu.b0 -text {Tab menu} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -fg black -background white -state disabled -disabledforeground black -font [subst $font]
  button .ctxmenu.b1 -text {Copy filename} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuCopy -compound left \
    -font [subst $font] -command "set retval 1; tab_ctx_cmd $tab_but copy; destroy .ctxmenu"
  button .ctxmenu.b2 -text {Open directory} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuOpendir -compound left \
    -font [subst $font] -command "set retval 2; tab_ctx_cmd $tab_but dir; destroy .ctxmenu"
  button .ctxmenu.b3 -text {Open circuit dir. term.} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuTerm -compound left \
    -font [subst $font] -command "set retval 3; tab_ctx_cmd $tab_but term; destroy .ctxmenu"
  button .ctxmenu.b4 -text {Open sim. dir. term.} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuTerm -compound left \
    -font [subst $font] -command "set retval 4; tab_ctx_cmd $tab_but simterm; destroy .ctxmenu"
  if {$counterpart ne {}} {
    button .ctxmenu.b6 -text $msg -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image $img -compound left \
      -font [subst $font] \
      -command "set retval 6; tab_ctx_cmd $tab_but {open {$counterpart} $filetype} ; destroy .ctxmenu"
  }
  if {$filetype ne {symbol}} {
    button .ctxmenu.b5 -text {Edit netlist} -padx 3 -pady 0 -anchor w -activebackground grey50 \
       -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuEdit -compound left \
      -font [subst $font] -command "set retval 5; tab_ctx_cmd $tab_but netlist; destroy .ctxmenu"
  }
  button .ctxmenu.b7 -text {Save} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuSave -compound left \
    -font [subst $font] -command "set retval 7; tab_ctx_cmd $tab_but save; destroy .ctxmenu"
  button .ctxmenu.b8 -text {Close tab} -padx 3 -pady 0 -anchor w -activebackground grey50 \
     -bg {#d9d9d9} -fg black -activeforeground black -image CtxmenuDelete -compound left \
    -font [subst $font] -command "set retval 8; tab_ctx_cmd $tab_but close; destroy .ctxmenu"

  pack .ctxmenu.b0 -fill x -expand true
  pack .ctxmenu.b1 -fill x -expand true
  pack .ctxmenu.b2 -fill x -expand true
  pack .ctxmenu.b3 -fill x -expand true
  pack .ctxmenu.b4 -fill x -expand true
  if {$counterpart ne {}} {
    pack .ctxmenu.b6 -fill x -expand true
  }
  if {$filetype ne {symbol}} {
    pack .ctxmenu.b5 -fill x -expand true
  }
  pack .ctxmenu.b7 -fill x -expand true
  pack .ctxmenu.b8 -fill x -expand true
  wm geometry .ctxmenu "+$x+$y"
  update
  # if window has been destroyed (by mouse pointer exiting) do nothing
  if { ![winfo exists .ctxmenu] } { return 0 }

  set wx [winfo width .ctxmenu]
  set wy [winfo height .ctxmenu]
  set sx [winfo screenwidth .]
  set sy [winfo screenheight .]
  if { $y + $wy > $sy } {
    set y [expr {$y - ( $y + $wy - $sy )} ]
  }
  if { $x + $wx > $sx } {
    set x [expr {$x - ( $x + $wx - $sx )} ]
  }
  wm geometry .ctxmenu "+$x+$y";# move away from screen edges
  bind .ctxmenu <Leave> {if { {%W} eq {.ctxmenu} } {destroy .ctxmenu}}
  tkwait window .ctxmenu
  return $retval
}

#
# toolbar: Public variables that we allow to be overridden
# Code contributed by Neil Johnson (github: nejohnson)
#
proc setup_toolbar {} {
  global toolbar_visible toolbar_horiz toolbar_list XSCHEM_SHAREDIR dark_gui_colorscheme
  global toolbar_icon_zoom ctxmenu_icon_zoom
  set_ne toolbar_visible 1
  set_ne toolbar_horiz   1
  set_ne toolbar_list { 
    FileOpen
    FileSave
    FileReload
    ---
    EditUndo
    EditRedo
    EditCut
    EditCopy
    EditPaste
    EditDelete
    ---
    EditDuplicate
    EditMove
    ---
    EditPushSch
    EditPushSym
    EditPop
    ---
    ViewRedraw
    ViewToggleColors
    ---
    ToolInsertSymbol
    ToolInsertText
    ToolInsertWire
    ToolInsertLine
    ToolInsertRect
    ToolInsertPolygon
    ToolInsertArc
    ToolInsertCircle
    ---
    ToolSearch
    ---
    ToolJoinTrim
    ToolBreak
  }
  #
  # Pull in the toolbar graphics resources
  #
  source $XSCHEM_SHAREDIR/resources.tcl
  #
  # Separation bar counter
  #
}

#
# Create a tool button which may be displayed
#
proc toolbar_add {name cmd { help "" } {topwin {} } } {
    global dark_gui_colorscheme toolbar_icon_zoom

    set toolbar_icon_size [expr {$toolbar_icon_zoom * 24}]
    if { $dark_gui_colorscheme ==1} {
      set bg black
    } else {
      set bg white
    }
    if {![winfo exists  $topwin.toolbar]} {
       frame $topwin.toolbar -relief raised -bd 0 -background $bg 
    }
    if { ![winfo exists $topwin.toolbar.b$name]} {
      button $topwin.toolbar.b$name -image img$name -relief flat -bd 0 \
      -background $bg -fg $bg -height $toolbar_icon_size  -padx 0 -pady 0 -command $cmd
      if { $help == "" } { balloon $topwin.toolbar.b$name $name } else { balloon $topwin.toolbar.b$name $help }
    }
}

#
# Show the toolbar in horizontal or vertical position, parsing the toolbar list and 
# adding any separators as needed.
#
proc toolbar_show { { topwin {} } } {
    global toolbar_horiz toolbar_list toolbar_visible tabbed_interface

    # puts "toolbar_show: $topwin"
    set toolbar_visible 1
    if {![winfo exists  $topwin.toolbar]} {
       frame $topwin.toolbar -relief raised -bd 0 -background white 
    }
    if {[winfo ismapped $topwin.toolbar]} {return}
    if { $toolbar_horiz } { 
        if {$tabbed_interface} {
          pack $topwin.toolbar -fill x -before $topwin.tabs 
        } else {
          pack $topwin.toolbar -fill x -before $topwin.drw
        }
    } else {
        pack $topwin.toolbar -side left -fill y -before $topwin.drw
    }
    set pos "top"
    if { $toolbar_horiz } { set pos "left" }
    set tlist [ winfo children $topwin.toolbar ]
    set toolbar_sepn 0
    foreach b $toolbar_list {
        if { $b == "---" } {
            if { $toolbar_horiz } {
                frame $topwin.toolbar.sep$toolbar_sepn -background lightgrey -width 2
                pack $topwin.toolbar.sep$toolbar_sepn -side $pos -padx 1 -pady 0 -fill y
            } else {
                frame $topwin.toolbar.sep$toolbar_sepn -background lightgrey -height 2
                pack $topwin.toolbar.sep$toolbar_sepn -side $pos -padx 0 -pady 1 -fill x
            }
            incr toolbar_sepn
        } else {
            if { [ lsearch -exact $tlist "$topwin.toolbar.b$b" ] != -1 } {
                pack $topwin.toolbar.b$b -side $pos
            } else {
                puts "Error: unknown toolbar item \"$b\""
            }
        }
    }
    set pos "bottom"
    if { $toolbar_horiz } { set pos "right" }
    foreach b { Waves Simulate Netlist } {
        pack $topwin.toolbar.b$b -side $pos
    }
}

#
# Hide the toolbar, unpack the buttons, and remove any separators
#
proc toolbar_hide { { topwin {} } } {
    global toolbar_visible
    set toolbar_visible 0
    if {![winfo exists $topwin.toolbar]} {return}
    if {![winfo ismapped $topwin.toolbar]} {return}
    set tlist [ winfo children $topwin.toolbar ]
    foreach b $tlist {
        pack forget $b
        if { [ string match "$topwin.toolbar.sep*" $b ] == 1 } { 
            destroy $b
        }
    }
    pack forget $topwin.toolbar
    set toolbar_visible 0
}


proc add_toolbuttons {{topwin {}}} {
  # toolbar_add FileNew {xschem clear schematic} "New Schematic" $topwin
  # toolbar_add FileNewSym {xschem clear symbol} "New Symbol" $topwin
  toolbar_add FileOpen "xschem load" "Open File" $topwin
  toolbar_add FileSave "xschem save" "Save File" $topwin
  # toolbar_add FileMerge "xschem merge" "Merge File" $topwin
  toolbar_add FileReload {
      if {[alert_ "Are you sure you want to reload?" {} 0 1] == 1} {
        xschem reload
      }
    } "Reload File" $topwin
  toolbar_add EditUndo "xschem undo; xschem redraw" "Undo" $topwin
  toolbar_add EditRedo "xschem redo; xschem redraw" "Redo" $topwin
  toolbar_add EditCut "xschem cut" "Cut" $topwin
  toolbar_add EditCopy "xschem copy" "Copy" $topwin
  toolbar_add EditPaste "xschem paste" "Paste" $topwin
  toolbar_add EditDelete "xschem delete" "Delete" $topwin
  toolbar_add EditDuplicate "xschem copy_objects" "Duplicate objects" $topwin
  toolbar_add EditMove "xschem move_objects" "Move objects" $topwin
  toolbar_add EditPushSch "xschem descend" "Push schematic" $topwin
  toolbar_add EditPushSym "xschem descend_symbol" "Push symbol" $topwin
  toolbar_add EditPop "xschem go_back" "Pop" $topwin
  toolbar_add ViewRedraw "xschem redraw" "Redraw" $topwin
  # toolbar_add ViewZoomIn "xschem zoom_in" "Zoom In" $topwin
  # toolbar_add ViewZoomOut "xschem zoom_out" "Zoom Out" $topwin
  # toolbar_add ViewZoomBox "xschem zoom_box" "Zoom Box" $topwin
  toolbar_add ViewToggleColors {
     xschem toggle_colorscheme
  } "Toggle Color Scheme" $topwin
  toolbar_add ToolInsertSymbol "xschem place_symbol" "Insert Symbol" $topwin
  toolbar_add ToolInsertText "xschem place_text" "Insert Text" $topwin
  toolbar_add ToolInsertWire "xschem wire" "Insert Wire" $topwin
  toolbar_add ToolInsertLine "xschem line" "Insert Line" $topwin
  toolbar_add ToolInsertRect "xschem rect" "Insert Rectangle" $topwin
  toolbar_add ToolInsertPolygon "xschem polygon" "Insert Polygon" $topwin
  toolbar_add ToolInsertArc "xschem arc" "Insert Arc" $topwin
  toolbar_add ToolInsertCircle "xschem circle" "Insert Circle" $topwin
  toolbar_add ToolSearch property_search "Search" $topwin
  toolbar_add ToolJoinTrim "xschem trim_wires" "Join/Trim Wires" $topwin
  toolbar_add ToolBreak "xschem break_wires" "Break wires at selected\ninstance pin intersections" $topwin
  toolbar_add Netlist { xschem netlist -erc } "Create netlist" $topwin
  toolbar_add Simulate "simulate_from_button" "Run simulation" $topwin
  toolbar_add Waves { waves } "View results" $topwin
}

proc pack_tabs {} {
  global toolbar_horiz
  if {[winfo exists .toolbar] && [winfo ismapped .toolbar] } {
    if { $toolbar_horiz == 1 } {
      pack .tabs  -fill x -side top -after .toolbar
    } else {
      pack .tabs  -fill x -side top -before .toolbar
    }
  } else {
    pack .tabs  -fill x -side top -before .drw
  }
}

proc setup_tabbed_interface {} {
  global tabbed_interface toolbar_horiz has_x

  if { $tabbed_interface } {
    if { [info exists has_x] && ![winfo exists .tabs] } {
      frame .tabs
      button .tabs.x0 -padx 2 -pady 0 -anchor nw -takefocus 0 \
          -text Main -command "xschem new_schematic switch .drw"
      bind .tabs.x0 <ButtonPress-3> {tab_context_menu %W}
      bind .tabs.x0 <ButtonPress-1> {swap_tabs %X %Y press}
      bind .tabs.x0 <ButtonRelease-1> {swap_tabs %X %Y release}
      button .tabs.add -padx 0 -pady 0  -takefocus 0 -text { + } \
          -command "xschem new_schematic create"
      pack .tabs.x0 .tabs.add -side left
      balloon .tabs.add {Create a new tab}
      pack_tabs
    }
  } else {
    destroy .tabs
  }
  if {$tabbed_interface} {
    set_tab_names 
  }
  # update tabbed window close (X) function
  if {$tabbed_interface} {
    wm protocol . WM_DELETE_WINDOW { 
      xschem exit closewindow
    }
  # restore non tabbed window close function for main window
  } else {
    wm protocol . WM_DELETE_WINDOW {
      set old  [xschem get current_win_path]
      save_ctx $old
      restore_ctx .drw
      housekeeping_ctx
      xschem new_schematic switch .drw
      xschem exit closewindow
      # did not exit (user cancel) ... switch back 
      restore_ctx $old
      housekeeping_ctx
      xschem new_schematic switch $old
    }
  }
}

proc delete_tab {path} {
  regsub {\.drw$} $path {} path
  destroy .tabs$path
}

# button press on a tab, drag onto another tab, release button --> swap
proc swap_tabs {x y what} {
  if {$what eq {press} } {
    # puts "From: [winfo containing $x $y]"
    set tctx::source_swap_tab [winfo containing $x $y]
  } else {
    # puts "To: [winfo containing $x $y]"
    set tctx::dest_swap_tab [winfo containing $x $y]
    if {[info exists tctx::source_swap_tab] && [info exists tctx::dest_swap_tab]} {
      set cond1 [regexp {\.tabs\.x} $tctx::source_swap_tab]
      set cond2 [regexp {\.tabs\.x} $tctx::dest_swap_tab]
      set cond3 [expr { $tctx::source_swap_tab ne $tctx::dest_swap_tab }]
      if { $cond1 && $cond2  && $cond3} {
        # puts "ok for swapping ctx"
        set tablist [pack slaves .tabs]
        set sourceidx [lsearch -exact $tablist $tctx::source_swap_tab]
        set destidx [lsearch -exact $tablist $tctx::dest_swap_tab]
        incr sourceidx
        incr destidx
        set following_source_tab [lindex $tablist $sourceidx]
        set following_dest_tab [lindex $tablist $destidx]
        # puts " $tablist  --> $following_source_tab   $following_dest_tab"
        pack $tctx::source_swap_tab -side left -before $following_dest_tab
        pack $tctx::dest_swap_tab -side left -before  $following_source_tab
      }
      set tctx::source_swap_tab {}
      set tctx::dest_swap_tab {}
    }
  }
}

proc prev_tab {} {
  global tabbed_interface
  if { !$tabbed_interface} { return}
  set currwin [xschem get current_win_path]
  regsub {\.drw} $currwin {} tabname
  if {$tabname eq {}} { set tabname .x0}
  regsub {\.x} $tabname {} number
  set next_tab $number
  set highest -10000000
  set xcoord [winfo rootx .tabs$tabname]
  for {set i 0} {$i < $tctx::max_new_windows} { incr i} {
    if { $i == $number} { continue}
    if { [winfo exists .tabs.x$i] } {
      set tab_coord  [winfo rootx .tabs.x$i]
      if {$tab_coord < $xcoord && $tab_coord > $highest} {
        set next_tab $i
        set highest $tab_coord
      }
    }
  }
  .tabs.x$next_tab invoke
}

proc next_tab {} {
  global tabbed_interface
  if { !$tabbed_interface} {return}
  set currwin [xschem get current_win_path]
  regsub {\.drw} $currwin {} tabname
  if {$tabname eq {}} { set tabname .x0}
  regsub {\.x} $tabname {} number
  set next_tab $number
  set lowest 10000000
  set xcoord [winfo rootx .tabs$tabname]
  for {set i 0} {$i < $tctx::max_new_windows} { incr i} {
    if { $i == $number} { continue}
    if { [winfo exists .tabs.x$i] } {
      set tab_coord [winfo rootx .tabs.x$i]
      if {$tab_coord > $xcoord && $tab_coord < $lowest} {
        set next_tab $i
        set lowest $tab_coord
      }
    }
  }
  .tabs.x$next_tab invoke
}

proc set_tab_names {{mod {}}} {
  global tabbed_interface has_x dark_gui_colorscheme

  if {$dark_gui_colorscheme == 0} { 
    set tab_color Palegreen
  } else {
     set tab_color DarkGreen
  }

  if {[info exists has_x] && $tabbed_interface } {
    set currwin [xschem get current_win_path]
    set currsch [xschem get schname]
    regsub {\.drw} $currwin {} tabname
    if {$tabname eq {}} { set tabname .x0}
    .tabs$tabname configure -text [file tail $currsch]$mod -background $tab_color
    # puts ".tabs$tabname --> name=[file tail $currsch]$mod"
    balloon .tabs$tabname $currsch
    for { set i 0} { $i < $tctx::max_new_windows} { incr i} {
      if { [winfo exists .tabs.x$i] && ($tabname ne ".x$i")} {
         .tabs.x$i configure -background $tctx::tab_bg
      }
    }
  }
}

proc quit_xschem { {force {}}} {
  global tabbed_interface

  xschem new_schematic destroy_all $force
  xschem new_schematic switch .drw
  set remaining [xschem exit closewindow $force]
  return $remaining
}

proc raise_dialog {parent window_path } {
  global file_dialog_loadfile component_browser_on_top
  foreach i ".dialog .graphdialog .load" {
    if {!$component_browser_on_top && [info exists file_dialog_loadfile ] &&
        $file_dialog_loadfile == 2 && $i eq {.load} } {
      continue
    }
    if {[winfo exists $i] && [winfo ismapped $i] && [winfo ismapped $parent] &&
        [wm stackorder $i isbelow $parent ]} {
      raise $i $window_path
    }
  }
}

proc set_old_tk_fonts {} {
  if {[info tclversion] <= 8.4} {
     set myfont {-*-helvetica-*-r-*-*-12-*-*-*-*-*-*-*}
     set mymonofont fixed
     option add *Button*font $myfont startupFile
     option add *Menubutton*font $myfont startupFile
     option add *Menu*font $myfont startupFile
     option add *Listbox*font $myfont startupFile
     option add *Entry*font $mymonofont startupFile
     option add *Text*font $mymonofont startupFile
     option add *Label*font $myfont startupFile
  }
}

proc every {interval script} {
    uplevel #0 $script
    after $interval [list every $interval $script]
}

## tcl context switching global namespace
namespace eval tctx {
  variable tctx
  variable i
  variable global_list
  variable global_array_list
  variable dialog_list
  variable tab_bg
  variable max_new_windows
  variable source_swap_tab
  variable dest_swap_tab
  variable colors
}

## list of dialogs: when open do not perform context switching
## do not include .infotext as it is always open (in withdrawn mode)
set tctx::dialog_list { .ctxmenu .alert .sim .dialog .tclcmd .sl .dim }
 
proc no_open_dialogs {} {
  set res 1
  foreach tctx::i $tctx::dialog_list {
    if { [winfo exists $tctx::i] } { 
      # puts "$tctx::i dialog open"
      set res 0
    }
  }
  return $res
}

## list of globals to save/restore on context switching
## EXCEPTIONS, not to be saved/restored:
## "textwindow_wcounter" should be kept unique as it is the number of open textwindows
## "viewdata_wcounter" should be kept unique as it is the number of open viewdatas
## "measure_id" should be kept unique since we allow only one measure tooltip in graphs
## "tabbed_interface"
## "case_insensitive" case insensitive symbol lookup (on case insensitive filesystems only!)
## "dark_colors_save"
## "light_colors_save" restore default colors
## "menu_debug_var" there is only a global debug mode
## "debug_var" there is only a global debug mode
## "xschem_server_getdata" only one tcp listener per process
## "bespice_server_getdata" only one tcp listener per process
## "file_dialog_*" only one load_file_dialog window is allowed
## new_symbol_browser_*
## new_file_browser_*

set tctx::global_list {
 INITIALINSTDIR INITIALLOADDIR INITIALPROPDIR INITIALTEXTDIR PDK PDK_ROOT SKYWATER_MODELS
 SKYWATER_STDCELLS XSCHEM_LIBRARY_PATH add_all_windows_drives auto_hilight
 auto_hilight_graph_nodes autofocus_mainwindow autotrim_wires bespice_listen_port big_grid_points
 bus_replacement_char cadence_compat cadgrid cadlayers cadsnap cairo_font_name cairo_font_scale
 change_lw color_ps compare_sch constr_mv copy_cell crosshair_layer crosshair_size cursor_2_hook
 custom_label_prefix custom_token dark_colors dark_colorscheme dark_gui_colorscheme delay_flag
 dim_bg dim_value disable_unique_names do_all_inst draw_crosshair draw_grid draw_grid_axes
 draw_window edit_prop_pos edit_prop_size edit_symbol_prop_new_sel editprop_sympath
 en_hilight_conn_inst enable_dim_bg enable_stretch enter_text_default_geometry filetmp
 fix_broken_tiled_fill flat_netlist fullscreen gaw_fd gaw_tcp_address graph_autoload graph_bus
 graph_change_done graph_dialog_default_geometry graph_digital graph_legend graph_linewidth_mult
 graph_logx graph_logy graph_private_cursor graph_rainbow graph_schname graph_sel_color
 graph_sel_wave graph_selected graph_sort graph_unlocked graph_use_ctrl_key hide_empty_graphs
 hide_symbols incr_hilight incremental_select infix_interface infowindow_text intuitive_interface
 keep_symbols launcher_default_program light_colors line_width live_cursor2_backannotate
 local_netlist_dir lvs_ignore lvs_netlist measure_text netlist_dir netlist_show netlist_type
 no_ask_save no_change_attrs nolist_libs noprint_libs old_selected_tok only_probes
 orthogonal_wiring path pathlist persistent_command preserve_unchanged_attrs prev_symbol ps_colors
 ps_paper_size rainbow_colors recentfile retval retval_orig rotated_text search_case search_exact
 search_found search_schematic search_select search_value select_touch selected_tok
 show_hidden_texts show_infowindow show_infowindow_after_netlist simconf_default_geometry
 simconf_vpos simulate_bg snap_cursor snap_cursor_size spiceprefix split_files svg_colors
 svg_font_name sym_txt symbol symbol_width tabstop tclcmd_txt tclstop tctx::colors tctx::hsize
 tctx::rcode tctx::vsize text_line_default_geometry text_replace_selection text_tabs_setting
 textwindow_fileid textwindow_filename textwindow_w toolbar_horiz toolbar_list toolbar_visible
 top_is_subckt transparent_svg undo_type unselect_partial_sel_wires uppercase_subckt
 use_cursor_for_selection use_lab_wire use_label_prefix use_tclreadline user_wants_copy_cell
 verilog_2001 verilog_bitblast
 viewdata_fileid viewdata_filename viewdata_w xschem_libs xschem_listen_port zoom_full_center
}

## list of global arrays to save/restore on context switching
## will be saved as tctx::.drw_dircolor, tctx::.x1.drw_dircolor and so on
## EXCEPTIONS, not to be saved/restored:
## execute
set tctx::global_array_list {
  replace_key dircolor sim enable_layer ngspice::ngspice_data
}

proc delete_ctx {context} {
  global has_x
  if {![info exists has_x]} {return}
  set tctx::tctx $context
  uplevel #0 {
    # puts "delete_ctx $tctx::tctx"
    array unset $tctx::tctx
    foreach tctx::i $tctx::global_array_list {
      regsub {::} $tctx::i {__} arr_name
      if { [array exists ${tctx::tctx}_$arr_name] } {
        array unset ${tctx::tctx}_$tctx::i
      }
    }
  }
}

proc restore_ctx {context} {
  global has_x
  if {![info exists has_x]} {return}
  # puts "restoring tcl context $context : semaphore=[xschem get semaphore]"
  set tctx::tctx $context
  array unset ::sim
  uplevel #0 {
    if { [ array exists tctx::$tctx::tctx ] } {
      # puts "restore_ctx tctx::$tctx::tctx"
      ## Cleanup these vars to avoid side effects from previous ctx
      unset -nocomplain gaw_fd
      foreach tctx::i $tctx::global_list {
        if { [info exists [subst tctx::$tctx::tctx]($tctx::i)] } {
          # puts "restoring:  $tctx::i"
          set $tctx::i [set [subst tctx::$tctx::tctx]($tctx::i)]
        }
      }
    }
    foreach tctx::i $tctx::global_array_list {
      unset -nocomplain $tctx::i
      regsub {::} $tctx::i {__} arr_name
      if { [array exists tctx::${tctx::tctx}_$arr_name] } {
        array set $tctx::i [array get [subst tctx::${tctx::tctx}_$arr_name]]
      }
    }
  }
}


# context is saved in array variable in tctx:: namespace.
# to get list of array names: 
#   array names  tctx::.drw
#   array names  tctx::.x1.drw
#   .....
proc save_ctx {context} {
  global has_x
  if {![info exists has_x]} {return}
  # puts "saving tcl context $context : semaphore=[xschem get semaphore]"
  set tctx::tctx $context
  uplevel #0 {
    # puts "save_ctx $tctx::tctx"
    foreach tctx::i $tctx::global_list {
      if { [info exists $tctx::i] } {
        # puts "saving:  $tctx::i"
        set [subst tctx::$tctx::tctx]($tctx::i) [set $tctx::i]
      }
    }
    foreach tctx::i $tctx::global_array_list {
      if { [array exists $tctx::i] } {
        regsub {::} $tctx::i {__} arr_name
        array set [subst tctx::${tctx::tctx}_$arr_name] [array get $tctx::i]
      }
    }
  }
}

proc housekeeping_ctx {} {
  global has_x simulate_bg show_hidden_texts case_insensitive draw_window hide_symbols
  global netlist_type intuitive_interface
  if {![info exists has_x]} {return}
  uplevel #0 {
  }
  # puts "housekeeping_ctx, path: [xschem get current_win_path]"
  xschem set hide_symbols $hide_symbols
  xschem set draw_window $draw_window
  xschem set intuitive_interface  $intuitive_interface
  xschem case_insensitive $case_insensitive
  set_sim_netlist_waves_buttons 
  .statusbar.7 configure -text $netlist_type
}

# callback that resets simulate button color at end of simulation
proc set_simulate_button {top_path win_path} {
  global simulate_bg execute has_x

  if {![info exists has_x]} return
  set current_win [xschem get current_win_path]
  set simvar tctx::${win_path}_simulate
  set sim_button $top_path.menubar

  # puts "current_win=|$current_win|"
  # puts "simvar=|$simvar|"
  # puts "win_path=|$win_path|"
  # puts "top_path=|$top_path|"
  # puts "sim_button=|$sim_button|"
  # puts "execute(exitcode,last)=|$execute(exitcode,last)|"

  if {![info exists execute(exitcode,last)]} {
    if { $current_win eq $win_path} {
      $sim_button entryconfigure Simulate -background $simulate_bg
    }
    set $simvar $simulate_bg
  } elseif { $execute(exitcode,last) == 0} {
    if { $current_win eq $win_path} {
      $sim_button entryconfigure Simulate -background Green
    }
    set $simvar Green
  } else {   
    if { $current_win eq $win_path} {
      $sim_button entryconfigure Simulate -background red
    }
    set $simvar red
  }
}


# set simulate and netlist buttons on context change
proc set_sim_netlist_waves_buttons {} {
  global simulate_bg execute has_x

  if {![info exists has_x]} {return}
  set win_path [xschem get current_win_path]
  set top_path [xschem get top_path]

  set netlist_var tctx::${win_path}_netlist
  set sim_var  tctx::${win_path}_simulate
  set waves_var tctx::${win_path}_waves
  if {![info exists $netlist_var] || [set $netlist_var] eq $simulate_bg} {
    $top_path.menubar entryconfigure Netlist -background  $simulate_bg
  } else { 
     $top_path.menubar entryconfigure Netlist -background [set $netlist_var]
  }

  if {![info exists $sim_var] || [set $sim_var] eq $simulate_bg} {
    $top_path.menubar entryconfigure Simulate -background  $simulate_bg
  } else { 
    $top_path.menubar entryconfigure Simulate -background [set $sim_var]
  }

  if {![info exists $waves_var] || [set $waves_var] eq $simulate_bg} {
    $top_path.menubar entryconfigure Waves -background  $simulate_bg
  } else { 
    $top_path.menubar entryconfigure Waves -background [set $waves_var]
  }
}

# these two routines are workarounds for broken remote desktop connection tools
# that do not correctly return mouse coordinates (%x, %y) on KeyPress events
proc getmousex {win} {
  set x [winfo pointerx $win]     ;# absolute mouse position in root window
  set originx [winfo rootx $win]  ;# absolute position of left window corner in root window
  set relx [expr {$x - $originx}] ;# mouse position relative to window origin
  return $relx
}
proc getmousey {win} {
  set y [winfo pointery $win]     ;# absolute mouse position in root window
  set originy [winfo rooty $win]  ;# absolute position of top window corner in root window
  set rely [expr {$y - $originy}] ;# mouse position relative to window origin
  return $rely
}

proc switch_window {parent topwin event window} {
  # puts "$parent $topwin $event $window"
  raise_dialog $parent $topwin
  
  if { $parent eq {.}} {
    if { $window eq $parent} {
      xschem callback .drw $event 0 0 0 0 0 0
    }
  } else {
    if {$window eq $parent} {
      # send a fake event just to force context switching in callback()
      xschem callback $parent.drw $event 0 0 0 0 0 0
    }
  }
}
proc show_bindkeys {} {
        global dark_colorscheme dim_value dim_bg
        if {[winfo exists .bk]} {wm deiconify .bk; raise .bk; return}
        toplevel .bk -bg {}
        # wm attributes .bk -fullscreen 1
        wm geometry .bk [winfo width .]x[winfo height .]+[winfo rootx .]+[expr {[winfo rooty .]+30}]

        # set save $dark_colorscheme
        # set dark_colorscheme 0
        # set dim_value 0.0
        # set dim_bg 0.0
        # xschem build_colors

        xschem preview_window create .bk
        xschem preview_window draw .bk [abs_sym_path bindkeys_cheatsheet.sym]
   
        # set dark_colorscheme $save
        # xschem build_colors

        bind .bk <ButtonPress> {xschem preview_window destroy .bk}
        bind .bk <KeyPress> {xschem preview_window destroy .bk}
        bind .bk <Expose> {xschem preview_window draw .bk }
        wm protocol .bk WM_DELETE_WINDOW {xschem preview_window destroy .bk}
}

proc set_bindings {topwin} {
global env has_x OS autofocus_mainwindow
  ###
  ### Tk event handling
  ###

  if {($OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
    set parent [winfo toplevel $topwin]
    # puts "set_binding: topwin=$topwin, parent=$parent"
  
    bind $parent <Expose> "if {{%W} eq {$parent}} {raise_dialog $parent $topwin}"
    bind $parent <Visibility> [list raise_dialog $parent $topwin]

    # Context switch.
    bind $parent <FocusIn> "
      focus $topwin ;# propagate focus to canvas
      switch_window $parent $topwin %T %W
    "

    bind $topwin <Leave> "
      if {{%W} eq {$topwin}} {
        xschem callback %W %T %x %y 0 0 0 %s
        graph_show_measure stop
      }
    "

    bind $topwin <Expose> "if {{%W} eq {$topwin}} {xschem callback %W %T %x %y 0 %w %h %s}"

    # transform mousewheel events into button4/5 events
    if {[info tclversion] > 8.7} {
      bind $topwin <MouseWheel> {
        if {%D > 0} {
          xschem callback %W 4 %x %y 0 4 0 %s
        } else {
          xschem callback %W 4 %x %y 0 5 0 %s
        }
      }
    }
 
    bind $topwin <Double-Button-1> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Double-Button-2> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Double-Button-3> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Configure> "xschem callback %W %T %x %y 0 %w %h 0"
    if {$autofocus_mainwindow} {
      bind $topwin <ButtonPress> "focus $topwin; xschem callback %W %T %x %y 0 %b 0 %s"
    } else {
      bind $topwin <ButtonPress> "xschem callback %W %T %x %y 0 %b 0 %s"
    }
    bind $topwin <ButtonRelease> "xschem callback %W %T %x %y 0 %b 0 %s"

    #### test: show keybindings
    bind $topwin <KeyPress> "
      if {{%K} eq {Escape}} { destroy .ctxmenu }
      xschem callback %W %T %x %y %N 0 0 %s"
    bind $topwin <KeyRelease> "xschem callback %W %T %x %y %N 0 0 %s"
    if {$autofocus_mainwindow} {
      bind $topwin <Motion> "focus $topwin; xschem callback %W %T %x %y 0 0 0 %s"
      bind $topwin <Enter> "
        if {{%W} eq {$topwin}} {
          destroy .ctxmenu
          focus $topwin
          xschem callback %W %T %x %y 0 0 0 0
        }
      "
    } else {
      bind $topwin <Motion> "xschem callback %W %T %x %y 0 0 0 %s"
      bind $topwin <Enter> "
        if {{%W} eq {$topwin}} {
          destroy .ctxmenu
          xschem callback %W %T %x %y 0 0 0 0
        }
      "
    }
    bind $topwin <Unmap> " wm withdraw .infotext; set show_infowindow 0 "
  
    # on Windows Alt key mask is reported as 131072 (1<<17) so build masks manually with values passed from C code 
    if {$OS == "Windows" } {
      bind $topwin <Mod4-KeyPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$Mod4Mask}]}
      bind $topwin <Shift-Mod4-KeyPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$ShiftMask + $Mod4Mask}]}
      bind $topwin <Control-Mod4-KeyPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$ControlMask + $Mod4Mask}]}
      bind $topwin <Alt-Mod4-KeyPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$Mod1Mask + $Mod4Mask}]}
      bind $topwin <Mod4-KeyPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$Mod4Mask}]}
      bind $topwin <Alt-ButtonPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$Mod1Mask}]}
      bind $topwin <Control-Alt-ButtonPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$ControlMask + $Mod1Mask}]}
      bind $topwin <Shift-Alt-ButtonPress> {xschem callback %W %T %x %y 0 %b 0 [expr {$ShiftMask + $Mod1Mask}]}
      bind $topwin <Alt-Motion> {xschem callback %W %T %x %y 0 0 0 [expr {$Mod1Mask+%s}]}
      bind $topwin <Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$Mod1Mask}]}
      bind $topwin <Control-Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$ControlMask + $Mod1Mask}]}
      bind $topwin <Shift-Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$ShiftMask + $Mod1Mask}]}
      bind $topwin <Shift-Insert> {xschem callback %W %T %x %y %N 0 0 [expr {$ShiftMask}]}
      bind $topwin <MouseWheel> { ;# transform MouseWheel into button 4/5 presses.
        if {%D<0} {
          xschem callback %W 4 %x %y 0 5 0 %s
        } else {
          xschem callback %W 4 %x %y 0 4 0 %s
        }
      }
    }
  }
}

## this function sets up all tk windows and binds X events. It is executed by xinit.c after completing 
## all X initialization. This avoids race conditions.
## In previous flow xschem.tcl was setting up windows and events before X initialization was completed by xinit.c.
## this could lead to crashes on some (may be slow) systems due to Configure/Expose events being delivered
## before xschem being ready to handle them.
proc pack_widgets { { topwin {} } } {
  global env has_x OS tabbed_interface toolbar_visible toolbar_horiz
  # puts "pack_widgets: $topwin"
  if {($OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
    pack $topwin.statusbar.2 -side left 
    pack $topwin.statusbar.3 -side left 
    pack $topwin.statusbar.4 -side left 
    pack $topwin.statusbar.5 -side left 
    pack $topwin.statusbar.6 -side left 
    pack $topwin.statusbar.7 -side left 
    pack $topwin.statusbar.10 -side left 
    pack $topwin.statusbar.11 -side left 
    pack $topwin.statusbar.9 -side left 
    pack $topwin.statusbar.8 -side left 
    pack $topwin.statusbar.1 -side left -fill x
    pack $topwin.statusbar -side bottom -fill x 
    pack $topwin.drw -side right -fill both -expand true
    setup_tabbed_interface
    if {$toolbar_visible}  {toolbar_show $topwin}

    bind $topwin.statusbar.5 <Leave> \
      "focus $topwin.drw; set cadgrid \[$topwin.statusbar.5 get\]; xschem set cadgrid \$cadgrid"
    bind $topwin.statusbar.3 <Leave> \
      "focus $topwin.drw; set cadsnap \[$topwin.statusbar.3 get\]; xschem set cadsnap \$cadsnap"
  }
  if {$topwin ne {}} {
    $topwin.menubar.view entryconfigure {Tabbed interface} -state disabled
  }
}

# if undo_type == disk save undo to disk
# if undo_type == memory keep undo in memory
# In memory undo is faster but in case of crashes nothing can be recovered
# On disk undo is slower but can be used to recover state just before a crash
proc switch_undo {} {
  global undo_type
  if { $undo_type eq {memory} } {
    set res [tk_messageBox -type yesno -parent [xschem get topwindow] \
            -message {Ok to keep undo in memory? Undo will be reset.\
            Memory undo is faster but in case of crashes nothing can be recovered.}]
    if {$res eq {yes} } {
      xschem undo_type $undo_type
    } else {
      set undo_type disk
    }
  } else { ;# disk
    set res [tk_messageBox -type yesno -parent [xschem get topwindow] \
            -message {Ok to save undo on disk? Undo will be reset.\
            Disk undo is slower but in case of crashes previous state can be recovered.}]
    if {$res eq {yes} } {
      xschem undo_type $undo_type
    } else {
      set undo_type memory
    }
  }
}


proc select_raw {} {
  global has_x netlist_dir
  regsub {/$} $netlist_dir {} netlist_dir
  set filename $netlist_dir/[file tail [file rootname [xschem get schname]]].raw
  set types {
      {{Raw Files}       {.raw}        }
      {{All Files}        *            }  
  } 
  if {[info exists has_x]} {
    set filename [tk_getOpenFile -title "Select file" -multiple 0 -initialdir $netlist_dir \
            -initialfile [file tail $filename]  -filetypes $types]
  }
  return $filename
}

proc load_raw {{type {}}} {
  global netlist_dir has_x

  regsub {/$} $netlist_dir {} netlist_dir
  if { [xschem raw_query loaded] != -1} { ;# unload existing raw file(s)
    xschem raw_clear
   }
  set filename [select_raw]
  if {[file exists $filename]} {
    if {$type ne {}} {
      xschem raw_read $filename $type
    } else {
      xschem raw_read $filename
    }
  }
}

## menubuttons not used anymore, but keep this code if needed in the future...
## modified binding for button-1 clicks on menubuttons. Clicking on posted menu will unpost it
# proc tk::menubutton1 {w x y} {
#     # puts $w
#     # puts "--> [array get tk::Priv] "
#     if {[winfo ismapped [$w cget -menu ]]} {
#       MenuUnpost {}
#       return
#     }
#     if {$tk::Priv(inMenubutton) ne ""} {
#       tk::MbPost $tk::Priv(inMenubutton) $x $y
#     }
# }
# 
# bind Menubutton <Button-1> {tk::menubutton1 %W %X %Y}

proc build_widgets { {topwin {} } } {
  global XSCHEM_SHAREDIR tabbed_interface simulate_bg OS sim
  global dark_gui_colorscheme draw_crosshair grid_point_size
  global recentfile color_ps transparent_svg menu_debug_var enable_stretch
  global netlist_show flat_netlist split_files compare_sch intuitive_interface
  global draw_grid big_grid_points sym_txt change_lw incr_hilight symbol_width cadence_compat
  global cadsnap cadgrid draw_window toolbar_visible hide_symbols undo_type snap_cursor
  global disable_unique_names persistent_command autotrim_wires infix_interface orthogonal_wiring en_hilight_conn_inst
  global local_netlist_dir editor netlist_type netlist_dir spiceprefix initial_geometry
  if { $dark_gui_colorscheme} {
    set selectcolor white
  } else {
    set selectcolor black
  }

  menu $topwin.menubar -tearoff 0 -borderwidth 0

  $topwin.menubar add cascade -label "File" -menu $topwin.menubar.file
  menu $topwin.menubar.file -tearoff 0
  $topwin.menubar add cascade -label "Edit" -menu $topwin.menubar.edit
  menu $topwin.menubar.edit -tearoff 0
  $topwin.menubar add cascade -label "Options" -menu $topwin.menubar.option
  menu $topwin.menubar.option -tearoff 0
  $topwin.menubar add cascade -label "View" -menu $topwin.menubar.view
  menu $topwin.menubar.view -tearoff 0
  $topwin.menubar add cascade -label "Properties" -menu $topwin.menubar.prop
  menu $topwin.menubar.prop -tearoff 0

  $topwin.menubar add cascade -label "Layers" -menu $topwin.menubar.layers
  menu $topwin.menubar.layers -tearoff 0
  $topwin.menubar add cascade -label "Tools" -menu $topwin.menubar.tools
  menu $topwin.menubar.tools -tearoff 0
  $topwin.menubar add cascade -label "Symbol" -menu $topwin.menubar.sym
  menu $topwin.menubar.sym -tearoff 0
  $topwin.menubar add cascade -label "Highlight" -menu $topwin.menubar.hilight
  menu $topwin.menubar.hilight -tearoff 0
  $topwin.menubar add cascade -label "Simulation" -menu $topwin.menubar.simulation
  menu $topwin.menubar.simulation -tearoff 0
  $topwin.menubar add cascade -label "Help" -menu $topwin.menubar.help
  menu $topwin.menubar.help -tearoff 0

  if {$topwin eq {}} {
    . configure -menu .menubar
  } else {
    $topwin configure -menu $topwin.menubar
  }

  ## menubuttons not used anymore, but keep code if needed in the future
  ## activate menus when hovering the mouse, even without button pressed
  # bind Menubutton <Motion> { tk::MbMotion %W down %X %Y} 

  $topwin.menubar.help add command -label "Help" -command "textwindow \"${XSCHEM_SHAREDIR}/xschem.help\" ro" \
       -accelerator {?}
  $topwin.menubar.help add command -label "Keys" -command "textwindow \"${XSCHEM_SHAREDIR}/keys.help\" ro"
  $topwin.menubar.help add command -label "Show Keybindings" -command "show_bindkeys"
  $topwin.menubar.help add command -label "About XSCHEM" -command "about"
  
  $topwin.menubar.file add command -label "Clear Schematic"  -accelerator Ctrl+N\
    -command {
      xschem clear schematic
    }
  $topwin.menubar.file add command -label "Clear Symbol" -accelerator Ctrl+Shift+N \
    -command {
      xschem clear symbol
    }
  $topwin.menubar.file add command -label "Component browser" -accelerator {Shift-Ins, Ctrl-I} \
    -command {
      if {$new_symbol_browser} {
        insert_symbol $new_symbol_browser_paths $new_symbol_browser_depth $new_symbol_browser_ext
      } else {
        load_file_dialog {Insert symbol} *.sym INITIALINSTDIR 2
      }
    }
  $topwin.menubar.file add command -label "Open" -command "xschem load" -accelerator {Ctrl+O}
  $topwin.menubar.file add command -label "Open Most Recent" \
    -command {xschem load -gui [lindex "$recentfile" 0]} -accelerator {Ctrl+Shift+O}
  $topwin.menubar.file add cascade -label "Open recent" -menu $topwin.menubar.file.recent
  menu $topwin.menubar.file.recent -tearoff 0
  setup_recent_menu $topwin
  $topwin.menubar.file add command -label {Create new window/tab} -command "xschem new_schematic create"

  $topwin.menubar.file add command -label "Open schematic in new window/tab" \
      -command "open_sub_schematic" -accelerator Alt+E
      # -command "xschem schematic_in_new_window" -accelerator Alt+E
  $topwin.menubar.file add command -label "Open symbol in new window/tab" \
      -command "xschem symbol_in_new_window" -accelerator Alt+I

  $topwin.menubar.file add command -label "Delete files" -command "xschem delete_files" -accelerator {Shift-D}
  $topwin.menubar.file add command -label "Save" -command "xschem save" -accelerator {Ctrl+S}
  $topwin.menubar.file add command -label "Merge" -command "xschem merge" -accelerator {B}
  $topwin.menubar.file add command -label "Reload" -accelerator {Alt+S} \
    -command {
      if {[alert_ "Are you sure you want to reload?" {} 0 1] == 1} {
        xschem reload
      }
    }
  $topwin.menubar.file add command -label "Save as" -command "xschem saveas" -accelerator {Ctrl+Shift+S}
  $topwin.menubar.file add command -label "Save as symbol" \
     -command "xschem saveas {} symbol" -accelerator {Ctrl+Alt+S}
  # added svg, png 20171022
  $topwin.menubar.file add cascade -label "Image export" -menu $topwin.menubar.file.im_exp
  menu $topwin.menubar.file.im_exp -tearoff 0
  $topwin.menubar.file.im_exp add command -label "EPS Selection Export" -command "xschem print eps" 
  $topwin.menubar.file.im_exp add command -label "PDF/PS Export" -command "xschem print pdf" -accelerator {*}
  $topwin.menubar.file.im_exp add command -label "PDF/PS Export Full" -command "xschem print pdf_full"
  $topwin.menubar.file.im_exp add command -label "Hierarchical PDF/PS Export" -command "xschem hier_psprint"
  $topwin.menubar.file.im_exp add command -label "PNG Export" -command "xschem print png" -accelerator {Ctrl+*}
  $topwin.menubar.file.im_exp add command -label "SVG Export" -command "xschem print svg" -accelerator {Alt+*}


  $topwin.menubar.file add separator
  $topwin.menubar.file add command -label "Start new Xschem process" -accelerator {X} -command {
    xschem new_process
  }
  $topwin.menubar.file add command -label "Close schematic" -accelerator {Ctrl+W} -command {
    xschem exit
  }
  $topwin.menubar.file add command -label "Quit Xschem" -accelerator {Ctrl+Q} -command {
    quit_xschem
  }
  $topwin.menubar.option add checkbutton -label "Color Postscript/SVG" -variable color_ps \
     -selectcolor $selectcolor -command {
        if { $color_ps==1 } {xschem set color_ps 1} else { xschem set color_ps 0}
     }
  $topwin.menubar.option add checkbutton -selectcolor $selectcolor  \
     -label "Transparent SVG background" -variable transparent_svg
  $topwin.menubar.option add checkbutton -label "Debug mode" -variable menu_debug_var \
    -selectcolor $selectcolor  -command {
        if { $menu_debug_var==1 } {xschem debug 1} else { xschem debug 0}
     }
  $topwin.menubar.option add checkbutton -selectcolor $selectcolor \
     -label "Undo buffer on Disk" -variable undo_type \
     -onvalue disk -offvalue memory -command {switch_undo}
  $topwin.menubar.option add checkbutton -label "Enable stretch" -variable enable_stretch \
     -selectcolor $selectcolor  -accelerator Y 
  $topwin.menubar.option add checkbutton -label "Enable infix-interface" -variable infix_interface \
     -selectcolor $selectcolor
  $topwin.menubar.option add checkbutton -label "Enable orthogonal wiring" -variable orthogonal_wiring \
     -selectcolor $selectcolor  -accelerator Shift-L
  $topwin.menubar.option add checkbutton -label "Unsel. partial sel. wires after stretch move" \
     -selectcolor $selectcolor -variable unselect_partial_sel_wires

  $topwin.menubar.option add checkbutton -label "Auto Join/Trim Wires" -variable autotrim_wires \
     -selectcolor $selectcolor -command {
         if {$autotrim_wires == 1} {
           xschem trim_wires
           xschem redraw
         }
     }
  $topwin.menubar.option add checkbutton -selectcolor $selectcolor \
    -label "Persistent wire/line place command" -variable persistent_command
  $topwin.menubar.option add checkbutton -label "Intuitive Click & Drag interface" \
    -variable intuitive_interface -selectcolor $selectcolor \
    -command {xschem set intuitive_interface $intuitive_interface}

  $topwin.menubar.option add cascade -label "Crosshair" \
       -menu $topwin.menubar.option.crosshair
  menu $topwin.menubar.option.crosshair -tearoff 0

  $topwin.menubar.option.crosshair add checkbutton -label "Draw snap cursor" \
    -variable snap_cursor -selectcolor $selectcolor  -accelerator {Alt-Z}
  $topwin.menubar.option.crosshair add checkbutton -label "Draw crosshair" \
    -variable draw_crosshair -selectcolor $selectcolor -accelerator {Alt-X}
  $topwin.menubar.option.crosshair add command -label "Crosshair size" \
    -command {
      input_line "Enter crosshair size (int, 0 = full screen width):" \
          "set crosshair_size" $crosshair_size
    }
  $topwin.menubar.option add command -label "Replace \[ and \] for buses in SPICE netlist" \
     -command {
       input_line "Enter two characters to replace default bus \[\] delimiters:" "set bus_replacement_char"
     }
  $topwin.menubar.option add checkbutton \
    -selectcolor $selectcolor -label "Group bus slices in Verilog instances" -variable verilog_bitblast
  $topwin.menubar.option add checkbutton -label "Draw grid" -variable draw_grid \
     -selectcolor $selectcolor -accelerator {%} \
     -command {
       xschem redraw
     }
  $topwin.menubar.option add command -label "Half Snap Threshold" -accelerator G -command {
         xschem set cadsnap [expr {$cadsnap / 2.0} ]
       }
  $topwin.menubar.option add command -label "Double Snap Threshold" -accelerator Shift-G -command {
         xschem set cadsnap [expr {$cadsnap * 2.0} ]
       }
  $topwin.menubar.option add checkbutton -label "Variable grid point size" -variable big_grid_points \
     -selectcolor $selectcolor -command { xschem redraw }
  $topwin.menubar.option add separator

  $topwin.menubar.option add checkbutton -label "No XCopyArea drawing model" -variable draw_window \
         -selectcolor $selectcolor -accelerator {Ctrl+$} \
         -command {
           if { $draw_window == 1} { xschem set draw_window 1} else { xschem set draw_window 0}
         }

  $topwin.menubar.option add checkbutton -label "Fix for GPUs with broken tiled fill" \
         -selectcolor $selectcolor -variable fix_broken_tiled_fill \
         -command {
           if { $fix_broken_tiled_fill == 1} {
             xschem set fix_broken_tiled_fill 1
           } else {
             xschem set fix_broken_tiled_fill 0
           }
           xschem resetwin 1 1 1 0 0
           xschem redraw
         }
  $topwin.menubar.option add checkbutton -label "Fix broken RDP mouse coordinates" \
     -selectcolor $selectcolor -variable fix_mouse_coord -command {xschem set fix_mouse_coord $fix_mouse_coord}

  $topwin.menubar.option add separator

  $topwin.menubar.option add cascade -label "Netlist format / Symbol mode" \
       -menu $topwin.menubar.option.netlist
  menu $topwin.menubar.option.netlist -tearoff 0

  $topwin.menubar.option.netlist add checkbutton -label "Flat netlist" -variable flat_netlist \
     -selectcolor $selectcolor -accelerator : \
     -command {
        if { $flat_netlist==1 } {xschem set flat_netlist 1} else { xschem set flat_netlist 0} 
     }
  $topwin.menubar.option.netlist add checkbutton -label "Split netlist" -variable split_files \
     -selectcolor $selectcolor -accelerator {} 
  $topwin.menubar.option.netlist add radiobutton -label "Spice netlist"\
       -background grey60 -variable netlist_type -value spice -accelerator {Ctrl+Shift+V} \
       -selectcolor $selectcolor -command "xschem set netlist_type spice; xschem redraw"
  $topwin.menubar.option.netlist add radiobutton -label "VHDL netlist"\
       -background grey60 -variable netlist_type -value vhdl -accelerator {Ctrl+Shift+V} \
       -selectcolor $selectcolor -command "xschem set netlist_type vhdl; xschem redraw"
  $topwin.menubar.option.netlist add radiobutton -label "Verilog netlist"\
       -background grey60 -variable netlist_type -value verilog -accelerator {Ctrl+Shift+V} \
       -selectcolor $selectcolor -command "xschem set netlist_type verilog; xschem redraw"
  $topwin.menubar.option.netlist add radiobutton -label "tEDAx netlist" \
       -background grey60 -variable netlist_type -value tedax -accelerator {Ctrl+Shift+V} \
       -selectcolor $selectcolor -command "xschem set netlist_type tedax; xschem redraw"
  $topwin.menubar.option.netlist add radiobutton -label "Symbol global attrs" \
       -background grey60 -variable netlist_type -value symbol -accelerator {Ctrl+Shift+V} \
       -selectcolor $selectcolor -command "xschem set netlist_type symbol; xschem redraw"
  $topwin.menubar.edit add command -label "Undo" -command "xschem undo; xschem redraw" -accelerator U
  $topwin.menubar.edit add command -label "Redo" -command "xschem redo; xschem redraw" -accelerator {Shift+U}
  $topwin.menubar.edit add command -label "Copy" -command "xschem copy" -accelerator Ctrl+C
  $topwin.menubar.edit add command -label "Cut" -command "xschem cut"   -accelerator Ctrl+X
  $topwin.menubar.edit add command -label "Paste" -command "xschem paste" -accelerator Ctrl+V
  $topwin.menubar.edit add command -label "Delete" -command "xschem delete" -accelerator Del
  $topwin.menubar.edit add command -label "Select all" -command "xschem select_all" -accelerator Ctrl+A
  $topwin.menubar.edit add command -label "Duplicate objects" -command "xschem copy_objects" -accelerator C
  $topwin.menubar.edit add command -label "Move objects" -command "xschem move_objects" -accelerator M
  $topwin.menubar.edit add command -label "Move objects stretching attached wires" \
      -command "xschem move_objects stretch" -accelerator Control+M
  $topwin.menubar.edit add command -label "Move objects adding wires to connected pins" \
      -command "xschem move_objects kissing" -accelerator Shift+M
  $topwin.menubar.edit add command -label "Horizontal Flip in place selected objects" -state normal \
     -command {xschem flip_in_place} -accelerator {Alt-F}
  $topwin.menubar.edit add command -label "Vertical Flip in place selected objects" -state normal \
     -command {xschem flipv_in_place} -accelerator {Alt-V}
  $topwin.menubar.edit add command -label "Rotate in place selected objects" -state normal \
      -command {xschem rotate_in_place} -accelerator {Alt-R}
  $topwin.menubar.edit add command -label "Vertical Flip selected objects" -state normal \
     -command {xschem flipv} -accelerator {Shift-V}
  $topwin.menubar.edit add command -label "Horizontal Flip selected objects" -state normal \
     -command {xschem flip} -accelerator {Shift-F}
  $topwin.menubar.edit add command -label "Rotate selected objects" -state normal \
      -command {xschem rotate} -accelerator {Shift-R}
  $topwin.menubar.edit add radiobutton -label "Unconstrained move" -variable constr_mv \
     -selectcolor $selectcolor -background grey60 -value 0 -command {xschem set constr_mv 0} 
  $topwin.menubar.edit add radiobutton -label "Constrained Horizontal move" -variable constr_mv \
     -selectcolor $selectcolor -background grey60 -value 1 -accelerator H -command {xschem set constr_mv 1} 
  $topwin.menubar.edit add radiobutton -label "Constrained Vertical move" -variable constr_mv \
     -selectcolor $selectcolor -background grey60 -value 2 -accelerator V -command {xschem set constr_mv 2} 
  $topwin.menubar.edit add command -label "Push schematic" -command "xschem descend" -accelerator E
  $topwin.menubar.edit add command -label "Push symbol" -command "xschem descend_symbol" -accelerator I
  $topwin.menubar.edit add command -label "Pop" -command "xschem go_back" -accelerator Ctrl+E

  $topwin.menubar add command -label { - } -state disabled
  $topwin.menubar add command -label Netlist -background {#888888} \
    -activebackground orange -command {xschem netlist -erc}

  $topwin.menubar add command -label Simulate -background green -background {#888888} \
    -activebackground orange -command {
     simulate_from_button
  }

  $topwin.menubar add cascade -label "Waves" -background {#888888} \
    -activebackground orange -menu $topwin.menubar.waves
  menu  $topwin.menubar.waves -tearoff 0
  $topwin.menubar.waves add command -label {External viewer} -command {waves external}
  $topwin.menubar.waves add separator
  $topwin.menubar.waves add command -label Clear -command {xschem raw_clear}
  $topwin.menubar.waves add separator
  $topwin.menubar.waves add command -label {Op Annotate} -command {
       set retval [select_raw]
       set show_hidden_texts 1
       if {$retval ne {}} {
         xschem annotate_op $retval
       } else {
         xschem annotate_op
       }
  }
  $topwin.menubar.waves add command -label Op -command {waves op}
  $topwin.menubar.waves add command -label Dc -command {waves dc}
  $topwin.menubar.waves add command -label Ac -command {waves ac}
  $topwin.menubar.waves add command -label Tran -command {waves tran}
  $topwin.menubar.waves add command -label Noise -command {waves noise}
  $topwin.menubar.waves add command -label Sp -command {waves ac}
  $topwin.menubar.waves add command -label Spectrum -command {waves ac}

  set simulate_bg [$topwin.menubar entrycget Simulate -background]

  # create  $topwin.menubar.layers.menu
  create_layers_menu $topwin
  $topwin.menubar.view add command -label "Redraw" -command "xschem redraw" -accelerator Esc
  $topwin.menubar.view add command -label "Fullscreen" \
     -accelerator "\\" -command "
        if {\$fullscreen == 1} {set fullscreen 2} ;# avoid hiding menu in true fullscreen
        xschem fullscreen $topwin.drw
     "
  $topwin.menubar.view add command -label "Zoom Full" -command "xschem zoom_full" -accelerator F
  $topwin.menubar.view add command -label "Zoom In" -command "xschem zoom_in" -accelerator Shift+Z
  $topwin.menubar.view add command -label "Zoom Out" -command "xschem zoom_out" -accelerator Ctrl+Z
  $topwin.menubar.view add command -label "Zoom box" -command "xschem zoom_box" -accelerator Z
  $topwin.menubar.view add command -label "Set snap value" \
         -command {
         input_line "Enter snap value (float):" "xschem set cadsnap" $cadsnap
       }
  $topwin.menubar.view add command -label "Set grid spacing" \
       -command {
         input_line "Enter grid spacing (float):" "xschem set cadgrid" $cadgrid
       }
  $topwin.menubar.view add command -label "Toggle colorscheme"  -accelerator {Shift+O} -command {
          xschem toggle_colorscheme
       }
  $topwin.menubar.view add command -label "Dim colors"  -accelerator {} -command {
          color_dim
       }
  $topwin.menubar.view add command -label "Change current layer color"  -accelerator {} -command {
          change_color
       }
  $topwin.menubar.view add command -label "Reset all colors to default" \
         -accelerator {} -command {
          reset_colors 1
         }

  $topwin.menubar.view add checkbutton -label "Toggle variable line width" -variable change_lw \
     -selectcolor $selectcolor -accelerator {_}
  $topwin.menubar.view add command -label "Set line width" -accelerator {Alt+-} \
       -command {
         input_line "Enter linewidth (float):" "xschem line_width"
       }
  $topwin.menubar.view add command -label "Set grid point size" \
       -command {
         input_line "Enter Grid point size (int or -1: $grid_point_size)" "set grid_point_size" $grid_point_size
       }
  $topwin.menubar.view add checkbutton -label "Tabbed interface" -variable tabbed_interface \
    -selectcolor $selectcolor -command setup_tabbed_interface

  $topwin.menubar.view add cascade -label "Show / Hide" \
       -menu $topwin.menubar.view.show 
  menu $topwin.menubar.view.show -tearoff 0

  $topwin.menubar.view.show add checkbutton -label "Show ERC Info window" \
    -selectcolor $selectcolor -variable show_infowindow -command {
       if { $show_infowindow != 0 } {wm deiconify .infotext
       } else {wm withdraw .infotext}
     }
  $topwin.menubar.view.show add command -label "Visible layers"  -accelerator {} \
    -command {
          select_layers
          xschem redraw
       }
  $topwin.menubar.view.show add checkbutton -label "Symbol text" -variable sym_txt \
     -selectcolor $selectcolor  \
     -accelerator {Ctrl+B} -command { xschem set sym_txt $sym_txt; xschem redraw }
  $topwin.menubar.view.show add checkbutton -label "Show Toolbar" -variable toolbar_visible \
     -selectcolor $selectcolor -command "
        if { \$toolbar_visible } \" toolbar_show $topwin\" else \"toolbar_hide $topwin\"
     "
  $topwin.menubar.view.show add checkbutton -label "Horizontal Toolbar" -variable toolbar_horiz \
     -selectcolor $selectcolor -command " 
        if { \$toolbar_visible } \" toolbar_hide $topwin; toolbar_show $topwin \"
     "
  $topwin.menubar.view.show add checkbutton -label "Show hidden texts"  -variable show_hidden_texts \
         -selectcolor $selectcolor -command {xschem update_all_sym_bboxes; xschem redraw}
  $topwin.menubar.view.show add checkbutton -label "Draw grid axes"  -variable draw_grid_axes \
         -selectcolor $selectcolor -command {xschem redraw}
  $topwin.menubar.prop add command -label "Edit" -command "xschem edit_prop" -accelerator Q
  $topwin.menubar.prop add command -label "Edit with editor" -command "xschem edit_vi_prop" -accelerator Shift+Q
  $topwin.menubar.prop add command -label "View" -command "xschem view_prop" -accelerator Ctrl+Shift+Q
  $topwin.menubar.prop add command -label "Toggle *_ignore attribute on selected instances" \
     -command "xschem toggle_ignore" -accelerator Shift+T
  $topwin.menubar.prop add command -label "Change selected object insertion order" \
     -command { xschem change_elem_order -1 } -accelerator Shift+S
  $topwin.menubar.prop add command -label "Edit Header/License text" \
     -command { update_schematic_header } -accelerator Shift+B
  $topwin.menubar.prop add command -background red -label "Edit file (danger!)" \
     -command "xschem edit_file" -accelerator Alt+Q

  $topwin.menubar.sym add cascade -label "Show symbols" -menu $topwin.menubar.sym.sym
  menu $topwin.menubar.sym.sym -tearoff 0
  
  $topwin.menubar.sym.sym add radiobutton -label "Show symbol details" \
     -selectcolor $selectcolor -background grey60 -variable hide_symbols -value 0 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
  $topwin.menubar.sym.sym add radiobutton -label "Show instance Bounding boxes for subcircuit symbols" \
     -selectcolor $selectcolor -background grey60 -variable hide_symbols -value 1 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
  $topwin.menubar.sym.sym add radiobutton -label "Show instance Bounding boxes for all symbols" \
     -selectcolor $selectcolor -background grey60 -variable hide_symbols -value 2 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B

  $topwin.menubar.sym add command -label "Set symbol width" \
     -command {
        input_line "Enter Symbol width ($symbol_width)" "set symbol_width" $symbol_width 
      }
  $topwin.menubar.sym add command -label "Make symbol from schematic" \
     -command "xschem make_symbol" -accelerator A
  $topwin.menubar.sym add command -label "Make schematic from symbol" \
     -command "xschem make_sch" -accelerator Ctrl+L
  $topwin.menubar.sym add command -label "Make schematic and symbol from selected components" \
     -command "xschem make_sch_from_sel" -accelerator Ctrl+Shift+H
  $topwin.menubar.sym add command -label "Attach net labels to component instance" \
     -command "xschem attach_labels" -accelerator Shift+H
  $topwin.menubar.sym add command -label "Create symbol pins from selected schematic pins" \
          -command "schpins_to_sympins" -accelerator Alt+H
  $topwin.menubar.sym add command -label "Place symbol pin" \
          -command "xschem add_symbol_pin" -accelerator Alt+P
  $topwin.menubar.sym add command -label "Place schematic input port" \
          -command "xschem net_label 2" -accelerator Ctrl+P
  $topwin.menubar.sym add command -label "Place schematic output port" \
          -command "xschem net_label 3" -accelerator Ctrl+Shift+P
  $topwin.menubar.sym add command -label "Place net pin label" \
          -command "xschem net_label 1" -accelerator Alt+L
  $topwin.menubar.sym add command -label "Place net wire label" \
          -command "xschem net_label 0" -accelerator Alt+Shift+L
  $topwin.menubar.sym add command -label "Change selected inst. texts to floaters" \
          -command "xschem floaters_from_selected_inst"
  $topwin.menubar.sym add command -label "Unselect attached floaters" \
          -command "xschem unselect_attached_floaters"


  $topwin.menubar.sym add cascade -label "List of nets" -menu $topwin.menubar.sym.list
  menu $topwin.menubar.sym.list -tearoff 0

  $topwin.menubar.sym.list add command -label "Print list of highlight nets" \
          -command "xschem print_hilight_net 1" -accelerator J
  $topwin.menubar.sym.list add command -label "Print list of highlight nets, with buses expanded" \
          -command "xschem print_hilight_net 3" -accelerator Alt-Ctrl-J
  $topwin.menubar.sym.list add command -label "Create labels from highlight nets" \
          -command "xschem print_hilight_net 4" -accelerator Alt-J
  $topwin.menubar.sym.list add command -label "Create labels from highlight nets with 'i' prefix" \
          -command "xschem print_hilight_net 2" -accelerator Alt-Shift-J
  $topwin.menubar.sym.list add command -label "Create pins from highlight nets" \
          -command "xschem print_hilight_net 0" -accelerator Ctrl-J

  $topwin.menubar.sym add checkbutton \
     -selectcolor $selectcolor  \
     -label "Search all search-paths for schematic associated to symbol" -variable search_schematic
  $topwin.menubar.sym add checkbutton -label "Allow duplicated instance names (refdes)" \
      -selectcolor $selectcolor -variable disable_unique_names
  $topwin.menubar.tools add command -label "Insert symbol" -command "xschem place_symbol" -accelerator {Ins, Shift-I}
  $topwin.menubar.tools add command -label "Insert text" -command "xschem place_text" -accelerator T
  $topwin.menubar.tools add command -label "Insert wire" -command "xschem wire" -accelerator W
  $topwin.menubar.tools add command -label "Insert snap wire" -command "xschem snap_wire" -accelerator Shift+W
  $topwin.menubar.tools add command -label "Insert line" -command "xschem line" -accelerator L
  $topwin.menubar.tools add command -label "Insert rect" -command "xschem rect" -accelerator R
  $topwin.menubar.tools add command -label "Insert polygon" -command "xschem polygon" -accelerator P
  $topwin.menubar.tools add command -label "Insert arc" -command "xschem arc" -accelerator Shift+C
  $topwin.menubar.tools add command -label "Insert circle" -command "xschem circle" -accelerator Ctrl+Shift+C
  $topwin.menubar.tools add command -label "Insert JPG/PNG/SVG image" -command "xschem add_image"
  $topwin.menubar.tools add command -label "Grab screen area" -command "xschem grabscreen" \
    -accelerator {Print Scrn}
  $topwin.menubar.tools add command -label "Search" -accelerator Ctrl+F -command  property_search
  $topwin.menubar.tools add command -label "Align to Grid" -accelerator Alt+U -command  "xschem align"
  $topwin.menubar.tools add command -label "Execute TCL command" -command  "tclcmd"
  $topwin.menubar.tools add command -label "Join/Trim wires" \
     -command "xschem trim_wires" -accelerator {&}
  $topwin.menubar.tools add command -label "Break wires at selected instance pins" \
     -command "xschem break_wires" -accelerator {!}
  $topwin.menubar.tools add command -label "Remove wires running through selected inst. pins" \
     -command "xschem break_wires 1" -accelerator {Ctrl-!}
  $topwin.menubar.tools add command -label "Break wires at mouse position" \
     -command "xschem wire_cut noalign" -accelerator {Alt-Shift-Right Butt.}
  $topwin.menubar.tools add command -label "Break wires at mouse position, align cut point" \
     -command "xschem wire_cut" -accelerator {Alt-Right Butt.}
  $topwin.menubar.tools add command -label "Select all connected wires/labels/pins" \
     -accelerator {Shift-Right Butt.} \
     -command { xschem connected_nets}
  $topwin.menubar.tools add command -label "Select conn. wires, stop at junctions" \
     -accelerator {Ctrl-Righ Butt.} -command { xschem connected_nets 1 }

  $topwin.menubar.hilight add command \
   -label {Set schematic to compare and compare with} \
   -command "set compare_sch 1; xschem compare_schematics" 
  $topwin.menubar.hilight add command \
   -label {Swap compare schematics} \
   -command "swap_compare_schematics" 
  $topwin.menubar.hilight add checkbutton \
   -selectcolor $selectcolor -label {Compare schematics} \
   -command {
      xschem unselect_all
      xschem redraw } \
   -variable compare_sch
  $topwin.menubar.hilight add checkbutton -label "View only Probes" -variable only_probes \
         -selectcolor $selectcolor -accelerator {5} \
         -command { xschem only_probes }
  $topwin.menubar.hilight add command \
   -label {Highlight net-pin mismatches on sel. instances} \
   -command "xschem net_pin_mismatch" \
   -accelerator {Shift-X} 
  $topwin.menubar.hilight add command -label {Highlight duplicate instance names} \
     -command "xschem check_unique_names 0"  -accelerator {#} 
  $topwin.menubar.hilight add command -label {Rename duplicate instance names} \
     -command "xschem check_unique_names 1" -accelerator {Ctrl+#}
  $topwin.menubar.hilight add command -label {Select overlapped instances} \
     -command "xschem warning_overlapped_symbols 1; xschem redraw" -accelerator {}
  $topwin.menubar.hilight add command -label {Propagate Highlight selected net/pins} \
     -command "xschem hilight drill" -accelerator {Ctrl+Shift+K}
  $topwin.menubar.hilight add checkbutton \
     -selectcolor $selectcolor -label "Increment Hilight Color" -variable incr_hilight
  $topwin.menubar.hilight add command -label {Highlight selected net/pins} \
     -command "xschem hilight" -accelerator K
  $topwin.menubar.hilight add command -label {Send selected net/pins to Viewer} \
     -command "xschem send_to_viewer" -accelerator Alt+G
  $topwin.menubar.hilight add command -label {Select hilight nets / pins} \
     -command "xschem select_hilight_net" \
     -accelerator Alt+K
  $topwin.menubar.hilight add command -label {Un-highlight all net/pins} \
     -command "xschem unhilight_all" -accelerator Shift+K
  $topwin.menubar.hilight add command -label {Un-highlight selected net/pins} \
     -command "xschem unhilight" -accelerator Ctrl+K
  # 20160413
  $topwin.menubar.hilight add checkbutton \
     -selectcolor $selectcolor -label {Auto-highlight net/pins} -variable auto_hilight
  $topwin.menubar.hilight add checkbutton -label {Enable highlight connected instances} \
    -selectcolor $selectcolor -variable en_hilight_conn_inst

  $topwin.menubar.simulation add command -label "Set netlist Dir" \
    -command {
          set local_netlist_dir 0
          set_netlist_dir 1
    }
  $topwin.menubar.simulation add command -label "Set top level netlist name" \
    -command {
          input_line {Set netlist file name} {xschem set netlist_name} [xschem get netlist_name] 40
    }
  $topwin.menubar.simulation add checkbutton -label "Show netlist after netlist command" \
     -selectcolor $selectcolor -variable netlist_show -accelerator {Shift+A} 
  $topwin.menubar.simulation add checkbutton -label "Keep symbols when traversing hierarchy" \
     -selectcolor $selectcolor -variable keep_symbols 
  $topwin.menubar.simulation add radiobutton -indicatoron 1 -label "Use netlist directory" \
    -background grey60 -variable local_netlist_dir -value 0 \
    -selectcolor $selectcolor -command {set_netlist_dir 1 }
  $topwin.menubar.simulation add radiobutton -indicatoron 1 -label "Use 'simulation' dir in schematic dir" \
    -background grey60 -variable local_netlist_dir -value 1 \
    -selectcolor $selectcolor -command {set_netlist_dir 1 }
  $topwin.menubar.simulation add radiobutton -indicatoron 1 -label "Use 'simulation/\[schname\]' dir in schematic dir" \
    -background grey60 -variable local_netlist_dir -value 2 \
    -selectcolor $selectcolor -command {set_netlist_dir 1 }
  $topwin.menubar.simulation add command -label {Configure simulators and tools} -command {simconf}
  if {$OS == {Windows}} {
    $topwin.menubar.simulation add command -label {List running sub-processes} -state disabled
  } else {
    $topwin.menubar.simulation add command -label {List running sub-processes} -command {
      list_running_cmds
    }
  }
  $topwin.menubar.simulation add command -label {View last job data} -command {
    if { [info exists execute(data,last)] } {
      viewdata $execute(data,last)
    }
  }
  $topwin.menubar.simulation add command -label {View last job errors} -command {
    if { [info exists execute(error,last)] } {
      viewdata $execute(error,last)
    }
  }
  $topwin.menubar.simulation add command -label {Utile Stimuli Editor (GUI)} -command {
     inutile [xschem get current_dirname]/stimuli.[file rootname [file tail [xschem get schname]]]
  }
  $topwin.menubar.simulation add command -label {Utile Stimuli Translate} -command {
     inutile_translate  [xschem get current_dirname]/stimuli.[file rootname [file tail [xschem get schname]]]
  }
  $topwin.menubar.simulation add command -label {Shell [simulation path]} -command {
     if { [set_netlist_dir 0] ne "" } {
        get_shell $netlist_dir
     }
   }
  $topwin.menubar.simulation add command -label {Edit Netlist} \
     -command {edit_netlist [xschem get netlist_name fallback]}
  $topwin.menubar.simulation add command -label {Send highlighted nets to viewer} \
    -command {xschem create_plot_cmd} -accelerator Ctrl+Shift+X
  $topwin.menubar.simulation add command -label {Changelog from current hierarchy} -command {
    viewdata [list_hierarchy]
  }
  $topwin.menubar.simulation add separator


  $topwin.menubar.simulation add cascade -label "Graphs" -menu $topwin.menubar.simulation.graph
  menu $topwin.menubar.simulation.graph -tearoff 0
  $topwin.menubar.simulation.graph add checkbutton -label {Auto highlight plotted nets} \
   -selectcolor $selectcolor  -variable auto_hilight_graph_nodes
  $topwin.menubar.simulation.graph add command -label {Add waveform graph} -command {xschem add_graph}
  $topwin.menubar.simulation.graph add command -label {Add waveform reload launcher} -command {
      xschem place_symbol [find_file_first launcher.sym] "name=h5\ndescr=\"load waves\" 
tclcommand=\"xschem raw_read \$netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran\"
"
  }
  $topwin.menubar.simulation.graph add command -label "Annotate Operating Point into schematic" \
    -command {
       set retval [select_raw]
       set show_hidden_texts 1
       if {$retval ne {}} {
         xschem annotate_op $retval
       } else {
         xschem annotate_op
       }
    }
  $topwin.menubar.simulation.graph add checkbutton -label "Live annotate probes with 'b' cursor" \
     -selectcolor $selectcolor -variable live_cursor2_backannotate 
  $topwin.menubar.simulation.graph add checkbutton -label "Hide graphs if no spice data loaded" \
     -selectcolor $selectcolor -variable hide_empty_graphs -command {xschem redraw}


  $topwin.menubar.simulation add cascade -label "LVS" -menu $topwin.menubar.simulation.lvs
  menu $topwin.menubar.simulation.lvs -tearoff 0
  $topwin.menubar.simulation.lvs add checkbutton -label "LVS netlist + Top level is a .subckt" \
  -selectcolor $selectcolor -variable lvs_netlist 
  $topwin.menubar.simulation.lvs add checkbutton -label "Upper case .SUBCKT and .ENDS" \
  -selectcolor $selectcolor -variable uppercase_subckt
  $topwin.menubar.simulation.lvs add checkbutton -label "Top level is a .subckt" \
  -selectcolor $selectcolor -variable top_is_subckt

  $topwin.menubar.simulation.lvs add checkbutton -label "Set 'lvs_ignore' variable" \
         -selectcolor $selectcolor \
         -variable lvs_ignore -command {xschem rebuild_connectivity; xschem unhilight_all}
  $topwin.menubar.simulation.lvs add checkbutton -label "Use 'spiceprefix' attribute" -variable spiceprefix \
         -selectcolor $selectcolor -command {xschem redraw} 


  # used to check status of Simulate button later. This variable is constant, never changed
  frame $topwin.drw -background {} -takefocus 1

  focus $topwin.drw
  if { $topwin == {} } {set rootwin .} else { set rootwin $topwin} 
  wm  title $rootwin "xschem - "
  wm iconname $rootwin "xschem - "
  $rootwin configure  -background {}
  wm  geometry $rootwin $initial_geometry
  #wm maxsize . 1600 1200
  if {$tabbed_interface} {
    wm protocol $rootwin WM_DELETE_WINDOW {
      xschem exit closewindow
    }
  } elseif { $rootwin == {.}} {
    wm protocol $rootwin WM_DELETE_WINDOW {
       set old  [xschem get current_win_path]
       save_ctx $old
       restore_ctx .drw
       housekeeping_ctx
       xschem new_schematic switch .drw
       xschem exit closewindow
       # did not exit ... switch back 
       restore_ctx $old
       housekeeping_ctx
       xschem new_schematic switch $old
    }
  } else {
    wm protocol $rootwin WM_DELETE_WINDOW "xschem new_schematic destroy $topwin.drw {}"
  }

  frame $topwin.statusbar  
  label $topwin.statusbar.1 -text "STATUS BAR 1"  
  label $topwin.statusbar.2 -text "SNAP:"
  entry $topwin.statusbar.3 \
         -width 7 -foreground black -takefocus 0
  entry_replace_selection $topwin.statusbar.3
  label $topwin.statusbar.4 -text "GRID:"
  entry $topwin.statusbar.5 \
         -width 7 -foreground black -takefocus 0
  entry_replace_selection $topwin.statusbar.5
  label $topwin.statusbar.6 -text "MODE:"
  label $topwin.statusbar.7 -width 7 
  label $topwin.statusbar.11 -text {Stretch:}
  label $topwin.statusbar.10 -activebackground green -text {}
  label $topwin.statusbar.9 -textvariable enable_stretch
  label $topwin.statusbar.8 -activebackground red -text {} 
  add_toolbuttons $topwin
}

proc set_initial_dirs {} {
  global INITIALLOADDIR INITIALINSTDIR INITIALPROPDIR pathlist
  set INITIALLOADDIR {}
  set INITIALINSTDIR {}
  set INITIALPROPDIR {}
  foreach i $pathlist  {
    if { [file exists $i] } {
      set INITIALLOADDIR $i
      set INITIALINSTDIR $i
      set INITIALPROPDIR $i
      break
    }
  }
}

# called whenever XSCHEM_LUBRARY_PATH changes (see trace command at end) 
proc trace_set_paths {varname idxname op} {
  if {$varname eq {XSCHEM_LIBRARY_PATH} } {
    # puts stderr "executing set_paths after XSCHEM_LIBRARY_PATH change"
    uplevel #0 set_paths
  }
}
proc cleanup_paths {paths} {
  global env
  set pathlist {}
  foreach i $paths {
    regsub {^~$} $i ${env(HOME)} i
    regsub {^~/} $i ${env(HOME)}/ i
    if { ![string compare $i .] } {
      lappend pathlist $i
    } elseif { [ regexp {\.\.\/} $i] } {
      lappend pathlist [file normalize $i]
    } elseif { [ file exists $i] } {
      lappend pathlist $i
    }
  }
  return $pathlist
}

# when XSCHEM_LIBRARY_PATH is changed call this function to refresh and cache
# new library search path.
proc set_paths {} {
  global XSCHEM_LIBRARY_PATH pathlist OS add_all_windows_drives
  # puts stderr "caching search paths"
  if { [info exists XSCHEM_LIBRARY_PATH] } {
    if {$OS == "Windows"} {
      set pathlist_orig [split $XSCHEM_LIBRARY_PATH \;]
      if {$add_all_windows_drives} {
        set win_vol [file volumes]
        foreach disk $win_vol {
          lappend pathlist_orig $disk
        }
      }
    } else {
      set pathlist_orig [split $XSCHEM_LIBRARY_PATH :]
    }

    set pathlist [cleanup_paths $pathlist_orig]
  }
  if {$pathlist eq {}} { set pathlist [pwd] }
  set_initial_dirs
}

proc print_help_and_exit {} {
  global XSCHEM_SHAREDIR
  if { [xschem get help ]} {
    set fd [open "${XSCHEM_SHAREDIR}/xschem.help" r]
    set helpfile [read $fd]
    puts $helpfile
    close $fd
  }
}

proc set_missing_colors_to_black {} {
  global enable_layer cadlayers svg_colors ps_colors light_colors dark_colors
  ## pad missing colors with black
  for {set i 0} { $i<$cadlayers } { incr i} {
    set_ne enable_layer($i) 1
    foreach j { svg_colors ps_colors light_colors dark_colors } {
      if { ![string compare [lindex [set $j] $i] {} ] } {
        if { ![string compare $j {ps_colors} ] || ![string compare $j {svg_colors} ]} {
          lappend $j {0x000000}
        } else {
          lappend $j {#000000}
        }
      }
    }
  }
  foreach i {svg_colors ps_colors light_colors dark_colors} {
    if { [llength [set $i]] > $cadlayers} {
       set $i [lrange [set $i] 0 [expr {$cadlayers -1}]]
    }
  }
}

proc create_layers_menu { {topwin {} } } {
  global dark_colorscheme
  if { $dark_colorscheme == 1 } { set txt_color black} else { set txt_color white} 
  set j 0
  foreach i $tctx::colors {
    ## 20121121
    if {  $j == [xschem get pinlayer] } { 
      set laylab [format %2d $j]-PIN
      set layfg $txt_color
    } elseif { $j == [xschem get wirelayer] } { 
      set laylab [format %2d $j]-WIRE
      set layfg $txt_color
    } elseif { $j == [xschem get textlayer] } { ;# 20161206
      set laylab [format %2d $j]-TEXT
      set layfg $txt_color
    } elseif { $j == [xschem get backlayer] } { ;# 20161206
      set laylab [format %2d $j]-BG
      if { $dark_colorscheme == 1 } {
        set layfg white
      } else {
        set layfg black
      }
    } elseif { $j == [xschem get gridlayer] } { ;# 20161206
      set laylab [format %2d $j]-GRID
      set layfg $txt_color
    } else {
      set laylab "[format %2d $j]        "
      set layfg $txt_color
    }
    $topwin.menubar.layers add command -label $laylab  -activeforeground $layfg \
       -foreground $layfg -background $i -activebackground $i \
       -command "xschem set rectcolor $j; reconfigure_layers_button $topwin"
    if { [expr {$j%10}] == 0 } { $topwin.menubar.layers entryconfigure $j -columnbreak 1 }
    incr j
  }
}   

proc set_replace_key_binding { {win_path {.drw}}} {
  global replace_key
  if {[array exists replace_key]} {
    foreach i [array names replace_key] {
      key_binding "$i" "$replace_key($i)" $win_path
    }
  }
}

proc source_user_tcl_files {} {
  global tcl_files
  foreach i $tcl_files {
    uplevel #0 [list source $i]
  }
}

proc eval_user_startup_commands {} {
  global user_startup_commands
  if {[info exists user_startup_commands]} {
    if {[catch {uplevel #0 $user_startup_commands} res]} {
      puts "executing $user_startup_commands:\n\n$res"
    }
  }         
}           

proc eval_postinit_commands {} {
  global postinit_commands
  if {[info exists postinit_commands]} {
    if {[catch {uplevel #0 $postinit_commands} res]} {
      puts "executing $postinit_commands:\n\n$res"
    }
  }
}

proc eval_netlist_postprocess {} {
  global netlist_postprocess
  if {[info exists netlist_postprocess]} {
    if {[catch {uplevel #0 $netlist_postprocess} res]} {
      puts "executing $netlist_postprocess:\n\n$res"
    } 
  }
}

proc setup_tcp_xschem { {port_number {}} } {
  global xschem_listen_port xschem_server_getdata

  # if a server socket was already set close it.
  if {[info exists xschem_server_getdata(server)]} { close $xschem_server_getdata(server) }
  if {$port_number ne {}} { set xschem_listen_port $port_number}
  if { [info exists xschem_listen_port] && ($xschem_listen_port ne {}) } { 
    if {[catch {socket -server xschem_server $xschem_listen_port} err]} {
      puts "setup_tcp_xschem: problems finding a free TCP port"
      puts $err
      return 0
    } else {
      set chan $err
      set xschem_server_getdata(server) $chan
      # this piece of code deals with automatic port number selection (port_number argument set to 0) 
      # tcl will automatically choose a free tcp port.
      set assigned_port [lindex [fconfigure $chan -sockname] end]
      set xschem_listen_port $assigned_port
      return $assigned_port
    }
  }
  return 0
}

proc setup_tcp_bespice {} {
  global bespice_listen_port bespice_server_getdata
  if { [info exists bespice_listen_port] && ($bespice_listen_port ne {}) } { 
    # We will attempt to open port $bespice_listen_port ... $bespice_listen_port + 1000 this should succeed ...
    # We need to make this attempt as several instances of xschem / bespice might be running.
    # Each of these instances needs it's own server listening on a dedicated port.
    # The variable $bespice_listen_port is passed to bespice wave when the application is started
    # in the function "set_sim_defaults()".
    
    # if $bespice_listen_port is set to 0 TCL will automatically find a free port
    if {[catch {socket -server bespice_server $bespice_listen_port} err]} {
      # failed
      puts "setup_tcp_bespice: problems finding a free TCP port"
      puts $err
      return 0
    } else {
      # succeded => set $bespice_listen_port and socket connection for communication
      set chan $err
      set bespice_server_getdata(server) $chan
      set assigned_port [lindex [fconfigure $chan -sockname] end]
      set bespice_listen_port $assigned_port
      puts stderr "setup_tcp_bespice: success : listening to TCP port: $bespice_listen_port"
      return 1
    }
    puts "setup_tcp_bespice: problems listening to TCP port: $bespice_listen_port ... $last_port"
    puts $err
    return 0
  } 
  # bespice_listen_port not defined, nothing to do
  # puts "setup_tcp_bespice: the functionallity was not set up as the variable \$bespice_listen_port"
  # puts "hasn't been defined in your xschemrc file."
  return 1
}

### 
###   MAIN PROGRAM
###

# This variable is set in xinit.c
# set running_in_src_dir 0
# if {[file exists ./systemlib] && [file exists ./xschem.tcl] && [file exists ./xschem]} {
#   set running_in_src_dir 1
# }

set_ne dark_colorscheme 1
set_ne dark_gui_colorscheme 0
if { [info exists has_x]} {
  if { $dark_gui_colorscheme == 0 } { ;# normal GUI
    option add *foreground black startupFile
    option add *activeForeground black startupFile
    option add *background {grey80} startupFile
    option add *activeBackground {#f8f8f8} startupFile
    option add *disabledForeground {grey50} startupFile
    option add *disabledBackground {grey70} startupFile
    option add *readonlyBackground {grey70} startupFile
    option add *highlightBackground {white} startupFile
    option add *highlightThickness 0 startupFile
    option add *highlightColor {grey30} startupFile
    option add *insertBackground {black} startupFile
    option add *selectColor {white} startupFile ;# checkbuttons, radiobuttons
    option add *selectForeground black
    option add *selectBackground grey70
    if { [info tclversion] > 8.4} {
      ttk::style configure TCombobox -fieldbackground grey90
    }
  } else { ;# dark GUI colorscheme
    option add *foreground white startupFile
    option add *activeForeground white startupFile
    option add *background {grey20} startupFile
    option add *activeBackground {grey10} startupFile
    option add *disabledForeground {grey50} startupFile
    option add *disabledBackground {grey20} startupFile
    option add *readonlyBackground {grey20} startupFile
    option add *highlightBackground {black} startupFile
    option add *highlightThickness 0 startupFile
    option add *highlightColor {grey70} startupFile
    option add *insertBackground {white} startupFile
    option add *selectColor {grey10} startupFile ;# checkbuttons, radiobuttons
    option add *selectForeground black
    option add *selectBackground grey70
    if { [info tclversion] > 8.4} {
      ttk::style configure TCombobox -fieldbackground grey20
    }
  }
}

set OS [lindex $tcl_platform(os) 0]
set env(LC_ALL) C
# tcl variable XSCHEM_LIBRARY_PATH  should already be set in xschemrc
set_ne add_all_windows_drives 1
set_paths
print_help_and_exit
set_ne text_replace_selection 1
if {$text_replace_selection && $OS != "Windows"} {
  # deletes selected text when pasting in text widgets, courtesy Wolf-Dieter Busch
  proc tk_textPaste w {
    global tcl_platform
    if {![catch {::tk::GetSelection $w CLIPBOARD} sel]} then {
      set oldSeparator [$w cget -autoseparators]
      if {$oldSeparator} then {
        $w configure -autoseparators 0
        $w edit separator
      }
      foreach {to from} [lreverse [$w tag ranges sel]] {
        $w delete $from $to
      }
      $w insert insert $sel
      if {$oldSeparator} then {
        $w edit separator
        $w configure -autoseparators 1
      }
    }
  }
}

## allow to unpost menu entries when clicking a posted menu

if { [info exists has_x] && [info tclversion] >= 8.6 } {
  bind Menu <Button> {
     if { [%W cget -type] eq "menubar" && [info exists tk::Priv(menuActivated)]} {
       %W activate none
     } else {
       %W activate @%x,%y
     }
     tk::MenuButtonDown %W
  }
}

## this proc must be called for any created entry widgets
proc entry_replace_selection {w} {
  global text_replace_selection OS
  if {$text_replace_selection && $OS != "Windows"} {
    bind $w <<Paste>> {if {[%W selection present]} {%W delete sel.first sel.last}}
  }
}

# focus the schematic window if mouse goes over it, even if a dialog box is displayed,
# without needing to click. This allows to move/zoom/pan the schematic while editing attributes.
set_ne autofocus_mainwindow 0
if {$OS == "Windows"} {
  set_ne XSCHEM_TMP_DIR [xschem get temp_dir]
} else {
  set_ne XSCHEM_TMP_DIR {/tmp}
}

# used in C code
set_ne xschem_libs {}
set_ne noprint_libs {}
set_ne nolist_libs {}
set_ne debug_var 0
set_ne debug_tcleval 0 ;# debug tclpropeval2 (tcleval() in xschem attributes)
# used to activate debug from menu
set_ne menu_debug_var 0
set textwindow_wcounter 1
set viewdata_wcounter 1
set retval ""
set prev_symbol ""
set symbol ""

# 20100204 flag to enable fix for dialog box positioning,  issues with some wm
set wm_fix 0 

# 20171010
set tclcmd_txt {}

###
### user preferences: set default values
###

if { ![info exists dircolor] } {
  set_ne dircolor(/share/xschem/) red
  set_ne dircolor(/share/doc/xschem/) {#338844}
}

set_ne file_dialog_globfilter {*}
set_ne component_browser_on_top 1
## list of tcl procedures to load at end of xschem.tcl
set_ne tcl_files {}
set_ne download_url_helper {curl -f -s -O -J}
set_ne netlist_dir "$USER_CONF_DIR/simulations"
# this global exists only for netlist_type radiobuttons, don't use, use [xschem] subcommand to get/set values
# it is also used in xschemrc to set initial netlist type.
set_ne netlist_type spice
set_ne local_netlist_dir 0 ;# if set use <sch_dir>/simulation for netlist and sims
set_ne bus_replacement_char {} ;# use {<>} to replace [] with <> in bussed signals
set_ne lvs_netlist 0
set_ne top_is_subckt 0
set_ne uppercase_subckt 0
set_ne lvs_ignore 0
set_ne hide_empty_graphs 0 ;# if set to 1 waveform boxes will be hidden if no raw file loaded
set_ne graph_use_ctrl_key 0;# if set forces to use Control key to operate on graphs
set_ne spiceprefix 1
set_ne verilog_2001 1
set_ne verilog_bitblast 0
set_ne search_schematic 0
# if set to 1 search symbols (if not in any of the XSCHEM_LIBRARY_PATH directories) in [pwd] 
# instead of in the directory of currently loaded schematic.
set_ne split_files 0
set_ne flat_netlist 0
set_ne netlist_show 0
set_ne color_ps 1
set_ne ps_page_title 1 ;# add a title in the top left page corner
set_ne draw_crosshair 0
set_ne crosshair_layer 8 ;# Yellow
set_ne crosshair_size 0
set_ne snap_cursor_size 6
set_ne ps_paper_size {a4 842 595}
set_ne transparent_svg 0
set_ne only_probes 0  ; # 20110112
set_ne fullscreen 0
set_ne unzoom_nodrift 0
set_ne zoom_full_center 0
set_ne change_lw 1
set_ne line_width 0
set_ne live_cursor2_backannotate 1
set_ne cursor_2_hook {}
set_ne draw_window 0
set_ne show_hidden_texts 0
set_ne incr_hilight 1
set_ne enable_stretch 0
set_ne constr_mv 0
set_ne unselect_partial_sel_wires 0
set_ne load_file_dialog_fullpath 1
set_ne file_dialog_others {} ;# contains 2nd, 3rd, ... selected filenames on mult. selections in load file

# if set show selected elements while dragging the selection rectangle.
# once selected these can not be unselected by retracting the selection rectangle
# if not set show selected items at end of drag.
set_ne incremental_select 1
set_ne select_touch 1

set_ne draw_grid 1
set_ne big_grid_points 0
set_ne grid_point_size -1 ;# grid point size (>=0) or unspecified (-1)
set_ne draw_grid_axes 1
set_ne persistent_command 0
set_ne intuitive_interface 1
set_ne use_cursor_for_selection 0
set_ne autotrim_wires 0
set_ne cadence_compat 0
set_ne infix_interface 1
set_ne snap_cursor 0
set_ne orthogonal_wiring 0
set_ne compare_sch 0
set_ne disable_unique_names 0
set_ne sym_txt 1
set_ne show_infowindow 0 
set_ne show_infowindow_after_netlist onerror 
set_ne no_ask_save 0 ;# if set to 1 ctrl-s (save) will not ask to save
set_ne symbol_width 150
set_ne editor {gvim -f}
set_ne rainbow_colors 0
set_ne initial_geometry {900x600}
set_ne edit_symbol_prop_new_sel {}
if {$OS == "Windows"} {
  set_ne launcher_default_program [auto_execok start]
} else {
  set_ne launcher_default_program {xdg-open}
}
set_ne auto_hilight 0
set_ne auto_hilight_graph_nodes 0
set_ne use_tclreadline 1
set_ne en_hilight_conn_inst 0
## xpm to png conversion
set_ne to_png {gm convert} 
## ps to pdf conversion
set_ne to_pdf {ps2pdf}
## tab stop position
set_ne tabstop 8
set text_tabs_setting {-tabs "[expr {$tabstop * [font measure TkFixedFont 0]}] left" -tabstyle wordprocessor}

# selected graph user is editing attributes with graph GUI
set_ne graph_bus 0
set_ne graph_private_cursor 0
set_ne graph_logx 0
set_ne graph_logy 0
set_ne graph_rainbow 0
set_ne graph_selected {}
set_ne graph_schname {}
set_ne graph_change_done 0 ;# used to push undo only once when editing graphs
set_ne graph_linewidth_mult 1.4 ;# default multiplier (w.r.t. xschem lines) for line width in graphs 
# user clicked this wave 
set_ne graph_sel_wave {}
# flag to force simulation stop (Esc key pressed) 
set_ne tclstop 0
## undo_type: disk or memory
set_ne undo_type disk

## show tab bar (tabbed interface) 
set_ne tabbed_interface 1
## case insensitive symbol lookup (on case insensitive filesystems only!)
set_ne case_insensitive 0

## New alternate symbol placement browser (default: not enabled).
set_ne new_symbol_browser 0
set_ne new_symbol_browser_paths {} ;# if empty use xschem search paths
set_ne new_symbol_browser_depth 2 ;# depth to descend into each dir of the search paths
set_ne new_symbol_browser_ext {\.(sch|sym|tcl)$} ;# file extensions (a regex) to look for

## New alternate load file browser (default: not enabled).
set_ne new_file_browser 0
set_ne new_file_browser_paths {} ;# if empty use xschem search paths
set_ne new_file_browser_depth 2 ;# depth to descend into each dir of the search paths
set_ne new_file_browser_ext {\.(sch|sym|tcl)$} ;# file extensions (a regex) to look for

set_ne file_dialog_ext {*}

#### toolbar icons are bitmaps. Their size is 24x24. This can be changed to
#### accommodate UHD displays.
#### the zoom factor is an integer that can be used to enlarge these icons
#### Default value: 1
set_ne toolbar_icon_zoom 1
#### context menu icons are 16 x 16. they can be enlarged by the following integer
#### default value: 1
set_ne ctxmenu_icon_zoom 1

## remember edit_prop widget size
set_ne edit_prop_size 80x12
set_ne text_line_default_geometry {}
set_ne enter_text_default_geometry {}
set_ne graph_dialog_default_geometry {}
set_ne terminal xterm

# xschem tcp port number (listen to port and execute commands from there if set) 
# set a port number in xschemrc if you want accept remote connections.
set_ne xschem_listen_port {}

# server for bespice waveform connection (listen to port and send commands to bespice if set)
# set a port number in xschemrc if you want xschem to be able to cross-probe to bespice
set_ne bespice_listen_port {}

set_ne keep_symbols 0 ;# if set loaded symbols will not be purged when descending/netlisting.

# hide instance details (show only bbox) 
set_ne hide_symbols 0
# gaw tcp {host port} 
set_ne gaw_tcp_address {localhost 2020}

## cairo stuff 20171112
set_ne cairo_font_scale 1.0
set_ne nocairo_font_xscale .85
set_ne nocairo_font_yscale .88
set_ne cairo_font_line_spacing 1.0
set_ne cairo_vert_correct 0
set_ne nocairo_vert_correct 0

# Arial, Monospace Sans-Serif; default font to use if unspecified in text elements
set_ne cairo_font_name {Sans-Serif}
set_ne svg_font_name {Sans-Serif}

# has_cairo set by c program if cairo enabled
set has_cairo 0 
set rotated_text {} ;#20171208
set_ne enable_dim_bg 0
set_ne dim_bg 0.0
set_ne dim_value 0.0
set_ne fix_broken_tiled_fill 1 ;# set to 1 on some broken X11 drivers / GPUs that show garbage on screen */
# this fix uses an alternative method for getting mouse coordinates on KeyPress/KeyRelease
# events. Some remote connection softwares do not generate the correct coordinates
# on such events */
set_ne fix_mouse_coord 0

##### set colors
if {!$rainbow_colors} {
  set_ne cadlayers 22
  ## 20171113
  set_ne light_colors {
   "#ffffff" "#0055ee" "#aaaaaa" "#222222" "#229900"
   "#bb2200" "#0055ee" "#ff0000" "#888800" "#00aaaa"
   "#880088" "#00ff00" "#0000cc" "#666600" "#557755"
   "#aa2222" "#7ccc40" "#00ffcc" "#ce0097" "#d2d46b"
   "#ef6158" "#fdb200"}
  set_ne dark_colors {
   "#000000" "#00ccee" "#4f4f4f" "#cccccc" "#88dd00" 
   "#bb2200" "#00ccee" "#ff0000" "#ffff00" "#ffffff"
   "#ff00ff" "#00ff00" "#0044ff" "#aaaa00" "#aaccaa"
   "#ff7777" "#bfff81" "#00ffcc" "#ce0097" "#d2d46b" 
   "#ef6158" "#fdb200"}
} else {
  # rainbow colors for bitmapping
  # skip if colors defined in ~/.xschem 20121110
  set_ne cadlayers 35
  #20171113
  set_ne light_colors {
    "#000000" "#00ccee" "#aaaaaa" "#ffffff" "#88dd00"
    "#bb2200" "#0000e0" "#2000e0" "#4000e0" "#6000e0"
    "#8000e0" "#a000e0" "#c000e0" "#e000e0" "#e000c0"
    "#e000a0" "#e00080" "#e00060" "#e00040" "#e00020"
    "#e00000" "#e02000" "#e04000" "#e06000" "#e08000"
    "#e0a000" "#e0c000" "#e0e000" "#e0e020" "#e0e040"
    "#e0e060" "#e0e080" "#e0e0a0" "#e0e0c0" "#e0e0e0"
  }
  set_ne dark_colors {
    "#000000" "#00ccee" "#3f3f3f" "#ffffff" "#88dd00" 
    "#bb2200" "#0000e0" "#2000e0" "#4000e0" "#6000e0"
    "#8000e0" "#a000e0" "#c000e0" "#e000e0" "#e000c0"
    "#e000a0" "#e00080" "#e00060" "#e00040" "#e00020"
    "#e00000" "#e02000" "#e04000" "#e06000" "#e08000"
    "#e0a000" "#e0c000" "#e0e000" "#e0e020" "#e0e040"
    "#e0e060" "#e0e080" "#e0e0a0" "#e0e0c0" "#e0e0e0"
  }
}


# every 0x#### hex data represents one 16 bit row of the 16x16 bit fill bitmap
# of the specified layer number.
set_ne pixdata(0) {
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
}
set_ne pixdata(1) {
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
}
set_ne pixdata(2) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}
set_ne pixdata(3) {
  0x5555 0x0000 0xaaaa 0x0000 0x5555 0x0000 0xaaaa 0x0000
  0x5555 0x0000 0xaaaa 0x0000 0x5555 0x0000 0xaaaa 0x0000
}
set_ne pixdata(4) {
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
}
set_ne pixdata(5) {
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
}
set_ne pixdata(6) {
  0x0101 0x0000 0x0000 0x0000 0x1010 0x0000 0x0000 0x0000
  0x0101 0x0000 0x0000 0x0000 0x1010 0x0000 0x0000 0x0000
}
set_ne pixdata(7) {
  0x0000 0x8000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0200
  0x0000 0x0080 0x0000 0x0000 0x0000 0x0000 0x0000 0x0002
}
set_ne pixdata(8) {
  0x1111 0x0000 0x0000 0x0000 0x1111 0x0000 0x0000 0x0000
  0x1111 0x0000 0x0000 0x0000 0x1111 0x0000 0x0000 0x0000
}
set_ne pixdata(9) {
  0x0441 0x0000 0x1004 0x0000 0x4110 0x0000 0x0441 0x0000
  0x1004 0x0000 0x4110 0x0000 0x0441 0x0000 0x1004 0x0000
}
set_ne pixdata(10) {
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
  0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff 0xffff
}
set_ne pixdata(11) {
  0x1111 0x0000 0x4444 0x0000 0x1111 0x0000 0x4444 0x0000
  0x1111 0x0000 0x4444 0x0000 0x1111 0x0000 0x4444 0x0000
}
set_ne pixdata(12) {
  0x0404 0x0202 0x0101 0x8080 0x4040 0x2020 0x1010 0x0808
  0x0404 0x0202 0x0101 0x8080 0x4040 0x2020 0x1010 0x0808
}
set_ne pixdata(13) {
  0x1111 0x2222 0x4444 0x8888 0x1111 0x2222 0x4444 0x8888
  0x1111 0x2222 0x4444 0x8888 0x1111 0x2222 0x4444 0x8888
}
set_ne pixdata(14) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}
set_ne pixdata(15) {
  0x4444 0x0000 0x0000 0x0000 0x4444 0x0000 0x0000 0x0000
  0x4444 0x0000 0x0000 0x0000 0x4444 0x0000 0x0000 0x0000
}
set_ne pixdata(16) {
  0x1111 0x0000 0x0000 0x0000 0x2222 0x0000 0x0000 0x0000
  0x4444 0x0000 0x0000 0x0000 0x8888 0x0000 0x0000 0x0000
}
set_ne pixdata(17) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}
set_ne pixdata(18) {
  0x5555 0xaaaa 0x5555 0xaaaa 0x5555 0xaaaa 0x5555 0xaaaa
  0x5555 0xaaaa 0x5555 0xaaaa 0x5555 0xaaaa 0x5555 0xaaaa
}
set_ne pixdata(19) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}
set_ne pixdata(20) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}
set_ne pixdata(21) {
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
  0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000
}


# for svg and pdf draw 20121108
regsub -all {"} $dark_colors  {} svg_colors
regsub -all {#} $svg_colors  {0x} svg_colors
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors
set_missing_colors_to_black
# read-only vars to store defaults (so we can switch to default colors) 
set dark_colors_save $dark_colors
set light_colors_save $light_colors

set_ne tctx::colors $dark_colors
##### end set colors


# 20111005 added missing initialization of globals...
set_ne no_change_attrs 0
set_ne preserve_unchanged_attrs 0
set search_select 0

# 20111106 these vars are overwritten by caller with mktemp file names
if {$OS == "Windows"} {
  set filetmp $env(windir)/.tmp2
} else {
  set filetmp [pwd]/.tmp2
}

# flag bound to a checkbutton in symbol editprop form
# if set cell is copied when renaming it
set_ne copy_cell 0

load_recent_file
# schematic to preload in new windows 20090708
set_ne XSCHEM_START_WINDOW {}

set custom_token {lab}
set search_value {}
set search_exact 0
set search_case 1

# 20171005
set custom_label_prefix {}

###
### build Tk widgets
###
if { ( $OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
  setup_toolbar
  # for hyperlink in about dialog
  eval  font create Underline-Font [ font actual TkDefaultFont ]
  font configure Underline-Font -underline true -size 24
  . configure -cursor left_ptr
  set_old_tk_fonts ;# for xschem compiled with old tcl-tk libs
  if { [info exists tk_scaling] } {tk scaling $tk_scaling} ;# useful for 4k displays (set bigger widgets)
  set infowindow_text {}
  infowindow

  build_widgets {}

  ##
  ## packing top windows (pack instructions) and event binding (bind instructions) done in proc pack_widgets
  ## executed by xinit.c after finalizing X initialization. This avoid potential race conditions
  ## like Configure or Expose events being generated before xschem being ready to handle them.
  ##

  # allow user to modify key bindings
  set_replace_key_binding

  update
  xschem windowid . ;# set icon for window
} ;# end if {[exists has_x]}

# read custom colors
if { [file exists ${USER_CONF_DIR}/colors] } {
  source ${USER_CONF_DIR}/colors
}

if { [llength $dark_colors] < $cadlayers || [llength $light_colors] < $cadlayers } {
  puts stderr { Warning: wrong number of configured layers in light_colors or dark_colors variables.}
}
if { $dark_colorscheme == 1} { 
  set tctx::colors $dark_colors
} else {
  set tctx::colors $light_colors
}
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors
regsub -all {"} $tctx::colors {} svg_colors
regsub -all {#} $svg_colors {0x} svg_colors

if { $show_infowindow } { wm deiconify .infotext } 

# xschem listen and bespice listen
setup_tcp_xschem
setup_tcp_bespice

# automatically build pathlist whenever XSCHEM_LIBRARY_PATH changes
trace add variable XSCHEM_LIBRARY_PATH write trace_set_paths

