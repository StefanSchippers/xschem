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
V {}
S {}
E {}
N 350 -370 350 -350 {lab=VCCPIN}
N 350 -170 350 -150 {lab=VSSPIN}
N 350 -290 660 -290 {lab=PP}
N 350 -230 660 -230 {lab=NN}
N 1090 -340 1090 -320 {lab=VCCPIN}
N 1090 -200 1090 -180 {lab=VSSPIN}
N 210 -200 310 -200 {lab=A}
N 210 -320 210 -200 {lab=A}
N 210 -320 310 -320 {lab=A}
N 260 -260 310 -260 {lab=E}
N 830 -230 1050 -230 {lab=NN}
N 830 -290 1050 -290 {lab=PP}
N 830 -310 830 -290 {lab=PP}
N 830 -390 830 -370 {lab=VCCPIN}
N 830 -150 830 -130 {lab=VSSPIN}
N 830 -230 830 -210 {lab=NN}
N 1090 -260 1150 -260 {lab=Z}
N 660 -290 840 -290 {lab=PP}
N 660 -230 850 -230 {lab=NN}
N 260 -400 260 -260 {lab=E}
N 260 -400 670 -400 {lab=E}
N 670 -400 670 -340 {lab=E}
N 670 -340 790 -340 {lab=E}
N 700 -260 700 -180 {lab=EN}
N 700 -180 790 -180 {lab=EN}
N 870 -620 870 -600 {lab=VCCPIN}
N 870 -480 870 -460 {lab=VSSPIN}
N 830 -570 830 -510 {lab=E}
N 870 -540 920 -540 {lab=EN}
N 570 -290 570 -200 {lab=PP}
N 440 -230 440 -200 {lab=NN}
C {title.sym} 170 10 0 0 {name=l3 author="Stefan Schippers"}
C {plv4t.sym} 680 -260 0 1 {name=m7 model=cmosp w=56u l=2.4u m=1
}
C {plv4t.sym} 330 -320 0 0 {name=m8 model=cmosp w=56u l=2.4u m=1
}
C {lab_pin.sym} 350 -370 0 0 {name=p22 lab=VCCPIN}
C {nlv.sym} 330 -200 0 0 {name=m9 model=cmosn w=20u l=2.4u m=1}
C {lab_pin.sym} 350 -150 0 0 {name=p23 lab=VSSPIN}
C {nlv.sym} 330 -260 0 0 {name=m1 model=cmosn w=24u l=2.4u m=1}
C {plv4t.sym} 1070 -290 0 0 {name=m11 model=cmosp w=200u l=2.4u m=1
}
C {lab_pin.sym} 1090 -340 0 0 {name=p8 lab=VCCPIN}
C {nlv.sym} 1070 -230 0 0 {name=m12 model=cmosn w=80u l=2.4u m=1}
C {lab_pin.sym} 1090 -180 0 0 {name=p1 lab=VSSPIN}
C {nlv.sym} 810 -180 0 0 {name=m13 model=cmosn w=20u l=2.4u m=1}
C {plv4t.sym} 810 -340 0 0 {name=m14 model=cmosp w=50u l=2.4u m=1
}
C {lab_pin.sym} 830 -390 0 0 {name=p9 lab=VCCPIN}
C {lab_pin.sym} 830 -130 0 0 {name=p2 lab=VSSPIN}
C {lab_pin.sym} 210 -200 0 0 {name=p28 lab=A}
C {lab_pin.sym} 260 -400 0 0 {name=p29 lab=E}
C {lab_pin.sym} 1150 -260 0 1 {name=p30 lab=Z}
C {lab_wire.sym} 760 -290 0 0 {name=l0 lab=PP}
C {lab_wire.sym} 760 -230 0 0 {name=l4 lab=NN}
C {lab_pin.sym} 700 -180 0 0 {name=p31 lab=EN}
C {ipin.sym} 390 -440 0 0 {name=p32 lab=A}
C {ipin.sym} 390 -480 0 0 {name=p33 lab=E}
C {opin.sym} 660 -460 0 0 {name=p34 lab=Z}
C {plv4t.sym} 850 -570 0 0 {name=m15 model=cmosp w=50u l=2.4u m=1
}
C {lab_pin.sym} 870 -620 0 0 {name=p10 lab=VCCPIN}
C {nlv.sym} 850 -510 0 0 {name=m16 model=cmosn w=20u l=2.4u m=1}
C {lab_pin.sym} 870 -460 0 0 {name=p3 lab=VSSPIN}
C {lab_pin.sym} 830 -540 0 0 {name=p41 lab=E}
C {lab_pin.sym} 920 -540 0 1 {name=p42 lab=EN}
C {capa.sym} 480 -370 0 0 {name=c0 m=1 value=3f}
C {lab_pin.sym} 480 -340 0 0 {name=p4 lab=VSSPIN}
C {capa.sym} 440 -170 0 0 {name=c4 m=1 value=3f}
C {lab_pin.sym} 440 -140 0 0 {name=p5 lab=VSSPIN}
C {capa.sym} 570 -170 0 0 {name=c5 m=1 value=3f}
C {lab_pin.sym} 570 -140 0 0 {name=p6 lab=VSSPIN}
C {capa.sym} 700 -150 0 0 {name=c6 m=1 value=3f}
C {lab_pin.sym} 700 -120 0 0 {name=p7 lab=VSSPIN}
C {lab_pin.sym} 660 -260 0 0 {name=p11 lab=VCCPIN}
C {lab_pin.sym} 350 -320 0 1 {name=p12 lab=VCCPIN}
C {lab_pin.sym} 870 -570 0 1 {name=p13 lab=VCCPIN}
C {lab_pin.sym} 830 -340 0 1 {name=p14 lab=VCCPIN}
C {lab_pin.sym} 1090 -290 0 1 {name=p15 lab=VCCPIN}
