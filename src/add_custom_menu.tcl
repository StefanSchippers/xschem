## Add a custom menu in xschem

## Create a menu entry 'Test' before 'Netlist'.  '$topwin.menubar' is xschem's main menu.
proc add_menu {} {
  set topwin [xschem get top_path]
  $topwin.menubar insert Netlist cascade -label Test -menu $topwin.menubar.test
  menu $topwin.menubar.test -tearoff 0

  ## Create a couple of entries
  $topwin.menubar.test add command -label "Test entry 1" -command {
      puts Hello
  }
  $topwin.menubar.test add command -label "Test entry 2" -command {
      puts World
  }
}

add_menu

