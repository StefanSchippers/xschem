v {xschem version=2.9.5_RC8 file_version=1.1}
G {}
V {}
S {}
E {}
T {COLPITTS OSCILLATOR
EXAMPLE} 460 -480 0 0 0.4 0.4 {}
N 240 -270 340 -270 {lab=B}
N 380 -300 690 -300 {lab=OUT}
N 250 -460 380 -460 {lab=VCC}
N 380 -100 380 -80 {lab=VSS}
N 380 -100 550 -100 {lab=VSS}
N 380 -340 380 -300 {lab=OUT}
N 380 -240 380 -220 {lab=E}
N 380 -220 380 -160 {lab=E}
N 550 -240 550 -200 {lab=E}
N 550 -140 550 -100 {lab=VSS}
N 380 -210 400 -210 {lab=E}
N 400 -210 550 -210 {lab=E}
C {code.sym} 750 -190 0 0 {
name=STIMULI 
value=".option RUNLVL=6 post 
vvss vss 0 dc 0
.save all
.temp 30

** models are generally not free: you must download
** SPICE models for active devices 
** (https://www.centralsemi.com/docs/csm/2N2222A.LIB)
** and put them  into the below 
** referenced file in simulation directory.
.include \\"models_osc.txt\\"


.tran 1n 100u 
.probe tran i1(Q1) i2(Q1) i3(Q1) i(L1) i(c1) i(c2) i(r1) i(r2) i(r3)
"}
C {npn.sym} 360 -270 0 0 {name=Q1 model=q2n2222a area=1
}
C {vsource.sym} 250 -430 0 1 {name=V2 value="5 pwl 0 5 1n 6 2n 6 3n 5"}
C {lab_pin.sym} 250 -400 0 0 {name=p0 lab=VSS}
C {lab_pin.sym} 690 -300 0 1 {name=p3 lab=OUT}
C {lab_pin.sym} 380 -80 0 0 {name=p5 lab=VSS}
C {lab_pin.sym} 250 -460 0 0 {name=p6 lab=VCC}
C {ind.sym} 380 -430 2 0 {name=L1 value=100u}
C {capa.sym} 550 -270 0 1 {name=C1 m=1 value=100p}
C {lab_pin.sym} 380 -230 0 0 {name=p2 lab=E}
C {vsource.sym} 240 -240 0 1 {name=V1 value=1}
C {lab_pin.sym} 240 -210 0 0 {name=p1 lab=VSS}
C {lab_pin.sym} 240 -270 0 0 {name=p4 lab=B}
C {res.sym} 380 -130 0 1 {name=R1 m=1 value=1k}
C {res.sym} 380 -370 0 1 {name=R2 m=1 value=2}
C {capa.sym} 550 -170 0 1 {name=C2 m=1 value=100p}
