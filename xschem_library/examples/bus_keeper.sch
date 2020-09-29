v {xschem version=2.9.8 file_version=1.2}
G {}
K {type=subcircuit
format="@name @pinlist @symname WN_FB=@WN_FB WP_FB=@WP_FB"
template="name=X1 WN_FB=1u WP_FB=2u"}
V {}
S {}
E {}
P 2 5 250 -880 250 -120 480 -120 480 -880 250 -880 {dash=5}
T {@name} 250 -915 0 0 0.5 0.5 {}
T {@symname} 253.75 -115 0 0 0.5 0.5 {}
N 250 -300 300 -300 {lab=A}
N 280 -680 300 -680 {lab=A}
N 280 -680 280 -300 {lab=A}
N 430 -300 450 -300 {lab=#net1}
N 450 -680 450 -300 {lab=#net1}
N 430 -680 450 -680 {lab=#net1}
N 350 -500 390 -500 { lab=GND}
C {cmos_inv.sch} 240 -70 0 0 {name=X2 WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sch} 490 -450 0 1 {name=X1 WN=WN_FB WP=WP_FB LLN=3u LLP=3u}
C {iopin.sym} 250 -300 0 1 {name=p1 lab=A}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {capa.sym} 420 -500 1 0 {name=C1
m=1
value=10f
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 350 -500 0 0 {name=l2 lab=GND}
