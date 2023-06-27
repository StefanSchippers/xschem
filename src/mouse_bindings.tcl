################################################################################
# $Id: mouse_bindings.tcl,v 1.3 2023/06/26 21:00:11 paul Exp $
#
#                                          $env(HOME)/.xschem/mouse_bindings.tcl
#
# The purpose of this code is to configure custom mouse button bindings for
# mice that have more buttons than the conventional 3 buttons and scroll wheel.
# This code also works to reconfigure bindings for conventional mice.
#
# INSTRUCTIONS:
#       1. use linux command 'xev -event button' to find the X11 mouse button ID
#               number for any mouse button.
#       2. modify 'proc mouse_buttons' below to define an action for the button
#               along with optional key modifier(s).
#       3. add the following line to user xschemrc file:
#                       lappend tcl_files $env(HOME)/.xschem/mouse_bindings.tcl
#
################################################################################

################################################################################
# PARAMETERS of proc mouse_buttons { b m }:
# NOTE THAT BIT 4 OF $m IS ALWAYS SET SO $m >= 16
# NOTE THAT THE 'OR' OF ALL MODIFIER KEYS = 0x4f
# $m: modifier key(s)
#    No Modifier:  0
#    Shift:        1
#    Caps-Lock:    2
#    Control:      4
#    Alt:          8
#    Num-Lock:    16
#    Win key:     64
# To encode multiple modifiers simply add the corresponding modifier numbers
# $b: mouse button: 1..9
################################################################################

proc mouse_buttons { b m } {
        # filter out Caps-Lock and Num-Lock status
        set m [ expr { $m & ~(16+2) } ]
        # puts "modifier state: $m"
        switch $m {
                0       { switch $b {
                                8       { xschem zoom_full center       }
                                9       { xschem unhilight_all          } } }
                1       { switch $b {
                                8       { xschem set_modify 1; xschem save      } } }
        }
}

# the global variable 'has_x' is provided by xschem. It is defined to '1' 
# if graphics has been initialized.

if { [ info exists has_x ] } {
#       puts "Loading ~/.xschem/mouse_bindings.tcl"
        bind .drw <Any-Button> {+mouse_buttons %b %s}
}
