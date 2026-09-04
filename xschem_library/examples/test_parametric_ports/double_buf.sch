v {xschem version=3.4.8RC file_version=1.3}
G {}
K {type=subcircuit
format="@name @pinlist @symname"
spectre_format="@name ( @pinlist ) @symname"
template="name=x1 width=8"
extra=width
default_schematic=ignore}
V {}
S {}
F {}
E {}
B 7 -850 -290 840 290 {dash=4 fill=0}
T {@symname} -841.5 264 0 0 0.3 0.3 {}
T {@name} -845 -282 0 0 0.3 0.3 {}
N -850 0 -770 0 {lab="IN[@width - 1:0]"}
N 770 0 840 0 {lab="OUT[@width - 1:0]"}
N -80 0 80 0 {lab="N[@width - 1:0]"}
C {ipin.sym} -850 0 0 0 {name=p1 lab="IN[expr(@width - 1):0]"}
C {opin.sym} 840 0 0 0 {name=p2 lab="OUT[expr(@width - 1):0]"}
C {test_parametric_ports/buf.sch} -420 0 0 0 {name=x1 
schematic=@buf
inv=@inv
width=@width}
C {test_parametric_ports/buf.sch} 430 0 0 0 {name=x2
schematic=@buf
inv=@inv
width=@width}
C {lab_wire.sym} 30 0 0 0 {name=p3 lab="N[expr(@width - 1):0]"}
