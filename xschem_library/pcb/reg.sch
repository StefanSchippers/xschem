v {xschem version=2.9.8 file_version=1.2}
G {}
K {type=subcircuit
format="@name @pinlist @symname"
template="name=x1"}
V {}
S {}
E {}
B 2 430 -240 940 -70 {dash=4}
T {@name} 940 -270 0 1 0.4 0.4 {}
T {@symname} 430 -270 0 0 0.4 0.4 {}
N 550 -190 670 -190 {lab=VCCFILT}
N 590 -130 590 -110 {lab=ANALOG_GND}
N 790 -190 940 -190 {lab=VCC5}
N 890 -130 890 -110 {lab=ANALOG_GND}
N 730 -110 890 -110 {lab=ANALOG_GND}
N 730 -160 730 -110 {lab=ANALOG_GND}
N 590 -110 730 -110 {lab=ANALOG_GND}
N 430 -110 590 -110 { lab=ANALOG_GND}
N 430 -190 490 -190 { lab=VCC_IN}
C {capa.sym} 590 -160 0 0 {name=C0 m=1 value=100u device="electrolitic capacitor"}
C {7805.sym} 730 -190 0 0 {name=U0
device=7805 footprint=TO220W
spiceprefix=X
url="https://www.sparkfun.com/datasheets/Components/LM7805.pdf"}
C {capa.sym} 890 -160 0 0 {name=C4 m=1 value=10u device="tantalium capacitor"}
C {res.sym} 520 -190 1 0 {name=R0 m=1 value=4.7 device="carbonresistor"}
C {lab_wire.sym} 650 -190 0 0 {name=l1 lab=VCCFILT}
C {opin.sym} 940 -190 0 0 { name=p1 lab=VCC5 }
C {ipin.sym} 430 -190 0 0 { name=p2 lab=VCC_IN }
C {ipin.sym} 430 -110 0 0 { name=p3 lab=ANALOG_GND }
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
