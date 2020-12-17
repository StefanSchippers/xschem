v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 3 850 -270 850 -60 {}
L 3 1050 -270 1050 -60 {}
L 3 1250 -270 1250 -60 {}
L 8 820 -220 850 -220 {}
L 8 850 -220 850 -180 {}
L 8 850 -180 950 -180 {}
L 8 950 -220 950 -180 {}
L 8 950 -220 1050 -220 {}
L 8 1050 -220 1050 -180 {}
L 8 1050 -180 1150 -180 {}
L 8 1150 -220 1150 -180 {}
L 8 1150 -220 1250 -220 {}
L 8 1250 -220 1250 -180 {}
L 8 1250 -180 1300 -180 {}
L 8 950 -160 950 -120 {}
L 8 950 -160 980 -160 {}
L 8 980 -160 980 -120 {}
L 8 980 -120 1150 -120 {}
L 8 1150 -160 1150 -120 {}
L 8 1150 -160 1180 -160 {}
L 8 1180 -160 1180 -120 {}
L 8 1180 -120 1300 -120 {}
L 8 820 -120 950 -120 {}
B 7 950 -250 980 -80 {}
B 7 1150 -250 1180 -80 {}
B 21 10 -970 240 -750 {}
T {CP} 810 -210 0 1 0.4 0.4 {}
T {PRECH} 810 -160 0 1 0.4 0.4 {}
T {current
monitors} 30 -1030 0 0 0.4 0.4 {}
T {was: vss} 880 -980 0 0 0.4 0.4 {}
T {16KB ROM Macrocell
16 bit Data I/O x 8KWords} 210 -1120 0 0 0.7 0.7 {}
N 150 -580 150 -560 {lab=vss}
N 150 -420 150 -400 {lab=vss}
N 10 -270 10 -250 {lab=vss}
N 10 -580 10 -560 {lab=vss}
N 10 -420 10 -400 {lab=vss}
N 1720 -350 1720 -330 {lab=vss}
N 1720 -530 1720 -510 {lab=vss}
N 1540 -350 1540 -330 {lab=vss}
N 1540 -530 1540 -510 {lab=vss}
N 1720 -180 1720 -160 {lab=vss}
N 1540 -180 1540 -160 {lab=vss}
C {title.sym} 170 0 0 0 {name=l3 author="Stefan Schippers"}
C {code.sym} 590 -160 0 0 {name=STIMULI value="
.options SCALE=0.10
.param VCC=1.5
.temp 25
.param WPRECH=30u

vvss vss 0 0

** to generate following file: 
** copy .../share/doc/xschem/rom8k/stimuli.rom8k to simulation directory
** then do 'Simulation->Utile Stimuli Editor (GUI)' and press 'Translate'
.include stimuli_rom8k.cir

* .op ALL  4n
*.dc vvcc 0 2 0.1
.tran 0.2n 480n uic

** download models from here: 
** http://www.amarketplaceofideas.com/wp-content/uploads/2014/11/180nm-V1.7z
** and save to 'models_rom8k.txt' in simulation directory
.include models_rom8k.txt

"
}
C {rom2_coldec.sym} 1190 -890 0 0 {name=xcdec[15:0]}
C {lab_pin.sym} 1040 -910 0 0 {name=p377 lab=LDY1[15:0]}
C {lab_pin.sym} 1040 -890 0 0 {name=p378 lab=LDBL[255:0]}
C {lab_pin.sym} 1040 -870 0 0 {name=p381 lab=vss}
C {lab_pin.sym} 1340 -910 0 1 {name=p382 lab=LDYMS[15:0]}
C {rom2_sa.sym} 560 -390 0 0 {name=xsa[15:0]}
C {lab_pin.sym} 710 -450 0 1 {name=p340 lab=LDQ[15:0]}
C {lab_pin.sym} 410 -430 0 0 {name=p39 lab=LDYMS[15:0]}
C {lab_pin.sym} 410 -350 0 0 {name=p26 lab=vccsa}
C {lab_pin.sym} 410 -330 0 0 {name=p31 lab=vss}
C {lab_pin.sym} 410 -410 0 0 {name=p40 lab=LDOE}
C {vsource.sym} 90 -920 0 0 {name=vsa value=0}
C {lab_pin.sym} 90 -950 0 0 {name=p44 lab=vcc}
C {lab_pin.sym} 90 -890 0 0 {name=p45 lab=vccsa}
C {vsource.sym} 190 -820 0 0 {name=vdec value=0}
C {lab_pin.sym} 190 -850 0 0 {name=p48 lab=vcc}
C {lab_pin.sym} 190 -790 0 0 {name=p49 lab=vccdec}
C {vsource.sym} 90 -820 0 0 {name=vl value=0}
C {lab_pin.sym} 90 -850 0 0 {name=p50 lab=vcc}
C {lab_pin.sym} 90 -790 0 0 {name=p51 lab=vccl}
C {lab_pin.sym} 410 -390 0 0 {name=p5 lab=LDPRECH}
C {lab_pin.sym} 410 -370 0 0 {name=p11 lab=LDSAL}
C {rom2_addlatch.sym} 1190 -490 0 0 {name=xlat}
C {lab_pin.sym} 1340 -530 0 1 {name=p10 lab=LDEN_LAT}
C {lab_pin.sym} 1340 -510 0 1 {name=p16 lab=LDAI[12:0]}
C {lab_pin.sym} 1040 -530 0 0 {name=p17 lab=LDEN}
C {lab_pin.sym} 1040 -490 0 0 {name=p19 lab=LDA[12:0]}
C {lab_pin.sym} 1040 -470 0 0 {name=p21 lab=vccl}
C {lab_pin.sym} 1040 -450 0 0 {name=p22 lab=vss}
C {rom2_ctrl.sym} 1190 -360 0 0 {name=xctrl}
C {lab_pin.sym} 1040 -420 0 0 {name=p27 lab=LDEN_LAT}
C {lab_pin.sym} 1040 -400 0 0 {name=p28 lab=LDCP}
C {lab_pin.sym} 1040 -360 0 0 {name=p29 lab=vss}
C {lab_pin.sym} 1040 -380 0 0 {name=p30 lab=vccl}
C {lab_pin.sym} 1340 -420 0 1 {name=p32 lab=LDPRECH}
C {lab_pin.sym} 1340 -400 0 1 {name=p33 lab=LDSAL}
C {lab_pin.sym} 1340 -380 0 1 {name=p34 lab=LDCP_ROWDEC}
C {lab_pin.sym} 1340 -320 0 1 {name=p37 lab=LDCP_COL_B}
C {lab_pin.sym} 1340 -340 0 1 {name=p36 lab=LDCP_ADDLAT_B}
C {lab_pin.sym} 1040 -510 0 0 {name=p18 lab=LDCP_ADDLAT_B}
C {lab_pin.sym} 1340 -360 0 1 {name=p8 lab=LDCP_SA}
C {lab_pin.sym} 410 -450 0 0 {name=p35 lab=LDCP_SA}
C {rom2_predec3.sym} 560 -520 0 0 {name=xcpre}
C {lab_pin.sym} 410 -540 0 0 {name=p25 lab=LDAI[3:0]}
C {lab_pin.sym} 410 -520 0 0 {name=p56 lab=vccdec}
C {lab_pin.sym} 410 -500 0 0 {name=p57 lab=vss}
C {lab_pin.sym} 710 -540 0 1 {name=p61 lab=LDY1[15:0]}
C {lab_pin.sym} 150 -640 0 1 {name=p53 lab=LDL1X[15:0]}
C {lab_pin.sym} 150 -560 0 1 {name=p54 lab=vss}
C {capa.sym} 150 -610 0 0 {name=c1[15:0] m=1 value=210f}
C {lab_pin.sym} 150 -400 0 1 {name=p62 lab=vss}
C {capa.sym} 150 -450 0 0 {name=c2[3:0] m=1 value=120f}
C {lab_pin.sym} 150 -480 0 1 {name=p63 lab=LDL2X[3:0]}
C {lab_pin.sym} 10 -250 0 1 {name=p67 lab=vss}
C {capa.sym} 10 -300 0 0 {name=c4[12:0] m=1 value=40f}
C {lab_pin.sym} 10 -330 0 1 {name=p68 lab=LDAI[12:0]}
C {lab_pin.sym} 10 -560 0 1 {name=p72 lab=vss}
C {capa.sym} 10 -610 0 0 {name=c5[15:0] m=1 value=45f}
C {lab_pin.sym} 10 -640 0 1 {name=p71 lab=LDY1[15:0]}
C {rom3_rowdec.sym} 560 -790 0 0 {name=xrdec[31:0]}
C {lab_pin.sym} 410 -800 0 0 {name=p1091 lab=LDL3X[7:0]*4}
C {lab_pin.sym} 410 -840 0 0 {name=p1092 lab=LDL1X[15:0]}
C {lab_pin.sym} 710 -840 0 1 {name=p1093 lab=LDWL[511:0]}
C {lab_pin.sym} 410 -760 0 0 {name=p1094 lab=vss}
C {lab_pin.sym} 410 -740 0 0 {name=p1095 lab=vccdec}
C {lab_pin.sym} 410 -780 0 0 {name=p1096 lab=LDCP_ROWDEC}
C {lab_pin.sym} 410 -820 0 0 {name=p1097 lab=8*LDL2X[3:0]}
C {lab_pin.sym} 710 -700 0 1 {name=p1098 lab=LDL1X[15:0]}
C {rom2_predec1.sym} 560 -680 0 0 {name=xrpre1}
C {lab_pin.sym} 410 -700 0 0 {name=p1099 lab=LDAI[7:4]}
C {lab_pin.sym} 410 -680 0 0 {name=p1101 lab=vccdec}
C {lab_pin.sym} 410 -660 0 0 {name=p1102 lab=vss}
C {rom2_predec4.sym} 560 -600 0 0 {name=xrpre2}
C {lab_pin.sym} 410 -620 0 0 {name=p1103 lab=LDAI[12:8]}
C {lab_pin.sym} 410 -600 0 0 {name=p1105 lab=vccdec}
C {lab_pin.sym} 410 -580 0 0 {name=p1106 lab=vss}
C {lab_pin.sym} 710 -620 0 1 {name=p1107 lab=LDL2X[3:0]}
C {lab_pin.sym} 710 -600 0 1 {name=p1108 lab=LDL3X[7:0]}
C {lab_pin.sym} 10 -400 0 1 {name=p4 lab=vss}
C {capa.sym} 10 -450 0 0 {name=c3[7:0] m=1 value=120f}
C {lab_pin.sym} 10 -480 0 1 {name=p0 lab=LDL3X[7:0]}
C {lab_pin.sym} 1040 -810 0 0 {name=p23 lab=vccdec}
C {rom2_coldec_ref.sym} 1190 -810 0 0 {name=xcdecref}
C {lab_pin.sym} 1340 -830 0 1 {name=p41 lab=LDYMSREF}
C {lab_pin.sym} 1040 -830 0 0 {name=p42 lab=LDBLREF}
C {lab_pin.sym} 1340 -300 0 1 {name=p6 lab=LDYMSREF}
C {lab_pin.sym} 1040 -790 0 0 {name=p12 lab=vss}
C {lab_pin.sym} 1340 -620 0 1 {name=p9 lab=LDBLREF}
C {rom3_array_ref.sym} 1190 -610 0 0 {name=xarr_ref}
C {lab_pin.sym} 1040 -620 0 0 {name=p2 lab=LDWL[511:0]}
C {lab_pin.sym} 1040 -600 0 0 {name=p14 lab=vss}
C {rom2_col_prech.sym} 1190 -990 0 0 {name=xcpr[256:0]}
C {lab_pin.sym} 1340 -1000 0 1 {name=p20 lab=LDBL[255:0],LDBLREF}
C {lab_pin.sym} 1040 -1000 0 0 {name=p1 lab=LDCP_COL_B}
C {lab_pin.sym} 1040 -980 0 0 {name=p38 lab=vss}
C {lab_pin.sym} 1720 -410 0 1 {name=p47 lab=LDPRECH}
C {lab_pin.sym} 1720 -330 0 1 {name=p52 lab=vss}
C {capa.sym} 1720 -380 0 0 {name=c0 m=1 value=66f}
C {lab_pin.sym} 1720 -590 0 1 {name=p55 lab=LDSAL}
C {lab_pin.sym} 1720 -510 0 1 {name=p58 lab=vss}
C {capa.sym} 1720 -560 0 0 {name=c6 m=1 value=34f}
C {lab_pin.sym} 1540 -410 0 1 {name=p59 lab=LDCP_ROWDEC}
C {lab_pin.sym} 1540 -330 0 1 {name=p60 lab=vss}
C {capa.sym} 1540 -380 0 0 {name=c7 m=1 value=280f}
C {lab_pin.sym} 1540 -590 0 1 {name=p64 lab=LDCP_SA}
C {lab_pin.sym} 1540 -510 0 1 {name=p65 lab=vss}
C {capa.sym} 1540 -560 0 0 {name=c8 m=1 value=44f}
C {lab_pin.sym} 1720 -240 0 1 {name=p15 lab=LDCP_COL_B}
C {lab_pin.sym} 1720 -160 0 1 {name=p24 lab=vss}
C {capa.sym} 1720 -210 0 0 {name=c9 m=1 value=63f}
C {lab_pin.sym} 1540 -240 0 1 {name=p43 lab=LDCP_ADDLAT_B}
C {lab_pin.sym} 1540 -160 0 1 {name=p66 lab=vss}
C {capa.sym} 1540 -210 0 0 {name=c10 m=1 value=12f}
C {spice_probe.sym} 710 -450 2 1 {name=p125 analysis=tran}
C {spice_probe.sym} 150 -640 0 0 {name=p69 analysis=tran}
C {spice_probe.sym} 10 -640 0 0 {name=p70 analysis=tran}
C {spice_probe.sym} 10 -480 0 0 {name=p73 analysis=tran}
C {spice_probe.sym} 150 -480 0 0 {name=p74 analysis=tran}
C {spice_probe.sym} 10 -330 0 0 {name=p75 analysis=tran}
C {spice_probe.sym} 1540 -590 0 0 {name=p76 analysis=tran}
C {spice_probe.sym} 1540 -410 0 0 {name=p83 analysis=tran}
C {spice_probe.sym} 1540 -240 0 0 {name=p84 analysis=tran}
C {spice_probe.sym} 1720 -240 0 0 {name=p85 analysis=tran}
C {spice_probe.sym} 1720 -410 0 0 {name=p86 analysis=tran}
C {spice_probe.sym} 1720 -590 0 0 {name=p87 analysis=tran}
C {spice_probe.sym} 1340 -300 2 1 {name=p88 analysis=tran}
C {spice_probe.sym} 1340 -530 0 0 {name=p89 analysis=tran}
C {spice_probe.sym} 1340 -620 0 0 {name=p90 analysis=tran}
C {spice_probe.sym} 1340 -910 0 0 {name=p93 analysis=tran}
C {lab_pin.sym} 800 -350 0 0 {name=p94 lab=LDOE}
C {spice_probe.sym} 800 -350 0 0 {name=p95 analysis=tran}
C {lab_pin.sym} 800 -300 0 0 {name=p96 lab=LDCP}
C {spice_probe.sym} 800 -300 0 0 {name=p97 analysis=tran}
C {lab_pin.sym} 890 -300 0 0 {name=p98 lab=LDEN}
C {spice_probe.sym} 890 -300 0 0 {name=p99 analysis=tran}
C {lab_pin.sym} 1450 -40 0 0 {name=p100 lab=LDA[12:0]}
C {spice_probe.sym} 1450 -40 0 0 {name=p101 analysis=tran}
C {opin.sym} 290 -120 0 0 {name=p7 lab=LDQ[15:0]}
C {ipin.sym} 210 -190 0 0 {name=p77 lab=LDA[12:0]}
C {ipin.sym} 210 -170 0 0 {name=p78 lab=LDCP}
C {ipin.sym} 210 -210 0 0 {name=p79 lab=LDEN}
C {ipin.sym} 210 -150 0 0 {name=p80 lab=LDOE}
C {ipin.sym} 210 -110 0 0 {name=p81 lab=vss}
C {ipin.sym} 210 -90 0 0 {name=p82 lab=vcc}
C {rom3_array.sym} 1190 -700 0 0 {name=xarr}
C {lab_pin.sym} 1040 -710 0 0 {name=l13 lab=LDWL[511:0]}
C {lab_pin.sym} 1040 -690 0 0 {name=l46 lab=vss}
C {lab_pin.sym} 1340 -710 0 1 {name=l1 lab=LDBL[255:0]}
C {spice_probe.sym} 1340 -830 0 0 {name=p3 analysis=tran}
C {spice_probe.sym} 190 -850 0 0 {name=p13 analysis=tran}
C {lab_pin.sym} 1670 -740 0 0 {name=l2 lab=LDWL[8:0]}
C {spice_probe.sym} 1670 -740 0 0 {name=p46 analysis=tran}
C {lab_pin.sym} 1670 -790 0 0 {name=l4 lab=LDBL[0,16,32,1,17,33,2,18,34]}
C {spice_probe.sym} 1670 -790 0 0 {name=p91 analysis=tran}
