v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
N 60 80 270 80 {lab=y}
N -70 -200 -10 -200 {lab=VCCPIN}
N -70 -170 -10 -170 {lab=VCCPIN}
N -70 -140 -70 80 {lab=y}
N 120 300 140 300 {lab=VSSBPIN}
N -130 -170 -110 -170 {lab=a}
N -20 -110 0 -110 {lab=b}
N -10 -200 -10 -170 {lab=VCCPIN}
N 120 170 120 190 {lab=#net1}
N 120 140 140 140 {lab=VSSBPIN}
N 120 80 120 110 {lab=y}
N 40 -140 100 -140 {lab=VCCPIN}
N 40 -110 100 -110 {lab=VCCPIN}
N 100 -140 100 -110 {lab=VCCPIN}
N 40 -80 40 80 {lab=y}
N -70 80 40 80 {lab=y}
N 40 80 60 80 {lab=y}
N -310 -170 -130 -170 {lab=a}
N -310 -110 -20 -110 {lab=b}
N 150 -80 210 -80 {lab=VCCPIN}
N 150 -50 210 -50 {lab=VCCPIN}
N 210 -80 210 -50 {lab=VCCPIN}
N -310 -50 110 -50 {lab=c}
N 150 -20 150 80 {lab=y}
N 120 220 140 220 {lab=VSSBPIN}
N 120 250 120 270 {lab=#net2}
N -180 300 80 300 {lab=a}
N -180 -170 -180 300 {lab=a}
N -160 -110 -160 220 {lab=b}
N -160 220 80 220 {lab=b}
N -140 140 80 140 {lab=c}
N -140 -50 -140 140 {lab=c}
C {lab_pin.sym} 120 330 0 0 {name=lg1 lab=VSSPIN}
C {lab_pin.sym} -70 -200 0 0 {name=lg2 lab=VCCPIN}
C {opin.sym} 270 80 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} -310 -170 0 0 {name=p2 lab=a}
C {nlv4t.sym} 100 300 0 0 {name=m1 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {plv4t.sym} -90 -170 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {plv4t.sym} 20 -110 0 0 {name=m3 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {nlv4t.sym} 100 140 0 0 {name=m5 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {ipin.sym} -310 -110 0 0 {name=p3 lab=b}
C {lab_pin.sym} 40 -140 0 0 {name=l1 lab=VCCPIN}
C {ipin.sym} -310 -50 0 0 {name=p4 lab=c}
C {plv4t.sym} 130 -50 0 0 {name=m4 model=cmosp w=wp l=lp  m=1 
as="'0.270u*(wp)'"
ad="'0.270u*(wp)'"
ps="'2*0.270u+wp'"
pd="'2*0.270u+wp'"}
C {lab_pin.sym} 150 -80 0 0 {name=l0 lab=VCCPIN}
C {nlv4t.sym} 100 220 0 0 {name=m6 model=cmosn w=wn l=lln  m=1
as="'0.270u*(wn)'"
ad="'0.270u*(wn)'"
ps="'2*0.270u+wn'"
pd="'2*0.270u+wn'"}
C {lab_pin.sym} 140 300 0 1 {name=l2 lab=VSSPIN}
C {lab_pin.sym} 140 220 0 1 {name=l3 lab=VSSPIN}
C {lab_pin.sym} 140 140 0 1 {name=l4 lab=VSSPIN}
