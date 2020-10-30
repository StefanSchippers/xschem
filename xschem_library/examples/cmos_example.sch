v {xschem version=2.9.8 file_version=1.2}
G {}
K {}
V {}
S {}
E {}
T {CMOS DIFFERENTIAL AMPLIFIER
EXAMPLE} 810 -570 0 0 0.4 0.4 {}
N 30 -240 30 -210 {lab=VCC}
N 30 -240 60 -240 {lab=VCC}
N 240 -260 240 -210 {lab=GN}
N 240 -210 280 -210 {lab=GN}
N 280 -210 280 -180 {lab=GN}
N 280 -180 460 -180 {lab=GN}
N 500 -250 500 -210 {lab=#net1}
N 430 -280 450 -280 {lab=0}
N 550 -280 570 -280 {lab=0}
N 430 -250 570 -250 {lab=#net1}
N 370 -280 390 -280 {lab=PLUS}
N 610 -280 630 -280 {lab=MINUS}
N 570 -430 620 -430 {lab=VCC}
N 380 -430 430 -430 {lab=VCC}
N 470 -430 530 -430 {lab=G}
N 470 -430 470 -400 {lab=G}
N 430 -400 470 -400 {lab=G}
N 430 -400 430 -310 {lab=G}
N 570 -400 570 -310 {lab=DIFFOUT}
N 570 -500 570 -460 {lab=VCC}
N 430 -500 570 -500 {lab=VCC}
N 430 -500 430 -460 {lab=VCC}
N 500 -520 500 -500 {lab=VCC}
N 570 -360 830 -360 {lab=DIFFOUT}
N 30 -370 30 -340 {lab=PLUS}
N 30 -370 60 -370 {lab=PLUS}
N 30 -520 30 -490 {lab=MINUS}
N 30 -520 60 -520 {lab=MINUS}
N 500 -150 500 -120 {lab=0}
N 500 -150 520 -150 {lab=0}
N 520 -180 520 -150 {lab=0}
N 500 -180 520 -180 {lab=0}
N 240 -150 240 -120 {lab=0}
N 220 -150 240 -150 {lab=0}
N 220 -180 220 -150 {lab=0}
N 220 -180 240 -180 {lab=0}
C {lab_pin.sym} 750 -300 0 0 {name=p20 lab=0  net_name=true}
C {lab_pin.sym} 30 -150 0 0 {name=p17 lab=0  net_name=true}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"  net_name=true}
C {nmos4.sym} 480 -180 0 0 {name=m1 model=cmosn w=5u l=2u m=1  net_name=true}
C {pmos4.sym} 550 -430 0 0 {name=m2 model=cmosp w=5u l=2u m=1  net_name=true}
C {vsource.sym} 30 -180 0 0 {name=VVCC value=5  net_name=true}
C {lab_pin.sym} 500 -120 0 0 {name=p1 lab=0  net_name=true}
C {lab_pin.sym} 60 -240 0 1 {name=p2 lab=VCC  net_name=true}
C {nmos4.sym} 260 -180 0 1 {name=m3 model=cmosn w=5u l=2u m=1  net_name=true}
C {lab_pin.sym} 240 -120 0 0 {name=p3 lab=0  net_name=true}
C {isource.sym} 240 -290 0 0 {name=IBIAS value=100u  net_name=true}
C {lab_pin.sym} 240 -320 0 0 {name=p4 lab=0  net_name=true}
C {nmos4.sym} 410 -280 0 0 {name=m4 model=cmosn w=10u l=1u m=1  net_name=true}
C {lab_pin.sym} 450 -280 0 1 {name=p5 lab=0  net_name=true}
C {nmos4.sym} 590 -280 0 1 {name=m5 model=cmosn w=10u l=1u m=1  net_name=true}
C {lab_pin.sym} 550 -280 0 0 {name=p0 lab=0  net_name=true}
C {lab_pin.sym} 620 -430 0 1 {name=p6 lab=VCC  net_name=true}
C {pmos4.sym} 450 -430 0 1 {name=m6 model=cmosp w=5u l=2u m=1  net_name=true}
C {lab_pin.sym} 380 -430 0 0 {name=p7 lab=VCC  net_name=true}
C {lab_pin.sym} 500 -520 0 0 {name=p8 lab=VCC  net_name=true}
C {lab_pin.sym} 370 -280 0 0 {name=p9 lab=PLUS  net_name=true}
C {lab_pin.sym} 630 -280 0 1 {name=p10 lab=MINUS  net_name=true}
C {lab_pin.sym} 830 -360 0 1 {name=p11 lab=DIFFOUT  net_name=true}
C {lab_pin.sym} 240 -230 0 0 {name=p13 lab=GN  net_name=true}
C {lab_pin.sym} 30 -280 0 0 {name=p14 lab=0  net_name=true}
C {vsource.sym} 30 -310 0 0 {name=VPLUS value="2.5 pwl 0 2.4 10n 2.4 10.1n 2.6"  net_name=true}
C {lab_pin.sym} 60 -370 0 1 {name=p15 lab=PLUS  net_name=true}
C {lab_pin.sym} 30 -430 0 0 {name=p16 lab=0  net_name=true}
C {vsource.sym} 30 -460 0 0 {name=V1 value=2.5  net_name=true}
C {lab_pin.sym} 60 -520 0 1 {name=p18 lab=MINUS  net_name=true}
C {capa.sym} 750 -330 0 0 {name=CL
m=1
value=2p
footprint=1206
device="ceramic capacitor"  net_name=true}
C {code.sym} 870 -190 0 0 {name=STIMULI
only_toplevel=true
value="
** ngspice
.temp 30

** xyce
* .step temp list 30

** ngspice
* .save all

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.option savecurrents
.include \\"models_cmos_example.txt\\"
.control
let cap = 2e-12
set filetype=binary
while cap <= 20e-12
alter CL cap
tran 0.8n 2000n
write cmos_example.raw
set appendwrite
let cap = cap + 2e-12
end
* op
write cmos_example.raw

.endc

** ngspice
* .save all

** xyce, not needed if -r given om cmdline
* .print tran format=raw v(diffout) v(plus) v(minus)
"  net_name=true}
C {ngspice_probe.sym} 430 -350 0 0 {name=r1}
C {ngspice_probe.sym} 480 -430 0 0 {name=r2}
C {ngspice_probe.sym} 550 -500 0 0 {name=r3}
C {ngspice_probe.sym} 620 -360 0 0 {name=r4}
C {ngspice_probe.sym} 330 -180 0 0 {name=r5}
C {ngspice_probe.sym} 30 -370 0 0 {name=r6}
C {ngspice_probe.sym} 30 -240 0 0 {name=r7}
C {ngspice_probe.sym} 30 -520 0 0 {name=r8}
C {ngspice_probe.sym} 500 -210 0 0 {name=r9}
C {ngspice_get_value.sym} 410 -440 0 1 {name=r11 node=i(@m6[id])}
C {ngspice_get_expr.sym} 590 -450 0 0 {name=r18 
node="[ngspice::get_current \{m2[id]\}]"
descr = Id
 }
C {lab_pin.sym} 430 -370 0 0 {name=p12 lab=G  net_name=true}
C {ngspice_get_expr.sym} 550 -450 0 1 {name=r10 
node="[format %.4g [expr [ngspice::get_voltage vcc] - [ngspice::get_voltage g]]]"
descr = Vsg
 }
C {ngspice_get_expr.sym} 530 -170 0 0 {name=r14 
node="[ngspice::get_current \{m1[id]\}]"
descr = Id
 }
C {ngspice_get_expr.sym} 210 -170 0 1 {name=r12 
node="[ngspice::get_current \{m3[id]\}]"
descr = Id
 }
