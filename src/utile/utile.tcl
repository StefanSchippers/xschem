#!/bin/sh

# File: utile.tcl
# 
# This file is part of XSCHEM,
# a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
# simulation.
# Copyright (C) 1998-2023 Stefan Frederik Schippers
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA



# the next line restarts using wish \
exec wish "$0" "$@"


proc text_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE ALIAS FILE"
 wm iconname $w "ALIAS"
 
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.code -text "See Code" -command "showCode $w"
 pack $w.buttons.dismiss $w.buttons.code -side left -expand 1
 
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30
 scrollbar $w.scroll -command "$w.text yview"
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 set fileid [open $filename "r"]
 $w.text insert 0.0 [read $fileid]
 close $fileid
} 

proc entry_line {txtlabel} {
   global entry1
   toplevel .ent2 -class Dialog
   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]
   wm geometry .ent2 "+$X+$Y"
   label .ent2.l1  -text $txtlabel
   entry .ent2.e1   -width 40
   .ent2.e1 delete 0 end
   .ent2.e1 insert 0 $entry1
   button .ent2.b1 -text "OK" -command  \
   {
     set entry1 [.ent2.e1 get ]
     destroy .ent2
   }
   bind .ent2 <Return> {
     set entry1 [.ent2.e1 get ]
     destroy .ent2
   }
   pack .ent2.l1 -side top -fill x
   pack .ent2.e1  -side top -fill both -expand yes
   pack .ent2.b1 -side top -fill x
   grab set .ent2
   focus .ent2.e1
   tkwait window .ent2
   return $entry1
}




proc write_data {w f} {
 set fid [open $f "w"]
 set t [$w get  0.0 {end - 1 chars}]
 puts  -nonewline $fid $t 
 close $fid
}
  
proc read_data {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 $w delete 0.0 end
 $w insert 0.0 $t
 close $fid
}

proc template {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 $w insert 0.0 $t
 close $fid
}

proc new_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE NEW WINDOW"
 wm iconname $w "STIM.WIND"
 set fileid [open $filename "RDONLY CREAT"]
 set testo [read $fileid]
 close $fileid
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
         -height 30
 $w.text insert 0.0 $testo
 scrollbar $w.scroll -command "$w.text yview"
 button $w.buttons.translate -text Translate -command "write_data $w.text $filename; \
         translate $filename ; get_time"
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 pack $w.buttons.dismiss  $w.buttons.translate -side left -expand 1
 
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
}
 
proc get_time {} {
 set fileid [open "inutile.simulationtime"  "RDONLY"]
 .buttons.time delete 0 end
 .buttons.time insert 0 [read -nonewline $fileid]
 close $fileid
 file delete "inutile.simulationtime"
}
 
 
proc alias_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE ALIAS FILE: $filename"
 wm iconname $w "ALIAS"

 set fileid [open $filename "RDONLY CREAT"]
 set testo [read $fileid]
 close $fileid
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30
 scrollbar $w.scroll -command "$w.text yview"
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "write_data $w.text $filename"
 button $w.buttons.load -text Reload -command "read_data $w.text $filename"
 pack $w.buttons.dismiss $w.buttons.save $w.buttons.load -side left -expand 1
 
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 $w.text insert 0.0 $testo
} 

proc help_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE ALIAS FILE"
 wm iconname $w "ALIAS"
 
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "write_data $w.text $filename"
 pack $w.buttons.dismiss  $w.buttons.save -side left -expand 1
 
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30 -width 90
 scrollbar $w.scroll -command "$w.text yview"
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 set fileid [open $filename "RDONLY CREAT"]
 $w.text insert 0.0 [read $fileid]
 close $fileid
} 

proc translate {f} {
 global tcl_platform
 set OS [lindex $tcl_platform(os) 0]
 if {$OS == "Windows"} {
    eval exec "utile.bat $f"
  } else {
    eval exec "utile $f"
  }
}


wm title . "(IN)UTILE (Stefan Schippers, sschippe)"
wm iconname . "(IN)UTILE"
set filename [lindex $argv 0]

if { ![string compare $filename  ""]  } then {
 wm withdraw .
 tk_messageBox -type ok -message "Please give a file name as argument"
 exit
}

set entry1 {}
frame .buttons
pack .buttons -side bottom -fill x -pady 2m
button .buttons.translate -text Translate -command  " \
  write_data .text $filename; translate $filename  ; get_time "
button .buttons.dismiss -text Dismiss -command "destroy ."
button .buttons.code -text "Help" -command {help_window .help $env(UTILE3_PATH)/utile.txt}

text .text -relief sunken -bd 2 -yscrollcommand ".scroll set" -setgrid 1 \
	-height 30
scrollbar .scroll -command ".text yview"
button .buttons.save -text Save -command {
  set entry1 $filename;set filename [entry_line {Filename}]
  write_data .text $filename 
}
button .buttons.load -text Reload -command {
  set entry1 $filename;set filename [entry_line {Filename}]
  read_data .text $filename 
}
button .buttons.send -text "Template" -command {
  if { ![string compare [.text get 0.0 {end - 1 chars}]  ""]  } then {
    template  .text  $env(UTILE3_PATH)/template.stimuli 
  }
}
label .buttons.timelab -text "time:"
entry .buttons.time  -width  11
button .buttons.new -text new_window -command "new_window .new_win new_window_stimuli"
pack .buttons.dismiss .buttons.code \
     .buttons.load .buttons.save .buttons.translate \
     .buttons.send .buttons.new .buttons.timelab .buttons.time  -side left -expand 1
pack .scroll -side right -fill y
pack .text -expand yes -fill both

# 20140408
if { [file exists $filename] }  {  
  set fileid [open $filename "RDONLY"]
  .text insert 0.0 [read $fileid]
  close $fileid
}


set tmp [.text index end]
regsub {\..*$} $tmp {} lines


for {set i 1} {$i <= $lines} {incr i} {
 set tmp [.text get $i.0 "$i.0 lineend"]
 if [regexp {^(include)|(\.include)} $tmp  ] { 
  alias_window .tw$i [lindex $tmp 1] 
 }
} 



