v {xschem version=3.4.5 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2023 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
G {}
K {}
V {}
S {}
E {}
L 3 250 -310 250 -120 {}
L 3 410 -310 410 -170 {}
L 3 570 -310 570 -120 {}
L 4 150 -100 250 -100 {}
L 4 250 -120 250 -100 {}
L 4 250 -120 570 -120 {}
L 4 570 -120 570 -100 {}
L 4 570 -100 690 -100 {}
L 4 150 -170 410 -170 {}
L 4 410 -170 410 -150 {}
L 4 410 -150 570 -150 {}
L 4 570 -170 570 -150 {}
L 4 570 -170 690 -170 {}
L 7 1090 -260 2520 -260 {}
B 2 350 -1020 810 -860 {flags=graph
y1 = 0
y2 = 0.93
divy = 5
x1=9.31247e-08
x2=4.30871e-07
divx=5
subdivx=4
unitx=n
node="cal%0
saout
saout%70
saout%3"
color="8 14 7 12"

linewidth_mult=1.0
hilight_wave=-1
dataset=-1}
B 2 350 -1160 810 -1030 {flags=graph
y1 = 0.647319
y2 = 0.652563
divy = 5
x1=9.31247e-08
x2=4.30871e-07
unitx=n
divx=5
subdivx=4
node="plus%0
minus%0"
color="4 5"
dataset=-1
linewidth_mult=1.0}
B 2 350 -1320 810 -1160 {flags=graph
y1 = 0
y2 = 0.9
divy = 5
x1=9.31247e-08
x2=4.30871e-07
unitx=n
divx=5
subdivx=4
node="en
cal
\\"SAOUT#3; vss,saout%3\\"
--slow--
\\"SAOUT#15; vss,saout%15\\"
--failure--
\\"SAOUT#70; vss,saout%70\\""
color="4 4 4 8 8 7 7"
dataset=-1
digital=1
ypos1=0.0825406
ypos2=0.767045
ylabmag=1.2
linewidth_mult=1.0
subdivy=0}
B 2 350 -1450 810 -1330 {flags=graph
y1 = -8.54414e-05
y2 = 2.7198e-05
divy = 5
x1=9.31247e-08
x2=4.30871e-07
divx=5
subdivx=4
unitx=n


dataset=-1
color="4 7"
node="\\"power dset 97 ; i(vvcc) vcc * % 23\\"
\\"power dset 70 ; i(vvcc) vcc * % 70\\""
subdivy=4
linewidth_mult=1.0}
T {CAL} 140 -180 0 1 0.4 0.4 {}
T {EN} 140 -130 0 1 0.4 0.4 {}
T {CALIBRATION
  30ns} 400 -300 0 1 0.4 0.4 {}
T {SENSING
  30ns} 530 -300 0 1 0.4 0.4 {}
T {OFF} 660 -300 0 1 0.4 0.4 {}
T {OFF} 210 -300 0 1 0.4 0.4 {}
T {NGSPICE MISMATCH SIMULATION} 1210 -310 0 0 0.8 0.8 {}
T {Offset-compensated comparator. Detects +/- 2mv differential signal on PLUS, MINUS.
Output on SAOUT
Gaussian Threshold variation (via delvto parameter) is added to all MOS transistors.} 1110 -240 0 0 0.6 0.6 {}
T {.param ABSVAR=0.05
delvto='agauss(0,ABSVAR,3)'} 1390 -120 0 0 0.6 0.6 {layer=8}
N 120 -470 120 -450 {lab=TEMPERAT}
N 360 -530 390 -530 {lab=VSS}
N 360 -500 360 -480 {lab=VSS}
N 360 -580 360 -560 {lab=VSSI}
N 1200 -1070 1230 -1070 {lab=VSS}
N 1200 -1230 1230 -1230 {lab=VCC}
N 1170 -1300 1200 -1300 {lab=VCC}
N 1200 -1300 1200 -1260 {lab=VCC}
N 1170 -940 1200 -940 {lab=VSSI}
N 1200 -980 1200 -940 {lab=VSSI}
N 1060 -1070 1160 -1070 {lab=ZERO0}
N 1060 -1160 1060 -1070 {lab=ZERO0}
N 980 -1160 1060 -1160 {lab=ZERO0}
N 870 -1160 900 -1160 {lab=#net1}
N 870 -1340 870 -1160 {lab=#net1}
N 870 -1340 1300 -1340 {lab=#net1}
N 1200 -1160 1300 -1160 {lab=#net1}
N 1690 -1070 1720 -1070 {lab=VSS}
N 1690 -1230 1720 -1230 {lab=VCC}
N 1660 -1300 1690 -1300 {lab=VCC}
N 1690 -1300 1690 -1260 {lab=VCC}
N 1660 -940 1690 -940 {lab=VSSI}
N 1690 -980 1690 -940 {lab=VSSI}
N 1550 -1070 1650 -1070 {lab=ZERO1}
N 1550 -1160 1550 -1070 {lab=ZERO1}
N 1470 -1160 1550 -1160 {lab=ZERO1}
N 1360 -1160 1390 -1160 {lab=#net2}
N 1360 -1340 1360 -1160 {lab=#net2}
N 1360 -1340 1790 -1340 {lab=#net2}
N 1690 -1160 1790 -1160 {lab=#net2}
N 1300 -1340 1300 -1160 {lab=#net1}
N 1200 -1200 1200 -1160 {lab=#net1}
N 1060 -1230 1060 -1160 {lab=ZERO0}
N 1690 -1200 1690 -1160 {lab=#net2}
N 1550 -1230 1550 -1160 {lab=ZERO1}
N 1790 -1340 1790 -1160 {lab=#net2}
N 970 -640 1000 -640 {lab=VSS}
N 1140 -640 1170 -640 {lab=VSS}
N 1170 -800 1200 -800 {lab=VCC}
N 940 -800 970 -800 {lab=VCC}
N 1010 -800 1130 -800 {lab=GN}
N 1010 -800 1010 -770 {lab=GN}
N 970 -770 1010 -770 {lab=GN}
N 970 -770 970 -670 {lab=GN}
N 1170 -710 1170 -670 {lab=OUTDIFF}
N 970 -860 970 -830 {lab=VCC}
N 1070 -860 1170 -860 {lab=VCC}
N 1170 -860 1170 -830 {lab=VCC}
N 1170 -610 1170 -590 {lab=SN}
N 1070 -590 1170 -590 {lab=SN}
N 970 -610 970 -590 {lab=SN}
N 1070 -510 1100 -510 {lab=VSS}
N 1070 -590 1070 -540 {lab=SN}
N 1070 -880 1070 -860 {lab=VCC}
N 1040 -380 1070 -380 {lab=VSSI}
N 1070 -420 1070 -380 {lab=VSSI}
N 1550 -510 1580 -510 {lab=VSS}
N 1550 -800 1580 -800 {lab=VCC}
N 1520 -870 1550 -870 {lab=VCC}
N 1550 -870 1550 -830 {lab=VCC}
N 1520 -380 1550 -380 {lab=VSSI}
N 1550 -420 1550 -380 {lab=VSSI}
N 1550 -710 1550 -540 {lab=SAOUTF}
N 1300 -710 1440 -710 {lab=OUTDIFF}
N 970 -860 1070 -860 {lab=VCC}
N 970 -590 1070 -590 {lab=SN}
N 1200 -1160 1200 -1100 {lab=#net1}
N 1690 -1160 1690 -1100 {lab=#net2}
N 1440 -710 1440 -510 {lab=OUTDIFF}
N 1300 -840 1300 -710 {lab=OUTDIFF}
N 1790 -840 1790 -710 {lab=SAOUTF}
N 1790 -710 1930 -710 {lab=SAOUTF}
N 1170 -710 1300 -710 {lab=OUTDIFF}
N 1170 -770 1170 -710 {lab=OUTDIFF}
N 1440 -800 1440 -710 {lab=OUTDIFF}
N 1550 -1230 1650 -1230 {lab=ZERO1}
N 1060 -1230 1160 -1230 {lab=ZERO0}
N 1440 -800 1510 -800 {lab=OUTDIFF}
N 1440 -510 1510 -510 {lab=OUTDIFF}
N 2040 -510 2070 -510 {lab=VSS}
N 2040 -800 2070 -800 {lab=VCC}
N 2010 -870 2040 -870 {lab=VCC}
N 2040 -870 2040 -830 {lab=VCC}
N 2010 -380 2040 -380 {lab=VSSI}
N 2040 -420 2040 -380 {lab=VSSI}
N 2040 -710 2040 -540 {lab=SAOUT}
N 1930 -710 1930 -510 {lab=SAOUTF}
N 2180 -1070 2210 -1070 {lab=VSS}
N 2180 -1230 2210 -1230 {lab=VCC}
N 2150 -1300 2180 -1300 {lab=VCC}
N 2180 -1300 2180 -1260 {lab=VCC}
N 2150 -940 2180 -940 {lab=VSSI}
N 2180 -980 2180 -940 {lab=VSSI}
N 2040 -1070 2140 -1070 {lab=ZERO2}
N 2040 -1160 2040 -1070 {lab=ZERO2}
N 1960 -1160 2040 -1160 {lab=ZERO2}
N 1850 -1160 1880 -1160 {lab=#net3}
N 1850 -1340 1850 -1160 {lab=#net3}
N 1850 -1340 2370 -1340 {lab=#net3}
N 2180 -1160 2370 -1160 {lab=#net3}
N 2180 -1200 2180 -1160 {lab=#net3}
N 2040 -1230 2040 -1160 {lab=ZERO2}
N 2370 -1340 2370 -1160 {lab=#net3}
N 2180 -1160 2180 -1100 {lab=#net3}
N 2370 -840 2370 -710 {lab=SAOUT}
N 2040 -1230 2140 -1230 {lab=ZERO2}
N 1930 -800 2000 -800 {lab=SAOUTF}
N 1930 -510 2000 -510 {lab=SAOUTF}
N 1550 -710 1790 -710 {lab=SAOUTF}
N 2040 -710 2370 -710 {lab=SAOUT}
N 1930 -800 1930 -710 {lab=SAOUTF}
N 2040 -770 2040 -710 {lab=SAOUT}
N 1550 -770 1550 -710 {lab=SAOUTF}
N 2260 -810 2290 -810 {lab=VCC}
N 2230 -880 2260 -880 {lab=VCC}
N 2260 -880 2260 -840 {lab=VCC}
N 2260 -780 2260 -710 {lab=SAOUT}
N 720 -590 750 -590 {lab=VCC}
N 550 -650 550 -620 {lab=SP}
N 650 -650 750 -650 {lab=SP}
N 750 -650 750 -620 {lab=SP}
N 550 -650 650 -650 {lab=SP}
N 550 -590 580 -590 {lab=VCC}
N 520 -460 550 -460 {lab=VSS}
N 750 -460 780 -460 {lab=VSS}
N 590 -460 710 -460 {lab=GP}
N 590 -510 590 -460 {lab=GP}
N 550 -510 590 -510 {lab=GP}
N 550 -560 550 -510 {lab=GP}
N 750 -560 750 -490 {lab=OUTDIFF}
N 750 -430 750 -410 {lab=VSSI}
N 550 -410 750 -410 {lab=VSSI}
N 550 -430 550 -410 {lab=VSSI}
N 620 -370 650 -370 {lab=VSSI}
N 650 -410 650 -370 {lab=VSSI}
N 660 -780 690 -780 {lab=VCC}
N 660 -680 660 -650 {lab=SP}
N 660 -830 660 -810 {lab=VCC}
N 750 -520 810 -520 {lab=OUTDIFF}
N 660 -750 660 -740 {lab=#net4}
N 550 -510 550 -490 {
lab=GP}
N 490 -590 510 -590 {
lab=MINUS}
N 790 -590 810 -590 {
lab=PLUS}
N 910 -640 930 -640 {
lab=MINUS}
N 1210 -640 1230 -640 {
lab=PLUS}
N 1300 -1160 1300 -900 {lab=#net1}
N 1790 -1160 1790 -900 {lab=#net2}
N 2370 -1160 2370 -900 {lab=#net3}
N 570 -780 620 -780 {
lab=GP}
N 990 -510 1030 -510 {
lab=GN}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 110 -850 0 0 { name=p92 lab=CAL }
C {ipin.sym} 110 -910 0 0 { name=p93 lab=PLUS }
C {ipin.sym} 110 -950 0 0 { name=p94 lab=MINUS }
C {ipin.sym} 110 -990 0 0 { name=p95 lab=EN }
C {ipin.sym} 110 -780 0 0 { name=p96 lab=VSS }
C {ipin.sym} 110 -810 0 0 { name=p97 lab=VCC }
C {vsource_arith.sym} 120 -420 0 0 {name=E5 VOL=temper MAX=200 MIN=-200}
C {lab_pin.sym} 120 -470 0 1 {name=p113 lab=TEMPERAT}
C {lab_pin.sym} 120 -390 0 0 {name=p114 lab=VSS}
C {opin.sym} 230 -900 0 0 { name=p116 lab=SAOUT }
C {lab_pin.sym} 200 -570 0 1 {name=p126 lab=CALB}
C {lab_pin.sym} 120 -570 0 0 {name=l50 lab=CAL}
C {code.sym} 840 -190 0 0 {name=STIMULI 
only_toplevel=true
xxplace=end
value="* .option SCALE=1e-6 
.option method=gear
.param VCC=0.9
.param VDL='VCC/2+0.2'
.param ABSVAR=0.05
.temp 25
.param RNDVAL = 'agauss(0, 10, 1)'
** to generate following file: 
** copy .../share/doc/xschem/ngspice/stimuli.autozero_comp to simulation directory
** then do 'Simulation->Utile Stimuli Editor (GUI)' and press 'Translate'
.include \\"stimuli_autozero_comp.cir\\"

** download models from here: 
** http://ptm.asu.edu/modelcard/2006/130nm_bulk.pm
** and save to 'models_autozero_comp.txt' in simulation directory
.include \\"models_autozero_comp.txt\\"


* .tran 0.1n 900n uic

.control
  setseed 17
  reset
  echo Seed=$rndseed
  let run=1
  dowhile run <= 100
    if run > 1
      set appendwrite
      reset
    end
    * save saout cal i(vvcc) en plus minus saoutf outdiff
    save all
    tran 0.3n 900n uic
    write autozero_comp.raw
    let run = run + 1
  end
.endc
"}
C {inv-2.sym} 160 -570 0 0 {name=x14 m=1 
+ wn=0.6u ln=0.2u
+ wp=1u lp=0.2u
+ VCCPIN=VCC VCCBPIN=VCC VSSPIN=VSS VSSBPIN=VSS}
C {inv-2.sym} 160 -700 0 0 {name=x5 m=1 
+ wn=0.6u ln=0.2u
+ wp=1u lp=0.2u
+ VCCPIN=VCC VCCBPIN=VCC VSSPIN=VSS VSSBPIN=VSS}
C {lab_pin.sym} 120 -700 0 0 {name=p15 lab=CALB}
C {lab_pin.sym} 200 -700 0 1 {name=l4 lab=CALBB}
C {lab_pin.sym} 390 -530 0 1 {name=p283 lab=VSS}
C {lab_pin.sym} 320 -530 0 0 {name=l56 lab=EN}
C {lab_pin.sym} 360 -480 0 0 {name=p284 lab=VSS}
C {lab_pin.sym} 360 -580 0 0 {name=p199 lab=VSSI}
C {nmos4-v.sym} 340 -530 0 0 {name=M67 verilog_gate=nmos del=50,50,50 model=nmos w=5u l=0.13u extra="delvto='agauss(0,ABSVAR,3)'"}
C {parax_cap.sym} 360 -470 0 0 {name=c38  value=2p}
C {passgate.sym} 940 -1160 0 1 {name=x1 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 940 -1190 0 1 {name=l19 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 940 -1130 0 1 {name=l44 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 1180 -1070 0 0 {name=M3 verilog_gate=nmos del=50,50,50 model=nmos w=1.0u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1230 -1070 0 1 {name=p179 lab=VSS}
C {pmos4-v.sym} 1180 -1230 0 0 {name=M4 verilog_gate=pmos del=50,50,50 model=pmos w=2.0u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1230 -1230 0 1 {name=p180 lab=VCC}
C {lab_pin.sym} 1170 -1300 0 0 {name=p181 lab=VCC}
C {lab_pin.sym} 1170 -940 0 0 {name=p182 lab=VSSI}
C {passgate.sym} 1430 -1160 0 1 {name=x2 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1430 -1190 0 1 {name=l45 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 1430 -1130 0 1 {name=l46 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 1670 -1070 0 0 {name=M7 verilog_gate=nmos del=50,50,50 model=nmos w=1.3u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1720 -1070 0 1 {name=p183 lab=VSS}
C {pmos4-v.sym} 1670 -1230 0 0 {name=M9 verilog_gate=pmos del=50,50,50 model=pmos w=2.6u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1720 -1230 0 1 {name=p184 lab=VCC}
C {lab_pin.sym} 1660 -1300 0 0 {name=p185 lab=VCC}
C {lab_pin.sym} 1660 -940 0 0 {name=p186 lab=VSSI}
C {lab_pin.sym} 1550 -1220 0 0 {name=l47 lab=ZERO1}
C {lab_pin.sym} 1060 -1220 0 0 {name=l48 lab=ZERO0}
C {nmos4-v.sym} 950 -640 0 0 {name=M20 verilog_gate=nmos del=50,50,50 model=nmos w=2u l=0.4u extra="delvto='agauss(0,ABSVAR,3)'"}
C {nmos4-v.sym} 1190 -640 0 1 {name=M8 verilog_gate=nmos del=50,50,50 model=nmos w=2u l=0.4u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1000 -640 0 1 {name=p187 lab=VSS}
C {lab_pin.sym} 1140 -640 0 0 {name=p188 lab=VSS}
C {pmos4-v.sym} 1150 -800 0 0 {name=M30 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1200 -800 0 1 {name=p189 lab=VCC}
C {pmos4-v.sym} 990 -800 0 1 {name=M12 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 940 -800 0 0 {name=p190 lab=VCC}
C {nmos4-v.sym} 1050 -510 0 0 {name=M32 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1100 -510 0 1 {name=p191 lab=VSS}
C {lab_pin.sym} 1070 -880 0 0 {name=p192 lab=VCC}
C {lab_pin.sym} 1040 -380 0 0 {name=p193 lab=VSSI}
C {nmos4-v.sym} 1530 -510 0 0 {name=M17 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1580 -510 0 1 {name=p194 lab=VSS}
C {pmos4-v.sym} 1530 -800 0 0 {name=M13 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1580 -800 0 1 {name=p195 lab=VCC}
C {lab_pin.sym} 1520 -870 0 0 {name=p196 lab=VCC}
C {lab_pin.sym} 1520 -380 0 0 {name=p197 lab=VSSI}
C {lab_pin.sym} 1790 -750 0 0 {name=l49 lab=SAOUTF}
C {parax_cap.sym} 1620 -700 0 0 {name=c3  value=4f}
C {lab_pin.sym} 1300 -750 0 0 {name=l51 lab=OUTDIFF}
C {lab_pin.sym} 1070 -570 0 0 {name=l52 lab=SN}
C {lab_pin.sym} 1010 -770 0 1 {name=l53 lab=GN}
C {parax_cap.sym} 1060 -790 0 0 {name=c5  value=4f}
C {parax_cap.sym} 990 -580 0 0 {name=c30  value=2f}
C {parax_cap.sym} 1260 -700 0 0 {name=c31  value=4f}
C {lab_pin.sym} 990 -510 0 0 {name=p198 lab=GN}
C {lab_pin.sym} 1230 -640 0 1 {name=l54 lab=PLUS}
C {lab_pin.sym} 910 -640 0 0 {name=l55 lab=MINUS}
C {ammeter.sym} 1070 -450 0 0 {name=vdiffn}
C {ammeter.sym} 1550 -450 0 0 {name=v3}
C {ammeter.sym} 1690 -1010 0 0 {name=v4}
C {ammeter.sym} 1200 -1010 0 0 {name=v6}
C {nmos4-v.sym} 2020 -510 0 0 {name=M19 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2070 -510 0 1 {name=p9 lab=VSS}
C {pmos4-v.sym} 2020 -800 0 0 {name=M21 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2070 -800 0 1 {name=p10 lab=VCC}
C {lab_pin.sym} 2010 -870 0 0 {name=p11 lab=VCC}
C {lab_pin.sym} 2010 -380 0 0 {name=p12 lab=VSSI}
C {parax_cap.sym} 2110 -700 0 0 {name=c1  value=4f}
C {ammeter.sym} 2040 -450 0 0 {name=v1}
C {lab_pin.sym} 2370 -710 0 1 {name=l3 lab=SAOUT}
C {passgate.sym} 1920 -1160 0 1 {name=x3 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1920 -1190 0 1 {name=l5 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 1920 -1130 0 1 {name=l6 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 2160 -1070 0 0 {name=M23 verilog_gate=nmos del=50,50,50 model=nmos w=1.5u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2210 -1070 0 1 {name=p13 lab=VSS}
C {pmos4-v.sym} 2160 -1230 0 0 {name=M24 verilog_gate=pmos del=50,50,50 model=pmos w=3u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2210 -1230 0 1 {name=p14 lab=VCC}
C {lab_pin.sym} 2150 -1300 0 0 {name=p16 lab=VCC}
C {lab_pin.sym} 2150 -940 0 0 {name=p17 lab=VSSI}
C {lab_pin.sym} 2040 -1220 0 0 {name=l8 lab=ZERO2}
C {ammeter.sym} 2180 -1010 0 0 {name=v5}
C {pmos4-v.sym} 2240 -810 0 0 {name=M6 verilog_gate=pmos del=50,50,50 model=pmos w=0.6u l=0.2u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2290 -810 0 1 {name=p18 lab=VCC}
C {lab_pin.sym} 2230 -880 0 0 {name=p19 lab=VCC}
C {lab_pin.sym} 2220 -810 0 0 {name=l2 lab=EN}
C {pmos4-v.sym} 770 -590 0 1 {name=M18 verilog_gate=pmos del=50,50,50 model=pmos w=4u l=0.4u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 720 -590 0 0 {name=p20 lab=VCC}
C {pmos4-v.sym} 530 -590 0 0 {name=M25 verilog_gate=pmos del=50,50,50 model=pmos w=4u l=0.4u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 580 -590 0 1 {name=p21 lab=VCC}
C {lab_pin.sym} 520 -460 0 0 {name=p22 lab=VSS}
C {lab_pin.sym} 780 -460 0 1 {name=p23 lab=VSS}
C {lab_pin.sym} 620 -370 0 0 {name=p24 lab=VSSI}
C {pmos4-v.sym} 640 -780 0 0 {name=M28 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 690 -780 0 1 {name=p25 lab=VCC}
C {lab_pin.sym} 660 -830 0 0 {name=p26 lab=VCC}
C {lab_pin.sym} 570 -780 0 0 {name=l7 lab=GP}
C {lab_pin.sym} 550 -530 0 1 {name=l9 lab=GP}
C {lab_pin.sym} 490 -590 0 0 {name=l10 lab=MINUS}
C {lab_pin.sym} 810 -590 0 1 {name=l11 lab=PLUS}
C {lab_pin.sym} 810 -520 0 1 {name=l12 lab=OUTDIFF}
C {nmos4-v.sym} 730 -460 0 0 {name=M26 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {nmos4-v.sym} 570 -460 0 1 {name=M1 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {parax_cap.sym} 630 -450 0 0 {name=c2  value=4f}
C {lab_pin.sym} 660 -670 0 0 {name=l13 lab=SP}
C {launcher.sym} 930 -260 0 0 {name=h2 
descr="Simulate" 
tclcommand="xschem netlist; xschem simulate"}
C {parax_cap.sym} 1060 -1060 0 0 {name=c4  value=5f}
C {parax_cap.sym} 1550 -1060 0 0 {name=c6  value=5f}
C {parax_cap.sym} 2040 -1060 0 0 {name=c7  value=5f}
C {launcher.sym} 65 -1075 0 0 {name=h1 
descr="Select arrow and 
Ctrl-Left-Click to
load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
"
}
C {ammeter.sym} 660 -710 0 0 {name=vdiffp}
C {ammeter.sym} 1300 -870 0 0 {name=vcalib0}
C {ammeter.sym} 1790 -870 0 0 {name=vcalib1}
C {ammeter.sym} 2370 -870 0 0 {name=vcalib2}
C {test_generators.sym} 480 -500 0 0 {name=x28}
