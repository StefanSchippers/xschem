v {xschem version=3.4.4 file_version=1.2
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
B 2 770 -500 1570 -100 {flags=graph
y1=-50
y2=40
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=2
x2=10.0002
divx=5
subdivx=8
xlabmag=1.0
ylabmag=1.0
node="\\"out db20()\\""
color=4
dataset=-1
unitx=1
logx=1
logy=0
}
N 450 -510 500 -510 {lab=B}
N 540 -750 540 -730 {lab=D}
N 360 -750 540 -750 {lab=D}
N 40 -750 160 -750 {lab=VCC}
N 40 -750 40 -730 {lab=VCC}
N 540 -560 540 -540 {lab=C}
N 540 -560 600 -560 {lab=C}
N 660 -560 690 -560 {lab=OUT}
N 690 -560 690 -500 {lab=OUT}
N 540 -470 540 -460 {lab=E}
N 630 -470 630 -460 {lab=E}
N 540 -470 630 -470 {lab=E}
N 360 -510 360 -460 {lab=#net1}
N 360 -630 360 -590 {lab=#net2}
N 270 -510 360 -510 {lab=#net1}
N 220 -750 360 -750 {lab=D}
N 540 -480 540 -470 {lab=E}
N 360 -530 360 -510 {lab=#net1}
N 40 -510 210 -510 {lab=IN}
N 40 -510 40 -480 {lab=IN}
N 690 -560 740 -560 {lab=OUT}
N 360 -510 390 -510 {lab=#net1}
N 360 -750 360 -690 {lab=D}
N 540 -610 540 -560 {lab=C}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
C {code_shown.sym} 30 -310 0 0 {name=CONTROL place=end value=".ac oct 1000 100 10G
.control
   listing e
   run
   write amp_xschem.raw
   let response = db(v(out)/v(in))
   settype decibel response
   gnuplot amp_xschem response xlog
   save all
   * op
   write amp_xschem.raw
.endc

"}
C {code.sym} 590 -240 0 0 {name=MODELS value=".model Q2N2219A  NPN(Is=14.34f Xti=3 Eg=1.11 Vaf=74.03 Bf=255.9 Ne=1.307 Ise=14.34f Ikf=.2847 Xtb=1.5 Br=6.092 Nc=2 Isc=0 Ikr=0 Rc=1 Cjc=7.306p
+ Mjc=.3416 Vjc=.75 Fc=.5 Cje=22.01p Mje=.377 Vje=.75 Tr=46.91n Tf=411.1p Itf=.6 Vtf=1.7 Xtf=3 Rb=10 Vceo=40 Icrating=800m )
"}
C {ind.sym} 190 -750 1 0 {name=l1 value=10uH}
C {vsource.sym} 40 -700 0 0 {name=v1 value=12}
C {gnd.sym} 40 -670 0 0 {name=l11 lab=0}
C {capa.sym} 280 -720 0 1 {name=C4 m=1 value=100nF footprint=1206 device="ceramic capacitor"}
C {res.sym} 360 -560 0 0 {name=R1 value=6.8k footprint=1206 device=resistor m=1}
C {npn.sym} 520 -510 0 0 {name=Q2 model=Q2N2219A device=Q2N5179 footprint=SOT23 area=1}
C {res.sym} 360 -430 0 0 {name=R2 value=1.8k footprint=1206 device=resistor m=1}
C {capa.sym} 630 -560 1 1 {name=C5 m=1 value=1nF footprint=1206 device="ceramic capacitor"}
C {res.sym} 690 -470 0 0 {name=R3 value=120k footprint=1206 device=resistor m=1}
C {gnd.sym} 690 -440 0 0 {name=l12 lab=0}
C {res.sym} 540 -430 0 0 {name=R4 value=100 footprint=1206 device=resistor m=1}
C {gnd.sym} 540 -400 0 0 {name=l13 lab=0}
C {capa.sym} 240 -510 1 1 {name=C6 m=1 value=1nF footprint=1206 device="ceramic capacitor"}
C {vsource.sym} 40 -450 0 0 {name=v2 value="dc 0 ac 1"}
C {gnd.sym} 40 -420 0 0 {name=l14 lab=0}
C {res.sym} 540 -700 0 0 {name=R5 value=330 footprint=1206 device=resistor m=1}
C {capa.sym} 630 -430 0 1 {name=C7 m=1 value=100nF footprint=1206 device="ceramic capacitor"}
C {gnd.sym} 630 -400 0 0 {name=l15 lab=0}
C {gnd.sym} 360 -400 0 0 {name=l16 lab=0}
C {gnd.sym} 280 -690 0 0 {name=l17 lab=0}
C {lab_wire.sym} 120 -750 0 0 {name=l18 sig_type=std_logic lab=VCC}
C {lab_wire.sym} 440 -750 0 0 {name=l19 sig_type=std_logic lab=D}
C {lab_wire.sym} 480 -510 0 0 {name=l20 sig_type=std_logic lab=B}
C {lab_wire.sym} 580 -470 0 0 {name=l21 sig_type=std_logic lab=E}
C {opin.sym} 740 -560 0 0 {name=p2 lab=OUT}
C {lab_wire.sym} 540 -570 0 0 {name=l22 sig_type=std_logic lab=C}
C {lab_pin.sym} 40 -510 0 0 {name=l23 sig_type=std_logic lab=IN}
C {ammeter.sym} 420 -510 3 0 {name=v3 current=8.9002e-05}
C {ammeter.sym} 360 -660 0 0 {name=v4 current=0.001414}
C {ammeter.sym} 540 -640 0 0 {name=v5 current=0.01657}
C {spice_probe.sym} 710 -560 0 0 {name=p1 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 560 -560 0 0 {name=p3 analysis=tran voltage=6.533}
C {spice_probe.sym} 40 -750 0 0 {name=p4 analysis=tran voltage=12}
C {spice_probe.sym} 60 -510 0 0 {name=p5 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 480 -510 0 0 {name=p6 analysis=tran voltage=2.385}
C {spice_probe.sym} 600 -470 0 0 {name=p7 analysis=tran voltage=1.666}
C {spice_probe.sym} 320 -750 0 0 {name=p8 analysis=tran voltage=12}
C {launcher.sym} 1010 -80 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/amp_xschem.raw ac"
}
