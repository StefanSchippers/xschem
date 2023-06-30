################################################################################
# $Id: mouse_bindings.tcl,v 1.6 2023/06/29 23:44:15 paul Exp $
#
#                    $env(HOME)/.xschem/mouse_bindings.tcl
#
# The purpose of this code is to configure custom mouse button bindings for
# mice that have more buttons than the conventional 3 buttons and scroll wheel.
# This code also works to reconfigure bindings for conventional mice. In my
# case, my mouse has 2 extra buttons that X11 has identified as 8 & 9.
#
# INSTRUCTIONS:
#  1. use linux command 'xev -event button' to find the X11 mouse button ID
#     number for any mouse button.
#  2. modify 'proc mouse_buttons' below to define an action for the button
#     along with optional key modifier(s).
#  3. add the following line to user xschemrc file:
#     lappend tcl_files $env(HOME)/.xschem/mouse_bindings.tcl
#
################################################################################

################################################################################
# PARAMETERS of proc mouse_buttons { b m }:
# NOTE BITS 1,4,5 (Cap-Lock,Num-Lock,Undefined) Need to be cleared
# NOTE THAT THE 'OR' OF ALL MODIFIER KEYS = 0x4f
# $m: modifier:   bit# bitval
#    No Modifier:  -     0
#    Shift:        0     1
#    Cap-Lock:     1     2
#    Control:      2     4
#    Alt:          3     8
#    Num-Lock:     4    16
#    Mod3:         5    32 (not mapped to any key on PC keyboards)
#    Win key:      6    64
# To encode multiple modifiers simply add the corresponding modifier bitvals
# $b: mouse button: 1..9
################################################################################

proc mouse_buttons { b m } {
   # filter out Cap-Lock, Num-Lock, and Undefined keys
   set m [ expr { $m & ~(2+16+32) } ]
   # puts "modifier state: $m"
   switch $m {
   # None
      0  { switch $b {
            8  { xschem zoom_full center           }
            9  { xschem unhilight_all              } } }
   # Shift
      1  { switch $b {
            8  { xschem set_modify 1; xschem save  }
            9  {                                   } } }
   # Cntl
      4  { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Cntl
      5  { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Alt
      8  { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Alt
      9  { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Cntl+Alt
      12 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Cntl+Alt
      13 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Winkey
      64 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Winkey
      65 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Cntl+Winkey
      68 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Cntl+Winkey
      69 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Alt+Winkey
      72 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Alt+Winkey
      73 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Cntl+Alt+Winkey
      76 { switch $b {
            8  {                                   }
            9  {                                   } } }
   # Shift+Cntl+Alt+Winkey
      77 { switch $b {
            8  {                                   }
            9  {                                   } } }
   }
}

# the global variable 'has_x' is provided by xschem. It is defined to '1' 
# if graphics has been initialized.

if { [ info exists has_x ] } {
#  puts "Loading ~/.xschem/mouse_bindings.tcl"
   bind .drw <Any-Button> {+mouse_buttons %b %s}
}
