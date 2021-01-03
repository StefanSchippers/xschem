v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
T {CMOS DIFFERENTIAL AMPLIFIER
EXAMPLE} 250 -650 0 0 0.4 0.4 {}
N 30 -310 30 -280 {lab=VCC}
N 30 -310 60 -310 {lab=VCC}
N 260 -240 260 -190 {lab=GN}
N 260 -190 300 -190 {lab=GN}
N 300 -190 300 -160 {lab=GN}
N 300 -160 530 -160 {lab=GN}
N 500 -290 520 -290 {lab=0}
N 670 -290 690 -290 {lab=0}
N 500 -260 690 -260 {lab=#net1}
N 440 -290 460 -290 {lab=PLUS}
N 730 -290 750 -290 {lab=MINUS}
N 690 -490 740 -490 {lab=VCC}
N 450 -490 500 -490 {lab=VCC}
N 540 -490 650 -490 {lab=G}
N 540 -490 540 -460 {lab=G}
N 500 -460 540 -460 {lab=G}
N 690 -560 690 -520 {lab=VCC}
N 500 -560 690 -560 {lab=VCC}
N 500 -560 500 -520 {lab=VCC}
N 570 -580 570 -560 {lab=VCC}
N 690 -420 830 -420 {lab=DIFFOUT}
N 30 -440 30 -410 {lab=PLUS}
N 30 -440 60 -440 {lab=PLUS}
N 30 -590 30 -560 {lab=MINUS}
N 30 -590 60 -590 {lab=MINUS}
N 570 -130 570 -100 {lab=0}
N 570 -130 590 -130 {lab=0}
N 590 -160 590 -130 {lab=0}
N 570 -160 590 -160 {lab=0}
N 260 -130 260 -100 {lab=0}
N 240 -130 260 -130 {lab=0}
N 240 -160 240 -130 {lab=0}
N 240 -160 260 -160 {lab=0}
N 690 -460 690 -380 { lab=DIFFOUT}
N 500 -460 500 -380 { lab=G}
N 570 -260 570 -250 { lab=#net1}
N 690 -380 690 -320 { lab=DIFFOUT}
N 500 -380 500 -320 { lab=G}
N 570 -250 570 -190 { lab=#net1}
C {lab_pin.sym} 750 -360 0 0 {name=p20 lab=0  net_name=true}
C {lab_pin.sym} 30 -220 0 0 {name=p17 lab=0  net_name=true}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"  net_name=true}
C {nmos4.sym} 550 -160 0 0 {name=m1 model=cmosn w=5u l=2u m=1  net_name=true}
C {pmos4.sym} 670 -490 0 0 {name=m2 model=cmosp w=5u l=2u m=1  net_name=true}
C {vsource.sym} 30 -250 0 0 {name=VVCC value=5  net_name=true}
C {lab_pin.sym} 570 -100 0 0 {name=p1 lab=0  net_name=true}
C {lab_pin.sym} 60 -310 0 1 {name=p2 lab=VCC  net_name=true}
C {nmos4.sym} 280 -160 0 1 {name=m3 model=cmosn w=5u l=2u m=1  net_name=true}
C {lab_pin.sym} 260 -100 0 0 {name=p3 lab=0  net_name=true}
C {isource.sym} 260 -270 0 0 {name=IBIAS value=100u  net_name=true}
C {lab_pin.sym} 260 -300 0 0 {name=p4 lab=0  net_name=true}
C {nmos4.sym} 480 -290 0 0 {name=m4 model=cmosn w=10u l=1u m=1  net_name=true}
C {lab_pin.sym} 520 -290 0 1 {name=p5 lab=0  net_name=true}
C {nmos4.sym} 710 -290 0 1 {name=m5 model=cmosn w=10u l=1u m=1  net_name=true}
C {lab_pin.sym} 670 -290 0 0 {name=p0 lab=0  net_name=true}
C {lab_pin.sym} 740 -490 0 1 {name=p6 lab=VCC  net_name=true}
C {pmos4.sym} 520 -490 0 1 {name=m6 model=cmosp w=5u l=2u m=1  net_name=true}
C {lab_pin.sym} 450 -490 0 0 {name=p7 lab=VCC  net_name=true}
C {lab_pin.sym} 570 -580 0 0 {name=p8 lab=VCC  net_name=true}
C {lab_pin.sym} 440 -290 0 0 {name=p9 lab=PLUS  net_name=true}
C {lab_pin.sym} 750 -290 0 1 {name=p10 lab=MINUS  net_name=true}
C {lab_pin.sym} 830 -420 0 1 {name=p11 lab=DIFFOUT  net_name=true}
C {lab_pin.sym} 260 -210 0 0 {name=p13 lab=GN  net_name=true}
C {lab_pin.sym} 30 -350 0 0 {name=p14 lab=0  net_name=true}
C {vsource.sym} 30 -380 0 0 {name=VPLUS value="2.5 pwl 0 2.4 10n 2.4 10.1n 2.6"  net_name=true}
C {lab_pin.sym} 60 -440 0 1 {name=p15 lab=PLUS  net_name=true}
C {lab_pin.sym} 30 -500 0 0 {name=p16 lab=0  net_name=true}
C {vsource.sym} 30 -530 0 0 {name=V1 value=2.5  net_name=true}
C {lab_pin.sym} 60 -590 0 1 {name=p18 lab=MINUS  net_name=true}
C {capa.sym} 750 -390 0 0 {name=CL
m=1
value=2p
footprint=1206
device="ceramic capacitor"  net_name=true}
C {code.sym} 900 -190 0 0 {name=STIMULI
only_toplevel=true
value=".temp 30
** models are generally not free: you must download
** SPICE models for active devices and put them into the below 
** referenced file in netlist/simulation directory.
.include \\"models_cmos_example.txt\\"
.option savecurrents
.save all @m4[gm] @m5[gm] @m1[gm]
.control
op
write cmos_example.raw
.endc

"  net_name=true}
C {lab_pin.sym} 500 -430 0 0 {name=p12 lab=G  net_name=true}
C {launcher.sym} 700 -60 0 0 {name=h1
descr=Backannotate
tclcommand="ngspice::annotate"}
C {launcher.sym} 700 -110 0 0 {name=h2
descr="View raw file"
tclcommand="textwindow $netlist_dir/cmos_example.raw"}
C {ngspice_probe.sym} 500 -390 0 0 {name=r1}
C {ngspice_probe.sym} 420 -160 0 0 {name=r2}
C {ngspice_probe.sym} 600 -260 0 0 {name=r3}
C {ngspice_probe.sym} 770 -420 0 0 {name=r4}
C {ngspice_get_value.sym} 620 -160 0 0 {name=r5 node=i(@$\{path\}m1[id])
descr="I="}
