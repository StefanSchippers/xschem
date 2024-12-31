## Add a custom menu in xschem

## Create a menu entry 'Test' before 'Netlist'.  '.menubar' is xschem's main menu.
.menubar insert Netlist cascade -label Test -menu .menubar.test
menu .menubar.test -tearoff 0

## Create a couple of entries
.menubar.test add command -label "Test entry 1" -command {
    puts Hello
}
.menubar.test add command -label "Test entry 2" -command {
    puts World
}

