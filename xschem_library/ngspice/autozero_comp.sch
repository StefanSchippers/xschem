v {xschem version=3.4.6 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
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
L 3 250 -270 250 -120 {}
L 3 410 -270 410 -170 {}
L 3 570 -270 570 -120 {}
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
B 2 270 -1020 680 -860 {flags=graph,unlocked
y1 = 0
y2 = 0.9
divy = 5
x1=1.9099218e-07
x2=3.4920522e-07
divx=5
subdivx=4
unitx=n
node="cal%0
saout"
color="8 6"

linewidth_mult=1.0
hilight_wave=-1
dataset=-1
rawfile=$netlist_dir/autozero_comp.raw
sim_type=tran
rainbow=0
hcursor1_y=0.13350312
hcursor2_y=0.58853305}
B 2 270 -1160 680 -1030 {flags=graph,unlocked
y1 = 0.647319
y2 = 0.652563

x1=1.9099218e-07
x2=3.4920522e-07
unitx=n
divx=5
subdivx=2
node="plus%0
minus%0"
color="4 5"
dataset=-1
linewidth_mult=1.0
divy=3
subdivy=1
rawfile=$netlist_dir/autozero_comp.raw
sim_type=tran}
B 2 270 -1320 680 -1160 {flags=graph,unlocked
y1 = 0.647319
y2 = 0.652563

x1=1.9099218e-07
x2=3.4920522e-07
unitx=n
divx=5
subdivx=2
node="plus%0
minus%0"
color="4 5"
dataset=-1
linewidth_mult=1.0
divy=3
subdivy=1
rawfile=$netlist_dir/autozero_comp.raw
sim_type=tran}
B 2 270 -1450 680 -1330 {flags=graph,unlocked
y1 = 0.647319
y2 = 0.652563

x1=1.9099218e-07
x2=3.4920522e-07
unitx=n
divx=5
subdivx=2
node="plus%0
minus%0"
color="4 5"
dataset=-1
linewidth_mult=1.0
divy=3
subdivy=1
rawfile=$netlist_dir/autozero_comp.raw
sim_type=tran}
B 2 690 -1020 870 -860 {flags=graph,unlocked
y1 = 0
y2 = 0.9
divy = 5
x1=0
x2=148
divx=5
subdivx=4
unitx=1



linewidth_mult=1.5
hilight_wave=-1
dataset=-1
rawfile=distrib
sim_type=distrib
color="4 7 8"
node="saout
saout
 0.45"
sweep="freq0 freq1 freq0"
mode=HistoH
xlabmag=1.3}
B 2 270 -1610 680 -1450 {flags=graph,unlocked
y1 = 0
y2 = 0.9
divy = 5
x1=1.9099218e-07
x2=3.4920522e-07
divx=5
subdivx=4
unitx=n
node="zero0
zero1
zero2"
color="12 4 7"

linewidth_mult=0.1
hilight_wave=-1

rawfile=$netlist_dir/autozero_comp.raw
sim_type=tran
rainbow=0
dataset=235}
T {CAL} 140 -180 0 1 0.4 0.4 {}
T {EN} 140 -130 0 1 0.4 0.4 {}
T {CALIBRATION
  30ns} 400 -260 0 1 0.4 0.4 {}
T {SENSING
  30ns} 530 -260 0 1 0.4 0.4 {}
T {OFF} 660 -260 0 1 0.4 0.4 {}
T {OFF} 210 -260 0 1 0.4 0.4 {}
T {NGSPICE MISMATCH SIMULATION} 1210 -310 0 0 0.8 0.8 {}
T {Offset-compensated comparator. Detects +/- 2mv differential signal on PLUS, MINUS.
Output on SAOUT
Gaussian Threshold variation (via delvto parameter) is added to all MOS transistors.} 1110 -240 0 0 0.6 0.6 {}
T {.param ABSVAR=0.05
delvto='agauss(0,ABSVAR,3)'} 1390 -120 0 0 0.6 0.6 {layer=8}
T {tcleval(Dataset=[xschem getprop rect 2 0 dataset])} 340 -850 0 0 0.7 0.7 {floater=xxx}
N 120 -470 120 -450 {lab=TEMPERAT}
N 360 -530 390 -530 {lab=VSS}
N 360 -500 360 -480 {lab=VSS}
N 360 -580 360 -560 {lab=VSSI}
N 1200 -1160 1230 -1160 {lab=VSS}
N 1200 -1320 1230 -1320 {lab=VCC}
N 1170 -1390 1200 -1390 {lab=VCC}
N 1200 -1390 1200 -1350 {lab=VCC}
N 1170 -1030 1200 -1030 {lab=VSSI}
N 1200 -1070 1200 -1030 {lab=VSSI}
N 1060 -1160 1160 -1160 {lab=ZERO0}
N 1060 -1250 1060 -1160 {lab=ZERO0}
N 980 -1250 1060 -1250 {lab=ZERO0}
N 870 -1250 900 -1250 {lab=#net1}
N 870 -1430 870 -1250 {lab=#net1}
N 870 -1430 1300 -1430 {lab=#net1}
N 1200 -1250 1300 -1250 {lab=#net1}
N 1690 -1160 1720 -1160 {lab=VSS}
N 1690 -1320 1720 -1320 {lab=VCC}
N 1660 -1390 1690 -1390 {lab=VCC}
N 1690 -1390 1690 -1350 {lab=VCC}
N 1660 -1030 1690 -1030 {lab=VSSI}
N 1690 -1070 1690 -1030 {lab=VSSI}
N 1550 -1160 1650 -1160 {lab=ZERO1}
N 1550 -1250 1550 -1160 {lab=ZERO1}
N 1470 -1250 1550 -1250 {lab=ZERO1}
N 1360 -1250 1390 -1250 {lab=#net2}
N 1360 -1430 1360 -1250 {lab=#net2}
N 1360 -1430 1790 -1430 {lab=#net2}
N 1690 -1250 1790 -1250 {lab=#net2}
N 1300 -1430 1300 -1250 {lab=#net1}
N 1200 -1290 1200 -1250 {lab=#net1}
N 1060 -1320 1060 -1250 {lab=ZERO0}
N 1690 -1290 1690 -1250 {lab=#net2}
N 1550 -1320 1550 -1250 {lab=ZERO1}
N 1790 -1430 1790 -1250 {lab=#net2}
N 970 -640 1000 -640 {lab=VSS}
N 1140 -640 1170 -640 {lab=VSS}
N 1170 -870 1200 -870 {lab=VCC}
N 940 -870 970 -870 {lab=VCC}
N 1030 -870 1130 -870 {lab=GN}
N 1030 -870 1030 -820 {lab=GN}
N 970 -820 1030 -820 {lab=GN}
N 970 -740 970 -670 {lab=#net3}
N 1170 -710 1170 -670 {lab=OUTDIFF}
N 970 -930 970 -900 {lab=VCC}
N 1070 -930 1170 -930 {lab=VCC}
N 1170 -930 1170 -900 {lab=VCC}
N 1170 -610 1170 -590 {lab=SN}
N 1070 -590 1170 -590 {lab=SN}
N 970 -610 970 -590 {lab=SN}
N 1070 -510 1100 -510 {lab=VSS}
N 1070 -590 1070 -540 {lab=SN}
N 1070 -950 1070 -930 {lab=VCC}
N 1040 -380 1070 -380 {lab=VSSI}
N 1070 -420 1070 -380 {lab=VSSI}
N 1550 -510 1580 -510 {lab=VSS}
N 1550 -870 1580 -870 {lab=VCC}
N 1520 -940 1550 -940 {lab=VCC}
N 1550 -940 1550 -900 {lab=VCC}
N 1520 -380 1550 -380 {lab=VSSI}
N 1550 -420 1550 -380 {lab=VSSI}
N 1550 -710 1550 -540 {lab=SAOUTF}
N 1300 -710 1440 -710 {lab=OUTDIFF}
N 970 -930 1070 -930 {lab=VCC}
N 970 -590 1070 -590 {lab=SN}
N 1200 -1250 1200 -1190 {lab=#net1}
N 1690 -1250 1690 -1190 {lab=#net2}
N 1440 -710 1440 -510 {lab=OUTDIFF}
N 1300 -840 1300 -710 {lab=OUTDIFF}
N 1790 -840 1790 -710 {lab=SAOUTF}
N 1790 -710 1930 -710 {lab=SAOUTF}
N 1170 -710 1300 -710 {lab=OUTDIFF}
N 1170 -740 1170 -710 {lab=OUTDIFF}
N 1440 -870 1440 -710 {lab=OUTDIFF}
N 1550 -1320 1650 -1320 {lab=ZERO1}
N 1060 -1320 1160 -1320 {lab=ZERO0}
N 1440 -870 1510 -870 {lab=OUTDIFF}
N 1440 -510 1510 -510 {lab=OUTDIFF}
N 2040 -510 2070 -510 {lab=VSS}
N 2040 -870 2070 -870 {lab=VCC}
N 2010 -940 2040 -940 {lab=VCC}
N 2040 -940 2040 -900 {lab=VCC}
N 2010 -380 2040 -380 {lab=VSSI}
N 2040 -420 2040 -380 {lab=VSSI}
N 2040 -710 2040 -540 {lab=SAOUT}
N 1930 -710 1930 -510 {lab=SAOUTF}
N 2180 -1160 2210 -1160 {lab=VSS}
N 2180 -1320 2210 -1320 {lab=VCC}
N 2150 -1390 2180 -1390 {lab=VCC}
N 2180 -1390 2180 -1350 {lab=VCC}
N 2150 -1030 2180 -1030 {lab=VSSI}
N 2180 -1070 2180 -1030 {lab=VSSI}
N 2040 -1160 2140 -1160 {lab=ZERO2}
N 2040 -1250 2040 -1160 {lab=ZERO2}
N 1960 -1250 2040 -1250 {lab=ZERO2}
N 1850 -1250 1880 -1250 {lab=#net4}
N 1850 -1430 1850 -1250 {lab=#net4}
N 1850 -1430 2370 -1430 {lab=#net4}
N 2180 -1250 2370 -1250 {lab=#net4}
N 2180 -1290 2180 -1250 {lab=#net4}
N 2040 -1320 2040 -1250 {lab=ZERO2}
N 2370 -1430 2370 -1250 {lab=#net4}
N 2180 -1250 2180 -1190 {lab=#net4}
N 2370 -840 2370 -710 {lab=SAOUT}
N 2040 -1320 2140 -1320 {lab=ZERO2}
N 1930 -870 2000 -870 {lab=SAOUTF}
N 1930 -510 2000 -510 {lab=SAOUTF}
N 1550 -710 1790 -710 {lab=SAOUTF}
N 2260 -710 2370 -710 {lab=SAOUT}
N 1930 -870 1930 -710 {lab=SAOUTF}
N 2040 -840 2040 -710 {lab=SAOUT}
N 1550 -840 1550 -710 {lab=SAOUTF}
N 2260 -810 2290 -810 {lab=VCC}
N 2230 -880 2260 -880 {lab=VCC}
N 2260 -880 2260 -840 {lab=VCC}
N 2260 -780 2260 -710 {lab=SAOUT}
N 720 -590 750 -590 {lab=VCC}
N 550 -650 550 -620 {lab=SP}
N 660 -650 750 -650 {lab=SP}
N 750 -650 750 -620 {lab=SP}
N 550 -650 660 -650 {lab=SP}
N 550 -590 580 -590 {lab=VCC}
N 520 -410 550 -410 {lab=VSS}
N 750 -410 780 -410 {lab=VSS}
N 590 -410 710 -410 {lab=GP}
N 590 -460 590 -410 {lab=GP}
N 550 -460 590 -460 {lab=GP}
N 550 -490 550 -460 {lab=GP}
N 750 -470 750 -440 {lab=OUTDIFF}
N 750 -380 750 -360 {lab=VSSI}
N 650 -360 750 -360 {lab=VSSI}
N 550 -380 550 -360 {lab=VSSI}
N 620 -320 650 -320 {lab=VSSI}
N 650 -360 650 -320 {lab=VSSI}
N 660 -780 690 -780 {lab=VCC}
N 660 -680 660 -650 {lab=SP}
N 660 -830 660 -810 {lab=VCC}
N 750 -470 810 -470 {lab=OUTDIFF}
N 660 -750 660 -740 {lab=#net5}
N 550 -460 550 -440 {
lab=GP}
N 490 -590 510 -590 {
lab=MINUS}
N 790 -590 810 -590 {
lab=PLUS}
N 910 -640 930 -640 {
lab=MINUS}
N 1210 -640 1230 -640 {
lab=PLUS}
N 1300 -1250 1300 -900 {lab=#net1}
N 1790 -1250 1790 -900 {lab=#net2}
N 2370 -1250 2370 -900 {lab=#net4}
N 570 -780 620 -780 {
lab=GP}
N 990 -510 1030 -510 {
lab=GN}
N 2040 -710 2260 -710 {lab=SAOUT}
N 550 -360 650 -360 {lab=VSSI}
N 750 -490 750 -470 {lab=OUTDIFF}
N 970 -820 970 -800 {lab=GN}
N 1170 -840 1170 -800 {lab=#net6}
N 550 -560 550 -550 {lab=#net7}
N 750 -560 750 -550 {lab=#net8}
N 1010 -870 1030 -870 {
lab=GN}
N 970 -840 970 -820 {
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
C {opin.sym} 180 -880 0 0 { name=p116 lab=SAOUT }
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
  * setseed 17
  * reset
  * echo Seed=$rndseed
  let run=1
  dowhile run <= 300
    if run > 1
      set appendwrite
      reset
    end
    * save saout cal i(vvcc) en plus minus saoutf outdiff
    save all
    tran 0.5n 900n uic
    write autozero_comp.raw
    let run = run + 1
  end
  quit 0
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
C {passgate.sym} 940 -1250 0 1 {name=x1 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 940 -1280 0 1 {name=l19 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 940 -1220 0 1 {name=l44 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 1180 -1160 0 0 {name=M3 verilog_gate=nmos del=50,50,50 model=nmos w=1.0u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1230 -1160 0 1 {name=p179 lab=VSS}
C {pmos4-v.sym} 1180 -1320 0 0 {name=M4 verilog_gate=pmos del=50,50,50 model=pmos w=2.0u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1230 -1320 0 1 {name=p180 lab=VCC}
C {lab_pin.sym} 1170 -1390 0 0 {name=p181 lab=VCC}
C {lab_pin.sym} 1170 -1030 0 0 {name=p182 lab=VSSI}
C {passgate.sym} 1430 -1250 0 1 {name=x2 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1430 -1280 0 1 {name=l45 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 1430 -1220 0 1 {name=l46 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 1670 -1160 0 0 {name=M7 verilog_gate=nmos del=50,50,50 model=nmos w=1.3u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1720 -1160 0 1 {name=p183 lab=VSS}
C {pmos4-v.sym} 1670 -1320 0 0 {name=M9 verilog_gate=pmos del=50,50,50 model=pmos w=2.6u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1720 -1320 0 1 {name=p184 lab=VCC}
C {lab_pin.sym} 1660 -1390 0 0 {name=p185 lab=VCC}
C {lab_pin.sym} 1660 -1030 0 0 {name=p186 lab=VSSI}
C {lab_pin.sym} 1550 -1310 0 0 {name=l47 lab=ZERO1}
C {lab_pin.sym} 1060 -1310 0 0 {name=l48 lab=ZERO0}
C {nmos4-v.sym} 950 -640 0 0 {name=M20 verilog_gate=nmos del=50,50,50 model=nmos w=2u l=0.4u extra="delvto='agauss(0,ABSVAR,3)'"}
C {nmos4-v.sym} 1190 -640 0 1 {name=M8 verilog_gate=nmos del=50,50,50 model=nmos w=2u l=0.4u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1000 -640 0 1 {name=p187 lab=VSS}
C {lab_pin.sym} 1140 -640 0 0 {name=p188 lab=VSS}
C {pmos4-v.sym} 1150 -870 0 0 {name=M30 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1200 -870 0 1 {name=p189 lab=VCC}
C {pmos4-v.sym} 990 -870 0 1 {name=M12 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 940 -870 0 0 {name=p190 lab=VCC}
C {nmos4-v.sym} 1050 -510 0 0 {name=M32 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1100 -510 0 1 {name=p191 lab=VSS}
C {lab_pin.sym} 1070 -950 0 0 {name=p192 lab=VCC}
C {lab_pin.sym} 1040 -380 0 0 {name=p193 lab=VSSI}
C {nmos4-v.sym} 1530 -510 0 0 {name=M17 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1580 -510 0 1 {name=p194 lab=VSS}
C {pmos4-v.sym} 1530 -870 0 0 {name=M13 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1580 -870 0 1 {name=p195 lab=VCC}
C {lab_pin.sym} 1520 -940 0 0 {name=p196 lab=VCC}
C {lab_pin.sym} 1520 -380 0 0 {name=p197 lab=VSSI}
C {lab_pin.sym} 1790 -750 0 0 {name=l49 lab=SAOUTF}
C {parax_cap.sym} 1620 -700 0 0 {name=c3  value=4f}
C {lab_pin.sym} 1300 -750 0 0 {name=l51 lab=OUTDIFF}
C {lab_pin.sym} 1070 -570 0 0 {name=l52 lab=SN}
C {lab_pin.sym} 1030 -820 0 1 {name=l53 lab=GN}
C {parax_cap.sym} 1060 -860 0 0 {name=c5  value=4f}
C {parax_cap.sym} 990 -580 0 0 {name=c30  value=2f}
C {parax_cap.sym} 1260 -700 0 0 {name=c31  value=4f}
C {lab_pin.sym} 990 -510 0 0 {name=p198 lab=GN}
C {lab_pin.sym} 1230 -640 0 1 {name=l54 lab=PLUS}
C {lab_pin.sym} 910 -640 0 0 {name=l55 lab=MINUS}
C {ammeter.sym} 1070 -450 0 0 {name=vdiffn}
C {ammeter.sym} 1550 -450 0 0 {name=v3}
C {ammeter.sym} 1690 -1100 0 0 {name=v4}
C {ammeter.sym} 1200 -1100 0 0 {name=v6}
C {nmos4-v.sym} 2020 -510 0 0 {name=M19 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2070 -510 0 1 {name=p9 lab=VSS}
C {pmos4-v.sym} 2020 -870 0 0 {name=M21 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2070 -870 0 1 {name=p10 lab=VCC}
C {lab_pin.sym} 2010 -940 0 0 {name=p11 lab=VCC}
C {lab_pin.sym} 2010 -380 0 0 {name=p12 lab=VSSI}
C {parax_cap.sym} 2110 -700 0 0 {name=c1  value=4f}
C {ammeter.sym} 2040 -450 0 0 {name=v1}
C {lab_pin.sym} 2370 -710 0 1 {name=l3 lab=SAOUT}
C {passgate.sym} 1920 -1250 0 1 {name=x3 m=1 
+ wn=0.4u ln=0.13u
+ wp=0.4u lp=0.13u
+ VCCBPIN=VCC VSSBPIN=VSS  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 1920 -1280 0 1 {name=l5 sig_type=std_logic lab=CALB}
C {lab_pin.sym} 1920 -1220 0 1 {name=l6 sig_type=std_logic lab=CALBB}
C {nmos4-v.sym} 2160 -1160 0 0 {name=M23 verilog_gate=nmos del=50,50,50 model=nmos w=1.5u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2210 -1160 0 1 {name=p13 lab=VSS}
C {pmos4-v.sym} 2160 -1320 0 0 {name=M24 verilog_gate=pmos del=50,50,50 model=pmos w=3u l=1.0u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2210 -1320 0 1 {name=p14 lab=VCC}
C {lab_pin.sym} 2150 -1390 0 0 {name=p16 lab=VCC}
C {lab_pin.sym} 2150 -1030 0 0 {name=p17 lab=VSSI}
C {lab_pin.sym} 2040 -1310 0 0 {name=l8 lab=ZERO2}
C {ammeter.sym} 2180 -1100 0 0 {name=v5}
C {pmos4-v.sym} 2240 -810 0 0 {name=M6 verilog_gate=pmos del=50,50,50 model=pmos w=0.6u l=0.2u extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 2290 -810 0 1 {name=p18 lab=VCC}
C {lab_pin.sym} 2230 -880 0 0 {name=p19 lab=VCC}
C {lab_pin.sym} 2220 -810 0 0 {name=l2 lab=EN}
C {pmos4-v.sym} 770 -590 0 1 {name=M18 verilog_gate=pmos del=50,50,50 model=pmos w=4u l=0.4u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 720 -590 0 0 {name=p20 lab=VCC}
C {pmos4-v.sym} 530 -590 0 0 {name=M25 verilog_gate=pmos del=50,50,50 model=pmos w=4u l=0.4u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 580 -590 0 1 {name=p21 lab=VCC}
C {lab_pin.sym} 520 -410 0 0 {name=p22 lab=VSS}
C {lab_pin.sym} 780 -410 0 1 {name=p23 lab=VSS}
C {lab_pin.sym} 620 -320 0 0 {name=p24 lab=VSSI}
C {pmos4-v.sym} 640 -780 0 0 {name=M28 verilog_gate=pmos del=50,50,50 model=pmos w=2u l=0.5u  extra="delvto='agauss(0,ABSVAR,3)'"}
C {lab_pin.sym} 690 -780 0 1 {name=p25 lab=VCC}
C {lab_pin.sym} 660 -830 0 0 {name=p26 lab=VCC}
C {lab_pin.sym} 570 -780 0 0 {name=l7 lab=GP}
C {lab_pin.sym} 550 -480 0 1 {name=l9 lab=GP}
C {lab_pin.sym} 490 -590 0 0 {name=l10 lab=MINUS}
C {lab_pin.sym} 810 -590 0 1 {name=l11 lab=PLUS}
C {lab_pin.sym} 810 -470 0 1 {name=l12 lab=OUTDIFF}
C {nmos4-v.sym} 730 -410 0 0 {name=M26 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {nmos4-v.sym} 570 -410 0 1 {name=M1 verilog_gate=nmos del=50,50,50 model=nmos w=1u l=0.5u extra="delvto='agauss(0,ABSVAR,3)'"}
C {parax_cap.sym} 630 -400 0 0 {name=c2  value=4f}
C {lab_pin.sym} 660 -670 0 0 {name=l13 lab=SP}
C {launcher.sym} 930 -260 0 0 {name=h2 
descr="Simulate" 
tclcommand="xschem netlist; xschem simulate"}
C {parax_cap.sym} 1060 -1150 0 0 {name=c4  value=5f}
C {parax_cap.sym} 1550 -1150 0 0 {name=c6  value=5f}
C {parax_cap.sym} 2040 -1150 0 0 {name=c7  value=5f}
C {launcher.sym} 75 -1075 0 0 {name=h1 
descr="Reload
Waves" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw

proc get_histo \{varname varlist mean min max step\} \{
  xschem raw switch 0
  proc xround \{a size\} \{ return [expr \{round($a/$size) * $size\}]\}
  #### get rounded data
  catch \{unset freq\}
  foreach v1 $varlist \{
    set v1 [xround [expr \{$v1 - $mean\}] $step]
    if \{![info exists freq($v1)]\} \{ set freq($v1) 1\} else \{incr freq($v1)\}
  \}
  #### create histogram raw data in memory
  xschem raw new distrib distrib $varname $min $max $step
  xschem raw add freq0
  set j 0
  for \{set i $min\} \{$i <= $max\} \{set i [expr \{$i + $step\}] \} \{
    set ii  [xround $i $step]
    set v1 0
    if \{[info exists freq($ii)]\} \{ set v1 $freq($ii) \}
    xschem raw set freq0 $j $v1
    incr j
  \}
\}

proc add_histo \{varname varlist mean min max step\} \{
  xschem raw switch 1
  proc xround \{a size\} \{ return [expr \{round($a/$size) * $size\}]\}
  #### get rounded data
  catch \{unset freq\}
  foreach v1 $varlist \{
    set v1 [xround [expr \{$v1 - $mean\}] $step]
    if \{![info exists freq($v1)]\} \{ set freq($v1) 1\} else \{incr freq($v1)\}
  \}
  #### create histogram raw data in memory
  xschem raw add freq1
  set j 0
  for \{set i $min\} \{$i <= $max\} \{set i [expr \{$i + $step\}] \} \{
    set ii  [xround $i $step]
    set v1 0
    if \{[info exists freq($ii)]\} \{ set v1 $freq($ii) \}
    xschem raw set freq1 $j $v1
    incr j
  \}
\}


proc get_values \{\} \{
  set l0 \{\}
  set dset [xschem raw datasets]
  set p [xschem raw pos_at time 259n]
  for \{set i 0\} \{ $i < $dset\} \{ incr i\} \{
     set v [xschem raw value saout $p $i]
     lappend l0 $v
  \}
  set l1 \{\}
  set p [xschem raw pos_at time 339n]
  for \{set i 0\} \{ $i < $dset\} \{ incr i\} \{
     set v [xschem raw value saout $p $i]
     lappend l1 $v
  \}
  get_histo saout $l0 0 0 0.9 0.01
  add_histo saout $l1 0 0 0.9 0.01
  xschem raw switch 0
\}


get_values
xschem redraw


"
}
C {ammeter.sym} 660 -710 0 0 {name=vdiffp}
C {ammeter.sym} 1300 -870 0 0 {name=vcalib0}
C {ammeter.sym} 1790 -870 0 0 {name=vcalib1}
C {ammeter.sym} 2370 -870 0 0 {name=vcalib2}
C {ammeter.sym} 970 -770 0 0 {name=vnleft}
C {ammeter.sym} 1170 -770 0 1 {name=vnright}
C {ammeter.sym} 550 -520 0 0 {name=vpleft}
C {ammeter.sym} 750 -520 0 0 {name=vpright}
