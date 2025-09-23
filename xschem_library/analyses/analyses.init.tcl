namespace eval ::analyses {

# Parenthesize string if not empty
proc parenthesize {str} {
    if {[string length $str] > 0} {
        return "( $str )"
    } else {
        return "$str"
    }
}

# Indent a multiline string
proc indent {str indent} {
    set lines [split $str \n]
    set indented_lines [lmap line $lines {string cat $indent $line}]
    return [join $indented_lines \n]
}

# Helper function for display
# props: flat list of name-type pairs
# type can be
#   N .. normal, print value
#   G .. given, print if parameter is given (not empty string)
#   SG .. string given, if quoted, print it out, otherwise just print given
#   NV .. normal, value only
proc format_props {symname props} {
    set args {}
    foreach {propname type} $props {
        set val [ xschem getprop instance $symname $propname ]
        set len [string len $val]
        set quoted [expr {[string index $val 0] eq "\""}]
        if {$len > 0} {
            if { $type eq "N" } {
                lappend args "$propname=$val"
            } elseif { $type eq "NV" } {
                lappend args "$val"
            } elseif { $type eq "G" } {
                lappend args "$propname given"
            } elseif { $type eq "SG" } {
                if {$quoted} {
                    lappend args "$propname=$val"
                } else {
                    lappend args "$propname given"
                }
            }
        }
    }
    return [join $args "\n"]
}

# Helper function for spectre/spice netlisting
# N .. normal
# NV .. normal, value only
# UNV .. unquote, normal, value only
# S .. string (dump quoted)
proc format_args {symname props} {
    set str ""
    set args {}
    foreach {propname type} $props {
        set val [ xschem getprop instance $symname $propname ]
        set len [string len $val]
        # Unquote value if requested
        if { $type eq "UNV" } {
            if {[string match "\"*\"" $val]} {
                set val [string range $val 1 end-1]
            }
        }
        # Add to formatted arguments list
        if {$len > 0} {
            set first false
            if { $type eq "N" } {
                lappend args "$propname=$val"
            } elseif { $type eq "NV" } {
                lappend args "$val"
            } elseif { $type eq "UNV" } {
                lappend args "$val"
            } elseif { $type eq "S" } {
                lappend args "$propname=\"$val\""
            }
        }
    }
    return [join $args " "]
}

# Display output order
proc display_order {symname} {
    set names [list order NV]
    set txt [format_props $symname $names]
    if {[string length $txt] > 0} {
        return "#$txt"
    } else {
        return "unordered (#0)"
    }
}

# Display simulator name
proc display_simulator {symname} {
    set names [list simulator NV]
    return [format_props $symname $names]
}

# Display sweep
proc display_sweep {symname} {
    set names [list sweep N instance N model N parameter N "option" N "variable" N from N to N step N mode N points N "values" N continuation N]
    return [format_props $symname $names]
}

# Display verbatim block
proc display_verbatim {symname} {
    return [format_props $symname [list verbatim NV]]
}

# Display OP analysis
proc display_op {symname} {
    set names [list sweep N nodeset SG store N write N]
    return [format_props $symname $names]
}

# Display 1D DC sweep analysis
proc display_dc1d {symname} {
    set names [list sweep N instance N model N parameter N "option" N "variable" N from N to N step N mode N points N "values" N continuation N nodeset SG store N write N]
    return [format_props $symname $names]
}

# Display DCINC analysis
proc display_dcinc {symname} {
    set names [list sweep N nodeset SG store N write N writeop N]
    return [format_props $symname $names]
}

# Display DCXF analysis
proc display_dcxf {symname} {
    set names [list sweep N outp N outn N in N nodeset SG store N write N writeop N]
    return [format_props $symname $names]
}

# Display AC analysis
proc display_ac {symname} {
    set names [list sweep N from N to N step N mode N points N values N nodeset SG store N write N writeop N]
    return [format_props $symname $names]
}

# Display XF analysis
proc display_acxf {symname} {
    set names [list sweep N outp N outn N from N to N step N mode N points N values N nodeset SG store N write N writeop N]
    return [format_props $symname $names]
}

# Display NOISE analysis
proc display_noise {symname} {
    set names [list sweep N outp N outn N in N from N to N step N mode N points N values N ptssum N nodeset SG store N write N writeop N]
    return [format_props $symname $names]
}

# Display TRAN analysis
proc display_tran {symname} {
    set names [list sweep N step N stop N start N maxstep N icmode N nodeset SG ic SG store N write N]
    return [format_props $symname $names]
}

# Display HB analysis
proc display_hb {symname} {
    set names [list sweep N freq N nharm N immax N truncate N samplefac N nper N sample N harmonic N imorder N nodeset N store N write N]
    return [format_props $symname $names]
}

# Display postprocessing
proc display_postprocess {symname} {
    set names [list file N]
    return [format_props $symname $names]
}

#
# Netlister
#

proc netlister {netlist_type} {
    set cmds {}
    set types [dict create]
    set prefix "netlist_command_"
    foreach {name symfile type} [xschem instance_list] {
        # Do not netlist entry point symbol
        if {[string match netlist_command_header $type]} {
            continue
        }
        # Collect only symbols of type netlist_*
        if {[string match netlist_* $type]} {
            dict set types $name $type
        }
        if {[string match $prefix* $type]} {
            # Get order
            set order [xschem getprop instance $name order]
            if {[string len $order] == 0}  {
                set order 0
            }
            # Append as sublist
            lappend cmds [list $order $name $type]
        }
    }
    # Sort
    set cmds [lsort -integer -index 0 $cmds]
    # Loop and format
    set blocks {}
    foreach tuple $cmds {
        lassign $tuple order name type
        set suffix [string range $type [string length $prefix] end]
        # Construct formatter function name
        set func [join [list "format_" "$suffix" "_" "$netlist_type"] ""]
        #try {
            set cmd [$func $name]
            if {[string length $cmd] > 0} {
                lappend blocks [format_sweep_chain_$netlist_type $name $cmd types]
            }
        #} on error (msg) {
        #    puts "Error during formatting command $name"
        #    continue
        #}
    }
    return [wrap_control_$netlist_type $blocks]
}


#
# Netlister for VACASK
#

# Wrap in control block
proc wrap_control_spectre {cmds} {
    set control [indent [join $cmds "\n"] "  "]
    return [join [list "//// begin user architecture code" "control" "$control" "endc" "//// end user architecture code"] "\n"]
}

# Add sweep chain to analysis
proc format_sweep_chain_spectre {name anstr types} {
    upvar 1 $types typesdict
    set sweep [ xschem getprop instance $name sweep ]
    set sweeplist {}
    format_sweep_spectre $name sweeplist typesdict
    # Do we have any sweeps
    if {[llength $sweeplist] > 0} {
        # Yes, reverse sweep chain
        set sweeplist [lreverse $sweeplist]
        # Join sweeps
        set sweeps [join $sweeplist "\n"]
        return "$sweeps\n  $anstr"
    } else {
        return "$anstr"
    }
}

# Sweep formatter, construct sweep chain, innermost first
proc format_sweep_spectre {parent sweeplist types} {
    upvar 1 $sweeplist swl
    upvar 1 $types typesdict
    set sweep [ xschem getprop instance $parent sweep ]
    if {[string length $sweep] > 0} {
        # Parent has sweep property
        set tag [ xschem getprop instance $sweep tag ]
        set type [dict get $typesdict $sweep]
        if {[string length $tag] > 0 && ($type eq "netlist_modifier_sweep")} {
            # Sweep has tag property
            try {
                lappend swl [format_single_sweep_spectre $sweep]
            } on error (msg) {
                # Stop traversing chain on error
                puts "Error during formating command $sweep"
                return 
            }
            # Recursion into parent sweep
            format_sweep_spectre $sweep swl typesdict
        }
    }
}

# Fomat a single sweep
proc format_single_sweep_spectre {sweep} {
    set tag [ xschem getprop instance $sweep tag ]
    return "sweep $tag ( [format_sweep_spectre_params $sweep] [format_sweep_spectre_range $sweep] )"
}

# Sweep formatter, what to sweep
proc format_sweep_spectre_params {name} {
    return [format_args $name [list instance N model N parameter N "option" N "variable" N]]
}

# Sweep formatter, how to sweep
proc format_sweep_spectre_range {name} {
    return [format_args $name [list from N to N step N mode N points N "values" N continuation N]]
}

# Analysis formatters
proc format_verbatim_spectre {name} {
    set sim [ xschem getprop instance $name simulator ]
    set dump false
    if {[string length $sim]==0} {
        set dump true
    } else {
        try {
            if {[sim_is_$sim]} {
                set dump true
            }
        } on error (msg) {}
    }
    if { !$dump } {
        return ""
    }
    return [format_args $name [list verbatim NV]]
}

proc format_analysis_op_spectre {name} {
    set args [format_args $name [list nodeset N store N write N]]
    return "analysis $name op [parenthesize $args]"
}

proc format_analysis_dc1d_spectre {name} {
    # OP formatting
    set args [format_args $name [list nodeset N store N write N]]
    set anstr "analysis $name op [parenthesize $args]"
    # 1D sweep formatting
    set swp [format_single_sweep_spectre $name]
    return "$swp\n  $anstr"
}

proc format_analysis_dcinc_spectre {name} {
    set args [format_args $name [list nodeset N store N write N writeop N]]
    return "analysis $name dcinc [parenthesize $args]"
}

proc format_signal_output_spectre {name} {
    set outp [ xschem getprop instance $name outp ]
    set outn [ xschem getprop instance $name outn ]
    set vecstr "\[ $outp"
    if {[string length $outn] > 0} {
        append vecstr ", $outn"
    }
    append vecstr " \]"
    return $vecstr
}

proc format_analysis_dcxf_spectre {name} {
    set args "out=[format_signal_output_spectre $name] "
    append args [format_args $name [list nodeset N store N write N writeop N]]
    return "analysis $name dcxf [parenthesize $args]"
}

proc format_analysis_ac_spectre {name} {
    set args "[format_sweep_spectre_range $name] "
    append args [format_args $name [list nodeset N store N write N writeop N]]
    return "analysis $name ac [parenthesize $args]"
}

proc format_analysis_acxf_spectre {name} {
    set args "out=[format_signal_output_spectre $name] "
    append args "[format_sweep_spectre_range $name] "
    append args [format_args $name [list nodeset N store N write N writeop N]]
    return "analysis $name acxf [parenthesize $args]"
}

proc format_analysis_noise_spectre {name} {
    set args "out=[format_signal_output_spectre $name] "
    append args "[format_args $name [list in N]] "
    append args "[format_sweep_spectre_range $name] "
    append args [format_args $name [list nodeset N store N write N writeop N]]
    return "analysis $name noise [parenthesize $args]"
}

proc format_analysis_tran_spectre {name} {
    set args [format_args $name [list step N stop N start N maxstep N icmode N nodeset N ic N store N write N]]
    return "analysis $name tran [parenthesize $args]"
}

proc format_analysis_hb_spectre {name} {
    set args [format_args $name [list freq N nharm N immax N truncate N samplefac N nper N sample N harmonic N imorder N nodeset N store N write N]]
    return "analysis $name hb [parenthesize $args]"
}

proc format_postprocess_spectre {name} {
    set tool [format_args $name [list tool NV]]
    set file [format_args $name [list file NV]]
    return "postprocess($tool, $file)"
}


#
# Netlister for Ngspice
#

# Wrap in control block
proc wrap_control_spice {cmds} {
    set control [join $cmds "\n"]
    return [join [list "**** begin user architecture code" ".control" "$control" ".endc" "**** end user architecture code"] "\n"]
}

# Add sweep chain to analysis
proc format_sweep_chain_spice {name anstr types} {
    upvar 1 $types typesdict
    # Get analysis type (op/dc1d or ac/noise)
    set antype [dict get $typesdict $name]
    # Is it a dec/oct/lin sweep
    if {$antype eq "netlist_command_analysis_ac"} {
        set decoctlin true
    } elseif {$antype eq "netlist_command_analysis_noise"} {
        set decoctlin true
    } elseif {$antype eq "netlist_command_analysis_op"} {
        # For swept op rename analysis to dc
        set decoctlin false
        set anstr "dc"
    } else {
        set decoctlin false
    }
    set sweep [ xschem getprop instance $name sweep ]
    set sweeplist {}
    format_sweep_spice $decoctlin $name sweeplist typesdict
    # TODO: maximal sweep chain depths
    # - op 2
    # - dc1d 1
    # - all others 0
    # Do we have any sweeps
    if {[llength $sweeplist] > 0} {
        # Yes, join sweeps
        set sweeps [join $sweeplist " "]
        return "$anstr $sweeps"
    } else {
        return "$anstr"
    }
}

# Sweep formatter, construct sweep chain, innermost first
proc format_sweep_spice {decoctlin parent sweeplist types} {
    upvar 1 $sweeplist swl
    upvar 1 $types typesdict
    set sweep [ xschem getprop instance $parent sweep ]
    if {[string length $sweep] > 0} {
        # Parent has sweep property
        set tag [ xschem getprop instance $sweep tag ]
        set type [dict get $typesdict $sweep]
        if {[string length $tag] > 0 && ($type eq "netlist_modifier_sweep")} {
            # Sweep has tag property
            try {
                lappend swl [format_single_sweep_spice $decoctlin $sweep]
            } on error (msg) {
                # Stop traversing chain on error
                puts "Error during formating command $sweep"
                return 
            }
            # Recursion into parent sweep
            format_sweep_spice $decoctlin $sweep swl typesdict
        }
    }
}

# Fomat a single sweep
proc format_single_sweep_spice {decoctlin sweep} {
    return "[format_sweep_spice_params $sweep] [format_sweep_spice_range $decoctlin $sweep]"
}

# Sweep formatter, what to sweep
proc format_sweep_spice_params {name} {
    return [format_args $name [list instance UNV]]
}

# Sweep formatter, how to sweep
proc format_sweep_spice_range {decoctlin name} {
    if {$decoctlin} {
        return [format_args $name [list mode UNV points NV from NV to NV]]
    } else {
        return [format_args $name [list from NV to NV step NV]]
    }
}

proc format_verbatim_spice {name} {
    return [format_verbatim_spectre $name]
}

proc format_analysis_op_spice {name} {
    return "op"
}

proc format_analysis_dc1d_spice {name} {
    # 1D sweep formatting
    set swp [format_single_sweep_spice false $name]
    return "dc $swp"
}

proc format_analysis_dcinc_spice {name} {
    puts "$name: dcinc is not supported by Ngspice"
    return ""
}

proc format_signal_output_spice {name} {
    set outp [ xschem getprop instance $name outp ]
    set outn [ xschem getprop instance $name outn ]
    # If outp starts with v(, use it literally, ignore outn
    if {[string match "v(*" $outp]} {
        return $outp
    }
    # If outp starts with i(, use it literally, ignore outn
    if {[string match "i(*" $outp]} {
        return $outp
    }
    # Unquote outp, outn
    if {[string match "\"*\"" $outp]} {
        set outp [string range $outp 1 end-1]
    }
    if {[string match "\"*\"" $outn]} {
        set outn [string range $outn 1 end-1]
    }
    # Is outn empty
    if {[string length $outn] == 0} {   
        return "v($outp)"
    } else {
        return "v($outp,$outn)"
    }
}

proc format_analysis_dcxf_spice {name} {
    set output "[format_signal_output_spice $name]"
    return "tf $output [format_args $name [list in UNV]]"
}

proc format_analysis_ac_spice {name} {
    set swp "[format_sweep_spice_range true $name] "
    return "ac $swp"
}

proc format_analysis_acxf_spice {name} {
    puts "$name: acxf is not supported by Ngspice"
    return ""
}

proc format_analysis_noise_spice {name} {
    set output "[format_signal_output_spice $name]"
    set swp "[format_sweep_spice_range true $name] "
    return "noise $output [format_args $name [list in UNV]] $swp [format_args $name [list ptssum NV]]"
}

proc format_analysis_tran_spice {name} {
    set args [format_args $name [list step NV stop NV start NV maxstep NV]]
    set icmode [format_args $name [list icmode UNV]]
    if {$icmode eq "uic"} {
        append args " uic"
    }
    return "tran $args"
}

proc format_analysis_hb_spice {name} {
    puts "$name: hb is not supported by Ngspice"
    return ""
}

proc format_postprocess_spice {name} {
    global tcl_platform
    set tool [format_args $name [list tool NV]]
    if {[string match "\"*\"" $tool]} {
        # Quoted, unquote, use literally
        set tool [string range $tool 1 end-1]
    } else {
        # Not quoted, check if it is PYTHON (VACASK variable for autodetected Python)
        if {$tool eq "PYTHON"} {
            if {$tcl_platform(platform) eq "windows"} {
                set tool "python.exe"
            } else {
                set tool "python3"
            }
        }
    }
    # Keep quotes around file
    set file [format_args $name [list file NV]]
    return "shell $tool $file"
}



# TODO: add ngspice formatting

}
