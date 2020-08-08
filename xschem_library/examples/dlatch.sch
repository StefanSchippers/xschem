v {xschem version=2.9.5_RC5 file_version=1.1}
G {}
V {}
S {}
E {}
N 160 -280 240 -280 {lab=#net1}
N 300 -280 400 -280 {lab=#net2}
C {ipin.sym} 100 -150 0 0 {name=p0 lab=D}
C {opin.sym} 400 -150 0 0 {name=p2 lab=Q}
C {ipin.sym} 100 -100 0 0 {name=p4 lab=VSS}
C {switch.sym} 270 -280 1 0 {name=G5 TABLE="'VTH-0.1' 10G 'VTH+0.1' 10"}
C {capa.sym} 300 -250 0 0 {name=c1 m=1 value=1p}
C {lab_pin.sym} 250 -320 0 0 {name=p6 lab=VSS}
C {lab_pin.sym} 300 -220 0 1 {name=p7 lab=VSS}
C {ipin.sym} 100 -130 0 0 {name=p1 lab=G}
C {lab_pin.sym} 270 -320 0 1 {name=p3 lab=G}
C {buf.sym} 120 -280 0 0 {name=E5 TABLE="'VTH-0.1' 0 'VTH+0.1' VHI"}
C {buf.sym} 440 -280 0 0 {name=E1 TABLE="'VTH-0.1' 0 'VTH+0.1' VHI"}
C {lab_pin.sym} 120 -250 0 0 {name=p5 lab=VSS}
C {lab_pin.sym} 440 -250 0 0 {name=p8 lab=VSS}
C {lab_pin.sym} 480 -280 0 1 {name=p9 lab=Q}
C {lab_pin.sym} 80 -280 0 0 {name=p10 lab=D}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
