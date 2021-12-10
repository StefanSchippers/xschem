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
# xschem --script /path/to/drawtest.tcl
## optionally with logging to catch memory leaks:
# xschem -d 3 -l log --script /path/to/drawtest.tcl
## and then running: xschemtest
## running this test with different xschem versions with profiling enabled (-pg) 
## allows to see differences in number of function calls / time spent.

## move schematic and redraw in a loop.
proc drawtest {} {
  set increment 5.0
  set a [time {
    for { set i 0 } { $i < 100 } { incr i} {
      set x [xschem get xorigin]
      set y [xschem get yorigin]
      set x [expr {$x +$increment}]
      set y [expr {$y +$increment}]
      xschem origin $x $y
      xschem redraw
    }
  }]
  set a [lindex $a 0]
  set fps [expr {100.0 / $a * 1e6} ] ;# calculate drawing frames per second
  puts "[rel_sym_path [xschem get schname]]: draw speed: $fps fps"
}

proc copy_paste_test {} {
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
  if {$n == 33024} { puts "trim wire test: $n segments, OK"} else { puts "trim wire test FAIL"}
  after 200 { xschem clear force}
}

proc xschemtest {{view 0}} {
  global tclstop netlist_dir OS XSCHEM_SHAREDIR show_pin_net_names
  set t [time {
    ## make sure ERC window wil not pop up above schematic while doing tests
    wm deiconify .infotext
    lower .infotext
    for {set n 0} {$n < 7} {incr n} {
      set show_pin_net_names 0
      if {$n % 7 == 1} {
        xschem set netlist_type verilog
        xschem load [abs_sym_path greycnt.sch]
      } elseif {$n % 7 == 2} {
        xschem set netlist_type spice
        xschem load [abs_sym_path autozero_comp.sch]
        xschem print png autozero_comp.png
      } elseif {$n % 7 == 3} {
        xschem set netlist_type vhdl
        xschem load [abs_sym_path loading.sch]
      } elseif {$n % 7 == 4} {
        xschem set netlist_type spice
        set show_pin_net_names 1
        xschem load [abs_sym_path mos_power_ampli.sch]
        xschem print svg mos_power_ampli.svg
      } elseif {$n % 7 == 5} {
        xschem set netlist_type spice
        xschem load [abs_sym_path LCC_instances.sch]
      } elseif {$n % 7 == 6} {
        xschem set netlist_type spice
        xschem load [abs_sym_path simulate_ff.sch]
        xschem print pdf simulate_ff.pdf
        eval [xschem getprop instance h3 tclcommand]
      } else {
        xschem set netlist_type spice
        xschem load [abs_sym_path rom8k.sch]
      }
      xschem search regex 1 lab . ;# select all nets
      xschem hilight ;# hilight all selected nets and labels
      xschem unselect_all
      xschem netlist
      drawtest
      if { $n == 4} { copy_paste_test }

    }
    xschem clear force
    draw_trim_wiregrid

    ## check netlist hashes, compare with gold hashes
    foreach {f h} {
        greycnt.v              389394682
        mos_power_ampli.spice  1186348644
        autozero_comp.spice    2011673313
        loading.vhdl           3834408538
        LCC_instances.spice    3918341865
        simulate_ff.spice      1321596936
        rom8k.spice            2198713988
    } {
      if { [xschem hash_file $netlist_dir/$f] == $h } {
        puts "$f netlist check OK"
      } else {
        puts "$f netlist check FAIL"
      }
    }
    ## test print files. Exact file content depend on window size and conversion tool,
    ## so we simply check if existing and size > 0.
    foreach i {autozero_comp.png mos_power_ampli.svg simulate_ff.pdf} {
      if {$view && $OS ne {Windows}} {
        execute 0 xdg-open $i
        alert_ "Check if $i print file look fine"
      }
      if {[file exists $i] && [file size $i] > 0} {
        puts "print file $i exists. OK"
      } else {
        puts "print file $i not existing or empty. FAIL"
      }
      file delete $i
    }
  }]
  puts $t
}

## this is the test to run from xschem console after sourcing this file
# xschemtest
