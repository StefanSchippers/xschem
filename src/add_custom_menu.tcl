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

