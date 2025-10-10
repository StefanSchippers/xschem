v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
T {These components use the @DEL
parameter passed from parent level
by Xschem} 240 -390 0 0 0.4 0.4 {}
N 500 -230 580 -230 {lab=Z}
N 500 -230 500 -190 {lab=Z}
N 360 -230 410 -230 {lab=A}
N 470 -230 500 -230 {lab=Z}
N 500 -130 500 -110 {lab=GND}
C {res.sym} 440 -230 1 0 {name=R1
value="expr( 2500 * @DEL )"
footprint=1206
device=resistor
m=1}
C {iopin.sym} 580 -230 0 0 {name=p1 sig_type=std_logic lab=Z}
C {gnd.sym} 500 -110 0 0 {name=l1 lab=GND}
C {iopin.sym} 360 -230 0 1 {name=p2 sig_type=std_logic lab=A}
C {capa.sym} 500 -160 0 0 {name=C1
m=1
value="expr(100f * @DEL )"
footprint=1206
device="ceramic capacitor"}
C {code_shown.sym} 10 -210 0 0 {name=COMMANDS only_toplevel=0 value="
.param DEL=@DEL
** do something with DEL
"

}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
