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
N 500 -250 500 -210 {lab=SN}
N 430 -280 450 -280 {lab=0}
N 550 -280 570 -280 {lab=0}
N 430 -250 570 -250 {lab=SN}
N 370 -280 390 -280 {lab=PLUS}
N 610 -280 630 -280 {lab=MINUS}
N 570 -430 590 -430 {lab=VCC}
N 410 -430 430 -430 {lab=VCC}
N 470 -430 530 -430 {lab=GP}
N 470 -430 470 -400 {lab=GP}
N 430 -400 470 -400 {lab=GP}
N 430 -400 430 -310 {lab=GP}
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
.endc

** ngspice
* .save all

** xyce, not needed if -r given om cmdline
* .print tran format=raw v(diffout) v(plus) v(minus)
"}
C {lab_pin.sym} 30 -150 0 0 {name=p17 lab=0}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {nmos4.sym} 480 -180 0 0 {name=m1 model=cmosn w=5u l=2u m=1 net_name=true}
C {pmos4.sym} 550 -430 0 0 {name=m2 model=cmosp w=5u l=2u m=1 net_name=true}
C {vsource.sym} 30 -180 0 0 {name=VVCC value=5 net_name=true}
C {lab_pin.sym} 500 -120 0 0 {name=p1 lab=0}
C {lab_pin.sym} 60 -240 0 1 {name=p2 lab=VCC}
C {nmos4.sym} 260 -180 0 1 {name=m3 model=cmosn w=5u l=2u m=1 net_name=true}
C {lab_pin.sym} 240 -120 0 0 {name=p3 lab=0}
C {isource.sym} 240 -290 0 0 {name=IBIAS value=100u net_name=true}
C {lab_pin.sym} 240 -320 0 0 {name=p4 lab=0}
C {nmos4.sym} 410 -280 0 0 {name=m4 model=cmosn w=10u l=1u m=1 net_name=true}
C {lab_pin.sym} 450 -280 0 1 {name=p5 lab=0}
C {nmos4.sym} 590 -280 0 1 {name=m5 model=cmosn w=10u l=1u m=1 net_name=true}
C {lab_pin.sym} 550 -280 0 0 {name=p0 lab=0}
C {lab_pin.sym} 590 -430 0 1 {name=p6 lab=VCC}
C {pmos4.sym} 450 -430 0 1 {name=m6 model=cmosp w=5u l=2u m=1 net_name=true}
C {lab_pin.sym} 410 -430 0 0 {name=p7 lab=VCC}
C {lab_pin.sym} 500 -520 0 0 {name=p8 lab=VCC}
C {lab_pin.sym} 370 -280 0 0 {name=p9 lab=PLUS}
C {lab_pin.sym} 630 -280 0 1 {name=p10 lab=MINUS}
C {lab_pin.sym} 830 -360 0 1 {name=p11 lab=DIFFOUT}
C {lab_pin.sym} 430 -380 0 0 {name=p12 lab=GP}
C {lab_pin.sym} 240 -230 0 0 {name=p13 lab=GN}
C {lab_pin.sym} 30 -280 0 0 {name=p14 lab=0}
C {vsource.sym} 30 -310 0 0 {name=VPLUS value="2.5 pwl 0 2.4 10n 2.4 10.1n 2.6" net_name=true}
C {lab_pin.sym} 60 -370 0 1 {name=p15 lab=PLUS}
C {lab_pin.sym} 30 -430 0 0 {name=p16 lab=0}
C {vsource.sym} 30 -460 0 0 {name=V1 value=2.5 net_name=true}
C {lab_pin.sym} 60 -520 0 1 {name=p18 lab=MINUS}
C {lab_pin.sym} 500 -230 0 0 {name=p19 lab=SN}
C {capa.sym} 750 -330 0 0 {name=CL
m=1
value=2p
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 750 -300 0 0 {name=p20 lab=0}
