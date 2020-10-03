#
#  File: xschem.tcl
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


### for tclreadline: disable customcompleters
proc completer { text start end line } { return {}}

###
### set var with $val if var Not existing
###
proc set_ne { var val } {
    upvar #0 $var v
    if { ![ info exists v ] } {
      set v $val
    }
}

###
### Tk procedures
###
# execute service function
proc execute_fileevent {id} {
  global execute_pipe execute_data execute_cmd  simulate_oldbg
  global execute_status execute_callback
  append execute_data($id) [read $execute_pipe($id) 1024]
  if {[eof $execute_pipe($id)]} {
      fileevent $execute_pipe($id) readable ""
      # setting pipe to blocking before closing allows to see if pipeline failed
      fconfigure $execute_pipe($id) -blocking 1
      set status 0
      if {[catch {close $execute_pipe($id)} err options]} {
        set details [dict get $options -errorcode]
        if {[lindex $details 0] eq "CHILDSTATUS"} {
            set status [lindex $details 2]
            if { $execute_status($id) } {
              viewdata "Failed: $execute_cmd($id)\nstderr:\n$err\ndata:\n$execute_data($id)" ro
            }
        } else {
          if { $execute_status($id) } {
            set status 1
            viewdata "Completed: $execute_cmd($id)\nstderr:\n$err\ndata:\n$execute_data($id)" ro
          }
        }
      } 
      if { $status == 0 } {
        if { $execute_status($id) } {
          viewdata "Completed: $execute_cmd($id)\ndata:\n$execute_data($id)" ro
        }
      }
      if { [info exists execute_callback($id)] } { eval $execute_callback($id); unset execute_callback($id) } 
      unset execute_pipe($id)
      unset execute_data($id)
      unset execute_status($id)
      unset execute_cmd($id)
  }
}


proc execute_wait {status args} {
  global execute_pipe 
  xschem set semaphore [expr [xschem get semaphore] +1]
  set id [eval execute $status $args]
  vwait execute_pipe($id)
  xschem set semaphore [expr [xschem get semaphore] -1]
  return $id
}

# equivalent to the 'exec' tcl function but keeps the event loop
# responding, so widgets get updated properly
# while waiting for process to end.
proc execute {status args} {
  global execute_id execute_status
  global execute_data
  global execute_cmd
  global execute_pipe
  if {![info exists execute_id]} {
      set execute_id 0
  } else {
      incr execute_id
  }
  set id $execute_id
  set pipe [open "|$args" r]
  set execute_status($id) $status
  set execute_pipe($id) $pipe
  set execute_cmd($id) $args
  set execute_data($id) ""
  fconfigure $pipe -blocking 0
  fileevent $pipe readable "execute_fileevent $id"
  return $id
}

proc netlist {source_file show netlist_file} {
 global XSCHEM_SHAREDIR flat_netlist hspice_netlist netlist_dir
 global verilog_2001 netlist_type

 if {$netlist_type eq {spice}} {
   if { $hspice_netlist == 1 } {
     set hspice {-hspice}
   } else {
     set hspice {}
   }
   if {$flat_netlist==0} {
     eval exec {awk -f ${XSCHEM_SHAREDIR}/spice.awk -- $hspice $source_file | \
          awk -f ${XSCHEM_SHAREDIR}/break.awk > $netlist_dir/$netlist_file}
   } else {
     eval exec {awk -f ${XSCHEM_SHAREDIR}/spice.awk -- $hspice $source_file | \
          awk -f ${XSCHEM_SHAREDIR}/flatten.awk | awk -f ${XSCHEM_SHAREDIR}/break.awk > $netlist_dir/$netlist_file}
   }
   if ![string compare $show "show"] {
      textwindow $netlist_dir/$netlist_file
   }
 } 
 if {$netlist_type eq {vhdl}} {
   eval exec {awk -f $XSCHEM_SHAREDIR/vhdl.awk $source_file > $netlist_dir/$netlist_file}
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }
 if {$netlist_type eq {tedax}} {
   eval exec {awk -f $XSCHEM_SHAREDIR/tedax.awk $source_file \
              > $netlist_dir/$netlist_file}
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }
 if {$netlist_type eq {verilog}} {
   eval exec {awk -f ${XSCHEM_SHAREDIR}/verilog.awk $source_file \
              > $netlist_dir/$netlist_file}

   # 20140409
   if { $verilog_2001==1 } { 
     set vv [pid]
     eval exec {awk -f ${XSCHEM_SHAREDIR}/convert_to_verilog2001.awk $netlist_dir/$netlist_file > $netlist_dir/${netlist_file}$vv}
     eval exec {mv $netlist_dir/${netlist_file}$vv $netlist_dir/$netlist_file}
   }
   if ![string compare $show "show"] {
     textwindow "$netlist_dir/$netlist_file"
   }
 }
 return {}
}

# 20161121
proc convert_to_pdf {filename dest} {
  global a3page
  if { $a3page == 1 } { set paper a3 } else { set paper a4 }
  if { ![catch "exec ps2pdf -sPAPERSIZE=$paper $filename" msg] } {
    # ps2pdf succeeded, so remove original .ps file
    file rename -force [file rootname $filename].pdf $dest
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  } else {
    puts stderr "problems converting postscript to pdf: $msg"
  }
}

# 20161121
proc convert_to_png {filename dest} {
  global to_png tcl_debug
  # puts "---> $to_png $filename $destfile"
  if { ![catch "exec $to_png $filename png:$dest" msg] } {
    # conversion succeeded, so remove original .xpm file
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  } else {
    puts stderr "problems converting xpm to png: $msg"
  }
}

#20171024
proc key_binding {  s  d } { 
# always specify Shift- modifier for capital letters
# see tk 'man keysyms' for key names
# example format for s, d: Control-Alt-Key-asterisk
#                          Control-Shift-Key-A
#                          Alt-Key-c
#                          ButtonPress-4
#
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
  if { [regexp {(Mod1|Alt)-} $d] } { set state [expr $state +8] }
  if { [regexp Control- $d] } { set state [expr $state +4] }
  if { [regexp Shift- $d] } { set state [expr $state +1] }
  if { [regexp ButtonPress-1 $d] } { set state [expr $state +0x100] }
  if { [regexp ButtonPress-2 $d] } { set state [expr $state +0x200] }
  if { [regexp ButtonPress-3 $d] } { set state [expr $state +0x400] }
  # puts "$state $key <${s}>"
  if {[regexp ButtonPress- $d]} {
    bind .drw "<${s}>" "xschem callback %T %x %y 0 $key 0 $state"
  } else {
    if {![string compare $d {} ] } {
      bind .drw "<${s}>" {}
    } else {
      bind .drw  "<${s}>" "xschem callback %T %x %y $keysym 0 0 $state"
    }
  }

}

proc edit_file {filename} {
 
 global editor
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
## netlist_type
## computerfarm
## terminal

proc save_sim_defaults {f} {
  global sim 
  
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
      puts $fd "# specify tool command (cmd), name (name), if tool must run in foreground and if exit status must be reported"
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


proc update_recent_file {f} {
  global recentfile 
  set old $recentfile
  set recentfile {}
  lappend recentfile $f
  foreach i $old {
    if {[abs_sym_path $i] ne [abs_sym_path $f]} {
      lappend recentfile [abs_sym_path $i]
    }
  }
  set recentfile [lreplace $recentfile 10 10]
  write_recent_file
  if { ![info exists ::no_x] } setup_recent_menu
}

proc write_recent_file {} {
  global recentfile USER_CONF_DIR
 
  set a [catch {open $USER_CONF_DIR/recent_files w} fd]
  if { $a } {
    puts "write_recent_file: error opening file $f: $fd"
    return
  }
  puts $fd "set recentfile {$recentfile}"
  close $fd
}

proc setup_recent_menu {} {
  global recentfile
  .menubar.file.menu.recent delete 0 9
  set i 0
  if { [info exists recentfile] } {
    foreach i $recentfile {
      .menubar.file.menu.recent add command \
        -command "xschem load {$i}" \
        -label [file tail $i]
    }
  }
}



proc set_sim_defaults {} {
  ### spice 
  global sim terminal USER_CONF_DIR no_x

  set failure 0
  if { ![info exists no_x] && [winfo exists .sim] } {
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
        if {![info exists no_x]} {
          tk_messageBox -message  "Problems opening simrc file: $err" -icon warning -parent . -type ok
        }
        set failure 1
      }
    }
  } 
  if {![info exists sim] || $failure} {
    if {[info exists sim]} {unset sim}
    # no simrc, set a reasonable default
    set sim(tool_list) {spice spicewave verilog verilogwave vhdl vhdlwave}
    if {$::OS == "Windows"} {
      set_ne sim(spice,0,cmd) {ngspice -i "$N" -a}
    } else {
      set_ne sim(spice,0,cmd) {$terminal -e 'ngspice -i "$N" -a || sh'}
    }
    set_ne sim(spice,0,name) {Ngspice}
    set_ne sim(spice,0,fg) 0
    set_ne sim(spice,0,st) 0
    
    set_ne sim(spice,1,cmd) {ngspice -b -r "$n.raw" -o "$n.out" "$N"}
    set_ne sim(spice,1,name) {Ngspice batch}
    set_ne sim(spice,1,fg) 0
    set_ne sim(spice,1,st) 1
    
    set_ne sim(spice,2,cmd) {Xyce "$N" -r "$n.raw"}
    set_ne sim(spice,2,name) {Xyce batch}
    set_ne sim(spice,2,fg) 0
    set_ne sim(spice,2,st) 1
    
    # number of configured spice simulators, and default one
    set_ne sim(spice,n) 3
    set_ne sim(spice,default) 0
    
    ### spice wave view
    set_ne sim(spicewave,0,cmd) {gaw "$n.raw" } 
    set_ne sim(spicewave,0,name) {Gaw viewer}
    set_ne sim(spicewave,0,fg) 0
    set_ne sim(spicewave,0,st) 0
   
    set_ne sim(spicewave,1,cmd) {echo load "$n.raw" > .spiceinit
    $terminal -e ngspice
    rm .spiceinit} 
    set_ne sim(spicewave,1,name) {Ngpice Viewer}
    set_ne sim(spicewave,1,fg) 0
    set_ne sim(spicewave,1,st) 0

    set_ne sim(spicewave,2,cmd) {rawtovcd -v 1.5 "$n.raw" > "$n.vcd" && gtkwave "$n.vcd" "$n.sav" 2>/dev/null} 
    set_ne sim(spicewave,2,name) {Rawtovcd}
    set_ne sim(spicewave,2,fg) 0
    set_ne sim(spicewave,2,st) 0
    # number of configured spice wave viewers, and default one
    set_ne sim(spicewave,n) 3
    set_ne sim(spicewave,default) 0
    
    ### verilog
    set_ne sim(verilog,0,cmd) {iverilog -o .verilog_object -g2012 "$N" && vvp .verilog_object}
    set_ne sim(verilog,0,name) {Icarus verilog}
    set_ne sim(verilog,0,fg) 0
    set_ne sim(verilog,0,st) 1
    # number of configured verilog simulators, and default one
    set_ne sim(verilog,n) 1
    set_ne sim(verilog,default) 0
    
    ### verilog wave view
    set_ne sim(verilogwave,0,cmd) {gtkwave dumpfile.vcd "$N.sav" 2>/dev/null}
    set_ne sim(verilogwave,0,name) {Gtkwave}
    set_ne sim(verilogwave,0,fg) 0
    set_ne sim(verilogwave,0,st) 0
    # number of configured verilog wave viewers, and default one
    set_ne sim(verilogwave,n) 1
    set_ne sim(verilogwave,default) 0
    
    ### vhdl
    set_ne sim(vhdl,0,cmd) {ghdl -c --ieee=synopsys -fexplicit "$N" -r "$s" --wave="$n.ghw"}
    set_ne sim(vhdl,0,name) {Ghdl}
    set_ne sim(vhdl,0,fg) 0
    set_ne sim(vhdl,0,st) 1
    # number of configured vhdl simulators, and default one
    set_ne sim(vhdl,n) 1
    set_ne sim(vhdl,default) 0
    
    ### vhdl wave view
    set_ne sim(vhdlwave,0,cmd) {gtkwave "$n.ghw" "$N.sav" 2>/dev/null}
    set_ne sim(vhdlwave,0,name) {Gtkwave}
    set_ne sim(vhdlwave,0,fg) 0
    set_ne sim(vhdlwave,0,st) 0
    # number of configured vhdl wave viewers, and default one
    set_ne sim(vhdlwave,n) 1
    set_ne sim(vhdlwave,default) 0
  }
} 

proc simconf_yview { args } {
  global simconf_vpos
  # puts "simconf_yview: $args"
  set_ne simconf_vpos 0
  if {[lindex $args 0] eq {place}} {
    place .sim.topf.f.scrl -in .sim.topf.f -x 0 -y 0 -relwidth 1
    update
  } 
  set ht [winfo height .sim.topf.f]
  set hs [winfo height .sim.topf.f.scrl]
  # puts "ht=$ht hs=$hs"
  set frac [expr {double($ht)/$hs}]
  if { [lindex $args 0] eq {scroll}} {
    set simconf_vpos [expr $simconf_vpos + [lindex $args 1] *(1.0/$frac)/5]
  } elseif { [lindex $args 0] eq {moveto}} {
    set simconf_vpos [lindex $args 1]
  }
  if { $simconf_vpos < 0.0 } { set simconf_vpos 0.0}
  if { $simconf_vpos > 1.0-$frac } { set simconf_vpos [expr 1.0 - $frac]}
  .sim.topf.vs set $simconf_vpos [expr $simconf_vpos + $frac]
  place .sim.topf.f.scrl -in .sim.topf.f -x 0 -y [expr -$hs * $simconf_vpos] -relwidth 1
}

proc simconf {} {
  global sim USER_CONF_DIR simconf_default_geometry

  catch { destroy .sim } 
  set_sim_defaults
  toplevel .sim -class dialog
  wm title .sim {Simulation Configuration}
  wm geometry .sim 700x340
  frame .sim.topf
  frame .sim.topf.f
  frame .sim.topf.f.scrl 
  scrollbar .sim.topf.vs -command {simconf_yview}
  pack .sim.topf.f -fill both -expand yes -side left
  pack .sim.topf.vs -fill y -expand yes
  frame .sim.topf.f.scrl.top
  frame .sim.topf.f.scrl.center
  frame .sim.bottom
  pack .sim.topf.f.scrl.top -fill x 
  pack .sim.topf.f.scrl.center -fill both -expand yes
  set bg(0) {#dddddd}
  set bg(1) {#aaaaaa}
  set toggle 0
  foreach tool $sim(tool_list) {
    frame .sim.topf.f.scrl.center.$tool
    label .sim.topf.f.scrl.center.$tool.l -width 12 -text $tool  -bg $bg($toggle)
    frame .sim.topf.f.scrl.center.$tool.r
    pack .sim.topf.f.scrl.center.$tool -fill both -expand yes
    pack .sim.topf.f.scrl.center.$tool.l -fill y -side left
    pack .sim.topf.f.scrl.center.$tool.r -fill both -expand yes
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      frame .sim.topf.f.scrl.center.$tool.r.$i
      pack .sim.topf.f.scrl.center.$tool.r.$i -fill x -expand yes
      entry .sim.topf.f.scrl.center.$tool.r.$i.lab -textvariable sim($tool,$i,name) -width 15 -bg $bg($toggle)
      radiobutton .sim.topf.f.scrl.center.$tool.r.$i.radio -bg $bg($toggle) \
         -variable sim($tool,default) -value $i
      text .sim.topf.f.scrl.center.$tool.r.$i.cmd -width 20 -height 3 -wrap none -bg $bg($toggle)
      .sim.topf.f.scrl.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      checkbutton .sim.topf.f.scrl.center.$tool.r.$i.fg -text Fg -variable sim($tool,$i,fg) -bg $bg($toggle)
      checkbutton .sim.topf.f.scrl.center.$tool.r.$i.st -text Status -variable sim($tool,$i,st) -bg $bg($toggle)

      pack .sim.topf.f.scrl.center.$tool.r.$i.lab -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.radio -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.cmd -side left -fill x -expand yes
      pack .sim.topf.f.scrl.center.$tool.r.$i.fg -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.st -side left -fill y 
    }
    incr toggle
    set toggle [expr {$toggle %2}]
  }
  button .sim.bottom.cancel  -text Cancel -command {destroy .sim}
  button .sim.bottom.help  -text Help -command {
    set h {The following variables are defined and will get substituted by
XSCHEM before sending commands to the shell:

 - N: complete filename of netlist for current netlisting mode
   (example: /home/schippes/.xschem/simulations/opamp.spice for spice)
   (example: /home/schippes/.xschem/simulations/opamp.v for verilog)
 - n: complete filename of netlist as above but without extension
   (example: /home/schippes/.xschem/simulations/opamp)
 - s: name of schematic being used (example: opamp)
 - d: simulation directory (example: /home/schippes/.xschem/simulations)
 - terminal: terminal to be used for applications that need to be
   executed in terminal (example: $terminal -e ngspice -i "$N" -a)
If for a given tool there are multiple rows then the radiobutton
tells which one will be called by xschem.
Variables should be used with the usual substitution character $: $n, $N, etc.
Foreground checkbutton tells xschem to wait for child process to finish.
Status checkbutton tells xschem to report a status dialog (stdout, stderr,
exit status) when process finishes.
Any changes made in the command or tool name entries will be saved in 
~/.xschem/simrc when 'Save Configuration' button is pressed.
If no ~/.xschem/simrc is present then a bare minumum skeleton setup is presented.
To reset to default just delete the ~/.xschem/simrc file manually.
    }
    viewdata $h ro
  }
  button .sim.bottom.ok  -text {Accept and Save Configuration} -command {
    foreach tool $sim(tool_list) {
      for {set i 0} { $i < $sim($tool,n)} {incr i} {
        set sim($tool,$i,cmd) [.sim.topf.f.scrl.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
      }
    }
    # destroy .sim
    save_sim_defaults ${USER_CONF_DIR}/simrc
    # puts "saving simrc"
  }
  button .sim.bottom.close -text {Accept and Close} -command {
    set_sim_defaults
    destroy .sim
  }
  wm protocol .sim WM_DELETE_WINDOW { set_sim_defaults; destroy .sim }
  pack .sim.bottom.cancel -side left -anchor w
  pack .sim.bottom.help -side left
  #foreach tool $sim(tool_list) {
  #  button .sim.bottom.add${tool} -text +${tool} -command "
  #    simconf_add $tool
  #    destroy .sim
  #    save_sim_defaults ${USER_CONF_DIR}/simrc
  ##    simconf
  #  "
  #  pack .sim.bottom.add${tool} -side left
  #}
  pack .sim.bottom.ok -side right -anchor e
  pack .sim.bottom.close -side right
  pack .sim.topf -fill both -expand yes
  pack .sim.bottom -fill x
  if { [info exists simconf_default_geometry]} {
     wm geometry .sim "${simconf_default_geometry}"
  }
 
  bind .sim.topf.f <Configure> {simconf_yview}
  bind .sim <Configure> {
    set simconf_default_geometry [wm geometry .sim]
  }
  bind .sim <ButtonPress-4> { simconf_yview scroll -0.2}
  bind .sim <ButtonPress-5> { simconf_yview scroll 0.2}
  simconf_yview place
  set maxsize [expr [winfo height .sim.topf.f.scrl] + [winfo height .sim.bottom]]
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

proc xschem_getdata {sock} {
  global xschem_server_getdata
  if {[eof $sock] || [catch {gets $sock xschem_server_getdata(line,$sock)}]} {
    close $sock
    puts "Close $xschem_server_getdata(addr,$sock)"
    unset xschem_server_getdata(addr,$sock)
    unset xschem_server_getdata(line,$sock)
    unset xschem_server_getdata(res,$sock)
  } else {
    puts "tcp--> $xschem_server_getdata(line,$sock)"
    # xschem command must be executed at global scope...
    uplevel #0 [list catch $xschem_server_getdata(line,$sock) xschem_server_getdata(res,$sock)]
    puts $sock "$xschem_server_getdata(res,$sock)"
  }
} 

proc xschem_server {sock addr port} {
  global xschem_server_getdata
  puts "Accept $sock from $addr port $port"
  fconfigure $sock -buffering line
  set xschem_server_getdata(addr,$sock) [list $addr $port]
  fileevent $sock readable [list xschem_getdata $sock]
}

## given a hierarchical net name x1.xamp.netname go down in the hierarchy and 
## highlight the specified net.
proc probe_net {net} {

  xschem unselect_all
  xschem set no_draw 1
  # return to top level if not already there
  while { [xschem get currentsch] } { xschem go_back } 
  while { [regexp {\.} $net] } {
    set inst $net
    regsub {\..*} $inst {} inst
    regsub {[^.]+\.} $net {} net
    xschem search exact 1 name $inst
    set full_inst [split [lindex [xschem get expandlabel [xschem selected_set]] 0] {,}]
    set instnum [expr [lsearch -exact  $full_inst $inst] + 1]
    # puts "$full_inst --> $instnum"
    xschem descend $instnum

#  set a [lindex [split [lindex [xschem get expandlabel {xrdec[31:0]}] 0] ,] 3]

  }
  set res [xschem hilight_netname $net]
  if {$res==0  && [regexp {^net[0-9]+$} $net]} {
    set res [xschem hilight_netname \#$net]
  }
  xschem set no_draw 0
  xschem redraw
  return $res
}

proc simulate {{callback {}}} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $d : netlist directory

  global netlist_dir netlist_type computerfarm terminal current_dirname sim
  global execute_callback XSCHEM_SHAREDIR
  set_sim_defaults
  
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool $netlist_type
    set s [file tail [file rootname [xschem get schname]]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nocommands $sim($tool,$def,cmd)]
    if {$::OS == "Windows"} {
      eval exec $cmd # $cmd cannot be surrounded by {} as exec will change forward slash to backward slash
    } else {
      set id [$fg $st sh -c "cd $netlist_dir; $cmd"]
      set execute_callback($id) $callback
      if {$fg eq {execute_wait}} { 
        eval $execute_callback($id); unset execute_callback($id) 
      }
    }
  }
}

proc gaw_echoline {} {
  global gaw_fd
  gets $gaw_fd line
  if {[eof $gaw_fd]} {
     puts "finishing connection from gaw"
     close $gaw_fd
     unset gaw_fd
  }
  # generate a variable event we can vwait for
  set gaw_fd $gaw_fd
  puts "gaw -> $line"
}

proc gaw_setup_tcp {} {
  global gaw_fd gaw_tcp_address netlist_dir no_x
  set s [file tail [file rootname [xschem get schname 0]]]

  if { [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {![info exists no_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return
  }
  chan configure $gaw_fd -blocking 1 -buffering line -encoding binary -translation binary
  fileevent $gaw_fd readable gaw_echoline
  puts $gaw_fd "table_set $s.raw"
}

proc gaw_cmd {cmd} {
  global gaw_fd gaw_tcp_address netlist_dir no_x
  if { [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {![info exists no_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return
  }
  chan configure $gaw_fd -blocking 1 -buffering line -encoding binary -translation binary
  puts $gaw_fd "$cmd"
  fileevent $gaw_fd readable gaw_echoline
  close $gaw_fd; unset gaw_fd;
}

proc waves {} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $d : netlist directory

  global netlist_dir netlist_type computerfarm terminal current_dirname sim gaw_fd XSCHEM_SHAREDIR
  set_sim_defaults
  
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool ${netlist_type}
    set s [file tail [file rootname [xschem get schname]]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }

    set tool ${tool}wave
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nocommands $sim($tool,$def,cmd)]
    $fg $st sh -c "cd $netlist_dir; $cmd"
  }
}
# ============================================================

proc utile_translate {schname} { 
  global netlist_dir netlist_type tcl_debug XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_cmd_path\" stimuli.$tmpname"}
}

proc utile_gui {schname} { 
  global netlist_dir netlist_type tcl_debug XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_gui_path\" stimuli.$tmpname"} &
}

proc utile_edit {schname} { 
  global netlist_dir netlist_type tcl_debug editor XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path 
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; $editor stimuli.$tmpname ; cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_cmd_path\" stimuli.$tmpname"} &
}

proc get_shell { curpath } {
 global netlist_dir netlist_type tcl_debug
 global  terminal

 execute 0 sh -c "cd $curpath; $terminal"
}

proc edit_netlist {schname } {
 global netlist_dir netlist_type tcl_debug
 global editor terminal
 set tmpname [file rootname "$schname"]

 if { [regexp vim $editor] } { set ftype "-c \":set filetype=$netlist_type\"" } else { set ftype {} }
 if { [select_netlist_dir 0] ne "" } {
   # puts "edit_netlist: \"$editor $ftype  ${schname}.v\" \"$netlist_dir\" bg"
   if { $netlist_type=="verilog" } {
     execute 0  sh -c "cd $netlist_dir; $editor $ftype  \"${tmpname}.v\""
   } elseif { $netlist_type=="spice" } {
     execute 0  sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.spice\""
   } elseif { $netlist_type=="tedax" } {
     execute 0 sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.tdx\""
   } elseif { $netlist_type=="vhdl" } { 
     execute 0 sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.vhdl\""
   }
 }
 return {}
}


# 20180926
# global_initdir should be set to:
#   INITIALLOADDIR  for load
#   INITIALINSTDIR  for instance placement
# ext:  .sch or .sym or .sch.sym or .sym.sch
#
proc save_file_dialog { msg ext global_initdir {initialfile {}} {overwrt 1} } {
  upvar #0 $global_initdir initdir
  set temp $initdir
  if { $initialfile ne {}} {
    set initialdir [file dirname $initialfile]
    set initialfile [file tail $initialfile]
  } else {
    set initialdir $initdir
    set initialfile {}
  }
  set initdir $initialdir

  set r [load_file_dialog $msg $ext $global_initdir $initialfile 0 $overwrt]


  set initdir $temp
  return $r
}

proc is_xschem_file {f} {
  set a [catch {open "$f" r} fd]
  set ret 0
  set score 0
  set instances 0
  if {$a} {
    puts stderr "Can not open file $f"
  } else {
    while { [gets $fd line] >=0 } {
      if { [regexp {^[TKGVSE] \{} $line] } { incr score }
      if { [regexp {^[BL] +[0-9]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } { incr score }
      if { [regexp {^N +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } { incr score }
      if { [regexp {^C +\{[^{}]+\} +[-0-9.eE]+ +[-0-9.eE]+ +[0-3]+ +[0-3]+ +\{} $line] } { incr instances; incr score }
      if { [regexp "^v\[ \t\]+\{xschem\[ \t\]+version\[ \t\]*=.*\[ \t\]+file_version\[ \t\]*=" $line] } {
        set ret 1
      }
    } 
    if { $score > 6 }  { set ret 1} ;# Heuristic decision :-)
    if { $ret } {
      if { $instances} {
        set ret SCHEMATIC
      } else { 
        set ret SYMBOL
      }
    }
    close $fd
  }
  # puts "score=$score"
  return $ret
}

proc list_dirs {pathlist } {
  global list_dirs_selected_dir INITIALINSTDIR
  toplevel .list -class dialog
  wm title .list {Select Library:}
  wm protocol .list WM_DELETE_WINDOW { set list_dirs_selected_dir {} } 
  set X [expr {[winfo pointerx .list] - 30}]
  set Y [expr {[winfo pointery .list] - 25}]
  if { $::wm_fix } { tkwait visibility .list }
  wm geometry .list "+$X+$Y"

  set x 0
  set dir {}
  label .list.title \
      -text "Choose path to start from. You can navigate anywhere\n with the file selector from there \n" \
      -background {#77dddd}
  pack .list.title -fill x -side top
  foreach elem $pathlist {
    frame .list.${x}
    label .list.${x}.l -text [expr {$x+1}] -width 4
    button .list.${x}.b -text $elem -command "set list_dirs_selected_dir $elem"
    pack .list.${x}.l -side left
    pack .list.${x}.b -side left -fill x -expand yes
    pack .list.${x} -side top -fill x 
    incr x
  }
  frame .list.${x}
  label .list.${x}.l -text [expr {$x+1}] -width 4
  button .list.${x}.b -text {Last used dir} -command "set list_dirs_selected_dir $INITIALINSTDIR"
  pack .list.${x}.l -side left
  pack .list.${x}.b -side left -fill x -expand yes
  pack .list.${x} -side top -fill x 
  frame .list.but
  button .list.but.cancel -text Cancel -command {set list_dirs_selected_dir {} }
  
  pack .list.but.cancel -side bottom
  pack .list.but -fill x -side top
  vwait list_dirs_selected_dir
  destroy .list
  return $list_dirs_selected_dir
}


proc myload_set_colors1 {} {
  global myload_files1 dircolor
  for {set i 0} { $i< [.dialog.l.paneleft.list index end] } { incr i} {
    set name "[lindex $myload_files1 $i]"
    .dialog.l.paneleft.list itemconfigure $i -foreground black -selectforeground black
    foreach j [array names dircolor] {
      set pattern $j
      set color $dircolor($j)
      if { [regexp $pattern $name] } {
        .dialog.l.paneleft.list itemconfigure $i -foreground $color -selectforeground $color
      }
    }
  }
}


proc myload_set_colors2 {} {
  global myload_index1 myload_files2 dircolor
  set dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
  for {set i 0} { $i< [.dialog.l.paneright.list index end] } { incr i} {
    set name "$dir1/[lindex $myload_files2 $i]"
    if {[ file isdirectory $name]} {
      .dialog.l.paneright.list itemconfigure $i -foreground blue
      foreach j [array names dircolor] {
        set pattern $j 
        set color $dircolor($j)
        if { [regexp $pattern $name] } {
          .dialog.l.paneright.list itemconfigure $i -foreground $color -selectforeground $color
        }
      }

    } else {
      .dialog.l.paneright.list itemconfigure $i -foreground black
    }
  }
}
proc myload_set_home {dir} {
  global pathlist  myload_files1 myload_index1 current_dirname

  .dialog.l.paneleft.list selection clear 0 end
  if { $dir eq {.}} { set dir $current_dirname}
  # puts "set home: dir=$dir, pathlist=$pathlist"
  set pl {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]  && [info exist current_dirname]} {
      set path_elem $current_dirname
    }
    lappend pl $path_elem
  }
  set i [lsearch -exact $pl $dir]
  if { $i>=0 } {
    set myload_files1 $pathlist
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 $i
    .dialog.l.paneleft.list selection set $myload_index1
  } else {
    set myload_files1 [list $dir]
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 0
    .dialog.l.paneleft.list selection set 0
  }
}
proc setglob {dir} {
      global globfilter myload_files2
      set myload_files2 [lsort [glob -nocomplain -directory $dir -tails -type d \{.*,*\}]]
      if { $globfilter eq {*}} {
        set myload_files2 ${myload_files2}\ [lsort [glob -nocomplain -directory $dir -tails -type {f} \{.*,$globfilter\}]]
      } else {
        set myload_files2 ${myload_files2}\ [lsort [glob -nocomplain -directory $dir -tails -type {f} \{$globfilter\}]]
      }
}

proc load_file_dialog {{msg {}} {ext {}} {global_initdir {INITIALINSTDIR}} {initialfile {}} {loadfile {1}} {confirm_overwrt {1}}} {
  global myload_index1 myload_files2 myload_files1 myload_retval myload_dir1 pathlist
  global myload_default_geometry myload_sash_pos myload_yview tcl_version globfilter myload_dirs2
  # return value
  set myload_retval {} 
  upvar #0 $global_initdir initdir
  if { [winfo exists .dialog] } return
  toplevel .dialog -class dialog
  wm title .dialog $msg
  set_ne myload_index1 0
  if { ![info exists myload_files1]} {
    set myload_files1 $pathlist
    set myload_index1 0
  }
  set_ne myload_files2 {}


  panedwindow  .dialog.l -orient horizontal

  frame .dialog.l.paneleft
  if {$tcl_version > 8.5} { set just {-justify right}} else {set just {}}
  eval [subst {listbox .dialog.l.paneleft.list -listvariable myload_files1 -width 20 -height 18 $just \
    -yscrollcommand ".dialog.l.paneleft.yscroll set" -selectmode browse \
    -xscrollcommand ".dialog.l.paneleft.xscroll set" -exportselection 0}]
  myload_set_colors1
  scrollbar .dialog.l.paneleft.yscroll -command ".dialog.l.paneleft.list yview" 
  scrollbar .dialog.l.paneleft.xscroll -command ".dialog.l.paneleft.list xview" -orient horiz
  pack  .dialog.l.paneleft.yscroll -side right -fill y
  pack  .dialog.l.paneleft.xscroll -side bottom -fill x
  pack  .dialog.l.paneleft.list -fill both -expand true
  bind .dialog.l.paneleft.list <<ListboxSelect>> { 
    # bind .dialog.l.paneright.pre <Expose> {}
    # .dialog.l.paneright.pre configure -background white
    set myload_sel [.dialog.l.paneleft.list curselection]
    if { $myload_sel ne {} } {
      set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_sel]]
      set myload_index1 $myload_sel
      setglob $myload_dir1
      myload_set_colors2
    }
  }

  frame .dialog.l.paneright
  frame .dialog.l.paneright.pre -background white -width 200 -height 200
  listbox .dialog.l.paneright.list  -listvariable myload_files2 -width 20 -height 18\
    -yscrollcommand ".dialog.l.paneright.yscroll set" -selectmode browswe \
    -xscrollcommand ".dialog.l.paneright.xscroll set" -exportselection 0
  scrollbar .dialog.l.paneright.yscroll -command ".dialog.l.paneright.list yview"
  scrollbar .dialog.l.paneright.xscroll -command ".dialog.l.paneright.list xview" -orient horiz
  pack .dialog.l.paneright.pre -side bottom -anchor s -fill x 
  pack  .dialog.l.paneright.yscroll -side right -fill y
  pack  .dialog.l.paneright.xscroll -side bottom -fill x
  pack  .dialog.l.paneright.list -side bottom  -fill both -expand true

  .dialog.l  add .dialog.l.paneleft -minsize 40
  .dialog.l  add .dialog.l.paneright -minsize 40
  # .dialog.l paneconfigure .dialog.l.paneleft -stretch always
  # .dialog.l paneconfigure .dialog.l.paneright -stretch always
  frame .dialog.buttons 
  button .dialog.buttons.ok -text OK -command { set myload_retval [.dialog.buttons.entry get]; destroy .dialog} 
  button .dialog.buttons.cancel -text Cancel -command {set myload_retval {}; destroy .dialog}
  button .dialog.buttons.home -text {Libs} -command {
    bind .dialog.l.paneright.pre <Expose> {}
    .dialog.l.paneright.pre configure -background white
    set myload_files1 $pathlist
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 0
    set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
    setglob $myload_dir1
    myload_set_colors2
    # .dialog.buttons.entry delete 0 end
    .dialog.l.paneleft.list selection clear 0 end
    .dialog.l.paneright.list selection clear 0 end
    .dialog.l.paneleft.list selection set $myload_index1
  }
  label .dialog.buttons.label  -text {File:}
  entry .dialog.buttons.entry
  if { $initialfile ne {} } { 
    .dialog.buttons.entry insert 0 $initialfile
  }
  radiobutton .dialog.buttons.all -text All -variable globfilter -value {*} -command { setglob $myload_dir1 }
  radiobutton .dialog.buttons.sym -text .sym -variable globfilter -value {*.sym} -command { setglob $myload_dir1 }
  radiobutton .dialog.buttons.sch -text .sch -variable globfilter -value {*.sch} -command { setglob $myload_dir1 }
  button .dialog.buttons.up -text UP -command {
    bind .dialog.l.paneright.pre <Expose> {}
    .dialog.l.paneright.pre configure -background white
    set d [file dirname $myload_dir1]
    if { [file isdirectory $d]} {
      myload_set_home $d
      setglob $d
      myload_set_colors2
      set myload_dir1 $d
      #.dialog.buttons.entry delete 0 end
    }
  }
  pack .dialog.buttons.ok .dialog.buttons.up .dialog.buttons.cancel \
       .dialog.buttons.home .dialog.buttons.label -side left
  pack .dialog.buttons.entry -side left -fill x -expand true
  pack .dialog.buttons.all .dialog.buttons.sym .dialog.buttons.sch -side left
  pack .dialog.l -expand true -fill both
  pack .dialog.buttons -side top -fill x
  if { [info exists myload_default_geometry]} {
     wm geometry .dialog "${myload_default_geometry}"
  }
  myload_set_home $initdir
  bind .dialog <Return> { 
    set myload_retval [.dialog.buttons.entry get]
    if {$myload_retval ne {} } {
      destroy .dialog
    }
  }
  bind .dialog.l.paneright.list <Double-Button-1> {
    set myload_retval [.dialog.buttons.entry get]
    if {$myload_retval ne {}  && ![file isdirectory "$myload_dir1/[.dialog.l.paneright.list get $myload_sel]"]} {
      bind .dialog.l.paneright.pre <Expose> {}
      destroy .dialog
    }
  }
  bind .dialog <Escape> { set myload_retval {}; destroy .dialog}

  ### update
  if { [ info exists myload_sash_pos] } {
    eval .dialog.l sash mark 0 [.dialog.l sash coord 0]
    eval .dialog.l sash dragto 0 [subst $myload_sash_pos]
  }
  ### update
  .dialog.l.paneleft.list xview moveto 1
  bind .dialog <Configure> {
    set myload_sash_pos [.dialog.l sash coord 0]
    set myload_default_geometry [wm geometry .dialog]
    .dialog.l.paneleft.list xview moveto 1
    # regsub {\+.*} $myload_default_geometry {} myload_default_geometry
  }

  bind .dialog.l.paneright.yscroll <Motion> {
    set myload_yview [.dialog.l.paneright.list yview]
  }

  xschem preview_window create .dialog.l.paneright.pre {}
  set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
  setglob $myload_dir1
  myload_set_colors2

  bind .dialog.l.paneright.list <ButtonPress> { 
    set myload_yview [.dialog.l.paneright.list yview]
  }
  bind .dialog.l.paneright.list <<ListboxSelect>> {
    set myload_yview [.dialog.l.paneright.list yview] 
    set myload_sel [.dialog.l.paneright.list curselection]
    if { $myload_sel ne {} } {
      set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
      set myload_dir2 [.dialog.l.paneright.list get $myload_sel]
      if {$myload_dir2 eq {..}} {
        set myload_d [file dirname $myload_dir1]
      } elseif { $myload_dir2 eq {.} } {
        set myload_d  $myload_dir1
      } else {
        if {$::OS == "Windows"} {
          if {[regexp {^[A-Za-z]\:/$} $myload_dir1]} {
            set myload_d "$myload_dir1$myload_dir2"
          } else {
            set myload_d "$myload_dir1/$myload_dir2"
          }
        } else {
          if {$myload_dir1 eq "/"} {
            set myload_d "$myload_dir1$myload_dir2"
          } else {
            set myload_d "$myload_dir1/$myload_dir2"
          }
        }
      }
      if { [file isdirectory $myload_d]} {
        bind .dialog.l.paneright.pre <Expose> {}
        .dialog.l.paneright.pre configure -background white
        myload_set_home $myload_d
        setglob $myload_d
        myload_set_colors2
        set myload_dir1 $myload_d
        # .dialog.buttons.entry delete 0 end
      } else {
        .dialog.buttons.entry delete 0 end
        .dialog.buttons.entry insert 0 $myload_dir2
         set t [is_xschem_file $myload_dir1/$myload_dir2]
         if { $t ne {0}  } {
	   ### update
           if { [winfo exists .dialog] } {
             .dialog.l.paneright.pre configure -background {}
             xschem preview_window draw .dialog.l.paneright.pre "$myload_dir1/$myload_dir2"
             bind .dialog.l.paneright.pre <Expose> {xschem preview_window draw .dialog.l.paneright.pre "$myload_dir1/$myload_dir2"}
           }
         } else {
           bind .dialog.l.paneright.pre <Expose> {}
           .dialog.l.paneright.pre configure -background white
         }
         # puts "xschem preview_window draw .dialog.l.paneright.pre \"$myload_dir1/$myload_dir2\""
      }
    }
  }
  if { [ info exists myload_yview]} {
   .dialog.l.paneright.list yview moveto  [lindex $myload_yview 0]
  }
  tkwait window .dialog
  xschem preview_window destroy {} {} 
  set initdir "$myload_dir1"
  if { $myload_retval ne {}} {
    if {![file exists "$myload_dir1/$myload_retval"] } {
      return "$myload_dir1/$myload_retval"
    }
    if { $loadfile == 0 } {
      if {[file exists "$myload_dir1/$myload_retval"]} {
        if {$confirm_overwrt == 1 } {
          set answer [tk_messageBox -message  "Overwrite $myload_dir1/${myload_retval}?" \
               -icon warning -parent . -type okcancel]
        } else {
          set answer ok
        }
        if {$answer eq {ok}} {
          return "$myload_dir1/$myload_retval"
        } else { 
          return {}
        }
      }
    }
    set t [is_xschem_file "$myload_dir1/$myload_retval"]
    if { $t eq {0}  } {
      set answer [tk_messageBox -message  "$myload_dir1/$myload_retval does not seem to be an xschem file...\nContinue?" \
           -icon warning -parent . -type yesno]
      if { $answer eq "no"} {
        set myload_retval {}
        return {}
      } else {
        return "$myload_dir1/$myload_retval"
      }
    } elseif { $t ne {SYMBOL} && ($ext eq {.sym}) } {
      set answer [tk_messageBox -message  "$myload_dir1/$myload_retval does not seem to be a SYMBOL file...\nContinue?" \
           -icon warning -parent . -type yesno]
      if { $answer eq "no"} {
        set myload_retval {}
        return {}
      } else {
        return "$myload_dir1/$myload_retval"
      }
    } else {
      return "$myload_dir1/$myload_retval"
    }
  } else {
    return {}
  }
}


# get last n path components: example , n=1 --> /aaa/bbb/ccc/ddd.sch -> ccc/ddd.sch
proc get_cell {s n } {
  set slist [file split $s]
  set l [llength $slist]
  if { $n >= $l } {set n [expr $l - 1]}
  set p {}
  for {set i [expr $l-1-$n]} {$i < $l} { incr i } {
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
  for {set i 0} {$i < [expr $l - $n]} { incr i } {
    append p [lindex $slist $i]
    if {$i < $l -$n- 1 && ([lindex $slist $i] ne {/})} {
      append p {/}
    }
  }
  return $p
}


proc delete_files { dir } { 
 if  { [ info tclversion]  >=8.4} {
   set x [tk_getOpenFile -title "DELETE FILES" -multiple 1 -initialdir [file dirname $dir] ]
 } else {
   set x [tk_getOpenFile -title "DELETE FILES" -initialdir [file dirname $dir] ]
 }
 foreach i  $x {
   file delete $i
 }
}

proc create_pins {} {
  global env retval USER_CONF_DIR
  global filetmp1 filetmp2

  set retval [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $retval {[} retval 
  regsub -all {>} $retval {]} retval 
  set lines [split $retval \n]
  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines { 
    if {$indirect} {
      puts $fd "C \{[rel_sym_path devices/[lindex $i 1].sym]\} 0 [set y [expr $y-20]]  0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{[rel_sym_path [lindex $i 1].sym]\} 0 [set y [expr $y-20]]  0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
    }
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
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
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
        puts $fd "G { $x0 $y0 } "
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
      lassign [rotation $x0 $y0 $linex1 $liney1 $rot $flip] linex1 liney1
      lassign [rotation $x0 $y0 $linex2 $liney2 $rot $flip] linex2 liney2
      lassign [order $linex1 $liney1 $linex2 $liney2] linex1 liney1 linex2 liney2
      lassign [rotation $x0 $y0 $textx0 $texty0 $rot $flip] textx0 texty0
      puts $fd "B 5 $pinx1 $piny1 $pinx2 $piny2 \{name=$lab dir=$dir\}"
      puts $fd "L 4 $linex1 $liney1 $linex2 $liney2 \{\}"
      puts $fd "T \{$lab\} $textx0 $texty0 $rot $textflip 0.2 0.2 \{\}"
    }
  }
  close $fd
  xschem paste
}


# 20120913
proc add_lab_no_prefix {} { 
  global env retval USER_CONF_DIR
  global filetmp1 filetmp2

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}


proc add_lab_prefix {} {
  global env retval USER_CONF_DIR
  global filetmp1 filetmp2

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}


proc make_symbol {name} {
 global XSCHEM_SHAREDIR symbol_width
 set name [abs_sym_path $name ]
 # puts "make_symbol{}, executing: ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width ${name}"
 eval exec {awk -f ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width $name}
 return {}
}

proc select_netlist_dir { force {dir {} }} {
#
# force==0: force creation of $netlist_dir (if not empty)
#           if netlist_dir empty and no dir given prompt user
#           else set netlist_dir to dir
#
# force==1: if no dir given prompt user
#           else set netlist_dir to dir
#
   global netlist_dir env

   if { ( $force == 0 )  && ( $netlist_dir ne {} ) } {
     if {![file exist $netlist_dir]} {
       file mkdir $netlist_dir
     }
     regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
     xschem set_netlist_dir $netlist_dir
     return $netlist_dir
   } 
   if { $dir eq {} } {
     if { $netlist_dir ne {} }  { 
       set initdir $netlist_dir
     } else {
       if {$::OS == "Windows"} {
         set initdir  $env(windir)
       } else {
         set initdir  $env(PWD) 
       }
     }
     # 20140409 do not change netlist_dir if user Cancels action
     set new_dir [tk_chooseDirectory -initialdir $initdir -parent . -title {Select netlist DIR} -mustexist false]
   } else {
     set new_dir $dir
   }

   if {$new_dir ne {} } {
     if {![file exist $new_dir]} {
       file mkdir $new_dir
     }
     set netlist_dir $new_dir  
   }
   regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
   xschem set_netlist_dir $netlist_dir
   return $netlist_dir
}

proc enter_text {textlabel {preserve_disabled disabled}} {
   global retval rcode has_cairo preserve_unchanged_attrs
   set rcode {}
   toplevel .t -class Dialog
   wm title .t {Enter text}

   set X [expr [winfo pointerx .t] - 30]
   set Y [expr [winfo pointery .t] - 25]

   # 20100203
   if { $::wm_fix } { tkwait visibility .t }
   wm geometry .t "+$X+$Y"
   frame .t.f1
   label .t.f1.txtlab -text $textlabel
   text .t.txt -width 100 -height 12
   .t.txt delete 1.0 end
   .t.txt insert 1.0 $retval
   checkbutton .t.f1.l1 -text "preserve unchanged props" -variable preserve_unchanged_attrs -state $preserve_disabled
   pack .t.f1 -side top -fill x -expand yes
   pack .t.f1.l1 -side left
   pack .t.f1.txtlab -side left -expand yes -fill x

   pack .t.txt -side top -fill  both -expand yes
   frame .t.edit
     frame .t.edit.lab
     frame .t.edit.entries
     pack  .t.edit.lab -side left 
     pack  .t.edit.entries -side left -fill x  -expand yes
     pack .t.edit  -side top  -fill x 
       if {$has_cairo } {
         entry .t.edit.entries.hsize -relief sunken -textvariable vsize -width 20
       } else {
         entry .t.edit.entries.hsize -relief sunken -textvariable hsize -width 20
       }
       entry .t.edit.entries.vsize -relief sunken -textvariable vsize -width 20
       entry .t.edit.entries.props -relief sunken -textvariable props -width 20 
       pack .t.edit.entries.hsize .t.edit.entries.vsize  \
        .t.edit.entries.props -side top  -fill x -expand yes
       label .t.edit.lab.hlab -text "hsize:"
       label .t.edit.lab.vlab -text "vsize:"
       label .t.edit.lab.proplab -text "props:"
       pack .t.edit.lab.hlab .t.edit.lab.vlab  \
        .t.edit.lab.proplab -side top
   frame .t.buttons
   button .t.buttons.ok -text "OK" -command  \
   {
    set retval [.t.txt get 1.0 {end - 1 chars}]
    if {$has_cairo} { 
      set hsize $vsize
    }
    set rcode {ok}
    destroy .t 
   }
   button .t.buttons.cancel -text "Cancel" -command  \
   {
    set retval {}
    set rcode {}
    destroy .t 
   }
   button .t.buttons.b3 -text "Load" -command \
   {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .t -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .t.txt  $a
     }
   }
   button .t.buttons.b4 -text "Del" -command \
   {
     .t.txt delete 1.0 end
   }

   pack .t.buttons.ok  -side left -fill x -expand yes
   pack .t.buttons.cancel  -side left -fill x -expand yes
   pack .t.buttons.b3  -side left -fill x -expand yes
   pack .t.buttons.b4  -side left -fill x -expand yes
   pack .t.buttons -side bottom -fill x
   bind .t <Escape> {
     if ![string compare $retval [.t.txt get 1.0 {end - 1 chars}]] {
       .t.buttons.cancel invoke
     }
   }
   bind .t <Control-Return> {.t.buttons.ok invoke}
   #grab set .t
   tkwait window .t
   return $retval
}

# evaluate a tcl command from GUI
proc tclcmd {} {
  global tclcmd_txt
  catch {destroy .tclcmd}
  toplevel .tclcmd -class dialog
  label .tclcmd.txtlab -text {Enter TCL expression:}
  label .tclcmd.result -text {Result:}
  text .tclcmd.t -width 100 -height 8
  text .tclcmd.r -width 100 -height 6 -yscrollcommand ".tclcmd.yscroll set" 
  scrollbar .tclcmd.yscroll -command  ".tclcmd.r yview"
  .tclcmd.t insert 1.0 $tclcmd_txt

  frame .tclcmd.b
  button .tclcmd.b.close -text Close -command {
    set tclcmd_txt [.tclcmd.t get 1.0 end]
    destroy .tclcmd
  }
  button .tclcmd.b.ok -text Evaluate -command {
    set tclcmd_txt [.tclcmd.t get 1.0 end]
    set res [eval $tclcmd_txt]
    .tclcmd.r delete 1.0 end
    .tclcmd.r insert 1.0 $res
  }
  pack .tclcmd.txtlab -side top -fill x
  pack .tclcmd.t -side top -fill  both -expand yes
  pack .tclcmd.result -side top -fill x
  pack .tclcmd.b -side bottom -fill x
  pack .tclcmd.yscroll -side right -fill y
  pack .tclcmd.r -side top -fill  both -expand yes
  pack .tclcmd.b.ok -side left -expand yes -fill x
  pack .tclcmd.b.close -side left -expand yes -fill x
}

proc select_layers {} {
   global dark_colorscheme colors enable_layer
   toplevel .sl -class dialog
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
   foreach i $colors {
     if { $dark_colorscheme == 1 } {
       set ind_bg white
     } else {
       set ind_bg black
     }
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
         set ind_bg black
       } else {
         set layfg black
         set ind_bg white
       }
     } elseif { $j == [xschem get gridlayer] } { ;# 20161206
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
         }
     pack .sl.f0.f$f.cb$j -side top -fill x
     incr j
     if { [expr $j%10] == 0 } {
       incr f
       frame .sl.f0.f$f
       pack .sl.f0.f$f -side left  -fill y
     }
   }
   tkwait window .sl
}

proc color_dim {} {
  toplevel .dim -class dialog
  wm title .dim {Dim colors}
  scale .dim.scale -digits 2 -label {Dim factor} -length 256 \
     -showvalue 1 -command {xschem color_dim} -orient horizontal \
     -from -5 -to 5 -resolution 0.1
  button .dim.ok -text OK -command {destroy .dim}
  .dim.scale set [xschem get dim]
  pack .dim.scale
  pack .dim.ok
}
proc about {} {

  if [winfo exists .about] { 
    bind .about.link <Button-1> {}
    bind .about.link2 <Button-1> {}
    destroy .about
  }
  toplevel .about -class dialog
  wm title .about {About XSCHEM}
  label .about.xschem -text "[xschem get version]" -font {Sans 24 bold}
  label .about.descr -text "Schematic editor / netlister for VHDL, Verilog, SPICE, tEDAx"
  button .about.link -text {http://repo.hu/projects/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link2 -text {https://github.com/StefanSchippers/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link3 -text {Online XSCHEM Manual} -font Underline-Font -fg blue -relief flat
  label .about.copyright -text "\n Copyright 1998-2020 Stefan Schippers (stefan.schippers@gmail.com) \n
 This is free software; see the source for copying conditions.  There is NO warranty;
 not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\n"
  button .about.close -text Close -command {destroy .about} -font {Sans 18}
  pack .about.xschem
  pack .about.link
  pack .about.link2
  pack .about.link3
  pack .about.descr
  pack .about.copyright
  pack .about.close
  bind .about.link <Button-1> { execute 0  xdg-open http://repo.hu/projects/xschem}
  bind .about.link2 <Button-1> { execute 0  xdg-open https://github.com/StefanSchippers/xschem}
  bind .about.link3 <Button-1> { execute 0  xdg-open http://repo.hu/projects/xschem/index.html}
}

proc property_search {} {
  global search_value
  global search_exact
  global search_select
  global custom_token

  if { [winfo exists .dialog] } return
  toplevel .dialog -class Dialog
  wm title .dialog {Search}
  ## 
  # 20100408
  set X [expr [winfo pointerx .dialog] - 60]
  set Y [expr [winfo pointery .dialog] - 35]
  wm geometry .dialog "+$X+$Y"
  frame .dialog.custom 
  label .dialog.custom.l -text "Token"
  entry .dialog.custom.e -width 32
  .dialog.custom.e insert 0 $custom_token
  pack .dialog.custom.e .dialog.custom.l -side right
  frame .dialog.val 
  label .dialog.val.l -text "Value"
  entry .dialog.val.e -width 32
  .dialog.val.e insert 0 $search_value
  pack .dialog.val.e .dialog.val.l -side right
  frame .dialog.but
  button .dialog.but.ok -text OK -command {
        set search_value [.dialog.val.e get]
        set custom_token [.dialog.custom.e get]
        if $tcl_debug<=-1 then { puts stderr "|$custom_token|" }
        set token $custom_token
        if { $search_exact==1 } {
          xschem searchmenu exact $search_select $token $search_value
        } else {
          xschem searchmenu regex $search_select $token $search_value
        }
        destroy .dialog 
  }
  button .dialog.but.cancel -text Cancel -command { destroy .dialog }
  
  # Window doesn't support regular expression, has to be exact match for now
  if {$::OS == "Windows"} {
    set search_exact 1 
    checkbutton .dialog.but.sub -text Exact_search -variable search_exact -state disable
  } else {
    checkbutton .dialog.but.sub -text Exact_search -variable search_exact 
  }
  radiobutton .dialog.but.nosel -text {Highlight} -variable search_select -value 0
  radiobutton .dialog.but.sel -text {Select} -variable search_select -value 1
  # 20171211 added unselect
  radiobutton .dialog.but.unsel -text {Unselect} -variable search_select -value -1
  pack .dialog.but.ok  -anchor w -side left
  pack .dialog.but.sub  -side left
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
  bind .dialog <Control-Return> {.dialog.but.ok invoke}
  grab set .dialog
  tkwait window .dialog
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
  # puts ">>>> $s $instname $symname"
  regsub {^@tcleval\(} $s {} s
  regsub {\)$} $s {} s
  return [eval $s]
}

#20171005
proc attach_labels_to_inst {} {
  global use_lab_wire use_label_prefix custom_label_prefix rcode do_all_inst rotated_text

  set rcode {}
  if { [winfo exists .dialog] } return
  toplevel .dialog -class Dialog
  wm title .dialog {Add labels to instances}

  # 20100408
  set X [expr [winfo pointerx .dialog] - 60]
  set Y [expr [winfo pointery .dialog] - 35]
  wm geometry .dialog "+$X+$Y"

  frame .dialog.custom 
  label .dialog.custom.l -text "Prefix"
  entry .dialog.custom.e -width 32
  .dialog.custom.e insert 0 $custom_label_prefix
  pack .dialog.custom.e .dialog.custom.l -side right


  frame .dialog.but
  button .dialog.but.ok -text OK -command {
        set custom_label_prefix [.dialog.custom.e get]
        set token $custom_token
        #### put command here
        set rcode yes
        destroy .dialog 
  }
  button .dialog.but.cancel -text Cancel -command { set rcode {}; destroy .dialog }
  checkbutton .dialog.but.wire -text {use wire labels} -variable use_lab_wire
  checkbutton .dialog.but.do_all -text {Do all} -variable do_all_inst
  label .dialog.but.rot -text {Rotated Text}
  entry .dialog.but.rotated -textvariable rotated_text -width 2
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
  bind .dialog <Control-Return> {.dialog.but.ok invoke}
  grab set .dialog
  tkwait window .dialog
  return {}
}

proc ask_save { {ask {save file?}} } {
   global rcode
   set rcode {}
   if { [winfo exists .dialog] } return
   toplevel .dialog -class Dialog
   wm title .dialog {Ask Save}

   set X [expr [winfo pointerx .dialog] - 60]
   set Y [expr [winfo pointery .dialog] - 35]

   # 20100203
   if { $::wm_fix } { tkwait visibility .dialog }

   wm geometry .dialog "+$X+$Y"
   label .dialog.l1  -text $ask
   frame .dialog.f1
   button .dialog.f1.b1 -text {Yes} -command\
   {
    set rcode {yes}
    destroy .dialog
   }
   button .dialog.f1.b2 -text {Cancel} -command\
   {
    set rcode {}
    destroy .dialog
   }
   button .dialog.f1.b3 -text {No} -command\
   {
    set rcode {no}
    destroy .dialog
   }
   pack .dialog.l1 .dialog.f1 -side top -fill x
   pack .dialog.f1.b1 .dialog.f1.b2 .dialog.f1.b3 -side left -fill x -expand yes
   bind .dialog <Escape> {.dialog.f1.b2 invoke}
   # needed, otherwise problems when descending with double clixk 23012004
   tkwait visibility .dialog
   grab set .dialog
   tkwait window .dialog
   return $rcode
}


proc edit_vi_prop {txtlabel} {
 global XSCHEM_TMP_DIR retval symbol prev_symbol rcode tcl_debug netlist_type editor
 global user_wants_copy_cell

 set user_wants_copy_cell 0
 set rcode {}
 set filename .xschem_edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix
 write_data $retval $XSCHEM_TMP_DIR/$filename
 eval execute_wait 0 $editor $XSCHEM_TMP_DIR/$filename ;# 20161119
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nretval=$retval\n---------\n"}
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nsymbol=$symbol\n---------\n"}
 set tmp [read_data $XSCHEM_TMP_DIR/$filename]
 file delete $XSCHEM_TMP_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $retval] {
        set retval $tmp
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}

proc edit_vi_netlist_prop {txtlabel} {
 global XSCHEM_TMP_DIR retval rcode tcl_debug netlist_type editor
 global user_wants_copy_cell

 set user_wants_copy_cell 0
 set filename .xschem_edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix
 regsub -all {\\?"} $retval {"} retval
 write_data $retval $XSCHEM_TMP_DIR/$filename
 if { [regexp vim $editor] } { set ftype "\{-c :set filetype=$netlist_type\}" } else { set ftype {} }
 eval execute_wait 0 $editor  $ftype $XSCHEM_TMP_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$retval\n---------\n"}
 set tmp [read_data $XSCHEM_TMP_DIR/$filename]
 file delete $XSCHEM_TMP_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $retval] {
        set retval $tmp
        regsub -all {\\?"} $retval {\\"} retval
        set retval \"${retval}\" 
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}

proc change_color {} {
  global colors dark_colors light_colors dark_colorscheme cadlayers ctrl_down USER_CONF_DIR svg_colors ps_colors

  set n [xschem get rectcolor]
  if { $n < 0 || $n >=$cadlayers} return
  if { $dark_colorscheme == 1 } {
    set c $dark_colors
  } else {
    set c $light_colors
  }
  set initial_color [lindex $c $n]
  set value [tk_chooseColor -initialcolor $initial_color]
  if {[string compare $value {}] } {
    set cc [lreplace $c $n $n $value]
    set colors $cc
    if { $dark_colorscheme == 1 } {
      set dark_colors $cc
    } else {
      set light_colors $cc
      regsub -all {"} $cc  {} ps_colors
      regsub -all {#} $ps_colors  {0x} ps_colors
    }
    regsub -all {"} $colors {} svg_colors
    regsub -all {#} $svg_colors {0x} svg_colors

    xschem change_colors
    set savedata "#### THIS FILE IS AUTOMATICALLY GENERATED BY XSCHEM, DO NOT EDIT.\n"
    set savedata "$savedata set cadlayers $cadlayers\n"
    set savedata "$savedata set light_colors {$light_colors}\n"
    set savedata "$savedata set dark_colors {$dark_colors}\n"
    write_data $savedata ${USER_CONF_DIR}/colors
  }
}

proc edit_prop {txtlabel} {
   global edit_prop_size infowindow_text selected_tok edit_symbol_prop_new_sel edit_prop_pos
   global prev_symbol retval symbol rcode no_change_attrs preserve_unchanged_attrs copy_cell tcl_debug
   global user_wants_copy_cell editprop_sympath retval_orig old_selected_tok
   set user_wants_copy_cell 0
   set rcode {}
   set retval_orig $retval
   if $tcl_debug<=-1 then {puts " edit_prop{}: retval=$retval"}
   if { [winfo exists .dialog] } return
   toplevel .dialog  -class Dialog 
   wm title .dialog {Edit Properties}
   set X [expr [winfo pointerx .dialog] - 60]
   set Y [expr [winfo pointery .dialog] - 35]

   # 20160325 change and remember widget size
   bind .dialog <Configure> { 
     # puts [wm geometry .dialog]
     if { $edit_symbol_prop_new_sel != 1 } {
       set geom [wm geometry .dialog]
       regsub {\+.*} $geom {} edit_prop_size
       regsub {[^+]*\+} $geom {+} edit_prop_pos
     }
   }
   wm geometry .dialog "${edit_prop_size}+$X+$Y"

   set prev_symbol $symbol
   set editprop_sympath [file dirname [abs_sym_path $symbol]]
   frame .dialog.f4
   label .dialog.f4.l1  -text $txtlabel
   label .dialog.f4.path  -text "Path:"
   entry .dialog.f4.e1  -textvariable editprop_sympath -width 0 -state readonly
   text .dialog.e1   -yscrollcommand ".dialog.yscroll set" -setgrid 1 \
                   -xscrollcommand ".dialog.xscroll set" -wrap none
     .dialog.e1 delete 1.0 end
     .dialog.e1 insert 1.0 $retval

   scrollbar .dialog.yscroll -command  ".dialog.e1 yview"
   scrollbar .dialog.xscroll -command ".dialog.e1 xview" -orient horiz
   frame .dialog.f1
   frame .dialog.f2
   label .dialog.f1.l2 -text "Symbol"
   entry .dialog.f1.e2 -width 30
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

     set retval [.dialog.e1 get 1.0 {end - 1 chars}]
     if { $selected_tok ne {<ALL>} } {
       regsub -all {\\?"} $retval {\\"} retval
       set retval \"${retval}\"
       set retval [xschem subst_tok $retval_orig $selected_tok $retval]
       set selected_tok {<ALL>}
     }

     set symbol [.dialog.f1.e2 get]
     set abssymbol [abs_sym_path $symbol]
     set rcode {ok}
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
     set rcode {}
     set edit_symbol_prop_new_sel {}
     destroy .dialog
   }

   wm protocol .dialog  WM_DELETE_WINDOW {
     set rcode {}
     set edit_symbol_prop_new_sel {}
     destroy .dialog
   }

   button .dialog.f1.b3 -text "Load" -command {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .dialog.e1  $a
     }
   }
   button .dialog.f1.b4 -text "Del" -command {
     .dialog.e1 delete 1.0 end
   }
   checkbutton .dialog.f2.r1 -text "No change properties" -variable no_change_attrs -state normal
   checkbutton .dialog.f2.r2 -text "Preserve unchanged props" -variable preserve_unchanged_attrs -state normal
   checkbutton .dialog.f2.r3 -text "Copy cell" -variable copy_cell -state normal
   set tok_list "<ALL> [xschem list_tokens $retval 0]"
   set selected_tok {<ALL>}
   set old_selected_tok {<ALL>}
   label .dialog.f2.r4 -text {   Edit Attr:}
   ttk::combobox .dialog.f2.r5 -values $tok_list -textvariable selected_tok -width 14

   pack .dialog.f1.l2 .dialog.f1.e2 .dialog.f1.b1 .dialog.f1.b2 .dialog.f1.b3 .dialog.f1.b4 .dialog.f1.b5 -side left -expand 1
   pack .dialog.f4 -side top  -anchor nw
   #pack .dialog.f4.path .dialog.f4.e1 .dialog.f4.l1 -side left -fill x 
   pack .dialog.f4.path -side left
   pack .dialog.f4.e1 -side left 
   pack .dialog.f1 .dialog.f2 -side top -fill x 
   pack .dialog.f2.r1 -side left
   pack .dialog.f2.r2 -side left
   pack .dialog.f2.r3 -side left
   pack .dialog.f2.r4 -side left
   pack .dialog.f2.r5 -side left
   pack .dialog.yscroll -side right -fill y 
   pack .dialog.xscroll -side bottom -fill x
   pack .dialog.e1  -fill both -expand yes
   bind .dialog <Control-Return> {.dialog.f1.b1 invoke}
   bind .dialog <Escape> {
     if { ![string compare $retval [.dialog.e1 get 1.0 {end - 1 chars}]] && \
          ![string compare $symbol [ .dialog.f1.e2 get]] } {
       .dialog.f1.b2 invoke
     }
   }

   bind .dialog.f2.r5 <<ComboboxSelected>> {
     if {$old_selected_tok ne $selected_tok} {
       if { $old_selected_tok eq {<ALL>} } {
         set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
       } else {
         set retval [.dialog.e1 get 1.0 {end - 1 chars}]
         regsub -all {\\?"} $retval {\\"} retval
         set retval \"${retval}\"
         set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
       }
     }
     if {$selected_tok eq {<ALL>} } { 
       set retval $retval_orig
     } else {
       set retval [xschem get_tok $retval_orig $selected_tok]
       regsub -all {\\?"} $retval {"} retval
     }
     .dialog.e1 delete 1.0 end
     .dialog.e1 insert 1.0 $retval
     set old_selected_tok $selected_tok
   }

   bind .dialog.f2.r5 <KeyRelease> {
     set selected_tok [.dialog.f2.r5 get]
     if { $old_selected_tok eq {<ALL>}} {
       set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
     } else {
       set retval [.dialog.e1 get 1.0 {end - 1 chars}]
       if {$retval ne {}} {
         regsub -all {\\?"} $retval {\\"} retval
         set retval \"${retval}\"
         set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
       }
     }
     if {$selected_tok eq {<ALL>} } {
       set retval $retval_orig
     } else {
       set retval [xschem get_tok $retval_orig $selected_tok]
       regsub -all {\\?"} $retval {"} retval
     }
     .dialog.e1 delete 1.0 end
     .dialog.e1 insert 1.0 $retval
     set old_selected_tok $selected_tok
   }


   if {$edit_symbol_prop_new_sel == 1} { 
     wm geometry .dialog $edit_prop_pos
   }
   set edit_symbol_prop_new_sel 0
   tkwait window .dialog
   return $rcode
}

proc read_data_nonewline {f} {
 set fid [open $f "r"]
 set data [read -nonewline $fid]
 close $fid
 return $data
}

proc read_data {f} {
 set fid [open $f "r"]
 set data [read $fid]
 close $fid
 return $data
}

proc read_data_window {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 #  $w delete 0.0 end
 ## 20171103 insert text at cursor position instead of at beginning (insert index tag)
 $w insert insert $t
 close $fid
}

proc write_data {data f} {
 set fid [open $f "w"]
 puts  -nonewline $fid $data
 close $fid
 return {}
}

proc text_line {txtlabel clear {preserve_disabled disabled} } {
   global text_line_default_geometry preserve_unchanged_attrs
   global retval rcode tcl_debug tok_list selected_tok retval_orig old_selected_tok
   set retval_orig $retval
   if $clear==1 then {set retval ""}
   if $tcl_debug<=-1 then {puts " text_line{}: clear=$clear"}
   if $tcl_debug<=-1 then {puts " text_line{}: retval=$retval"}
   set rcode {}
   if { [winfo exists .dialog] } return
   toplevel .dialog  -class Dialog
   wm title .dialog {Text input}
   set X [expr [winfo pointerx .dialog] - 60]
   set Y [expr [winfo pointery .dialog] - 35]

   set tok_list "<ALL> [xschem list_tokens $retval 0]"
   set selected_tok {<ALL>}
   set old_selected_tok {<ALL>}
   # 20160325 change and remember widget size
   bind .dialog <Configure> {
     # puts [wm geometry .dialog]
     set geom [wm geometry .dialog]
     regsub {\+.*} $geom {} geom
     set text_line_default_geometry $geom
   }

   # 20100203
   if { $::wm_fix } { tkwait visibility .dialog }
   wm geometry .dialog "${text_line_default_geometry}+$X+$Y"

   frame .dialog.f0
   frame .dialog.f1
   label .dialog.f0.l1  -text $txtlabel

   text .dialog.e1 -relief sunken -bd 2 -yscrollcommand ".dialog.yscroll set" -setgrid 1 \
        -xscrollcommand ".dialog.xscroll set" -wrap none -width 90 -height 40
   scrollbar .dialog.yscroll -command  ".dialog.e1 yview"
   scrollbar .dialog.xscroll -command ".dialog.e1 xview" -orient horiz
   .dialog.e1 delete 1.0 end
   .dialog.e1 insert 1.0 $retval
   button .dialog.f1.b1 -text "OK" -command  \
   {
     set retval [.dialog.e1 get 1.0 {end - 1 chars}]
     if { $selected_tok ne {<ALL>} } {
       regsub -all {\\?"} $retval {\\"} retval
       set retval \"${retval}\"
       set retval [xschem subst_tok $retval_orig $selected_tok $retval]
       set selected_tok {<ALL>}
     }
     destroy .dialog
     set rcode {ok}
   }
   button .dialog.f1.b2 -text "Cancel" -command  \
   {
     set retval [.dialog.e1 get 1.0 {end - 1 chars}]
     set rcode {}
     destroy .dialog
   }
   button .dialog.f1.b3 -text "Load" -command \
   {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .dialog.e1  $a
     }
   }
   button .dialog.f1.b4 -text "Del" -command \
   {
     .dialog.e1 delete 1.0 end
   }
   label .dialog.f1.r4 -text {   Edit Attr:}
   ttk::combobox .dialog.f1.r5 -values $tok_list -textvariable selected_tok -width 14

   checkbutton .dialog.f0.l2 -text "preserve unchanged props" -variable preserve_unchanged_attrs -state $preserve_disabled
   pack .dialog.f0 -fill x
   pack .dialog.f0.l2 -side left
   pack .dialog.f0.l1 -side left -expand yes
   pack .dialog.f1  -fill x
   pack .dialog.f1.b1 -side left -fill x -expand yes
   pack .dialog.f1.b2 -side left -fill x -expand yes
   pack .dialog.f1.b3 -side left -fill x -expand yes
   pack .dialog.f1.b4 -side left -fill x -expand yes
   pack .dialog.f1.r4 -side left
   pack .dialog.f1.r5 -side left
 

   pack .dialog.yscroll -side right -fill y 
   pack .dialog.xscroll -side bottom -fill x
   pack .dialog.e1   -expand yes -fill both
   bind .dialog <Escape> {
     if ![string compare $retval [.dialog.e1 get 1.0 {end - 1 chars}]] {
       .dialog.f1.b2 invoke
     }
   }

   bind .dialog.f1.r5 <<ComboboxSelected>> {
     if {$old_selected_tok ne $selected_tok} {
       if { $old_selected_tok eq {<ALL>} } {
         set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
       } else {
         set retval [.dialog.e1 get 1.0 {end - 1 chars}]
         regsub -all {\\?"} $retval {\\"} retval
         set retval \"${retval}\"
         set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
       }
     }
     if {$selected_tok eq {<ALL>} } {
       set retval $retval_orig
     } else {
       set retval [xschem get_tok $retval_orig $selected_tok]
       regsub -all {\\?"} $retval {"} retval
     }
     .dialog.e1 delete 1.0 end
     .dialog.e1 insert 1.0 $retval
     set old_selected_tok $selected_tok
   }

   bind .dialog.f1.r5 <KeyRelease> {
     set selected_tok [.dialog.f1.r5 get]
     if { $old_selected_tok eq {<ALL>}} {
       set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
     } else {
       set retval [.dialog.e1 get 1.0 {end - 1 chars}]
       if {$retval ne {}} {
         regsub -all {\\?"} $retval {\\"} retval
         set retval \"${retval}\"
         set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
       }
     }
     if {$selected_tok eq {<ALL>} } {
       set retval $retval_orig
     } else {
       set retval [xschem get_tok $retval_orig $selected_tok]
       regsub -all {\\?"} $retval {"} retval
     }
     .dialog.e1 delete 1.0 end
     .dialog.e1 insert 1.0 $retval
     set old_selected_tok $selected_tok
   }

   bind .dialog <Control-Return> {.dialog.f1.b1 invoke}
   #tkwait visibility .dialog
   #grab set .dialog
   #focus .dialog.e1

   # 20100208
   set rcode {}   

   tkwait window .dialog
   return $rcode
}

proc alert_ {txtlabel {position +200+300} {nowait {0}}} {
   toplevel .alert -class Dialog
   wm title .alert {Alert}
   set X [expr [winfo pointerx .alert] - 60]
   set Y [expr [winfo pointery .alert] - 60]

   if { [string compare $position ""] != 0 } {
     wm geometry .alert $position
   } else {
     wm geometry .alert "+$X+$Y"
   }
   label .alert.l1  -text $txtlabel -wraplength 700
   button .alert.b1 -text "OK" -command  \
   {
     destroy .alert
   } 
   pack .alert.l1 -side top -fill x
   pack .alert.b1 -side top -fill x
   tkwait visibility .alert
   grab set .alert
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
   return {}
}


proc infowindow {} {
 global infowindow_text

 set infotxt $infowindow_text
 if { $infowindow_text ne {}} {append infotxt \n}
 set z {.infotext}

  if ![string compare $infotxt ""] { 
     if [winfo exists $z] {
       $z.text delete 1.0 end
     }
  }
  if ![winfo exists $z] {
   toplevel $z
   wm title $z {Info window}
   wm  geometry $z 90x24+0+400
   wm iconname $z {Info window}
   wm withdraw $z
   wm protocol .infotext WM_DELETE_WINDOW {wm withdraw .infotext; set show_infowindow 0}
#   button $z.dismiss -text Dismiss -command  "destroy $z"
   text $z.text -relief sunken -bd 2 -yscrollcommand "$z.yscroll set" -setgrid 1 \
        -height 6 -width 50  -xscrollcommand "$z.xscroll set" -wrap none
   scrollbar $z.yscroll -command "$z.text yview" -orient v 
   scrollbar $z.xscroll -command "$z.text xview" -orient h 
   grid $z.text - $z.yscroll -sticky nsew
   grid $z.xscroll - -sticky ew
#   grid $z.dismiss - -
   grid rowconfig $z 0 -weight 1
   grid columnconfig $z 0 -weight 1
  }
#  $z.text delete 1.0 end 
#  $z.text insert 1.0 $infotxt
  $z.text insert end $infotxt
  $z.text see end
  bind $z <Escape> {wm withdraw .infotext; set show_infowindow 0}
  return {}
}
proc textwindow {filename {ro {}}} {
   global textwindow_wcounter
   global textwindow_w
   # set textwindow_w .win$textwindow_wcounter
   # catch {destroy $textwindow_w}
   set textwindow_wcounter [expr $textwindow_wcounter+1]
   set textwindow_w .win$textwindow_wcounter


   global fff
   global fileid
   set fff $filename
   toplevel $textwindow_w
   wm title $textwindow_w $filename
   wm iconname $textwindow_w $filename
  frame $textwindow_w.buttons
   pack $textwindow_w.buttons -side bottom -fill x -pady 2m
   button $textwindow_w.buttons.dismiss -text Dismiss -command "destroy $textwindow_w"
   pack $textwindow_w.buttons.dismiss -side left -expand 1
   if { $ro eq {} } {
     button $textwindow_w.buttons.save -text "Save" -command \
      { 
       set fileid [open $fff "w"]
       puts -nonewline $fileid [$textwindow_w.text get 1.0 {end - 1 chars}]
       close $fileid 
       destroy $textwindow_w
      }
     pack $textwindow_w.buttons.save  -side left -expand 1
   }

   text $textwindow_w.text -relief sunken -bd 2 -yscrollcommand "$textwindow_w.yscroll set" -setgrid 1 \
        -xscrollcommand "$textwindow_w.xscroll set" -wrap none -height 30
   scrollbar $textwindow_w.yscroll -command  "$textwindow_w.text yview" 
   scrollbar $textwindow_w.xscroll -command "$textwindow_w.text xview" -orient horiz
   pack $textwindow_w.yscroll -side right -fill y
   pack $textwindow_w.text -expand yes -fill both
   pack $textwindow_w.xscroll -side bottom -fill x
   bind $textwindow_w <Escape> "$textwindow_w.buttons.dismiss invoke"
   set fileid [open $filename "r"]

   # 20171103 insert at insertion cursor(insert tag) instead of 0.0
   $textwindow_w.text insert insert [read $fileid]
   close $fileid
   return {}
}


proc viewdata {data {ro {}}} {
   global viewdata_wcounter  rcode
   global viewdata_w
   # set viewdata_w .view$viewdata_wcounter
   # catch {destroy $viewdata_w}
   set viewdata_wcounter [expr $viewdata_wcounter+1]
   set viewdata_w .view$viewdata_wcounter
   set rcode {}
   toplevel $viewdata_w
   wm title $viewdata_w {View data}
   frame $viewdata_w.buttons
   pack $viewdata_w.buttons -side bottom -fill x -pady 2m

   button $viewdata_w.buttons.dismiss -text Dismiss -command  "destroy $viewdata_w" 
   pack $viewdata_w.buttons.dismiss -side left -expand 1

   if { $ro eq {} } {
     button $viewdata_w.buttons.saveas -text {Save As} -command  {
       if {$::OS == "Windows"} {
         set fff [tk_getSaveFile -initialdir $env(windir) ]
       } else {
         set fff [tk_getSaveFile -initialdir $env(PWD) ]
       }
       if { $fff != "" } {
         set fileid [open $fff "w"]
         puts -nonewline $fileid [$viewdata_w.text get 1.0 {end - 1 chars}]
         close $fileid
       }
     } 
     pack $viewdata_w.buttons.saveas  -side left -expand 1
   }

   text $viewdata_w.text -relief sunken -bd 2 -yscrollcommand "$viewdata_w.yscroll set" -setgrid 1 \
        -xscrollcommand "$viewdata_w.xscroll set" -wrap none -height 30
   scrollbar $viewdata_w.yscroll -command  "$viewdata_w.text yview" 
   scrollbar $viewdata_w.xscroll -command "$viewdata_w.text xview" -orient horiz
   pack $viewdata_w.yscroll -side right -fill y
   pack $viewdata_w.text -expand yes -fill both
   pack $viewdata_w.xscroll -side bottom -fill x
   # 20171103 insert at insertion cursor(insert tag) instead of 0.0
   $viewdata_w.text insert insert $data
   return $rcode
}

# given an absolute path of a symbol/schematic remove the path prefix
# if file is in a library directory (a $pathlist dir)
proc rel_sym_path {symbol} {
  global pathlist current_dirname

  set name {}
  if {[regexp {^/} $symbol] || [regexp {^[A-Za-z]:} $symbol]} {set symbol [file normalize $symbol]}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]  && [info exist current_dirname]} {
      set path_elem $current_dirname
    }
    set pl [string length $path_elem]
    if { [string equal -length $pl $path_elem $symbol] } {
      set name [string range $symbol [expr $pl+1] end]
      break
    }
  }
  if { ![string compare $name {} ] } {
    # no known lib, so return full path
    set name ${symbol}
  }
  return $name
}


# given a library/symbol return its absolute path
proc abs_sym_path {fname {ext {} } } {
  global pathlist current_dirname

  if {$fname eq {} } return {}
  if {$::OS == "Windows"} {
    if { [regexp {^[A-Za-z]\:/$} $fname ] } {
      return $fname;
    } 
  } else {
    if { $fname eq "/"} {
      return $fname;
    }
  }
  # add extension for 1.0 file format compatibility
  if { $ext ne {} } { 
    set fname [file rootname $fname]$ext
  }
  # transform ./file_or_path to file_or_path, resolve (normalize) ../file_or_path
  if { [regexp {^\.\./} $fname ] } {
    set fname [file normalize $fname]
  } elseif {[regexp {^\./} $fname ] } {
    regsub {^\./} $fname {} fname
  }
  set name {}
  if { ![regexp {^/} $fname] && ![regexp {^[A-Za-z]:} $fname] } {
    foreach path_elem $pathlist {
      if { ![string compare $path_elem .]  && [info exist current_dirname]} {
        set path_elem $current_dirname
      }
      set fullpath "$path_elem/$fname"
      if { [file exists $fullpath] } {
        set name $fullpath
        break
      }
    }
  }
  if { ![string compare $name {}] } {
    if { [regexp {^/} $fname] || [regexp {^[a-zA-Z]:} $fname] } {
      set name $fname
    } else {
      set name "$current_dirname/$fname"
    }
  }
  regsub {/\.$} $name {} name
  return $name
}

proc add_ext {fname ext} {
#  if {![string match "*$ext" $fname]} {
#    set fname ${fname}$ext
#  }
#  return $fname
  return [file rootname $fname]$ext
}
 

proc input_line {txt {cmd {}} {preset {}}  {w 12}} {
  global input_line_cmd input_line_data
  set input_line_data {}
  if { [winfo exists .dialog] } return
  toplevel .dialog -class Dialog
  wm title .dialog {Input number}
  set X [expr [winfo pointerx .dialog] - 60]
  set Y [expr [winfo pointery .dialog] - 35]
  # 20100203
  if { $::wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "+$X+$Y"
  set input_line_cmd $cmd
  frame .dialog.f1
  label .dialog.f1.l -text $txt
  entry .dialog.f1.e -width $w
  .dialog.f1.e insert 0 $preset
  .dialog.f1.e selection range 0 end
  
  pack .dialog.f1.l .dialog.f1.e -side left
  frame .dialog.f2
  button .dialog.f2.ok -text OK  -command {
    if {$input_line_cmd ne {}} {
      eval [subst -nocommands {$input_line_cmd [.dialog.f1.e get]}]
    }
    set input_line_data [.dialog.f1.e get]
    destroy .dialog
  }
  button .dialog.f2.cancel -text Cancel -command { destroy .dialog }
  pack .dialog.f2.ok  -anchor w -side left
  pack .dialog.f2.cancel -anchor e
  pack .dialog.f1
  pack .dialog.f2 -expand yes -fill x
  bind .dialog <Escape> {.dialog.f2.cancel invoke}
  bind .dialog <Return> {.dialog.f2.ok invoke}
  grab set .dialog
  focus .dialog.f1.e
  tkwait window .dialog
  return $input_line_data
}

## 20161102
proc launcher {} {
  # XSCHEM_SHAREDIR and netlist_dir not used directly but useful in paths passed thru launcher_var
  global launcher_var launcher_default_program launcher_program env XSCHEM_SHAREDIR netlist_dir
  
  ## puts ">>> $launcher_program $launcher_var "
  # 20170413
  if { ![string compare $launcher_program {}] } { set launcher_program $launcher_default_program}
  eval exec  [subst $launcher_program] {[subst $launcher_var]} &
}

proc reconfigure_layers_button {} {
   global colors dark_colorscheme
   set c [xschem get rectcolor]
   .menubar.layers configure -background [lindex $colors $c]
   if { $dark_colorscheme == 1 && $c == 0} {
     .menubar.layers configure -foreground white
   } else {
     .menubar.layers configure -foreground black
   }
}

proc reconfigure_layers_menu {} {
   global colors dark_colorscheme
   set j 0
   foreach i $colors {
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
     .menubar.layers.menu entryconfigure $j -activebackground $i \
        -background $i -foreground $layfg -activeforeground $layfg
     incr j
   }
   reconfigure_layers_button
}

proc get_file_path {ff} {
  global env
  # Absolute path ? return as is.
  #        Linux                Windows
  if { [regexp {^/} $ff] || [regexp {^[a-zA-Z]:} $ff] } { return $ff }
  if {$::OS == "Windows"} {
    set pathlist [split $env(PATH) \;]
  } else {
    set pathlist [split $env(PATH) :]
  }
  foreach i $pathlist {
    set ii $i/$ff
    if { [file exists $ii]} {return $ii}
  }
  # nothing found, return $ff as is and hope for the best :-)
  return $ff
}

###
### ToolBar
###
#
# Public variables that we allow to be overridden
#

set_ne toolbar_visible 0
set_ne toolbar_horiz   1
set_ne toolbar_list { 
    FileNew
    FileNewSym
    FileOpen
    FileSave
    FileMerge
    FileReload
    "---"
    EditUndo
    EditRedo
    EditCut
    EditCopy
    EditPaste
    EditDelete
    "---"
    EditDuplicate
    EditMove
    "---"
    EditPushSch
    EditPushSym
    EditPop
    "---"
    ViewRedraw
    ViewZoomIn
    ViewZoomOut
    ViewZoomBox
    ViewToggleColors
    "---"
    ToolInsertSymbol
    ToolInsertText
    ToolInsertWire
    ToolInsertLine
    ToolInsertRect
    ToolInsertPolygon
    ToolInsertArc
    ToolInsertCircle
    "---"
    ToolSearch
    "---"
    ToolJoinTrim
    ToolBreak
}
#
# Balloon help system, from https://wiki.tcl-lang.org/page/balloon+help
#
proc balloon {w help} {
    bind $w <Any-Enter> "after 1000 [list balloon_show %W [list $help]]"
    bind $w <Any-Leave> "destroy %W.balloon"
}

proc balloon_show {w arg} {
    if {[eval winfo containing  [winfo pointerxy .]]!=$w} {return}
    set top $w.balloon
    catch {destroy $top}
    toplevel $top -bd 1 -bg black
    wm overrideredirect $top 1
    if {[string equal [tk windowingsystem] aqua]}  {
        ::tk::unsupported::MacWindowStyle style $top help none
    }   
    pack [message $top.txt -aspect 10000 -bg lightyellow \
        -font fixed -text $arg]
    set wmx [winfo rootx $w]
    set wmy [expr [winfo rooty $w]+[winfo height $w]]
    wm geometry $top [winfo reqwidth $top.txt]x[
        winfo reqheight $top.txt]+$wmx+$wmy
    raise $top
}
#
# Pull in the toolbar graphics resources
#
if {![info exists no_x] } {source $XSCHEM_SHAREDIR/resources.tcl}
#
# Separation bar counter
#
set toolbar_sepn 0
#
# Toolbar constructor
#
proc toolbar_toolbar {} {
    frame .toolbar -relief raised -bd 1 -bg white
}
#
# Create a tool button which may be displayed
#
proc toolbar_create {name cmd { help "" } } {
    button .toolbar.b$name -image img$name -relief flat -bd 3 -bg white -fg white -command $cmd
    if { $help == "" } { balloon .toolbar.b$name $name } else { balloon .toolbar.b$name $help }
}
#
# Show the toolbar in horizontal or vertical position, parsing the toolbar list and 
# adding any separators as needed.
#
proc toolbar_show {} {
    global toolbar_horiz toolbar_list toolbar_sepn toolbar_visible
    if { ! $toolbar_visible } { return }
    if { $toolbar_horiz } { 
        pack .toolbar -fill x -before .drw
    } else {
        pack .toolbar -side left -anchor w -fill y -before .drw
    }
    set pos "top"
    if { $toolbar_horiz } { set pos "left" }
    set tlist [ winfo children .toolbar ]
    foreach b $toolbar_list {
        if { $b == "---" } {
            if { $toolbar_horiz } {
                frame .toolbar.sep$toolbar_sepn -bg lightgrey -width 2
                pack .toolbar.sep$toolbar_sepn -side $pos -padx 1 -pady 1 -fill y
            } else {
                frame .toolbar.sep$toolbar_sepn -bg lightgrey -height 2
                pack .toolbar.sep$toolbar_sepn -side $pos -padx 1 -pady 1 -fill x
            }
            incr toolbar_sepn
        } else {
            if { [ lsearch -exact $tlist ".toolbar.b$b" ] != -1 } {
                pack .toolbar.b$b -side $pos
            } else {
                puts "Error: unknown toolbar item \"$b\""
            }
        }
    }
    set pos "bottom"
    if { $toolbar_horiz } { set pos "right" }
    foreach b { Waves Simulate Netlist } {
        pack .toolbar.b$b -side $pos
    }
    set $toolbar_visible 1
}
#
# Hide the toolbar, unpack the buttons, and remove any separators
#
proc toolbar_hide {} {
    global toolbar_sepn toolbar_visible
    set tlist [ winfo children .toolbar ]
    foreach b $tlist {
        pack forget $b
        if { [ string match ".toolbar.sep*" $b ] == 1 } { 
            destroy $b
        }
    }
    set toolbar_sepn 0
    pack forget .toolbar
    set $toolbar_visible 0
}


## this function sets up all tk windows and binds X events. It is executed by xinit.c after completing 
## all X initialization. This avoids race conditions.
## In previous flow xschem.tcl was setting up windows and events before X initialization was completed by xinit.c.
## this could lead to crashes on some (may be slow) systems due to Configure/Expose events being delivered
## before xschem being ready to handle them.
proc build_windows {} {
  global env
  if { ( $::OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 )
     && ![info exists no_x]} {
    pack .statusbar.2 -side left 
    pack .statusbar.3 -side left 
    pack .statusbar.4 -side left 
    pack .statusbar.5 -side left 
    pack .statusbar.6 -side left 
    pack .statusbar.7 -side left 
    pack .statusbar.8 -side left 
    pack .statusbar.1 -side left -fill x
    pack .drw -anchor n -side top -fill both -expand true
    pack .menubar -anchor n -side top -fill x  -before .drw
    toolbar_show
    pack .statusbar -after .drw -anchor sw  -fill x 
    bind .statusbar.5 <Leave> { xschem set cadgrid $grid; focus .drw}
    bind .statusbar.3 <Leave> { xschem set cadsnap $snap; focus .drw}
    ###
    ### Tk event handling
    ###

    #    bind . <Enter> {
    #      if { [winfo exists .dialog] && [winfo ismapped .dialog] && [winfo ismapped .] && [wm stackorder .dialog isbelow . ]} {
    #        raise .dialog .drw 
    #      }
    #    }
    bind . <Expose> {
      if { [winfo exists .dialog] && [winfo ismapped .dialog] && [winfo ismapped .] && [wm stackorder .dialog isbelow . ]} {
        raise .dialog .drw 
      }
    }
    bind . <Visibility> {
      if { [winfo exists .dialog] && [winfo ismapped .dialog] && [winfo ismapped .] && [wm stackorder .dialog isbelow . ]} {
        raise .dialog .drw
      }
    }
    bind . <FocusIn> {
      if { [winfo exists .dialog] && [winfo ismapped .dialog] && [winfo ismapped .] && [wm stackorder .dialog isbelow . ]} {
        raise .dialog .drw
      }
    }

    bind .drw <Double-Button-1> {xschem callback -3 %x %y 0 %b 0 %s}
    bind .drw <Double-Button-2> {xschem callback -3 %x %y 0 %b 0 %s}
    bind .drw <Double-Button-3> {xschem callback -3 %x %y 0 %b 0 %s}
    bind .drw <Expose> {xschem callback %T %x %y 0 %w %h %s}
    bind .drw <Configure> {xschem windowid; xschem callback %T %x %y 0 %w %h 0}
    bind .drw <ButtonPress> {xschem callback %T %x %y 0 %b 0 %s}
 
    if {$::OS == "Windows"} {
      bind .drw <MouseWheel> {
        if {%D<0} {
          xschem callback 4 %x %y 0 5 0 %s
        } else {
          xschem callback 4 %x %y 0 4 0 %s
        }
      }
    }
 
    bind .drw <ButtonRelease> {xschem callback %T %x %y 0 %b 0 %s}

    # on Windows Alt key mask is reported as 131072 (1<<17) so build masks manually with values passed from C code 
    if {$::OS == "Windows" } {
      bind .drw <Alt-KeyPress> {xschem callback %T %x %y %N 0 0 [expr $Mod1Mask]}
      bind .drw <Control-Alt-KeyPress> {xschem callback %T %x %y %N 0 0 [expr $ControlMask + $Mod1Mask]}
      bind .drw <Shift-Alt-KeyPress> {xschem callback %T %x %y %N 0 0 [expr $ShiftMask + $Mod1Mask]}
    }

    bind .drw <KeyPress> {xschem callback %T %x %y %N 0 0 %s}
    bind .drw <KeyRelease> {xschem callback %T %x %y %N 0 0 %s} ;# 20161118
    bind .drw <Motion> {xschem callback %T %x %y 0 0 0 %s}
    bind .drw  <Enter> {xschem callback %T %x %y 0 0 0 0 }
    bind .drw <Leave> {}
    bind .drw <Unmap> {
     wm withdraw .infotext
     set show_infowindow 0
    }
    bind .drw  "?" { textwindow "${XSCHEM_SHAREDIR}/xschem.help" }
  }
}

### 
###   MAIN PROGRAM
###

set OS [lindex $tcl_platform(os) 0]

# tcl variable XSCHEM_LIBRARY_PATH  should already be set in xschemrc
set pathlist {}
if { [info exists XSCHEM_LIBRARY_PATH] } {
  if {$::OS == "Windows"} {
    set pathlist_orig [split $XSCHEM_LIBRARY_PATH \;]
  } else {
    set pathlist_orig [split $XSCHEM_LIBRARY_PATH :]
  }
  foreach i $pathlist_orig {
    regsub {^~} $i $env(HOME) i
    if { ![string compare $i .] } {
      lappend pathlist $i
    } elseif { [ file exists $i] } {
      lappend pathlist [file normalize ${i}]
    }
  }
}

if { [xschem get help ]} {
  set fd [open "${XSCHEM_SHAREDIR}/xschem.help" r]
  set helpfile [read $fd]
  puts $helpfile
  close $fd
  exit
}

if {$::OS == "Windows"} {
  set_ne XSCHEM_TMP_DIR [xschem get temp_dir]
} else {
  set_ne XSCHEM_TMP_DIR {/tmp}
}

# used in C code
set_ne xschem_libs {}
set_ne tcl_debug 0
# used to activate debug from menu
set_ne menu_tcl_debug 0
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

set_ne dircolor(/share/xschem/) red
set_ne dircolor(/share/doc/xschem/) {#338844}

set_ne globfilter {*}
## list of tcl procedures to load at end of xschem.tcl
set_ne tcl_files {}
set_ne netlist_dir "$USER_CONF_DIR/simulations"
set_ne user_top_netl_name {}
set_ne bus_replacement_char {} ;# use {<>} to replace [] with <> in bussed signals
set_ne hspice_netlist 0
set_ne top_subckt 0
set_ne spiceprefix 1
set_ne verilog_2001 1
set_ne split_files 0
set_ne flat_netlist 0
set_ne netlist_type vhdl
set_ne netlist_show 0
set_ne color_ps 0
set_ne only_probes 0  ; # 20110112
set_ne a3page 0
set_ne fullscreen 0
set_ne unzoom_nodrift 1
set_ne change_lw 0
set_ne draw_window 0
set_ne line_width 0
set_ne incr_hilight 1
set_ne enable_stretch 0
set_ne horizontal_move 0 ; # 20171023
set_ne vertical_move 0 ; # 20171023
set_ne draw_grid 1
set_ne snap 10
set_ne grid 20
set_ne persistent_command 0
set_ne disable_unique_names 1
set_ne sym_txt 1
set_ne show_infowindow 0 
set_ne symbol_width 150
set_ne editor {gvim -f}
set_ne rainbow_colors 0
set_ne initial_geometry {700x448+10+10}
set_ne edit_symbol_prop_new_sel {}
#20161102
set_ne launcher_var {}
set_ne launcher_default_program {xdg-open}
set_ne launcher_program {}
#20160413
set_ne auto_hilight 0
## 20161121 xpm to png conversion
set_ne to_png {gm convert} 

## 20160325 remember edit_prop widget size
set_ne edit_prop_size 80x12
set_ne text_line_default_geometry 80x12
set_ne terminal xterm

# set_ne analog_viewer waveview
set_ne computerfarm {} ;# 20151007

# xschem tcp port number (listen to port and execute commands from there if set) 
# set a port number in xschemrc if you want accept remote connections.
set_ne xschem_listen_port {}

# hide instance details (show only bbox) 
set_ne hide_symbols 0
# show net names if symbol has attributes like @#n:net_name (where n = pin number or pin name) 
# and net_name=true global attribute set on symbol or instance.
set_ne show_pin_net_names 0
# gaw tcp {host port} 
set_ne gaw_tcp_address {localhost 2020}

## utile
set_ne utile_gui_path "${XSCHEM_SHAREDIR}/utile/utile3"
set_ne utile_cmd_path "${XSCHEM_SHAREDIR}/utile/utile"

## cairo stuff 20171112
set_ne cairo_font_scale 1.0
set_ne nocairo_font_xscale .85
set_ne nocairo_font_yscale .88
set_ne cairo_font_line_spacing 1.0
set_ne cairo_vert_correct 0
set_ne nocairo_vert_correct 0

# Arial, Monospace
set_ne cairo_font_name {Arial}
# has_cairo set by c program if cairo enabled
set has_cairo 0 
set rotated_text {} ;#20171208
set_ne dark_colorscheme 1
##### set colors
if {!$rainbow_colors} {
  set_ne cadlayers 22
  ## 20171113
  set_ne light_colors {
   "#ffffff" "#0044ee" "#aaaaaa" "#222222" "#229900"
   "#bb2200" "#00ccee" "#ff0000" "#888800" "#00aaaa"
   "#880088" "#00ff00" "#0000cc" "#666600" "#557755"
   "#aa2222" "#7ccc40" "#00ffcc" "#ce0097" "#d2d46b"
   "#ef6158" "#fdb200"}
  set_ne dark_colors {
   "#000000" "#00ccee" "#3f3f3f" "#cccccc" "#88dd00" 
   "#bb2200" "#00ccee" "#ff0000" "#ffff00" "#ffffff"
   "#ff00ff" "#00ff00" "#0000cc" "#aaaa00" "#aaccaa"
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

# for svg and pdf draw 20121108
regsub -all {"} $dark_colors  {} svg_colors
regsub -all {#} $svg_colors  {0x} svg_colors
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors

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

set_ne colors $dark_colors
##### end set colors


# 20111005 added missing initialization of globals...
set_ne no_change_attrs 0
set_ne preserve_unchanged_attrs 0
set search_select 0

# 20111106 these vars are overwritten by caller with mktemp file names
if {$::OS == "Windows"} {
  set filetmp1 $env(windir)/.tmp1
  set filetmp2 $env(windir)/.tmp2
} else {
  set filetmp1 $env(PWD)/.tmp1
  set filetmp2 $env(PWD)/.tmp2
}
# /20111106

# flag bound to a checkbutton in symbol editprop form
# if set cell is copied when renaming it
set_ne copy_cell 0


# recent files
set recentfile {}
if { [file exists $USER_CONF_DIR/recent_files] } {
  if {[catch { source $USER_CONF_DIR/recent_files } err] } {
    puts "Problems opening recent_files: $err"
    if {![info exists no_x]} {
      tk_messageBox -message  "Problems opening recent_files: $err" -icon warning -parent . -type ok
    }

  }
}

# schematic to preload in new windows 20090708
set_ne XSCHEM_START_WINDOW {}

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

set txt ""
set custom_token {lab}
set search_value {}
set search_exact 0


# 20171005
set custom_label_prefix {}

# 20171112 cairo stuff
xschem set cairo_font_scale $cairo_font_scale
xschem set nocairo_font_xscale $nocairo_font_xscale
xschem set nocairo_font_yscale $nocairo_font_yscale
xschem set cairo_font_line_spacing $cairo_font_line_spacing
xschem set cairo_vert_correct $cairo_vert_correct
xschem set nocairo_vert_correct $nocairo_vert_correct
xschem set persistent_command $persistent_command
xschem set disable_unique_names $disable_unique_names
# font name can not be set here as we need to wait for X-initialization 
# to complete. Done in xinit.c

###
### build Tk widgets
###
if { ( $::OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 )
     && ![info exists no_x]} {

# for hyperlink in about dialog
eval  font create Underline-Font [ font actual TkDefaultFont ]
font configure Underline-Font -underline true -size 24


   . configure -cursor left_ptr
#   option add *Button*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menubutton*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menu*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Listbox*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Entry*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   #option add *Text*font "-*-courier-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Label*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile

   if { [info exists tk_scaling] } {tk scaling $tk_scaling}
   set infowindow_text {}
   infowindow
   #proc unknown  {comm args} { puts "unknown command-> \<$comm\> $args" }
   frame .menubar -relief raised -bd 2 
   toolbar_toolbar

   menubutton .menubar.file -text "File" -menu .menubar.file.menu -padx 3 -pady 0
   menu .menubar.file.menu -tearoff 0
   menubutton .menubar.edit -text "Edit" -menu .menubar.edit.menu -padx 3 -pady 0
   menu .menubar.edit.menu -tearoff 0
   menubutton .menubar.option -text "Options" -menu .menubar.option.menu -padx 3 -pady 0
   menu .menubar.option.menu -tearoff 0
   menubutton .menubar.zoom -text "View" -menu .menubar.zoom.menu -padx 3 -pady 0
   menu .menubar.zoom.menu -tearoff 0
   menubutton .menubar.prop -text "Properties" -menu .menubar.prop.menu -padx 3 -pady 0
   menu .menubar.prop.menu -tearoff 0
   menubutton .menubar.layers -text "Layers" -menu .menubar.layers.menu -padx 3 -pady 0 \
    -background [lindex $colors 4]
   menu .menubar.layers.menu -tearoff 0
   menubutton .menubar.tools -text "Tools" -menu .menubar.tools.menu -padx 3 -pady 0
   menu .menubar.tools.menu -tearoff 0
   menubutton .menubar.sym -text "Symbol" -menu .menubar.sym.menu -padx 3 -pady 0
   menu .menubar.sym.menu -tearoff 0
   menubutton .menubar.hilight -text "Highlight" -menu .menubar.hilight.menu -padx 3 -pady 0
   menu .menubar.hilight.menu -tearoff 0
   menubutton .menubar.simulation -text "Simulation" -menu .menubar.simulation.menu -padx 3 -pady 0
   menu .menubar.simulation.menu -tearoff 0
   menubutton .menubar.help -text "Help" -menu .menubar.help.menu -padx 3 -pady 0
   menu .menubar.help.menu -tearoff 0
   .menubar.help.menu add command -label "Help" -command "textwindow \"${XSCHEM_SHAREDIR}/xschem.help\" ro" \
        -accelerator {?}
   .menubar.help.menu add command -label "Keys" -command "textwindow \"${XSCHEM_SHAREDIR}/keys.help\" ro"
   .menubar.help.menu add command -label "About XSCHEM" -command "about"
   
   .menubar.file.menu add command -label "New Schematic"  -accelerator Ctrl+N\
     -command {
       xschem clear SCHEMATIC
     }
   toolbar_create FileNew {xschem clear SCHEMATIC} "New Schematic"
   .menubar.file.menu add command -label "New Symbol" -accelerator Ctrl+Shift+N \
     -command {
       xschem clear SYMBOL
     }
   toolbar_create FileNewSym {xschem clear SYMBOL} "New Symbol"
   .menubar.file.menu add command -label "Open" -command "xschem load" -accelerator {Ctrl+O}
   toolbar_create FileOpen "xschem load" "Open File"

   menu .menubar.file.menu.recent -tearoff 0
   setup_recent_menu
   .menubar.file.menu add cascade -label "Open Recent" -menu .menubar.file.menu.recent

   .menubar.file.menu add command -label "Save" -command "xschem save" -accelerator {Ctrl+S}
   toolbar_create FileSave "xschem save" "Save File"
   .menubar.file.menu add command -label "Merge" -command "xschem merge" -accelerator {Shift+B}
   toolbar_create FileMerge "xschem merge" "Merge File"
   .menubar.file.menu add command -label "Reload" -accelerator {Alt+S} \
     -command {
      if { [string compare [tk_messageBox -type okcancel -message {Are you sure you want to reload?}] ok]==0 } {
              xschem reload
         }
     }
   toolbar_create FileReload {
      if { [string compare [tk_messageBox -type okcancel -message {Are you sure you want to reload?}] ok]==0 } {
              xschem reload
         }
     } "Reload File"
   .menubar.file.menu add command -label "Save as" -command "xschem saveas" -accelerator {Ctrl+Shift+S}
   .menubar.file.menu add command -label "Save as symbol" \
      -command "xschem saveas {} SYMBOL" -accelerator {Ctrl+Alt+S}
   # added svg, png 20171022
   .menubar.file.menu add command -label "PDF Export" -command "xschem print pdf" -accelerator {*}
   .menubar.file.menu add command -label "PNG Export" -command "xschem print png" -accelerator {Ctrl+*}
   .menubar.file.menu add command -label "SVG Export" -command "xschem print svg" -accelerator {Alt+*}
   .menubar.file.menu add separator
   .menubar.file.menu add command -label "Exit" -command {xschem exit} -accelerator {Ctrl+Q}
   
   .menubar.option.menu add checkbutton -label "Show info win" -variable show_infowindow \
     -command {
        if { $show_infowindow != 0 } {wm deiconify .infotext
        } else {wm withdraw .infotext}
      }
   .menubar.option.menu add checkbutton -label "Color Postscript/SVG" -variable color_ps \
      -command {
         if { $color_ps==1 } {xschem set color_ps 1} else { xschem set color_ps 0}
      }
   .menubar.option.menu add checkbutton -label "A3 page" -variable a3page \
      -command {
         if { $a3page==1 } {xschem set a3page 1} else { xschem set a3page 0}
      }
   .menubar.option.menu add checkbutton -label "Debug mode" -variable menu_tcl_debug \
      -command {
         if { $menu_tcl_debug==1 } {xschem debug 1} else { xschem debug 0}
      }
   .menubar.option.menu add checkbutton -label "Enable stretch" -variable enable_stretch \
      -accelerator Y \
      -command {
         if { $enable_stretch==1 } {xschem set enable_stretch 1} else { xschem set enable_stretch 0} 
      }
   .menubar.option.menu add checkbutton -label "Show netlist win" -variable netlist_show \
      -accelerator {Shift+A} \
      -command {
         if { $netlist_show==1 } {xschem set netlist_show 1} else { xschem set netlist_show 0} 
      }
   .menubar.option.menu add checkbutton -label "Flat netlist" -variable flat_netlist \
      -accelerator : \
      -command {
         if { $flat_netlist==1 } {xschem set flat_netlist 1} else { xschem set flat_netlist 0} 
      }
   .menubar.option.menu add checkbutton -label "Split netlist" -variable split_files \
      -accelerator {} \
      -command {
         if { $split_files==1 } {xschem set split_files 1} else { xschem set split_files 0} 
      }
   .menubar.option.menu add checkbutton -label "hspice / ngspice netlist" -variable hspice_netlist \
      -accelerator {} \
      -command {
         if { $hspice_netlist==1 } {xschem set hspice_netlist 1} else { xschem set hspice_netlist 0} 
      }
   .menubar.option.menu add command -label "Replace \[ and \] for buses in SPICE netlist" \
      -command {
        input_line "Enter two characters to replace default bus \[\] delimiters:" "set tmp_bus_char"
        if { [info exists tmp_bus_char] && [string length $tmp_bus_char] >=2} { set bus_replacement_char $tmp_bus_char } 
      }
   .menubar.option.menu add checkbutton -label "Verilog 2001 netlist variant" -variable verilog_2001 \
   
   .menubar.option.menu add checkbutton -label "Draw grid" -variable draw_grid \
      -accelerator {%} \
      -command {
        if { $draw_grid == 1} { xschem set draw_grid 1; xschem redraw} else { xschem set draw_grid 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "Symbol text" -variable sym_txt \
      -accelerator {Ctrl+B} \
      -command {
        if { $sym_txt == 1} { xschem set sym_txt 1; xschem redraw} else { xschem set sym_txt 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "Toggle variable line width" -variable change_lw \
      -accelerator {_} \
      -command {
        if { $change_lw == 1} { xschem set change_lw 1} else { xschem set change_lw 0}
      }
   .menubar.option.menu add checkbutton -label "Increment Hilight Color" -variable incr_hilight \
      -command {
        if { $incr_hilight == 1} { xschem set incr_hilight 1} else { xschem set incr_hilight 0}
      }
   
   .menubar.option.menu add command -label "Set line width" \
        -command {
          input_line "Enter linewidth (float):" "xschem line_width"
        }
   .menubar.option.menu add command -label "Set symbol width" \
        -command {
          input_line "Enter Symbol width ($symbol_width)" "set symbol_width" $symbol_width 
        }
   .menubar.option.menu add checkbutton -label "Allow duplicated instance names (refdes)" \
       -variable disable_unique_names -command {
          xschem set disable_unique_names $disable_unique_names
       }

   .menubar.option.menu add separator
   .menubar.option.menu add radiobutton -label "Spice netlist" -variable netlist_type -value spice \
        -accelerator {Shift+V} \
        -command "xschem netlist_type spice"
   .menubar.option.menu add radiobutton -label "VHDL netlist" -variable netlist_type -value vhdl \
        -accelerator {Shift+V} \
        -command "xschem netlist_type vhdl"
   .menubar.option.menu add radiobutton -label "Verilog netlist" -variable netlist_type -value verilog \
        -accelerator {Shift+V} \
        -command "xschem netlist_type verilog"
   .menubar.option.menu add radiobutton -label "tEDAx netlist" -variable netlist_type -value tedax \
        -accelerator {Shift+V} \
        -command "xschem netlist_type tedax"
   .menubar.option.menu add radiobutton -label "Symbol global attrs" -variable netlist_type -value symbol \
        -accelerator {Shift+V} \
        -command "xschem netlist_type symbol"
   .menubar.edit.menu add command -label "Undo" -command "xschem undo; xschem redraw" -accelerator U
   toolbar_create EditUndo "xschem undo; xschem redraw" "Undo"
   .menubar.edit.menu add command -label "Redo" -command "xschem redo; xschem redraw" -accelerator {Shift+U}
   toolbar_create EditRedo "xschem redo; xschem redraw" "Redo"
   .menubar.edit.menu add command -label "Copy" -command "xschem copy" -accelerator Ctrl+C
   toolbar_create EditCopy "xschem copy" "Copy"
   .menubar.edit.menu add command -label "Cut" -command "xschem cut"   -accelerator Ctrl+X
   toolbar_create EditCut "xschem cut" "Cut"
   .menubar.edit.menu add command -label "Paste" -command "xschem paste" -accelerator Ctrl+V
   toolbar_create EditPaste "xschem paste" "Paste"
   .menubar.edit.menu add command -label "Delete" -command "xschem delete" -accelerator Del
   toolbar_create EditDelete "xschem delete" "Delete"
   .menubar.edit.menu add command -label "Select all" -command "xschem select_all" -accelerator Ctrl+A
   .menubar.edit.menu add command -label "Edit selected schematic in new window" \
       -command "xschem schematic_in_new_window" -accelerator Alt+E
   .menubar.edit.menu add command -label "Edit selected symbol in new window" \
       -command "xschem symbol_in_new_window" -accelerator Alt+I
   .menubar.edit.menu add command -label "Duplicate objects" -command "xschem copy_objects" -accelerator C
   toolbar_create EditDuplicate "xschem copy_objects" "Duplicate objects"
   .menubar.edit.menu add command -label "Move objects" -command "xschem move_objects" -accelerator M
   toolbar_create EditMove "xschem move_objects" "Move objects"
   .menubar.edit.menu add checkbutton -label "Constrained Horizontal move" -variable horizontal_move \
      -command "xschem set horizontal_move" -accelerator H
   .menubar.edit.menu add checkbutton -label "Constrained Vertical move" -variable vertical_move \
      -command "xschem set vertical_move" -accelerator V
   # added collapse/join/break wires menu command  (& key) 20171022
   .menubar.edit.menu add command -label "Push schematic" -command "xschem descend" -accelerator E
   toolbar_create EditPushSch "xschem move_objects" "Push schematic"
   .menubar.edit.menu add command -label "Push symbol" -command "xschem descend_symbol" -accelerator I
   toolbar_create EditPushSym "xschem descend_symbol" "Push symbol"
   .menubar.edit.menu add command -label "Pop" -command "xschem go_back" -accelerator Ctrl+E
   toolbar_create EditPop "xschem go_back" "Pop"
   button .menubar.waves -text "Waves"  -activebackground red  -takefocus 0 -padx 2 -pady 0 \
     -command {
       waves
      }
   button .menubar.simulate -text "Simulate"  -activebackground red  -takefocus 0 -padx 2 -pady 0 \
     -command {
       if { ![info exists simulate_oldbg] } {
         set simulate_oldbg [.menubar.simulate cget -bg]
         .menubar.simulate configure -bg red
         simulate {.menubar.simulate configure -bg $::simulate_oldbg; unset ::simulate_oldbg}
       }
      }
   button .menubar.netlist -text "Netlist"  -activebackground red  -takefocus 0 -padx 2 -pady 0 \
     -command {
       xschem netlist
      }
   toolbar_create Waves { waves } "View results"
   toolbar_create Simulate {
      if { ![info exists simulate_oldbg] } {
         set simulate_oldbg [.menubar.simulate cget -bg]
         .menubar.simulate configure -bg red
         simulate {.menubar.simulate configure -bg $::simulate_oldbg; unset ::simulate_oldbg}
      }
   } "Run simulation"
   toolbar_create Netlist { xschem netlist } "Create netlist"
   if { $dark_colorscheme == 1 } { set txt_color black} else { set txt_color white} 
   set j 0
   foreach i $colors {
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

     .menubar.layers.menu add command -label $laylab  -activeforeground $layfg \
        -foreground $layfg -background $i -activebackground $i \
        -command "xschem set rectcolor $j; reconfigure_layers_button"
     if { [expr $j%10] == 0 } { .menubar.layers.menu entryconfigure $j -columnbreak 1 }
     incr j
     
   }
   .menubar.zoom.menu add command -label "Redraw" -command "xschem redraw" -accelerator Esc
   toolbar_create ViewRedraw "xschem redraw" "Redraw"
   .menubar.zoom.menu add checkbutton -label "Fullscreen" -variable fullscreen \
      -accelerator {Alt+Shift+F} -command {
         xschem fullscreen
      }
   .menubar.zoom.menu add command -label "Zoom Full" -command "xschem zoom_full" -accelerator F
   .menubar.zoom.menu add command -label "Zoom In" -command "xschem zoom_in" -accelerator Shift+Z
   toolbar_create ViewZoomIn "xschem zoom_in" "Zoom In"
   .menubar.zoom.menu add command -label "Zoom Out" -command "xschem zoom_out" -accelerator Ctrl+Z
   toolbar_create ViewZoomOut "xschem zoom_out" "Zoom Out"
   .menubar.zoom.menu add command -label "Zoom box" -command "xschem zoom_box" -accelerator Z
   toolbar_create ViewZoomBox "xschem zoom_box" "Zoom Box"
   .menubar.zoom.menu add command -label "Half Snap Threshold" -accelerator G -command {
          xschem set cadsnap [expr [xschem get cadsnap] / 2.0 ]
        }
   .menubar.zoom.menu add command -label "Double Snap Threshold" -accelerator Shift-G -command {
          xschem set cadsnap [expr [xschem get cadsnap] * 2.0 ]
        }
   .menubar.zoom.menu add command -label "Set snap value" \
          -command {
          input_line "Enter snap value ( default: [xschem get cadsnap_default] current: [xschem get cadsnap])" \
          "xschem set cadsnap" [xschem get cadsnap]
        }
   .menubar.zoom.menu add command -label "Set grid spacing" \
        -command {
          input_line "Enter grid spacing (float):" "xschem set cadgrid" $grid
        }
   .menubar.zoom.menu add checkbutton -label "View only Probes" -variable only_probes \
          -accelerator {5} \
          -command { xschem only_probes }
   .menubar.zoom.menu add command -label "Toggle colorscheme"  -accelerator {Shift+O} -command {
           xschem toggle_colorscheme
           xschem change_colors
        }
    toolbar_create ViewToggleColors {
           xschem toggle_colorscheme
           xschem change_colors
        } "Toggle Color Scheme"
   .menubar.zoom.menu add command -label "Dim colors"  -accelerator {} -command {
           color_dim
           xschem color_dim
        }
   .menubar.zoom.menu add command -label "Visible layers"  -accelerator {} -command {
           select_layers
           xschem redraw
        }
   .menubar.zoom.menu add command -label "Change Current Layer color"  -accelerator {} -command {
           change_color
        }
   .menubar.zoom.menu add checkbutton -label "No XCopyArea drawing model" -variable draw_window \
          -accelerator {Ctrl+$} \
          -command {
           if { $draw_window == 1} { xschem set draw_window 1} else { xschem set draw_window 0}
        }
   .menubar.zoom.menu add checkbutton -label "Show Toolbar" -variable toolbar_visible \
      -command {
         if { $toolbar_visible } { toolbar_show } else { toolbar_hide }
      }
   .menubar.zoom.menu add checkbutton -label "Enable show net names on symbol pins" -variable show_pin_net_names \
      -command {
         xschem set show_pin_net_names $show_pin_net_names
         xschem redraw
      }
   .menubar.zoom.menu add checkbutton -label "Horizontal Toolbar" -variable toolbar_horiz \
      -command { 
         if { $toolbar_visible } {
            toolbar_hide
            toolbar_show
         }
      }
   .menubar.prop.menu add command -label "Edit" -command "xschem edit_prop" -accelerator Q
   .menubar.prop.menu add command -label "Edit with editor" -command "xschem edit_vi_prop" -accelerator Shift+Q
   .menubar.prop.menu add command -label "View" -command "xschem view_prop" -accelerator Ctrl+Q
   .menubar.prop.menu add command -background red -label "Edit file (danger!)" -command "xschem edit_file" -accelerator Alt+Q
   .menubar.sym.menu add radiobutton -label "Show Symbols" -variable hide_symbols -value 0 \
      -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
   .menubar.sym.menu add radiobutton -label "Show instance Bounding boxes for subcircuit symbols" -variable hide_symbols -value 1 \
      -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
   .menubar.sym.menu add radiobutton -label "Show instance Bounding boxes for all symbols" -variable hide_symbols -value 2 \
      -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
   .menubar.sym.menu add command -label "Make symbol from schematic" -command "xschem make_symbol" -accelerator A
   .menubar.sym.menu add command -label "Make schematic from symbol" -command "xschem make_sch" -accelerator Ctrl+L
   .menubar.sym.menu add command -label "Attach pins to component instance" -command "xschem attach_pins" -accelerator Shift+H
   .menubar.sym.menu add command -label "Create symbol pins from selected schematic pins" \
           -command "schpins_to_sympins" -accelerator Alt+H
   .menubar.sym.menu add command -label "Place symbol pin" \
           -command "xschem add_symbol_pin" -accelerator Alt+P
   .menubar.sym.menu add command -label "Print list of highlight nets" \
           -command "xschem print_hilight_net 1" -accelerator J
   .menubar.sym.menu add command -label "Print list of highlight nets, with buses expanded" \
           -command "xschem print_hilight_net 3" -accelerator Alt-Ctrl-J
   .menubar.sym.menu add command -label "Create labels from highlight nets" \
           -command "xschem print_hilight_net 4" -accelerator Alt-J
   .menubar.sym.menu add command -label "Create labels from highlight nets with 'i' prefix" \
           -command "xschem print_hilight_net 2" -accelerator Alt-Shift-J
   .menubar.sym.menu add command -label "Create pins from highlight nets" \
           -command "xschem print_hilight_net 0" -accelerator Ctrl-J

   .menubar.tools.menu add checkbutton -label "Remember last command" -variable persistent_command \
      -accelerator {} \
      -command {
        if { $persistent_command == 1} { xschem set persistent_command 1} else { xschem set persistent_command 0}
      }
   .menubar.tools.menu add command -label "Insert symbol" -command "xschem place_symbol" -accelerator Ins
   toolbar_create ToolInsertSymbol "xschem place_symbol" "Insert Symbol"
   .menubar.tools.menu add command -label "Insert text" -command "xschem place_text" -accelerator T
   toolbar_create ToolInsertText "xschem place_text" "Insert Text"
   .menubar.tools.menu add command -label "Insert wire" -command "xschem wire" -accelerator W
   toolbar_create ToolInsertWire "xschem wire" "Insert Wire"
   .menubar.tools.menu add command -label "Insert snap wire" -command "xschem snap_wire" -accelerator Shift+W
   .menubar.tools.menu add command -label "Insert line" -command "xschem line" -accelerator L
   toolbar_create ToolInsertLine "xschem line" "Insert Line"
   .menubar.tools.menu add command -label "Insert rect" -command "xschem rect" -accelerator R
   toolbar_create ToolInsertRect "xschem rect" "Insert Rectangle"
   .menubar.tools.menu add command -label "Insert polygon" -command "xschem polygon" -accelerator Ctrl+W
   toolbar_create ToolInsertPolygon "xschem polygon" "Insert Polygon"
   .menubar.tools.menu add command -label "Insert arc" -command "xschem arc" -accelerator Shift+C
   toolbar_create ToolInsertArc "xschem arc" "Insert Arc"
   .menubar.tools.menu add command -label "Insert circle" -command "xschem circle" -accelerator Ctrl+Shift+C
   toolbar_create ToolInsertCircle "xschem circle" "Insert Circle"
   .menubar.tools.menu add command -label "Search" -accelerator Ctrl+F -command  property_search
   toolbar_create ToolSearch property_search "Search"
   .menubar.tools.menu add command -label "Align to Grid" -accelerator Alt+U -command  "xschem align"
   .menubar.tools.menu add command -label "Execute TCL command" -command  "tclcmd"
   .menubar.tools.menu add command -label "Join/Trim wires" \
      -command "xschem collapse_wires" -accelerator {&}
    toolbar_create ToolJoinTrim "xschem collapse_wires" "Join/Trim Wires"
   .menubar.tools.menu add command -label "Break wires" \
      -command "xschem break_wires" -accelerator {!}
    toolbar_create ToolBreak "xschem break_wires" "Break Wires"

   .menubar.hilight.menu add command -label {Highlight duplicate instance names} -command "xschem check_unique_names 0" -accelerator {#} 
   .menubar.hilight.menu add command -label {Rename duplicate instance names} -command "xschem check_unique_names 1" -accelerator {Ctrl+#}
   .menubar.hilight.menu add command -label {Highlight selected net/pins} -command "xschem hilight" -accelerator K
   .menubar.hilight.menu add command -label {Send selected net/pins to GAW} -command "xschem send_to_gaw" -accelerator Alt+G
   .menubar.hilight.menu add command -label {Select connected nets / pins} -command "xschem select_connected_nets" -accelerator Alt+K
   .menubar.hilight.menu add command -label {Un-highlight all net/pins} \
        -command "xschem clear_hilights" -accelerator Shift+K
   .menubar.hilight.menu add command -label {Un-highlight selected net/pins} \
        -command "xschem unhilight" -accelerator Ctrl+K
   # 20160413
   .menubar.hilight.menu add checkbutton -label {Auto-highlight net/pins} -variable auto_hilight \
      -command {
        if { $auto_hilight == 1} {
          xschem set auto_hilight 1
        } else {
          xschem set auto_hilight 0
        }
      }

   .menubar.simulation.menu add command -label "Set netlist Dir" \
     -command {
           select_netlist_dir 1
     }
   .menubar.simulation.menu add command -label "Set top level netlist name" \
     -command {
           input_line {Set netlist file name} {xschem set user_top_netl_name} [xschem get user_top_netl_name] 40
     }
   .menubar.simulation.menu add command -label {Configure simulators and tools} -command {simconf}
   .menubar.simulation.menu add command -label {Utile Stimuli Editor (GUI)} -command {utile_gui [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label "Utile Stimuli Editor ([lindex $editor 0])" -command {utile_edit [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label {Utile Stimuli Translate} -command {utile_translate [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label {Shell [simulation path]} \
      -command {
         if { [select_netlist_dir 0] ne "" } {
           get_shell $netlist_dir
         }
       }
   .menubar.simulation.menu add command -label {Edit Netlist} -command {edit_netlist [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label {Send highlighted nets to GAW} -command {xschem create_plot_cmd gaw}
   .menubar.simulation.menu add command -label {Create Ngspice 'xplot' file} -command {xschem create_plot_cmd ngspice} -accelerator Shift+J
   .menubar.simulation.menu add separator
   .menubar.simulation.menu add checkbutton -label "LVS netlist: Top level is a .subckt" -variable top_subckt 
   .menubar.simulation.menu add checkbutton -label "Use 'spiceprefix' attribute" -variable spiceprefix \
          -command {xschem set spiceprefix $spiceprefix; xschem save; xschem reload}

   pack .menubar.file -side left
   pack .menubar.edit -side left
   pack .menubar.option -side left
   pack .menubar.zoom -side left
   pack .menubar.prop -side left
   pack .menubar.layers -side left
   pack .menubar.tools -side left
   pack .menubar.sym -side left
   pack .menubar.hilight -side left
   pack .menubar.simulation -side left
   pack .menubar.help -side right
   pack .menubar.waves -side right
   pack .menubar.simulate -side right
   pack .menubar.netlist -side right

   frame .drw -background {} -takefocus 1

   wm  title . "xschem - "
   wm iconname . "xschem - "
   . configure  -background {}
   wm  geometry . $initial_geometry
   #wm maxsize . 1600 1200
   wm protocol . WM_DELETE_WINDOW {xschem exit}
   focus .drw

   frame .statusbar  
   label .statusbar.1   -text "STATUS BAR 1"  
   label .statusbar.2   -text "SNAP:"
   entry .statusbar.3 -textvariable snap -relief sunken -bg white \
          -width 10 -foreground black -takefocus 0
   label .statusbar.4   -text "GRID:"
   entry .statusbar.5 -textvariable grid -relief sunken -bg white \
          -width 10 -foreground black -takefocus 0
   label .statusbar.6   -text "NETLIST MODE:"
   entry .statusbar.7 -textvariable netlist_type -relief sunken -bg white \
          -width 8 -state disabled -disabledforeground black 
   label .statusbar.8 -activebackground red -text {} 


   ##
   ## building windows (pack instructions) and event binding (bind instructions) done in proc build_windows
   ## executed by xinit.c after finalizing X initialization. This avoid potential race conditions
   ## like Configure or Expose events being generated before xschem being ready to handle them.
   ##

   if {[array exists replace_key]} {
     foreach i [array names replace_key] {
       key_binding "$i" "$replace_key($i)"
     }
   }
}

# read custom colors
if { [file exists ${USER_CONF_DIR}/colors] } {
  source ${USER_CONF_DIR}/colors
}

if { [llength $dark_colors] < $cadlayers || [llength $light_colors] < $cadlayers } {
  puts stderr { Warning: wrong number of configured layers in light_colors or dark_colors variables.}
}
if { $dark_colorscheme == 1} { 
  set colors $dark_colors
} else {
  set colors $light_colors
}
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors
regsub -all {"} $colors {} svg_colors
regsub -all {#} $svg_colors {0x} svg_colors

if { $show_infowindow } { wm deiconify .infotext } 

foreach i $tcl_files {
  source $i
}

if { [info exists xschem_listen_port] && ($xschem_listen_port ne {}) } { 
  if {[catch {socket -server xschem_server $xschem_listen_port} err]} {
    puts "problems listening to TCP port: $xschem_listen_port"
    puts $err
  }
}

