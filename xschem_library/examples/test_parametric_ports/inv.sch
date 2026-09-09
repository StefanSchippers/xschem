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
B 10 -80 -210 110 200 {dash=4 fill=0}
T {@symname} -71.5 174 0 0 0.3 0.3 {}
T {@path@name} -75 -202 0 0 0.3 0.3 {}
N 40 110 40 170 {lab=GND}
N 40 -170 40 -110 {lab=VDD}
N 40 -80 40 80 {lab=OUT[2:0]}
N -0 -110 0 110 {lab=IN[2:0]}
N -80 -0 -0 0 {lab=IN[2:0]}
N 40 -0 110 -0 {lab=OUT[2:0]}
C {nmos4.sym} 20 110 0 0 {name="M1[expr(@width - 1):0]" model=cmosn w=2u l=1u del=0 m=1}
C {pmos4.sym} 20 -110 0 0 {name="M2[expr(@width -1):0]" model=cmosp w=5u l=1u del=0 m=1
xname="M2[@width\\\\:1]"}
C {vdd.sym} 40 -170 0 0 {name=l1 lab=VDD}
C {gnd.sym} 40 170 0 0 {name=l2 lab=GND}
C {ipin.sym} -80 0 0 0 {name=p1 lab="IN[expr(@width - 1):0]"}
C {opin.sym} 110 0 0 0 {name=p2 lab="OUT[expr(@width - 1):0]"}
