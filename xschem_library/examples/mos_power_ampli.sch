v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 15 270 -460 340 -390 {}
L 15 270 -330 340 -390 {}
T {ANALOG AUDIO AMPLIFIER
N-Channel only power stage} 430 -270 0 0 0.5 0.5 {layer=8}
N 180 -500 180 -470 {lab=E9}
N 260 -470 340 -470 {lab=E9}
N 340 -500 340 -470 {lab=E9}
N 1110 -700 1110 -670 {lab=SA}
N 840 -1020 840 -980 {lab=E4}
N 1110 -590 1110 -570 {lab=OUTI}
N 1110 -510 1110 -470 {lab=#net1}
N 180 -1020 180 -980 {lab=E1}
N 390 -790 520 -790 {lab=C2}
N 230 -950 230 -900 {lab=#net2}
N 180 -900 230 -900 {lab=#net2}
N 390 -790 390 -740 {lab=C2}
N 340 -740 390 -740 {lab=C2}
N 180 -920 180 -900 {lab=#net2}
N 220 -950 230 -950 {lab=#net2}
N 340 -760 340 -740 {lab=C2}
N 380 -790 390 -790 {lab=C2}
N 180 -1120 180 -1080 {lab=VBOOST}
N 840 -1120 840 -1080 {lab=VBOOST}
N 180 -1120 690 -1120 {lab=VBOOST}
N 690 -1120 840 -1120 {lab=VBOOST}
N 1110 -1180 1400 -1180 {lab=VPP}
N 840 -1120 1240 -1120 {lab=VBOOST}
N 860 -440 1070 -440 {lab=GB}
N 560 -460 560 -440 {lab=C8}
N 1240 -1120 1400 -1120 {lab=VBOOST}
N 1240 -900 1240 -870 {lab=#net3}
N 560 -580 560 -520 {lab=E8}
N 560 -760 560 -640 {lab=C6}
N 840 -780 840 -760 {lab=GA}
N 690 -810 690 -790 {lab=B1}
N 690 -810 800 -810 {lab=B1}
N 690 -1120 690 -880 {lab=VBOOST}
N 690 -820 690 -810 {lab=B1}
N 260 -470 260 -460 {lab=E9}
N 150 -70 260 -70 {lab=VSS}
N 50 -210 220 -210 {lab=B3}
N 260 -180 260 -130 {lab=E3}
N 150 -150 150 -70 {lab=VSS}
N 50 -150 50 -70 {lab=VSS}
N 50 -70 150 -70 {lab=VSS}
N 690 -600 1110 -600 {lab=OUTI}
N 180 -470 260 -470 {lab=E9}
N 1110 -610 1110 -600 {lab=OUTI}
N 860 -380 1110 -380 {lab=SB}
N 860 -700 1110 -700 {lab=SA}
N 1240 -1120 1240 -960 {lab=VBOOST}
N 1110 -410 1110 -380 {lab=SB}
N 1110 -730 1110 -700 {lab=SA}
N 860 -760 1070 -760 {lab=GA}
N 340 -740 340 -690 {lab=C2}
N 340 -630 340 -560 {lab=C9}
N 220 -630 340 -630 {lab=C9}
N 180 -600 180 -560 {lab=C5}
N 1110 -320 1110 -280 {lab=VNN}
N 1330 -590 1390 -590 {lab=OUT}
N 1110 -590 1240 -590 {lab=OUTI}
N 1110 -600 1110 -590 {lab=OUTI}
N 340 -860 340 -820 {lab=#net4}
N 560 -860 560 -820 {lab=#net5}
N 560 -1020 560 -920 {lab=E6}
N 340 -1020 340 -920 {lab=E2}
N 260 -280 260 -240 {lab=C3}
N 730 -440 770 -440 {lab=#net6}
N 560 -440 670 -440 {lab=C8}
N 690 -650 690 -600 {lab=OUTI}
N 690 -730 690 -710 {lab=#net7}
N 180 -840 180 -660 {lab=C7}
N 840 -860 840 -840 {lab=E11}
N 1240 -810 1240 -590 {lab=OUTI}
N 860 -760 860 -750 {lab=GA}
N 860 -710 860 -700 {lab=SA}
N 860 -440 860 -430 {lab=GB}
N 860 -390 860 -380 {lab=SB}
N 1240 -590 1270 -590 {lab=OUTI}
N 830 -440 860 -440 {lab=GB}
N 840 -760 860 -760 {lab=GA}
N 340 -1180 340 -1080 { lab=VPP}
N 560 -1180 560 -1080 { lab=VPP}
N 60 -1180 340 -1180 {lab=VPP}
N 340 -1180 560 -1180 {lab=VPP}
N 1110 -1180 1110 -790 { lab=VPP}
N 560 -1180 1110 -1180 {lab=VPP}
N 230 -950 800 -950 { lab=#net2}
C {ipin.sym} 530 -160 0 0 {name=p0 lab=PLUS}
C {ipin.sym} 530 -120 0 0 {name=p2 lab=VPP}
C {ipin.sym} 530 -100 0 0 {name=p3 lab=VNN}
C {nmos3.sym} 1090 -440 0 0 {name=xm2 model=irf540 m=1
program=evince
url="https://www.vishay.com/docs/91021/91021.pdf"
 net_name=true}
C {res.sym} 960 -410 0 1 {name=R7 m=1 value=190 net_name=true}
C {nmos3.sym} 1090 -760 0 0 {name=xm1 model=irf540 m=1
program=evince
url="https://www.vishay.com/docs/91021/91021.pdf" net_name=true}
C {res.sym} 960 -730 0 1 {name=R0 m=1 value=190 net_name=true}
C {lab_pin.sym} 1390 -590 0 1 {name=p14 lab=OUT}
C {lab_pin.sym} 1110 -280 0 0 {name=p18 lab=VNN}
C {lab_wire.sym} 920 -440 0 0 {name=l8 lab=GB}
C {res.sym} 340 -1050 0 1 {name=R2 m=1 value=50 net_name=true}
C {res.sym} 180 -1050 0 1 {name=R3 m=1 value=50 net_name=true}
C {lab_pin.sym} 340 -1080 0 0 {name=p10 lab=VPP}
C {npn.sym} 160 -530 0 0 {name=Q5 model=q2n2222 area=1 net_name=true}
C {pnp.sym} 820 -950 0 0 {name=Q4 model=q2n2907p area=1 net_name=true}
C {res.sym} 840 -1050 0 1 {name=R9 m=1 value=50 net_name=true}
C {pnp.sym} 540 -790 0 0 {name=Q6 model=q2n2907p area=1 net_name=true}
C {res.sym} 560 -1050 0 1 {name=R4 m=1 value=50 net_name=true}
C {lab_pin.sym} 180 -580 0 0 {name=p15 lab=C5}
C {lab_pin.sym} 340 -580 0 1 {name=p16 lab=C9}
C {lab_pin.sym} 180 -1000 0 0 {name=p17 lab=E1}
C {lab_pin.sym} 560 -1080 0 0 {name=p25 lab=VPP}
C {lab_pin.sym} 340 -970 0 1 {name=p23 lab=E2}
C {lab_pin.sym} 560 -970 0 1 {name=p28 lab=E6}
C {lab_pin.sym} 840 -1000 0 0 {name=p29 lab=E4}
C {lab_pin.sym} 180 -1120 0 0 {name=p34 lab=VBOOST}
C {ammeter.sym} 1110 -540 0 0 {name=vd  net_name=true current=0.2069}
C {ammeter.sym} 1110 -640 0 0 {name=vu  net_name=true current=0.2005}
C {lab_pin.sym} 60 -1180 0 0 {name=p27 lab=VPP}
C {pnp.sym} 200 -950 0 1 {name=Q1 model=q2n2907p area=1 net_name=true}
C {pnp.sym} 360 -790 0 1 {name=Q2 model=q2n2907p area=1 net_name=true}
C {lab_pin.sym} 140 -530 0 0 {name=p6 lab=PLUS}
C {lab_pin.sym} 380 -530 0 1 {name=p24 lab=MINUS}
C {npn.sym} 360 -530 0 1 {name=Q9 model=q2n2222 area=1 net_name=true}
C {lab_pin.sym} 560 -670 0 0 {name=p9 lab=C6}
C {code.sym} 1040 -180 0 0 {name=STIMULI
only_toplevel=true
value=".option PARHIER=LOCAL RUNLVL=6 post MODMONTE=1 warn maxwarn=6000
.option sampling_method = SRS ingold=1
.option method=gear
.option savecurrents
.temp 30



vvss vss 0 dc 0 
vvpp vpp 0 dc 50
vvnn vnn 0 dc -50

rfb1 vpp minus 100k
rfb2 out minus 100k
rfb3 minus vdc '100k/43'


vvdc vdc 0 dc 21.3
eref vref 0 plus vdc 45

vplus plus vdc dc 0

evboost vboost 0 vol='v(out)>=40 ? v(out)+10 : 50'
rload out 0 4

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in simulation directory.
.include \\"models_poweramp.txt\\"
.dc vplus '-1.5' '1.5' 0.1
.save all
.op
*.probe dc v(plus,vdc)
"}
C {lab_wire.sym} 920 -760 0 0 {name=l1 lab=GA}
C {res.sym} 800 -440 1 1 {name=R11 m=1 value=1300 net_name=true}
C {pnp.sym} 540 -490 0 0 {name=Q8 model=q2n2907p area=1 net_name=true}
C {capa.sym} 1240 -930 0 0 {name=C12 m=1 value="40u" net_name=true}
C {diode.sym} 1240 -1150 0 0 {name=D0 model=d1n4148 area=1
url="http://pdf.datasheetcatalog.com/datasheet/bytes/1N5406.pdf" net_name=true}
C {res.sym} 1240 -840 0 1 {name=R18 m=1 value=200 net_name=true}
C {zener.sym} 1400 -1150 0 0 {name=D1 model=d1n758 area=1
url="http://www.futurlec.com/Datasheet/Diodes/1N746-1N759.pdf" net_name=true}
C {res.sym} 690 -760 0 1 {name=R14 m=1 value=4k net_name=true}
C {pnp.sym} 820 -810 0 0 {name=Q11 model=q2n2907p area=1 net_name=true}
C {res.sym} 690 -850 0 1 {name=R15 m=1 value=4k net_name=true}
C {res.sym} 260 -430 0 1 {name=R5 m=1 value=120 net_name=true}
C {res.sym} 260 -370 0 1 {name=R6 m=1 value=120 net_name=true}
C {zener.sym} 150 -180 2 0 {name=D2 model=d1n755 area=1
url="http://www.futurlec.com/Datasheet/Diodes/1N746-1N759.pdf"
 net_name=true}
C {npn.sym} 240 -210 0 0 {name=Q3 model=q2n2222 area=1 net_name=true}
C {res.sym} 150 -240 0 1 {name=R1 m=1 value=10k net_name=true}
C {lab_pin.sym} 150 -270 0 0 {name=p7 lab=VPP}
C {res.sym} 260 -100 0 1 {name=R10 m=1 value=170 net_name=true}
C {lab_pin.sym} 50 -70 0 0 {name=p11 lab=VSS}
C {capa.sym} 50 -180 0 0 {name=C3 m=1 value=100n net_name=true}
C {res.sym} 560 -610 0 1 {name=R12 m=1 value=1300 net_name=true}
C {lab_pin.sym} 690 -800 0 0 {name=p12 lab=B1}
C {lab_pin.sym} 340 -470 0 1 {name=p13 lab=E9}
C {lab_pin.sym} 560 -440 0 0 {name=p19 lab=C8}
C {lab_pin.sym} 560 -560 0 1 {name=p20 lab=E8}
C {lab_pin.sym} 840 -850 0 0 {name=p21 lab=E11}
C {lab_pin.sym} 260 -160 0 1 {name=p22 lab=E3}
C {lab_pin.sym} 260 -270 0 0 {name=p26 lab=C3}
C {lab_pin.sym} 50 -210 0 0 {name=p30 lab=B3}
C {lab_pin.sym} 520 -490 0 0 {name=p33 lab=VSS}
C {res.sym} 340 -660 0 1 {name=R13 m=1 value=300 net_name=true}
C {npn.sym} 200 -630 0 1 {name=Q7 model=q2n2222 area=1 net_name=true}
C {lab_pin.sym} 180 -690 0 0 {name=p8 lab=C7}
C {lab_pin.sym} 340 -710 0 1 {name=p31 lab=C2}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
C {lab_pin.sym} 860 -700 0 0 {name=p32 lab=SA}
C {ammeter.sym} 1110 -350 0 0 {name=v0  net_name=true current=0.2263}
C {lab_pin.sym} 860 -380 0 0 {name=p35 lab=SB}
C {ammeter.sym} 560 -890 0 0 {name=v1  net_name=true current=0.01956}
C {ammeter.sym} 340 -890 0 0 {name=v2  net_name=true current=0.01947}
C {ammeter.sym} 260 -310 0 0 {name=v3  net_name=true current=0.03918}
C {ammeter.sym} 700 -440 3 0 {name=v4  net_name=true current=0.01941}
C {ammeter.sym} 690 -680 0 0 {name=v5  net_name=true current=0.006184}
C {ammeter.sym} 180 -870 0 1 {name=v6  net_name=true current=0.01949}
C {ammeter.sym} 840 -890 0 0 {name=v7  net_name=true current=0.01944}
C {spice_probe_vdiff.sym} 860 -410 0 0 {name=p37 voltage=3.688}
C {spice_probe_vdiff.sym} 860 -730 0 0 {name=p38 voltage=3.68}
C {ammeter.sym} 1300 -590 3 0 {name=v8  net_name=true current=-2.1216e-04}
C {opin.sym} 600 -130 0 0 {name=p5 lab=OUT}
C {ipin.sym} 530 -180 0 0 {name=p1 lab=MINUS}
C {ipin.sym} 530 -140 0 0 {name=p4 lab=VSS}
C {launcher.sym} 510 -330 0 0 {name=h2
descr="Ctrl-Click
Clear all probes" 
tclcommand="
    xschem push_undo
    xschem set no_undo 1
    xschem set no_draw 1
 
    set lastinst [xschem get instances]
    for \{ set i 0 \} \{ $i < $lastinst \} \{incr i \} \{
      set type [xschem getprop instance $i cell::type]
      if \{ [regexp \{(^|/)probe$\} $type ] \} \{
        xschem setprop $i voltage fast
      \}
      if \{ [regexp \{current_probe$\} $type ] \} \{
        xschem setprop $i current fast
      \}
      if \{ [regexp \{differential_probe$\} $type ] \} \{
        xschem setprop $i voltage fast
      \}
    \}
    xschem set no_undo 0
    xschem set no_draw 0
    xschem redraw
"
}
C {ngspice_probe.sym} 750 -1120 0 0 {name=p54}
C {ngspice_probe.sym} 180 -760 0 0 {name=p53}
C {ngspice_probe.sym} 560 -710 0 0 {name=p55}
C {ngspice_get_value.sym} 1130 -780 0 0 {name=nmos1 node=i(@r.$\{path\}xm1.rd[i])
descr="Id="}
C {ngspice_get_expr.sym} 800 -1000 0 1 {name=r8 
node="[format %.4g [expr [ngspice::get_voltage e4] - [ngspice::get_voltage c7]]]"
descr = veb
}
C {ngspice_get_expr.sym} 860 -980 0 0 {name=r9 
node="[format %.4g [expr [ngspice::get_current \{q4[ic]\}] / [ngspice::get_current \{q4[ib]\}] ] ]"
descr = beta
}
C {ngspice_probe.sym} 560 -830 0 0 {name=p41}
C {ngspice_probe.sym} 560 -530 0 1 {name=p42}
C {ngspice_probe.sym} 590 -440 2 1 {name=p47}
C {ngspice_get_expr.sym} 860 -920 0 0 {name=r15 
node="[format %.4g [expr ([ngspice::get_voltage e4] - [ngspice::get_voltage e11]) * [ngspice::get_current \{q4[ic]\}]]] W"
descr = power
}
C {ngspice_probe.sym} 260 -260 0 0 {name=p48}
C {ngspice_probe.sym} 90 -70 0 0 {name=p49}
C {ngspice_probe.sym} 100 -210 0 1 {name=p52}
C {ngspice_probe.sym} 1190 -590 0 1 {name=p39}
C {ngspice_probe.sym} 890 -700 2 1 {name=p43}
C {ngspice_probe.sym} 460 -790 2 1 {name=p44}
C {ngspice_probe.sym} 730 -810 2 1 {name=p46}
C {ngspice_probe.sym} 440 -950 0 0 {name=p50}
C {ngspice_probe.sym} 200 -470 0 0 {name=p45}
C {ngspice_probe.sym} 340 -600 0 0 {name=p51}
C {ngspice_get_expr.sym} 330 -900 0 1 {name=r17 
node="[ngspice::get_current v2]" 
descr = current
}
C {ngspice_get_expr.sym} 360 -1040 0 0 {name=r18 
node="[ngspice::get_current \{r2[i]\}]"
descr = current
}
C {ngspice_get_expr.sym} 860 -1040 0 0 {name=r19 
node="[ngspice::get_current \{r9[i]\}]"
descr = current
}
C {ngspice_get_expr.sym} 820 -890 0 1 {name=r2 
node="[ngspice::get_current \{q4[ic]\}]"
descr = current
}
C {ngspice_get_expr.sym} 800 -970 0 1 {name=r1 
node="[ngspice::get_current \{q4[ib]\}]"
descr = Ib
}
C {ngspice_get_expr.sym} 580 -460 0 0 {name=r11 
node="[format %.4g [expr ([ngspice::get_voltage e8] - [ngspice::get_voltage c8]) * [ngspice::get_current \{q8[ic]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 190 -860 0 0 {name=r12 
node="[ngspice::get_current v6]" 
descr = current
}
C {ngspice_get_expr.sym} 240 -210 0 1 {name=r6 
node="[format %.4g [expr [ngspice::get_current \{q3[ic]\}] / [ngspice::get_current \{q3[ib]\}] ] ]"
descr = beta
}
C {ngspice_get_expr.sym} 860 -780 0 0 {name=r16 
node="[format %.4g [expr ([ngspice::get_voltage e11] - [ngspice::get_voltage ga]) * [ngspice::get_current \{q11[ic]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 980 -720 0 0 {name=r20 
node="[ngspice::get_current \{r0[i]\}]" 
descr = current
}
C {ngspice_get_expr.sym} 280 -200 2 1 {name=r3 
node="[format %.4g [expr ([ngspice::get_voltage c3] - [ngspice::get_voltage e3]) * [ngspice::get_current \{q3[ic]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 240 -190 2 0 {name=r21 
node="[format %.4g [expr [ngspice::get_voltage b3] - [ngspice::get_voltage e3]]]"
descr = vbe
}
C {ngspice_get_expr.sym} 340 -390 0 0 {name=r7 
node="[format %.4g [expr ([ngspice::get_voltage e9] - [ngspice::get_voltage c3]) * [ngspice::get_current \{r5[i]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 570 -760 0 0 {name=r10 
node="[format %.4g [expr ([ngspice::get_voltage e6] - [ngspice::get_voltage c6]) * [ngspice::get_current \{q6[ic]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 210 -650 0 0 {name=r14 
node="[format %.4g [expr ([ngspice::get_voltage c7] - [ngspice::get_voltage c5]) * [ngspice::get_current \{q7[ic]\}]]]"
descr = power
}
C {ngspice_probe.sym} 180 -560 0 0 {name=p36}
C {ngspice_get_expr.sym} 200 -500 0 0 {name=r13 
node="[format %.4g [expr ([ngspice::get_voltage c5] - [ngspice::get_voltage e9]) * [ngspice::get_current \{q5[ic]\}]]]"
descr = power
}
C {ngspice_get_expr.sym} 320 -500 0 1 {name=r22 
node="[format %.4g [expr ([ngspice::get_voltage c9] - [ngspice::get_voltage e9]) * [ngspice::get_current \{q9[ic]\}]]]"
descr = power
}
C {ngspice_get_expr.sym} 330 -760 0 1 {name=r23 
node="[format %.4g [expr [ngspice::get_current \{q2[ic]\}] / [ngspice::get_current \{q2[ib]\}] ] ]"
descr = beta
}
C {ngspice_get_expr.sym} 1090 -640 0 1 {name=r24 
node="[ngspice::get_current vu]" 
descr = current
}
C {ngspice_get_expr.sym} 1090 -530 0 1 {name=r25 
node="[ngspice::get_current vd]" 
descr = current
}
C {ngspice_get_expr.sym} 1210 -1140 0 1 {name=r5 
node="[ngspice::get_current \{d0[id]\}]"
descr = current
}
C {ngspice_get_expr.sym} 1370 -1150 0 1 {name=r26 
node="[ngspice::get_current \{d1[id]\}]"
descr = current
}
C {ngspice_get_value.sym} 1130 -460 0 0 {name=r27 node=i(@r.$\{path\}xm2.rd[i])
descr="Id="}
C {ngspice_get_expr.sym} 160 -230 0 0 {name=r28 
node="[format %.4g [expr ([ngspice::get_node v(vpp)] - [ngspice::get_voltage b3]) * [ngspice::get_current \{r1[i]\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 1130 -740 2 1 {name=r29 
node="[format %.4g [expr ([ngspice::get_node v(vpp)] - [ngspice::get_voltage outi]) * [ngspice::get_current \{vu\}]]] W"
descr = power
}
C {lab_wire.sym} 910 -600 0 1 {name=l40 lab=OUTI}
C {ngspice_get_expr.sym} 980 -400 0 0 {name=r30 
node="[ngspice::get_current \{r7[i]\}]" 
descr = current
}
C {ngspice_get_expr.sym} 700 -440 2 0 {name=r31 
node="[ngspice::get_current \{v4\}]" 
descr = current
}
C {ngspice_get_expr.sym} 1130 -420 2 1 {name=r32 
node="[format %.4g [expr ([ngspice::get_voltage outi] - [ngspice::get_node v(vnn)]) * [ngspice::get_current \{vd\}]]] W"
descr = power
}
C {ngspice_get_expr.sym} 150 -160 2 0 {name=r33 
node="[format %.4g [expr -[ngspice::get_voltage b3] * [ngspice::get_current \{d2[id]\}]]] W"
descr = power
}
C {launcher.sym} 770 -70 0 0 {name=h3
descr="Load file into gaw" 
comment="
  This launcher gets raw filename from current schematic using 'xschem get schname'
  and stripping off path and suffix.  It then loads raw file into gaw.
  This allow to use it in any schematic without changes.
"
tclcommand="
set rawfile [file tail [file rootname [xschem get schname]]].raw
gaw_cmd \\"tabledel $rawfile
load $netlist_dir/$rawfile
table_set $rawfile\\"
unset rawfile"
}
C {spice_probe.sym} 1010 -760 0 0 {name=p40  voltage=3.543}
C {spice_probe.sym} 1000 -440 0 0 {name=p56  voltage=-46.18}
C {spice_probe.sym} 420 -790 0 0 {name=p57 voltage=48.06}
C {spice_probe.sym} 280 -950 0 0 {name=p58  voltage=47.27}
C {spice_probe.sym} 180 -720 0 0 {name=p59  voltage=47.27}
C {spice_probe.sym} 1020 -1120 0 0 {name=p62  voltage=49.04}
C {launcher.sym} 770 -110 0 0 {name=h1
descr=Backannotate
tclcommand="ngspice::annotate"}
C {launcher.sym} 770 -150 0 0 {name=h4
descr="View Raw file" 
tclcommand="textwindow $netlist_dir/[file tail [file rootname [ xschem get schname 0 ] ] ].raw"
}
