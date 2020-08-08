v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
N 40 -270 40 -250 {lab=vss}
N 220 -270 220 -250 {lab=vss}
N 870 -1120 870 -1090 {lab=LDA[3]}
N 870 -1090 910 -1090 {lab=LDA[3]}
N 870 -1050 870 -1020 {lab=LDA[2]}
N 870 -1050 910 -1050 {lab=LDA[2]}
N 870 -920 870 -890 {lab=LDA[3]}
N 870 -890 910 -890 {lab=LDA[3]}
N 870 -850 870 -820 {lab=#net1}
N 870 -850 910 -850 {lab=#net1}
N 870 -720 870 -690 {lab=#net2}
N 870 -690 910 -690 {lab=#net2}
N 870 -650 870 -620 {lab=LDA[2]}
N 870 -650 910 -650 {lab=LDA[2]}
N 870 -520 870 -490 {lab=#net3}
N 870 -490 910 -490 {lab=#net3}
N 870 -450 870 -420 {lab=#net4}
N 870 -450 910 -450 {lab=#net4}
N 790 -1120 870 -1120 {lab=LDA[3]}
N 800 -1020 870 -1020 {lab=LDA[2]}
N 790 -1020 800 -1020 {lab=LDA[2]}
N 790 -920 870 -920 {lab=LDA[3]}
N 790 -620 870 -620 {lab=LDA[2]}
N 330 -1120 330 -1090 {lab=LDA[1]}
N 330 -1090 370 -1090 {lab=LDA[1]}
N 330 -1050 330 -1020 {lab=LDA[0]}
N 330 -1050 370 -1050 {lab=LDA[0]}
N 330 -920 330 -890 {lab=LDA[1]}
N 330 -890 370 -890 {lab=LDA[1]}
N 330 -850 330 -820 {lab=#net5}
N 330 -850 370 -850 {lab=#net5}
N 330 -720 330 -690 {lab=#net6}
N 330 -690 370 -690 {lab=#net6}
N 330 -650 330 -620 {lab=LDA[0]}
N 330 -650 370 -650 {lab=LDA[0]}
N 330 -520 330 -490 {lab=#net7}
N 330 -490 370 -490 {lab=#net7}
N 330 -450 330 -420 {lab=#net8}
N 330 -450 370 -450 {lab=#net8}
N 250 -1120 330 -1120 {lab=LDA[1]}
N 260 -1020 330 -1020 {lab=LDA[0]}
N 250 -1020 260 -1020 {lab=LDA[0]}
N 250 -920 330 -920 {lab=LDA[1]}
N 250 -620 330 -620 {lab=LDA[0]}
N 900 -280 900 -160 {lab=LDZ[15:0]}
N 820 -280 820 -160 {lab=#net9}
N 740 -280 740 -160 {lab=#net10}
C {opin.sym} 250 -190 0 0 {name=p5 lab=LDZ[15:0]}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {ipin.sym} 130 -190 0 0 {name=p1 lab=LDA[3:0]}
C {ipin.sym} 130 -150 0 0 {name=p3 lab=vcc}
C {ipin.sym} 130 -130 0 0 {name=p0 lab=vss}
C {lvnot.sym} 780 -160 0 0 {name=x325[15:0] m=1 
+ wn=10u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 860 -160 0 0 {name=x8[15:0] m=1 
+ wn=24u lln=2.4u wp=80u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnor2.sym} 680 -160 0 0 {name=x9[15:0] m=1 
+ wna=10u lna=2.4u wpa=40u lpa=2.4u
+ wnb=10u lnb=2.4u wpb=40u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 900 -160 0 1 {name=p59 lab=LDZ[15:0]}
C {lab_pin.sym} 640 -180 0 0 {name=p61 lab=LDY[3:0]*4}
C {lab_pin.sym} 640 -140 0 0 {name=p62 lab=4*LDX[3:0]}
C {lab_pin.sym} 40 -250 0 1 {name=p18 lab=vss}
C {capa.sym} 40 -300 0 0 {name=c1[3:0] m=1 value=14f}
C {lab_pin.sym} 40 -330 0 1 {name=p4 lab=LDX[3:0]}
C {lab_pin.sym} 220 -250 0 1 {name=p6 lab=vss}
C {capa.sym} 220 -300 0 0 {name=c2[3:0] m=1 value=14f}
C {lab_pin.sym} 220 -330 0 1 {name=p7 lab=LDY[3:0]}
C {lvnand2.sym} 950 -1070 0 0 {name=x2 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 790 -1120 0 0 {name=p2 lab=LDA[3]}
C {lab_pin.sym} 790 -1020 0 0 {name=p9 lab=LDA[2]}
C {lab_pin.sym} 1010 -1070 0 1 {name=p10 lab=LDY[3]}
C {lvnand2.sym} 950 -870 0 0 {name=x3 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 1010 -870 0 1 {name=p13 lab=LDY[2]}
C {lvnand2.sym} 950 -670 0 0 {name=x4 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 1010 -670 0 1 {name=p16 lab=LDY[1]}
C {lvnand2.sym} 950 -470 0 0 {name=x6 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 1010 -470 0 1 {name=p20 lab=LDY[0]}
C {lvnot.sym} 830 -820 0 0 {name=x12 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 830 -720 0 0 {name=x1 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 830 -420 0 0 {name=x5 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 830 -520 0 0 {name=x7 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 790 -920 0 0 {name=p11 lab=LDA[3]}
C {lab_pin.sym} 790 -820 0 0 {name=p12 lab=LDA[2]}
C {lab_pin.sym} 790 -720 0 0 {name=p14 lab=LDA[3]}
C {lab_pin.sym} 790 -620 0 0 {name=p15 lab=LDA[2]}
C {lab_pin.sym} 790 -520 0 0 {name=p17 lab=LDA[3]}
C {lab_pin.sym} 790 -420 0 0 {name=p19 lab=LDA[2]}
C {lvnand2.sym} 410 -1070 0 0 {name=x10 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 250 -1120 0 0 {name=p21 lab=LDA[1]}
C {lab_pin.sym} 250 -1020 0 0 {name=p22 lab=LDA[0]}
C {lab_pin.sym} 470 -1070 0 1 {name=p23 lab=LDX[3]}
C {lvnand2.sym} 410 -870 0 0 {name=x20 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 470 -870 0 1 {name=p8 lab=LDX[2]}
C {lvnand2.sym} 410 -670 0 0 {name=x11 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 470 -670 0 1 {name=p24 lab=LDX[1]}
C {lvnand2.sym} 410 -470 0 0 {name=x13 m=1 
+ wna=20u lna=2.4u wpa=20u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=20u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss VSSBPIN=VSS}
C {lab_pin.sym} 470 -470 0 1 {name=p25 lab=LDX[0]}
C {lvnot.sym} 290 -820 0 0 {name=x14 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 290 -720 0 0 {name=x15 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 290 -420 0 0 {name=x16 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 290 -520 0 0 {name=x17 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 250 -920 0 0 {name=p28 lab=LDA[1]}
C {lab_pin.sym} 250 -820 0 0 {name=p30 lab=LDA[0]}
C {lab_pin.sym} 250 -720 0 0 {name=p31 lab=LDA[1]}
C {lab_pin.sym} 250 -620 0 0 {name=p32 lab=LDA[0]}
C {lab_pin.sym} 250 -520 0 0 {name=p33 lab=LDA[1]}
C {lab_pin.sym} 250 -420 0 0 {name=p34 lab=LDA[0]}
C {lvnot.sym} 780 -280 0 0 {name=x18[15:0] m=1 
+ wn=10u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 860 -280 0 0 {name=x19[15:0] m=1 
+ wn=24u lln=2.4u wp=80u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
