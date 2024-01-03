## xschemtest.tcl
## script for testing various xschem functions:
## works on xschem versions newer than r1819
## - drawing and drawing speed, with selections and highlights
## - netlisting in various formats
## - show net names on instance pins
## - logic simulation engine
## - print png, svg, pdf
## - trim wires
## - copy/paste operations
## run:
# xschem --script /path/to/xschemtest.tcl
## optionally with logging to catch memory leaks:
# xschem -d 3 -l log --script /path/to/xschemtest.tcl
## and then running: xschemtest
## running this test with different xschem versions with profiling enabled (-pg) 
## allows to see differences in number of function calls / time spent.

## move schematic and redraw in a loop.
proc draw_test {{filelist {-}} {hide_graphs 0}} {
  global show_pin_net_names hide_empty_graphs
  set hide_empty_graphs $hide_graphs
  set show_pin_net_names 1
  foreach f $filelist  {
    if { $f ne {-}} {
      xschem load [abs_sym_path $f]
    }
    xschem search regex 1 lab . ;# select all nets
    xschem hilight ;# hilight all selected nets and labels
    xschem unselect_all
    set increment 5.0
    set n 30.0
    set a [time {
      for { set i 0 } { $i < $n } { incr i} {
        set x [xschem get xorigin]
        set y [xschem get yorigin]
        set x [expr {$x +$increment}]
        set y [expr {$y +$increment}]
        xschem origin $x $y
        xschem redraw
      }
    }]
    set a [lindex $a 0]
    set fps [expr {$n / $a * 1e6} ] ;# calculate drawing frames per second
    puts "$f: draw speed: $fps fps"
  }
  set show_pin_net_names 0
}
proc test_windows { {tabs 0} {destroy 1} } {
  global tabbed_interface

  xschem clear force
  set tabbed_interface $tabs
  setup_tabbed_interface
  xschem load [abs_sym_path testbench.sch]
  xschem load_new_window [abs_sym_path poweramp.sch]
  xschem load_new_window [abs_sym_path mos_power_ampli.sch]
  xschem load_new_window [abs_sym_path rom8k.sch]
  xschem load_new_window [abs_sym_path autozero_comp.sch]
  xschem load_new_window [abs_sym_path LCC_instances.sch]
  xschem load_new_window [abs_sym_path simulate_ff.sch]
  xschem load_new_window [abs_sym_path led_driver.sch]
  xschem load_new_window [abs_sym_path solar_panel.sch]
  update
  if {[xschem new_schematic ntabs] == 8} {set res OK} else {set res FAIL}
  if {$tabs} {
    puts "test tabbed windows: $res"
  } else {
    puts "test multiple windows: $res"
  }
  if {$destroy} {
    after 2000
    xschem new_schematic destroy_all
    xschem clear force
  }
}

## select all loaded schematic and paste 32 times in different places,
## check if number of elements after paste matches.
proc copy_paste_test {{f mos_power_ampli.sch}} {
  xschem load [abs_sym_path $f]
  xschem zoom_box -18000 -18000 18000 18000
  xschem select_all
  set n [xschem get lastsel]
  xschem copy
  for { set i 3000 } {$i < 12001} { set i [expr {$i + 3000}]} {
    xschem paste 0 $i
    xschem paste 0 -$i
    xschem paste $i 0
    xschem paste -$i 0
    xschem paste $i $i
    xschem paste $i -$i
    xschem paste -$i $i
    xschem paste -$i -$i
  }
  update
  xschem select_all
  set m [xschem get lastsel]
  if { $m == $n * 33 } {
    puts "Copy / paste 32 additional circuits: $m elements == $n * 33. OK"
  } else {
    puts "Copy / paste 32 additional circuits: $m elements != $n * 33. FAIL"
  }
  xschem unselect_all
  xschem clear force
}

## draw a grid of long vertical wires and small horizontal wire segments
## after a trim wire operation vertical wires are cut at intersection points.
proc draw_trim_wiregrid {} {
  xschem unhilight_all
  xschem clear force
  xschem set no_undo 1
  for {set i 0} {$i < 129} {incr i} {
    set x [expr {$i * 40.0}]
    set y [expr {128.0*40}]
    xschem wire $x 0 $x $y
  }
  for {set i 0} {$i < 129} {incr i} {
    for {set j 0} {$j < 128} {incr j} {
      set x1 [expr {$j * 40}]
      set x2 [expr {$j * 40 + 40}]
      set y [expr {$i * 40}]
      xschem wire $x1 $y $x2 $y
    }
  }
  xschem set no_undo 0
  xschem zoom_full
  update ;# so updated window will be visible.
  xschem trim_wires ;# will also draw result
  update
  xschem select_all
  set n [xschem get lastsel]
  xschem unselect_all
  ## if all wires trimmed correctly we should have 129*128*2 = 33024 segments.
  if {$n == 33024} { puts "Trim wire test: $n segments, OK"} else { puts "Trim wire test FAIL"}
  xschem clear force
}


## test print files. Exact file content depend on window size and conversion tool,
## so we simply check if existing and size > 0.
## view parameter allows to view print file (works on linux)
proc print_test {{view 0}} {
  global OS
  foreach {f t} {
    autozero_comp.sch    png
    mos_power_ampli.sch  svg
    simulate_ff.sch      pdf
  } {
    set filepath [abs_sym_path $f]
    set printfile [file rootname $f].$t
    xschem load $filepath
    puts "Printing: $printfile in $t format"
    xschem print $t  $printfile
    if {$view && $OS ne {Windows}} {
      execute 0 xdg-open $printfile
      alert_ "Opening print file. Check if $printfile print file looks fine"
    }
    if {[file exists $printfile] && [file size $printfile] > 0} {
      puts "Print file $printfile exists. [file size $printfile] bytes. OK"
    } else {
      puts "Print file $printfile not existing or empty. FAIL"
    }
    file delete $printfile
  }
  xschem clear force
}

## test xschem's own simulation engine
## there is no built in testing, just see if it works.
proc test_xschem_simulation {{f simulate_ff.sch}} {
  global tclstop OS
  xschem load [abs_sym_path $f]
  ## search element with tclcommand attribute 
  if {$OS ne {Windows}} {
    xschem search regex  1 tclcommand {}
  } else {
    xschem search exact 1 name h3
  }
  ## join transform a list element {foo} into a plain string foo
  set instname [join [xschem selected_set]]
  ## run tcl testbench
  eval [xschem getprop instance $instname tclcommand]
  puts "Xschem simulation test done. OK"
}

## netlist some files in various formats and check netlist with known gold hashes
## hashes should be calculated in same way on windows and linux.
proc netlist_test {} {
  global netlist_dir
  foreach {f t h} {
    rom8k.sch               spice       665784374
    greycnt.sch             verilog    2899796185
    autozero_comp.sch       spice       751826850
    test_generators.sch     spice        49312823
    inst_sch_select.sch     spice      2240134366
    test_bus_tap.sch        spice       188702715
    loading.sch             vhdl       2975204502
    mos_power_ampli.sch     spice       125840804
    hierarchical_tedax.sch  tedax       998070173
    LCC_instances.sch       spice      1888015492
    pcb_test1.sch           tedax      1925087189
    test_doublepin.sch      spice      4159808692
    simulate_ff.sch         spice       574849766
    test_symbolgen.sch      spice      2593807370
    test_mosgen.sch         spice      2380524013
  } {
    xschem set netlist_type $t
    xschem load [abs_sym_path $f]
    if {$t eq {verilog}} { set t v}
    if {$t eq {tedax}} { set t tdx}
    set netlist_file $netlist_dir/[file rootname $f].$t
    file delete $netlist_file
    xschem netlist
    ## check netlist hashes, compare with gold hashes
    set netlist_hash [xschem hash_file $netlist_file 1]
    if { $netlist_hash == $h } {
      puts "$f netlist check OK"
    } else {
      puts "$f netlist check, expected hash: ${h}, calculated hash: ${netlist_hash}, FAIL"
    }
  }
}

proc xschemtest {{view 0}} {
  set t [time {
    ## make sure ERC window will not pop up above schematic while doing tests
    wm deiconify .infotext
    lower .infotext

    netlist_test
    print_test $view
    draw_test [list 0_examples_top.sch rom8k.sch greycnt.sch autozero_comp.sch \
               loading.sch mos_power_ampli.sch LCC_instances.sch simulate_ff.sch]

    copy_paste_test mos_power_ampli.sch
    draw_trim_wiregrid
    test_xschem_simulation simulate_ff.sch
    test_windows 0 ;# windows
    test_windows 1 ;# tabs
  }]
  puts "Test time: [lindex $t 0] microseconds"
}

## this is the test to run from xschem console after sourcing this file
# xschemtest
