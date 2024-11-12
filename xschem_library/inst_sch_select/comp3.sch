v {xschem version=3.4.5 file_version=1.2
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
T {( @#0:resolved_net )} 100 -335 0 1 0.2 0.2 {name=p161 layer=15}
T {( @#0:resolved_net )} 100 -285 0 1 0.2 0.2 {name=p1 layer=15}
T {( @#0:resolved_net )} 240 -315 0 1 0.2 0.2 {name=p20 layer=15}
N 590 -150 590 -130 { lab=0}
N 370 -180 550 -180 { lab=GN1}
N 330 -230 330 -210 { lab=GN1}
N 330 -310 330 -290 { lab=VCC}
N 330 -150 330 -130 { lab=0}
N 590 -240 670 -240 { lab=#net1}
N 590 -240 590 -210 { lab=#net1}
N 550 -410 550 -380 { lab=#net2}
N 510 -380 550 -380 { lab=#net2}
N 510 -380 510 -300 { lab=#net2}
N 510 -460 510 -440 { lab=VCC}
N 800 -460 800 -400 { lab=VCC}
N 670 -370 760 -370 { lab=#net3}
N 330 -210 370 -210 { lab=GN1}
N 370 -210 370 -180 { lab=GN1}
N 670 -460 670 -440 { lab=VCC}
N 550 -410 640 -410 { lab=#net2}
N 800 -150 800 -130 { lab=0}
N 800 -290 800 -210 { lab=#net4}
N 550 -180 550 -170 { lab=GN1}
N 550 -170 660 -170 { lab=GN1}
N 660 -180 660 -170 { lab=GN1}
N 660 -180 760 -180 { lab=GN1}
N 670 -370 670 -300 { lab=#net3}
N 1160 -290 1200 -290 { lab=OUT}
N 800 -290 890 -290 { lab=#net4}
N 980 -150 980 -130 { lab=0}
N 980 -420 980 -400 { lab=VCC}
N 940 -290 940 -180 { lab=#net4}
N 980 -290 980 -210 { lab=#net5}
N 890 -290 940 -290 { lab=#net4}
N 1160 -150 1160 -130 { lab=0}
N 1160 -420 1160 -400 { lab=VCC}
N 1120 -290 1120 -180 { lab=#net5}
N 1160 -290 1160 -210 { lab=OUT}
N 1060 -290 1120 -290 { lab=#net5}
N 990 -560 990 -510 { lab=#net5}
N 930 -560 930 -470 { lab=#net4}
N 890 -560 930 -560 { lab=#net4}
N 890 -560 890 -290 { lab=#net4}
N 1060 -560 1060 -290 { lab=#net5}
N 960 -660 990 -660 { lab=#net5}
N 990 -660 990 -560 { lab=#net5}
N 930 -470 960 -470 { lab=#net4}
N 710 -270 740 -270 {
lab=PLUS}
N 440 -270 470 -270 {
lab=MINUS}
N 670 -410 700 -410 {
lab=VCC}
N 480 -410 510 -410 {
lab=VCC}
N 800 -370 830 -370 {
lab=VCC}
N 980 -370 1010 -370 {
lab=VCC}
N 510 -240 590 -240 { lab=#net1}
N 670 -380 670 -370 { lab=#net3}
N 1160 -340 1160 -290 { lab=OUT}
N 800 -340 800 -290 { lab=#net4}
N 940 -370 940 -290 { lab=#net4}
N 980 -340 980 -290 { lab=#net5}
N 1120 -370 1120 -290 { lab=#net5}
N 930 -620 930 -560 { lab=#net4}
N 980 -290 1060 -290 { lab=#net5}
N 990 -560 1060 -560 { lab=#net5}
N 1160 -370 1190 -370 {
lab=VCC}
C {nmos4.sym} 570 -180 0 0 {name=M1 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 590 -180 0 1 {name=p2 lab=0}
C {lab_pin.sym} 590 -130 0 0 {name=p6 lab=0}
C {nmos4.sym} 350 -180 0 1 {name=M2 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 330 -180 0 0 {name=p7 lab=0}
C {isource.sym} 330 -260 0 0 {name=I0 value=30u}
C {lab_pin.sym} 330 -310 0 0 {name=p9 lab=VCC}
C {lab_pin.sym} 330 -130 0 0 {name=p16 lab=0}
C {nmos4.sym} 490 -270 0 0 {name=M3 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 510 -270 0 1 {name=p17 lab=0}
C {nmos4.sym} 690 -270 0 1 {name=M4 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 670 -270 0 0 {name=p18 lab=0 l=0.2u}
C {pmos4.sym} 530 -410 0 1 {name=M5 model=pmos w=6u l=0.3u m=1}
C {pmos4.sym} 650 -410 0 0 {name=M6 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 670 -460 0 0 {name=p21 lab=VCC}
C {lab_pin.sym} 480 -410 0 0 {name=p23 lab=VCC}
C {lab_pin.sym} 700 -410 0 1 {name=p33 lab=VCC}
C {lab_wire.sym} 500 -180 0 0 {name=l2 lab=GN1}
C {ipin.sym} 100 -310 0 0 {name=p161 lab=PLUS}
C {ipin.sym} 100 -260 0 0 {name=p1 lab=MINUS}
C {opin.sym} 180 -290 0 0 {name=p20 lab=OUT}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 740 -270 0 1 {name=p3 lab=PLUS}
C {lab_pin.sym} 440 -270 0 0 {name=p4 lab=MINUS}
C {lab_pin.sym} 1200 -290 0 1 {name=p14 lab=OUT}
C {pmos4.sym} 780 -370 0 0 {name=M14 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 830 -370 0 1 {name=p15 lab=VCC}
C {lab_pin.sym} 800 -460 0 0 {name=p22 lab=VCC}
C {spice_probe.sym} 420 -180 0 0 {name=p27 attrs=""}
C {spice_probe.sym} 570 -410 0 0 {name=p28 attrs=""}
C {nmos4.sym} 780 -180 0 0 {name=M7 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 800 -180 0 1 {name=p5 lab=0}
C {lab_pin.sym} 800 -130 0 1 {name=p8 lab=0}
C {nmos4.sym} 960 -180 0 0 {name=M8 model=nmos w=1u l=0.4u m=1}
C {lab_pin.sym} 980 -180 0 1 {name=p10 lab=0}
C {lab_pin.sym} 980 -130 0 1 {name=p11 lab=0}
C {pmos4.sym} 960 -370 0 0 {name=M9 model=pmos w=2u l=0.4u m=1}
C {lab_pin.sym} 1010 -370 0 1 {name=p12 lab=VCC}
C {lab_pin.sym} 980 -420 0 0 {name=p13 lab=VCC}
C {nmos4.sym} 1140 -180 0 0 {name=M10 model=nmos w=1u l=0.4u m=1}
C {lab_pin.sym} 1160 -180 0 1 {name=p24 lab=0}
C {lab_pin.sym} 1160 -130 0 1 {name=p25 lab=0}
C {pmos4.sym} 1140 -370 0 0 {name=M11 model=pmos w=2u l=0.4u m=1}
C {lab_pin.sym} 1190 -370 0 1 {name=p26 lab=VCC}
C {lab_pin.sym} 1160 -420 0 0 {name=p29 lab=VCC}
C {nmos4.sym} 960 -490 3 0 {name=M13 model=nmos w=2u l=0.1u m=1}
C {lab_pin.sym} 960 -510 3 1 {name=p31 lab=0}
C {spice_probe.sym} 1070 -290 0 0 {name=p35 attrs=""}
C {spice_probe.sym} 840 -290 0 0 {name=p36 attrs=""}
C {nmos4.sym} 960 -640 1 0 {name=M12 model=nmos w=2u l=0.1u m=1}
C {lab_pin.sym} 960 -620 1 1 {name=p30 lab=0}
C {spice_probe.sym} 560 -240 0 0 {name=p32 attrs=""}
C {parax_cap.sym} 390 -170 0 0 {name=C2 gnd=0 value=200f m=1}
C {vdd.sym} 510 -460 0 0 {name=l3 lab=VCC}
