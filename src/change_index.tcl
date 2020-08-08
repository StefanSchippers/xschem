
# increments index of bussed label by $incr
proc change_index {incr} {

  set sel [xschem selected_set]
  foreach i $sel {
    set mylabel [xschem getprop instance $i lab]
    regsub {.*\[} $mylabel {} myindex
    regsub {\].*} $myindex {} myindex
    regsub {\[.*} $mylabel {} mybasename
    if { [regexp {^[0-9][0-9]*$} $myindex] } {
      set myindex [expr $myindex + $incr]
      set mylabel "$mybasename\[$myindex\]"
      xschem setprop instance $i lab $mylabel
    }
  }
}

bind .drw + {change_index 1}
bind .drw - {change_index -1}
