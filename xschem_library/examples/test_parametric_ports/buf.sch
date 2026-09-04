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
B 8 -350 -250 340 250 {dash=4 fill=0}
T {@symname} -341.5 224 0 0 0.3 0.3 {}
T {@path@name} -345 -242 0 0 0.3 0.3 {}
N -350 0 -270 0 {lab="IN[@width - 1:0]"}
N 270 0 340 0 {lab="OUT[@width - 1:0]"}
N -80 0 80 0 {lab="N[@width - 1:0]"}
C {ipin.sym} -350 0 0 0 {name=p1 lab="IN[expr(@width - 1):0]"}
C {opin.sym} 340 0 0 0 {name=p2 lab="OUT[expr(@width - 1):0]"}
C {test_parametric_ports/inv.sch} -190 0 0 0 {name=x1 
schematic=@inv
width=@width}
C {lab_wire.sym} 20 0 0 0 {name=p3 lab="N[expr(@width - 1):0]"}
C {test_parametric_ports/inv.sch} 160 0 0 0 {name=x2 
schematic=@inv
width=@width}
