v {xschem version=2.9.5_RC5 file_version=1.1}
G {}
V {}
S {}
E {}
T {NE555 TIMER IC EXAMPLE} 360 -490 0 0 0.4 0.4 {}
N 130 -150 200 -150 {lab=TRIG}
C {code.sym} 710 -190 0 0 {
name=STIMULI 
value=".option SCALE=1e-6 PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
* .option method=gear
vvss vss 0 dc 0

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.include \\"model_test_ne555.txt\\"
.save all
.temp 30
.tran 0.1u 1m uic
"}
C {ne555.sym} 490 -220 0 0 {name=x1}
C {res.sym} 130 -180 0 0 {name=r3 m=1 value=2k}
C {capa.sym} 130 -120 0 0 {name=c6 m=1 value=2n}
C {vsource.sym} 130 -360 0 1 {name=v1 value=5}
C {lab_pin.sym} 130 -330 0 0 {name=p16 lab=VSS}
C {lab_pin.sym} 130 -390 0 1 {name=p6 lab=VSUPPLY}
C {lab_pin.sym} 570 -340 0 1 {name=p8 lab=VSUPPLY}
C {lab_pin.sym} 410 -340 0 0 {name=p9 lab=VSS}
C {lab_pin.sym} 410 -260 0 0 {name=p11 lab=TRIG}
C {lab_pin.sym} 410 -180 0 0 {name=p12 lab=OUT}
C {lab_pin.sym} 570 -180 0 1 {name=p15 lab=TRIG}
C {lab_pin.sym} 200 -150 0 1 {name=p19 lab=TRIG}
C {lab_pin.sym} 130 -270 0 0 {name=p20 lab=VSUPPLY}
C {lab_pin.sym} 130 -90 0 0 {name=p21 lab=VSS}
C {lab_pin.sym} 570 -100 0 1 {name=p14 lab=CTRL}
C {lab_pin.sym} 410 -100 0 0 {name=p13 lab=VSUPPLY}
C {lab_pin.sym} 570 -260 0 1 {name=p17 lab=DIS}
C {res.sym} 130 -240 0 0 {name=r5 m=1 value=2k}
C {lab_pin.sym} 130 -210 0 0 {name=p18 lab=DIS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
