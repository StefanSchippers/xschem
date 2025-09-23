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

# Helper function for spectre netlisting
# N .. normal
# NV .. normal, value only
# S .. string (dump quoted)
proc format_args_spectre {symname props} {
    set str ""
    set args {}
    foreach {propname type} $props {
        set val [ xschem getprop instance $symname $propname ]
        set len [string len $val]
        if {$len > 0} {
            set first false
            if { $type eq "N" } {
                lappend args "$propname=$val"
            } elseif { $type eq "NV" } {
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
    set names [list sweep N outp N outn N nodeset SG store N write N writeop N]
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

# Netlister for spectre
proc netlister {netlist_type} {
    set cmds {}
    set types [dict create]
    set prefix "netlist_command_"
    foreach {name symfile type} [xschem instance_list] {
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
        try {
            set cmd [$func $name]
            if {[string length $cmd] > 0} {
                lappend blocks [format_sweep_chain_spectre $name $cmd types]
            }
        } on error (msg) {
            puts "Error during formatting command $name"
            continue
        }
    }
    set control [indent [join $blocks "\n"] "  "]
    return [join [list "//// begin user architecture code" "control" "$control" "endc" "//// end user architecture code"] "\n"]
}

# Add sweep chain to analysis (spectre)
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
        set sweeps [join [lreverse $sweeplist] "\n"]
        return "$sweeps\n  $anstr"
    } else {
        return "$anstr"
    }
}

# Sweep formatter (spectre), construct sweep chain, innermost first
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
    # Parent has sweep property
    set tag [ xschem getprop instance $sweep tag ]
    return "sweep $tag ( [format_sweep_spectre_params $sweep] [format_sweep_spectre_range $sweep] )"
}

# Sweep formatter (spectre), what to sweep
proc format_sweep_spectre_params {name} {
    return [format_args_spectre $name [list instance N model N parameter N "option" N "variable" N]]
}

# Sweep formatter (spectre), how to sweep
proc format_sweep_spectre_range {name} {
    return [format_args_spectre $name [list from N to N step N mode N points N "values" N continuation N]]
}

# Analysis formatters, spectre
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
    return [format_args_spectre $name [list verbatim NV]]
}

proc format_analysis_op_spectre {name} {
    set args [format_args_spectre $name [list nodeset N store N write N]]
    return "analysis $name op [parenthesize $args]"
}

proc format_analysis_dc1d_spectre {name} {
    # OP formatting
    set args [format_args_spectre $name [list nodeset N store N write N]]
    set anstr "analysis $name op [parenthesize $args]"
    # 1D sweep formatting
    set swp [format_single_sweep_spectre $name]
    return "$swp\n  $anstr"
}

proc format_analysis_dcinc_spectre {name} {
    set args [format_args_spectre $name [list nodeset N store N write N writeop N]]
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
    append args [format_args_spectre $name [list nodeset N store N write N writeop N]]
    return "analysis $name dcxf [parenthesize $args]"
}

proc format_analysis_ac_spectre {name} {
    set args "[format_sweep_spectre_range $name] "
    append args [format_args_spectre $name [list nodeset N store N write N writeop N]]
    return "analysis $name ac [parenthesize $args]"
}

proc format_analysis_acxf_spectre {name} {
    set args "out=[format_signal_output_spectre $name] "
    append args "[format_sweep_spectre_range $name] "
    append args [format_args_spectre $name [list nodeset N store N write N writeop N]]
    return "analysis $name acxf [parenthesize $args]"
}

proc format_analysis_noise_spectre {name} {
    set args "out=[format_signal_output_spectre $name] "
    append args "[format_args_spectre $name [list in N]] "
    append args "[format_sweep_spectre_range $name] "
    append args [format_args_spectre $name [list nodeset N store N write N writeop N]]
    return "analysis $name noise [parenthesize $args]"
}

proc format_analysis_tran_spectre {name} {
    set args [format_args_spectre $name [list step N stop N start N maxstep N icmode N nodeset N ic N store N write N]]
    return "analysis $name tran [parenthesize $args]"
}

proc format_analysis_hb_spectre {name} {
    set args [format_args_spectre $name [list freq N nharm N immax N truncate N samplefac N nper N sample N harmonic N imorder N nodeset N store N write N]]
    return "analysis $name hb [parenthesize $args]"
}

proc format_postprocess_spectre {name} {
    set tool [format_args_spectre $name [list tool NV]]
    set file [format_args_spectre $name [list file NV]]
    return "postprocess($tool, $file)"
}

# TODO: add x, meg, mil to vacask SI prefixes to VACASK
# TODO: add all VACASK analyses
# TODO: add ngspice formatting

}
