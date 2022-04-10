v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
T {Voc = 21
Wp = 100W
Vmp = 18.2} 610 -370 0 0 0.4 0.4 {}
N 510 -340 510 -320 {
lab=minus}
N 510 -140 510 -120 {
lab=minus}
N 510 -430 510 -400 {
lab=mn}
N 510 -320 510 -260 {
lab=minus}
N 510 -550 510 -490 {
lab=pn}
N 510 -490 600 -490 {
lab=pn}
N 510 -430 600 -430 {
lab=mn}
N 510 -260 510 -140 {
lab=minus}
C {iopin.sym} 510 -610 2 1 {name=p1 lab=plus}
C {iopin.sym} 510 -120 2 1 {name=p2 lab=minus}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {ammeter.sym} 510 -370 2 1 {name=Vm}
C {ammeter.sym} 510 -580 2 1 {name=Vp}
C {bsource.sym} 510 -460 2 0 {name=B1 VAR=I FUNC="pwl(V(pn,mn),
+ 0, 6.3,
+ 18, 5.7,
+ 18.3, 5.6,
+ 18.6, 5.4,
+ 18.9, 5.1,
+ 19.5, 4.0,
+ 21, 0,
+ 22, 0)"
}
C {lab_pin.sym} 510 -510 2 0 {name=l2 sig_type=std_logic lab=pn}
C {lab_pin.sym} 510 -410 2 0 {name=l3 sig_type=std_logic lab=mn}
C {res.sym} 600 -460 0 0 {name=R1
value=500000
footprint=1206
device=resistor
m=1}
