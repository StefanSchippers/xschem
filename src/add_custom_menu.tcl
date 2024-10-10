## Add a custom menu in xschem

## Create a menu entry 'Test'.  '.menubar' is xschem's main menu frame.
menubutton .menubar.test -text "Test" -menu .menubar.test.menu -padx 3 -pady 0
menu .menubar.test.menu -tearoff 0

## Create a couple of entries
.menubar.test.menu add command -label "Test entry 1" -command {
    puts Hello
}
.menubar.test.menu add command -label "Test entry 2" -command {
    puts World
}

## make the menu appear in xschem window
pack .menubar.test -side left
## or place it before some other menu entry:
# pack .menubar.test -before .menubar.file -side left

## To remove the menu without destroying it:
# pack forget .menubar.test


################################################################
## adding a button in the menubar (not in the toolbar) 
################################################################
# button .menubar.test -pady 0 -highlightthickness 0 -text Test -command {puts Test}
# pack  .menubar.test -side left


## create a menu 'PDK' with radiobutton entries
#  proc menupdk {} {
#    global libptr dark_gui_colorscheme
#    if { $dark_gui_colorscheme} {
#      set selectcolor white
#    } else {
#      set selectcolor black
#    }
#
#    ## Create a menu entry 'Test'.  '.menubar' is xschem's main menu frame.
#    menubutton .menubar.pdk -text "PDK" -menu .menubar.pdk.menu -padx 3 -pady 0
#    menu .menubar.pdk.menu -tearoff 0
#
#    .menubar.pdk.menu add radiobutton -selectcolor $selectcolor -variable libptr \
#        -value gf180mcu -label "GF180MCU" -command {
#        set_env gf180mcu
#    }
#
#    .menubar.pdk.menu add radiobutton -selectcolor $selectcolor -variable libptr \
#        -value sky130 -label "SKY130" -command {
#        set_env sky130
#    }
#
#    .menubar.pdk.menu add radiobutton -selectcolor $selectcolor -variable libptr \
#        -value repo -label "xschem repo" -command {
#        set_env repo
#    }
#    ## make the menu appear in xschem window
#    pack .menubar.pdk -side left
#    ## or place it before some other menu entry:
#    # pack .menubar.pdk -before .menubar.file -side left
#
#    ## To remove the menu without destroying it:
#    # pack forget .menubar.pdk
#  }
#  menupdk

