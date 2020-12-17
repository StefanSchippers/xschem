v {xschem version=2.9.8 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
T {To run this example a 'calc_rc' procedure must be defined. 
you can paste  example procedure below into xschem command prompt before
creating a netlist.
Descend into the symbol ('i' key) and see how it is used in the 'format'
attribute to calculate "Res" and "Cap" subcircuit parameters from W and L
instance parameters.
Tcl scripts can be loaded in xcshemrc by appending a tcl file name to variable
tcl_files.} 40 -390 2 1 0.5 0.5 {layer=8 }
T {proc calc_rc \{ L W \} \{ return "Res=[expr 1200*$L/$W] Cap=[expr 1e-3*$W*$L]"\} } 30 -320 0 0 0.4 0.4 { font=monospace}
T {Dynamic calculation of subcircuit parameters} 80 -730 0 0 0.8 0.8 {}
N 140 -130 140 -100 { lab=0}
N 140 -210 160 -210 { lab=IN}
N 140 -210 140 -190 { lab=IN}
C {lab_pin.sym} 620 -190 0 1 {name=p1 lab=OUT}
C {lab_pin.sym} 320 -190 0 0 {name=p2 lab=IN}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {vsource.sym} 140 -160 0 0 {name=V1 value="pwl 0 0 10n 0 11n 5"}
C {lab_pin.sym} 140 -100 0 0 {name=l2 sig_type=std_logic lab=0}
C {lab_pin.sym} 160 -210 0 1 {name=p3 lab=IN}
C {rcline.sym} 470 -190 0 0 {name=x1 L=1e-4 W=0.5e-6
}
C {code_shown.sym} 890 -190 0 0 {name=STIMULI
only_toplevel=false 

value="
.control
tran 0.1n 50n
plot in out
.endc
"}
